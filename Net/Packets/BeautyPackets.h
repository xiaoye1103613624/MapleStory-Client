//////////////////////////////////////////////////////////////////////////////////
// BeautyPackets — 美容院 C→S 0x174（十进制 372）。
//   byte action(0 REQ/1 SAVE/2 APPLY/3 DELETE) + [byte slot, byte type]
//   action 4 UNLOCK: short cashSlot
// 与 s9 RecvOpcode.BEAUTY_ACTION / BeautyHandler 逐字节对齐。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class BeautyRequestPacket : public OutPacket
	{
	public:
		BeautyRequestPacket() : OutPacket(OutPacket::Opcode::BEAUTY_ACTION) { write_byte(0); }
	};

	class BeautySavePacket : public OutPacket
	{
	public:
		BeautySavePacket(int16_t slot, int16_t type)
			: OutPacket(OutPacket::Opcode::BEAUTY_ACTION)
		{
			write_byte(1); write_byte(slot); write_byte(type);
		}
	};

	class BeautyApplyPacket : public OutPacket
	{
	public:
		BeautyApplyPacket(int16_t slot, int16_t type)
			: OutPacket(OutPacket::Opcode::BEAUTY_ACTION)
		{
			write_byte(2); write_byte(slot); write_byte(type);
		}
	};

	class BeautyDeletePacket : public OutPacket
	{
	public:
		BeautyDeletePacket(int16_t slot, int16_t type)
			: OutPacket(OutPacket::Opcode::BEAUTY_ACTION)
		{
			write_byte(3); write_byte(slot); write_byte(type);
		}
	};

	class BeautyUnlockPacket : public OutPacket
	{
	public:
		explicit BeautyUnlockPacket(int16_t cashSlot)
			: OutPacket(OutPacket::Opcode::BEAUTY_ACTION)
		{
			write_byte(4); write_short(cashSlot);
		}
	};
}
