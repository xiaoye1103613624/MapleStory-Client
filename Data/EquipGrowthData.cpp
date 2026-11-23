//////////////////////////////////////////////////////////////////////////////////
#include "EquipGrowthData.h"

#include "../Util/Resource/ResourceService.h"
#include "../MapleStory.h"

#include <nlnx/nx.hpp>

#include <cstdio>
#include <sstream>

namespace ms
{
	std::unordered_map<int32_t, EquipGrowthData::TipEntry> EquipGrowthData::cache;
	std::unordered_map<int32_t, int> EquipGrowthData::request_attempts;
	std::unordered_map<int32_t, bool> EquipGrowthData::pending;

	namespace
	{
		struct StatKey
		{
			const char* wz;
			const char* label;
			int bonus_idx;
		};

		constexpr StatKey kKeys[] = {
			{ "incSTRMin",   "力量",     0 },
			{ "incDEXMin",   "敏捷",     1 },
			{ "incINTMin",   "智力",     2 },
			{ "incLUKMin",   "运气",     3 },
			{ "incMHPMin",   "MaxHP",    4 },
			{ "incMMPMin",   "MaxMP",    5 },
			{ "incPADMin",   "攻击力",   6 },
			{ "incMADMin",   "魔法力",   7 },
			{ "incPDDMin",   "防御力",   8 },
			{ "incMDDMin",   "魔法防御", 9 },
			{ "incACCMin",   "命中",     10 },
			{ "incEVAMin",   "回避",     11 },
			{ "incSpeedMin", "移动速度", 13 },
			{ "incJumpMin",  "跳跃",     14 },
		};

		std::string category_folder(int32_t item_id)
		{
			const int prefix = item_id / 10000;

			switch (prefix)
			{
			case 100: return "Cap";
			case 101: case 102: case 103: case 112: case 114: return "Accessory";
			case 104: return "Coat";
			case 105: return "Longcoat";
			case 106: return "Pants";
			case 107: return "Shoes";
			case 108: return "Glove";
			case 109: return "Shield";
			case 110: return "Cape";
			case 111: return "Ring";
			case 113: return "Belt";
			case 115: return "Shoulder";
			default: break;
			}

			if (prefix >= 130 && prefix <= 170)
				return "Weapon";

			return {};
		}

		nl::node level_info_root(int32_t item_id)
		{
			const std::string folder = category_folder(item_id);

			if (folder.empty())
				return {};

			char idbuf[16];
			std::snprintf(idbuf, sizeof(idbuf), "%08d", item_id);

			// 与 EquipData 同口径：Character/<Cat>/<id>.img/info/level[/info]
			nl::node info = nl::nx::Character[folder][std::string(idbuf) + ".img"]["info"];
			if (!info)
			{
				nl::node ch = res::ResourceService::get().root(res::RootId::Character);
				if (ch)
					info = ch[folder][std::string(idbuf) + ".img"]["info"];
			}
			if (!info)
				return {};

			nl::node level = info["level"];
			if (!level)
				return {};

			nl::node nested = level["info"];
			return nested ? nested : level;
		}

		int resolve_max_level(nl::node root)
		{
			if (!root)
				return 1;

			int cur = 1;

			for (; cur <= 40; ++cur)
			{
				nl::node node = root[std::to_string(cur)];

				if (!node)
					return cur;

				bool any = false;

				for (const StatKey& sk : kKeys)
				{
					if (static_cast<int32_t>(node[sk.wz].get_integer()) != 0)
					{
						any = true;
						break;
					}
				}

				if (!any)
					return cur;
			}

			return 40;
		}
	}

