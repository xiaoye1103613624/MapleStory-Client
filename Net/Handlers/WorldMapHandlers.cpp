////////////////////////////////////////////////////////////////////////////////
//	World-map handler implementations.
//	WORLD_MAP_DROP_RESULT(0x179)：解析体在 WorldMapHandlers.h 内联（既有）。
//	WORLD_MAP_PLAYERS(0x178)：把 s9 WorldMapPlayersHandler 的回包灌进
//	UIWorldMap，让大地图显示「此地图当前玩家（等级/频道）」。
////////////////////////////////////////////////////////////////////////////////
#include "WorldMapHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIWorldMap.h"

namespace ms
{
	void WorldMapPlayersHandler::handle(InPacket& recv) const
	{
		// 与服务端写出顺序严格对齐：int mapId → short count → N×{string,short,byte}
		const int32_t mapid = recv.read_int();
		const int16_t count = recv.read_short();

		std::vector<WorldMapPlayer> players;
		if (count > 0 && count < 200)
		{
			players.reserve(static_cast<size_t>(count));

			for (int16_t i = 0; i < count && recv.available(); i++)
			{
				WorldMapPlayer p;
				p.name = recv.read_string();
				p.level = recv.read_short();
				p.channel = recv.read_byte();
				players.push_back(std::move(p));
			}
		}

		// 尾字节防御性吃掉（服务端将来追加字段不至于错位报错）
		while (recv.available())
			recv.skip_byte();

		if (auto wmap = UI::get().get_element<UIWorldMap>())
			wmap->set_map_players(mapid, std::move(players));

		LOG(LOG_INFO, "[WorldMap] players map=" << mapid << " n=" << players.size());
	}

	void WorldMapQuestMarksHandler::handle(InPacket& recv) const
	{
		// 与服务端写出顺序对齐：int prefix → short mapCount →
		// mapCount × { int mapid, byte flags(bit0=可接/灯泡, bit1=可交付/书) }
		const int32_t prefix = recv.read_int();
		const int16_t mapCount = recv.read_short();

		std::vector<WorldMapQuestMarks> marks;
		if (mapCount > 0 && mapCount < 10000)
		{
			marks.reserve(static_cast<size_t>(mapCount));
			for (int16_t i = 0; i < mapCount && recv.available() >= 5; i++)
			{
				WorldMapQuestMarks m;
				m.mapid = recv.read_int();
				const uint8_t flags = recv.read_byte();
				m.can_accept = (flags & 0x01) != 0;
				m.can_complete = (flags & 0x02) != 0;
				marks.push_back(std::move(m));
			}
		}

		while (recv.available())
			recv.skip_byte();

		if (auto wmap = UI::get().get_element<UIWorldMap>())
			wmap->set_quest_marks(prefix, std::move(marks));

		LOG(LOG_INFO, "[WorldMap] quest marks prefix=" << prefix << " n=" << marks.size());
	}
}
