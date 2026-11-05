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

	class WorldMapDropRequestPacket : public OutPacket
	{
	public:
		WorldMapDropRequestPacket(int32_t mobid, int32_t mapid, bool includeGlobal)
			: OutPacket(OutPacket::Opcode::WORLD_MAP_DROP_REQUEST)
		{
			write_int(mobid);
			write_int(mapid);
			write_byte(includeGlobal ? 1 : 0);
		}
	};

	// s9 RecvOpcode.SET_HPMPALERT(0x1000)：上报 HP/MP 报警阈值挡位。
	// 量纲：服务端 HpMpAlertService 按 step(0..19)/20 折算百分比（每挡 5%）；
	// 客户端 ui.sysopt.hp_warn/mp_warn 存 0..100 百分比，故由调用侧换算后传入。
	class SetHpMpAlertPacket : public OutPacket
	{
	public:
		SetHpMpAlertPacket(uint8_t hp_step, uint8_t mp_step)
			: OutPacket(OutPacket::Opcode::SET_HPMPALERT)
		{
			write_byte(static_cast<int8_t>(hp_step));
			write_byte(static_cast<int8_t>(mp_step));
		}
	};
}
