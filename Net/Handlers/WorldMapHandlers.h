////////////////////////////////////////////////////////////////////////////////
//	World-map drop-data query result handler (s2c WORLD_MAP_DROP_RESULT = 0x179)
//	Populates the MobDropData cache so the info panel can render drops.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"
#include "../../Data/MobDropData.h"

namespace ms
{
	class WorldMapDropResultHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override
		{
			int32_t version = recv.read_int();
			int32_t mob_id = recv.read_int();
			int32_t count = recv.read_int();

			std::vector<MobDropEntry> entries;
			if (count > 0 && count < 100000)
				entries.reserve(static_cast<size_t>(count));

			for (int32_t i = 0; i < count; i++)
			{
				MobDropEntry e;
				e.item_id = recv.read_int();
				recv.read_byte();            // isGlobal (display-independent)
				int32_t weight = recv.read_int();
				int32_t minq = recv.read_int();
				int32_t maxq = recv.read_int();
				recv.read_int();            // questid (unused for display)

				// chance weight is out of 1,000,000 (matches server drop roll).
				e.chance = static_cast<double>(weight) / 1000000.0;
				e.min_qty = static_cast<int16_t>(minq > 0 ? minq : 1);
				e.max_qty = static_cast<int16_t>(maxq > 0 ? maxq : 1);
				entries.push_back(e);
			}

			MobDropData::set_version(version);
			MobDropData::load(mob_id, entries);
		}
	};

	// s2c WORLD_MAP_PLAYERS = 0x178（s9 WorldMapPlayersHandler 回包）。
	// body 对账（逐字段读服务端源码）：int mapId / short count /
	// count × { string name, short level, byte channel }。
	class WorldMapPlayersHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
}
