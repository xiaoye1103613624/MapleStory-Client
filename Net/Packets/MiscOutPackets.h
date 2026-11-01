////////////////////////////////////////////////////////////////////////////////
//	Thin outbound shells — ENTER_MTS / REMOTE_STORE / WATER_OF_LIFE /
//	OPEN_ITEMUI / WORLD_MAP_PLAYERS
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class EnterMtsPacket : public OutPacket
	{
	public:
		EnterMtsPacket() : OutPacket(OutPacket::Opcode::ENTER_MTS) {}
	};

	class RemoteStorePacket : public OutPacket
	{
	public:
		RemoteStorePacket() : OutPacket(OutPacket::Opcode::REMOTE_STORE) {}
	};

	class WaterOfLifePacket : public OutPacket
	{
	public:
		WaterOfLifePacket() : OutPacket(OutPacket::Opcode::WATER_OF_LIFE) {}
	};

	class OpenItemUiPacket : public OutPacket
	{
	public:
		OpenItemUiPacket(int16_t info_number) : OutPacket(OutPacket::Opcode::OPEN_ITEMUI)
		{
			write_short(info_number);
		}
	};

	class WorldMapPlayersPacket : public OutPacket
	{
	public:
		WorldMapPlayersPacket(int32_t mapid) : OutPacket(OutPacket::Opcode::WORLD_MAP_PLAYERS)
		{
			write_int(mapid);
		}
	};
}
