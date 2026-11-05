//////////////////////////////////////////////////////////////////////////////////
//	MobDropData — reserved drop-table cache for world-map drop panel
//	本版：空实现（无 JSON/服务端表）；UI 已按字段排版，后续接装载即可。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Cache.h"

#include <cstdint>
#include <vector>

namespace ms
{
	// One drop row. chance is probability in [0, 1] (e.g. 0.05 = 5%).
	struct MobDropEntry
	{
		int32_t item_id = 0;
		double chance = 0.0;
		int16_t min_qty = 1;
		int16_t max_qty = 1;
	};

	// Cache keyed by mob id. Entries are always sorted by item_id ascending.
	class MobDropData : public Cache<MobDropData>
	{
	public:
		bool is_valid() const;
		explicit operator bool() const;

		int32_t get_mob_id() const;
		const std::vector<MobDropEntry>& get_entries() const;

		// Convenience: returns sorted drops (empty until a real loader is wired).
		static std::vector<MobDropEntry> get_drops(int32_t mob_id);

		// Populated from the server drop-data packet. Replaces any prior entry.
		static void load(int32_t mob_id, std::vector<MobDropEntry> entries);
		// True once the server has supplied drops for this mob.
		static bool is_loaded(int32_t mob_id);
		// Server drop-table version, used to detect reloads.
		static int32_t get_version();
		static void set_version(int32_t version);

	private:
		friend Cache<MobDropData>;
		MobDropData(int32_t mob_id);

		int32_t mob_id;
		std::vector<MobDropEntry> entries;
		bool valid;
		static int32_t data_version;
	};
}