	bool EquipGrowthData::build_local(int32_t item_id, int32_t item_level, int32_t scroll, TipEntry& out)
	{
		out = TipEntry{};
		out.item_id = item_id;
		out.item_level = item_level;
		out.scroll = scroll;

		nl::node root = level_info_root(item_id);
		const int max_lv = resolve_max_level(root);

		if (max_lv <= 1)
		{
			out.resolved = true;
			out.local_only = true;
			return false;
		}

		std::ostringstream sb;
		sb << "装备成长属性\n";
		bool any = false;
		out.bonus.fill(0);

		for (int node = 1; node < max_lv; ++node)
		{
			nl::node lvl = root[std::to_string(node)];
			if (!lvl)
				continue;

			std::ostringstream segment;
			bool seg_any = false;

			for (const StatKey& sk : kKeys)
			{
				const int32_t v = static_cast<int32_t>(lvl[sk.wz].get_integer());

				if (v == 0)
					continue;

				segment << sk.label << " +" << v << "\n";
				seg_any = true;

				if (sk.bonus_idx >= 0 && sk.bonus_idx < BONUS_SLOTS && node < item_level)
					out.bonus[static_cast<size_t>(sk.bonus_idx)] =
						static_cast<int16_t>(out.bonus[static_cast<size_t>(sk.bonus_idx)] + v);
			}

			if (!seg_any)
				continue;

			any = true;
			sb << node << "级效果\n" << segment.str();
		}

		if (!any)
			return false;

		out.text = sb.str();

		while (!out.text.empty() && (out.text.back() == '\n' || out.text.back() == '\r'))
			out.text.pop_back();

		out.has_bonus = (item_level > 1);
		out.resolved = true;
		out.local_only = true;
		cache[item_id] = out;
		return true;
	}

	void EquipGrowthData::set_server_tip(int32_t item_id, bool has_data, const std::string& text,
		const int16_t* bonus, bool has_bonus, const int16_t* flame, bool has_flame,
		int32_t item_level, int32_t scroll)
	{
		TipEntry& e = cache[item_id];
		e.item_id = item_id;
		e.item_level = item_level;
		e.scroll = scroll;
		e.from_server = true;
		// 第二轮修复：keep_local 必须在覆盖 local_only 之前读旧值。
		// 原实现先 e.local_only=false，再用 e.local_only 算 keep_local ⇒ 恒 false，
		// 「服务端 has_data=false 时保留本地成长文案」分支永不触发。
		const bool keep_local = e.local_only && e.text.find("级效果") != std::string::npos;
		e.local_only = false;
		e.resolved = true;
		clear_pending(item_id);
		request_attempts.erase(item_id);


		if (!has_data)
		{
			if (!keep_local)
				e.text.clear();
		}
		else if (!text.empty() && !keep_local)
		{
			e.text = text;
		}

		if (has_bonus && bonus)
		{
			e.has_bonus = true;
			for (int i = 0; i < BONUS_SLOTS; ++i)
				e.bonus[static_cast<size_t>(i)] = bonus[i];
		}

		if (has_flame && flame)
		{
			e.has_flame = true;
			for (int i = 0; i < BONUS_SLOTS; ++i)
				e.flame[static_cast<size_t>(i)] = flame[i];
		}
	}

	bool EquipGrowthData::try_get(int32_t item_id, TipEntry& out)
	{
		auto it = cache.find(item_id);

		if (it == cache.end())
			return false;

		out = it->second;
		return true;
	}

	bool EquipGrowthData::has_usable(int32_t item_id)
	{
		auto it = cache.find(item_id);
		return it != cache.end() && it->second.resolved && !it->second.text.empty();
	}

	bool EquipGrowthData::begin_request(int32_t item_id)
	{
		if (item_id <= 0)
			return false;

		if (has_usable(item_id))
			return false;

		auto it = cache.find(item_id);

		if (it != cache.end() && it->second.resolved && it->second.local_only && it->second.text.empty())
			return false; // 已确认无成长

		if (pending[item_id])
			return false;

		int& attempts = request_attempts[item_id];

		if (attempts >= 1)
			return false;

		++attempts;
		pending[item_id] = true;
		return true;
	}

	void EquipGrowthData::clear_pending(int32_t item_id)
	{
		pending.erase(item_id);
	}

	void EquipGrowthData::invalidate(int32_t item_id)
	{
		cache.erase(item_id);
		pending.erase(item_id);
		request_attempts.erase(item_id);
	}

	void EquipGrowthData::clear_all()
	{
		cache.clear();
		pending.clear();
		request_attempts.clear();
	}
}
