////////////////////////////////////////////////////////////////////////////////
//	Solomon / GachaExp outbound — 157 / 158
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// USE_SOLOMON_ITEM(157)：tick + slot + itemId
	class UseSolomonPacket : public OutPacket
	{
	public:
		UseSolomonPacket(int16_t slot, int32_t itemid) : OutPacket(OutPacket::Opcode::USE_SOLOMON_ITEM)
		{
			write_time();
			write_short(slot);
			write_int(itemid);
		}
	};

	// USE_GACHA_EXP(158)：空包，兑换缓存 gacha exp
	class UseGachaExpPacket : public OutPacket
	{
	public:
		UseGachaExpPacket() : OutPacket(OutPacket::Opcode::USE_GACHA_EXP)
		{
		}
	};
}
