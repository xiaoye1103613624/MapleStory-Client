//////////////////////////////////////////////////////////////////////////////////
// FusionAnvilPackets — 融合外观 C→S（USE_CASH_ITEM 0x4F + itemId 5900000）。
// layout（与 s9 UseCashItemHandler.itemType==590 一致）：
//   short cashSlot | int 5900000 | short skinPos | short basePos
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class FusionAnvilPacket : public OutPacket
	{
	public:
		FusionAnvilPacket(int16_t cashSlot, int16_t skinPos, int16_t basePos)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cashSlot);
			write_int(5900000);
			write_short(skinPos);
			write_short(basePos);
		}
	};
}
