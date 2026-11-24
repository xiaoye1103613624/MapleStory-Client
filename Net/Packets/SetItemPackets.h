//////////////////////////////////////////////////////////////////////////////////
//	套装 / 装备成长 出站封包
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	/// C→S 0x177：悬停按需拉某套装展示文案
	class SetItemBonusRequestPacket : public OutPacket
	{
	public:
		SetItemBonusRequestPacket(int32_t set_id) : OutPacket(OutPacket::Opcode::SET_ITEM_BONUS_REQUEST)
		{
			write_int(set_id);
		}
	};

	/// C→S 0x17D：悬停按需拉装备成长 tip
	class EquipGrowthTipRequestPacket : public OutPacket
	{
	public:
		EquipGrowthTipRequestPacket(int32_t item_id) : OutPacket(OutPacket::Opcode::EQUIP_GROWTH_TIP_REQUEST)
		{
			write_int(item_id);
		}
	};
}
