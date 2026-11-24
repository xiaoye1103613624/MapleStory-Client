//////////////////////////////////////////////////////////////////////////////////
// DailyCheckinPackets — 每日签到出站包
// 对接 s9 RecvOpcode.DAILY_CHECKIN(0x3717) → DailyCheckinHandler：
//   OPEN  = byte 0
//   CLAIM = byte 1, byte day(1..28)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class DailyCheckinOpenPacket : public OutPacket
	{
	public:
		DailyCheckinOpenPacket() : OutPacket(OutPacket::Opcode::DAILY_CHECKIN)
		{
			write_byte(0);
		}
	};

	class DailyCheckinClaimPacket : public OutPacket
	{
	public:
		explicit DailyCheckinClaimPacket(int16_t day)
			: OutPacket(OutPacket::Opcode::DAILY_CHECKIN)
		{
			write_byte(1);
			write_byte(static_cast<uint8_t>(day));
		}
	};
}
