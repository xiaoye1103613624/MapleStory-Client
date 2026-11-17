//////////////////////////////////////////////////////////////////////////////////
//	SetItemData — 套装花名册（2026-11-12 装备槽位扩展 ⑤）
//////////////////////////////////////////////////////////////////////////////////
#include "SetItemData.h"

#include "../Configuration.h"
#include "../MapleStory.h"
#include "../Util/Resource/ResourceService.h"

#include <nlnx/nx.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace ms
{
	std::unordered_map<int32_t, SetItemData::Definition> SetItemData::definitions;
	std::unordered_map<int32_t, int32_t> SetItemData::item_to_set;
	bool SetItemData::loaded = false;

	namespace
	{
		std::string trim(const std::string& s)
		{
			size_t a = s.find_first_not_of(" \t\r\n");

			if (a == std::string::npos)
				return "";

			size_t b = s.find_last_not_of(" \t\r\n");
			return s.substr(a, b - a + 1);
		}
	}

	bool SetItemData::load_from_wz()
	{
		definitions.clear();
		item_to_set.clear();
		loaded = false;

		// 必须经资源门面取 Etc 根，不能直接读 nl::nx::Etc：
		// 堆式 ImgBackend 下 Etc 是 lazy 根，门面 root() 会按需 ensure() 装载；
		// 裸读全局变量在未绑定时为空 → 静默降级 txt（WZ 改造形同虚设）。
		// wz/nx/.nxcache mmap 后端 open() 已绑全部根，门面同样返回有效节点。
		nl::node etc_root = res::ResourceService::get().root(res::RootId::Etc);
		nl::node root = etc_root ? etc_root["SetItemInfo.img"] : nl::node{};

		if (!root)
		{
			LOG(LOG_DEBUG, "SetItemData: Etc/SetItemInfo.img not available -> txt fallback");
			return false;
		}

		// WZ Effect 属性键 → 本端 EquipStat/stat_key 白名单。
		// 未列出的键（activeSkill/Option/incMHPr/incMMPr/nbdR/incPQEXPr 等）按
		// 用户裁决跳过：本端 EquipStat 模型不支持，强导会乱码/丢属性。
		static const struct { const char* wz; const char* key; } kMap[] = {
			{ "incSTR",     "STR"     },
			{ "incDEX",     "DEX"     },
			{ "incINT",     "INT"     },
			{ "incLUK",     "LUK"     },
			{ "incMHP",     "HP"      },
			{ "incMMP",     "MP"      },
			{ "incPAD",     "WATK"    },
			{ "incMAD",     "MAGIC"   },
			{ "incPDD",     "WDEF"    },
			{ "incMDD",     "MDEF"    },
			{ "incACC",     "ACC"     },
			{ "incEVA",     "AVOID"   },
			{ "incSpeed",   "SPEED"   },
			{ "incSPEED",   "SPEED"   },
			{ "incJump",    "JUMP"    },
			{ "incJUMP",    "JUMP"    },
			{ "incAllStat", "ALLSTAT" },
			{ "damR",       "DAMAGE"  },
		};

		auto map_key = [](const std::string& wz) -> const char* {
			for (const auto& m : kMap)
				if (wz == m.wz)
					return m.key;
			return nullptr;
		};

		int mapped = 0;
		int skipped = 0;

		for (const auto& setnode : root)
		{
			std::string nodename = setnode.name();

			if (nodename.empty())
				continue;

			// set 名为数字（含 "-1" 特殊套装）；其余结构节点跳过。
			char c0 = nodename[0];

			if (!std::isdigit(static_cast<unsigned char>(c0)) && c0 != '-')
				continue;

			int32_t set_id = std::atoi(nodename.c_str());

			if (set_id == 0)
				continue;

			Definition def{};
			def.set_id = set_id;
			def.name = setnode["setItemName"].get_string();
			def.complete_count = static_cast<int16_t>(setnode["completeCount"].get_integer());

			// ItemID 是 LIST，子节点 1..N，值为 itemId（get_integer 自动处理 SHORT/INT）。
			for (const auto& itemnode : setnode["ItemID"])
			{
				int32_t item_id = static_cast<int32_t>(itemnode.get_integer());

				if (item_id > 0)
				{
					def.item_ids.push_back(item_id);
					item_to_set[item_id] = set_id;
				}
			}

			// Effect 是 LIST，子节点名为件套数（"2"/"4"…），其下为 inc* 属性。
			for (const auto& tiernode : setnode["Effect"])
			{
				std::string cnt = tiernode.name();

				if (cnt.empty() || !std::isdigit(static_cast<unsigned char>(cnt[0])))
					continue;

				int16_t count = static_cast<int16_t>(std::atoi(cnt.c_str()));

				if (count <= 0)
					continue;

				for (const auto& prop : tiernode)
				{
					const char* key = map_key(prop.name());

					if (!key)
					{
						++skipped;
						continue;
					}

					Tier tier{};
					tier.count = count;
					tier.stat_key = key;
					tier.value = static_cast<int32_t>(prop.get_integer());
					def.tiers.push_back(tier);
					++mapped;
				}
			}

			definitions[set_id] = std::move(def);
		}

		loaded = !definitions.empty();
		LOG(LOG_DEBUG, "SetItemData: WZ loaded " << definitions.size() << " sets, "
			<< item_to_set.size() << " item mappings, " << mapped
			<< " supported tiers, " << skipped << " unsupported keys skipped");
		return loaded;
	}

	bool SetItemData::load(const std::string& data_dir)
	{
		definitions.clear();
		item_to_set.clear();
		loaded = false;

		std::string base = data_dir.empty() ? Configuration::get().get_data_dir() : data_dir;

		if (base.empty())
			base = "Data";

		std::ifstream file(base + "/SetItem.txt");

		if (!file.good())
		{
			// 花名册是可选增强：缺失时优雅降级，套装系统按「无套装」运行。
			LOG(LOG_DEBUG, "SetItemData: SetItem.txt not found in " << base << " -> no set bonuses");
			return false;
		}

		int32_t current_set = 0;
		std::string line;

		while (std::getline(file, line))
		{
			std::string t = trim(line);

			if (t.empty() || t[0] == '#')
				continue;

			std::istringstream ss(t);
			std::string tag;
			ss >> tag;

			if (tag == "SET")
			{
				SetItemData::Definition def{};
				ss >> def.set_id >> def.name >> def.complete_count;

				if (def.set_id <= 0)
					continue;

				definitions[def.set_id] = def;
				current_set = def.set_id;
			}
			else if (tag == "ITEM")
			{
				int32_t item_id = 0;
				ss >> item_id;

				auto it = definitions.find(current_set);

				if (item_id > 0 && it != definitions.end())
				{
					it->second.item_ids.push_back(item_id);
					item_to_set[item_id] = current_set;
				}
			}
			else if (tag == "TIER")
			{
				SetItemData::Tier tier{};
				ss >> tier.count >> tier.stat_key >> tier.value;

				auto it = definitions.find(current_set);

				if (tier.count > 0 && !tier.stat_key.empty() && it != definitions.end())
					it->second.tiers.push_back(tier);
			}
		}

		loaded = true;
		LOG(LOG_DEBUG, "SetItemData: loaded " << definitions.size() << " sets, "
			<< item_to_set.size() << " item mappings");
		return true;
	}

	int32_t SetItemData::set_of(int32_t item_id)
	{
		auto it = item_to_set.find(item_id);
		return it == item_to_set.end() ? 0 : it->second;
	}

	const SetItemData::Definition* SetItemData::get(int32_t set_id)
	{
		auto it = definitions.find(set_id);
		return it == definitions.end() ? nullptr : &it->second;
	}

	int16_t SetItemData::count_equipped(int32_t set_id, const std::vector<int32_t>& equipped_item_ids)
	{
		const Definition* def = get(set_id);

		if (!def)
			return 0;

		// 去重：同 itemid 穿多件（如双戒指）只计一次，与官方 completeCount 语义一致。
		std::vector<int32_t> owned;

		for (int32_t id : def->item_ids)
			if (std::find(equipped_item_ids.begin(), equipped_item_ids.end(), id) != equipped_item_ids.end())
				if (std::find(owned.begin(), owned.end(), id) == owned.end())
					owned.push_back(id);

		return static_cast<int16_t>(owned.size());
	}

	std::vector<SetItemData::Tier> SetItemData::active_tiers(int32_t set_id, int16_t equipped_count)
	{
		std::vector<Tier> out;
		const Definition* def = get(set_id);

		if (!def)
			return out;

		for (const Tier& tier : def->tiers)
			if (equipped_count >= tier.count)
				out.push_back(tier);

		return out;
	}

	size_t SetItemData::definition_count()
	{
		return definitions.size();
	}
}
