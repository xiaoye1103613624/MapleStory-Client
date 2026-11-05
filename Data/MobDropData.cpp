//////////////////////////////////////////////////////////////////////////////////
//	MobDropData — stub loader (no drop table source in this client build)
//////////////////////////////////////////////////////////////////////////////////
#include "MobDropData.h"

#include "../MapleStory.h"

#include <algorithm>

namespace ms
{
	MobDropData::MobDropData(int32_t id) : mob_id(id), valid(false)
	{
		// Reserved: load from JSON / server export and sort by item_id.
		// This build ships with no drop tables — keep entries empty and valid=false
		// so the drop panel can show an explicit empty state instead of inventing data.
		LOG(LOG_INFO, "[MobDropData] stub empty drops for mob_id=" << mob_id);

		std::sort(entries.begin(), entries.end(), [](const MobDropEntry& a, const MobDropEntry& b)
		{
			return a.item_id < b.item_id;
		});
	}

	bool MobDropData::is_valid() const
	{
		return valid;
	}

	MobDropData::operator bool() const
	{
		return is_valid();
	}

	int32_t MobDropData::get_mob_id() const
	{
		return mob_id;
	}

	const std::vector<MobDropEntry>& MobDropData::get_entries() const
	{
		return entries;
	}

	std::vector<MobDropEntry> MobDropData::get_drops(int32_t mob_id)
	{
		return get(mob_id).get_entries();
	}

	void MobDropData::load(int32_t mob_id, std::vector<MobDropEntry> entries)
	{
		MobDropData md(mob_id);
		md.entries = std::move(entries);
		md.valid = true;
		// MobDropData 没有默认构造（Cache::get 走 T{id}），unordered_map::operator[]
		// 会强制默认构造 ⇒ C2512。insert_or_assign 只需要可移动/可赋值。
		Cache<MobDropData>::cache.insert_or_assign(mob_id, std::move(md));
	}

	bool MobDropData::is_loaded(int32_t mob_id)
	{
		auto it = Cache<MobDropData>::cache.find(mob_id);
		return it != Cache<MobDropData>::cache.end() && it->second.valid;
	}

	int32_t MobDropData::get_version()
	{
		return data_version;
	}

	void MobDropData::set_version(int32_t version)
	{
		data_version = version;
	}

	int32_t MobDropData::data_version = 0;
}
