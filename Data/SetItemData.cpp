//////////////////////////////////////////////////////////////////////////////////
//	SetItemData — 套装花名册（2026-11-12 装备槽位扩展 ⑤）
//////////////////////////////////////////////////////////////////////////////////
#include "SetItemData.h"

#include "../Configuration.h"
#include "../MapleStory.h"

#include <algorithm>
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
