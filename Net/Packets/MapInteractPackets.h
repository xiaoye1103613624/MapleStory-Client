////////////////////////////////////////////////////////////////////////////////
//	W4 地图特殊交互 c2s
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// Opcode: CHANGE_MAP_SPECIAL(100 / 0x64)
	// ChangeMapSpecialHandler: byte, string portalName, short
	class ChangeMapSpecialPacket : public OutPacket
	{
	public:
		explicit ChangeMapSpecialPacket(const std::string& portal_name)
			: OutPacket(OutPacket::Opcode::CHANGE_MAP_SPECIAL)
		{
			write_byte(0);
			write_string(portal_name);
			write_short(0);
		}
	};

	// Opcode: USE_INNER_PORTAL(101 / 0x65)
	// InnerPortalHandler: byte, string portalName
	class UseInnerPortalPacket : public OutPacket
	{
	public:
		explicit UseInnerPortalPacket(const std::string& portal_name)
			: OutPacket(OutPacket::Opcode::USE_INNER_PORTAL)
		{
			write_byte(0);
			write_string(portal_name);
		}
	};

	// Opcode: TOUCHING_REACTOR(206 / 0xCE)
	// TouchReactorHandler: int oid, byte touching(1/0)
	class TouchingReactorPacket : public OutPacket
	{
	public:
		TouchingReactorPacket(int32_t oid, bool touching)
			: OutPacket(OutPacket::Opcode::TOUCHING_REACTOR)
		{
			write_int(oid);
			write_byte(touching ? 1 : 0);
		}
	};

	// Opcode: USE_TREASUER_CHEST(115 / 0x73)
	// UseTreasureChestHandler: short slot, int itemid
	class UseTreasureChestPacket : public OutPacket
	{
	public:
		UseTreasureChestPacket(int16_t slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::USE_TREASUER_CHEST)
		{
			write_short(slot);
			write_int(itemid);
		}
	};

	// Opcode: USE_ITEM_REWARD(112 / 0x70)
	// ItemRewardHandler: short slot, int itemId
	class UseItemRewardPacket : public OutPacket
	{
	public:
		UseItemRewardPacket(int16_t slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::USE_ITEM_REWARD)
		{
			write_short(slot);
			write_int(itemid);
		}
	};
}
