//////////////////////////////////////////////////////////////////////////////////
// DailyGiftPackets — 192「每日礼物」出站包（私有协议 0x3721，双端已定）
//
// C→S 0x3721（本文件）：
//   byte action(0=OPEN, 1=CLAIM) + [byte day(1..28)]
//     OPEN  = 请求配置快照，服务端回 0x3722/respType=0；
//     CLAIM = 领取第 day 天奖励，服务端回 0x3722/respType=1。
//
// S→C 0x3722（解析见 Net/Handlers/WaveAShellHandlers.cpp::DailyGiftHandler）：
//   respType=0 配置快照；respType=1 领奖结果。
//
// 写包顺序与服务端 RecvOpcode.DAILY_GIFT_ACTION 逐字节对齐；day 以 uint8_t 写入。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 请求每日礼物配置快照：byte action(0=OPEN)。构造即请求，服务端回 0x3722/0。
	class DailyGiftOpenPacket : public OutPacket
	{
	public:
		DailyGiftOpenPacket() : OutPacket(OutPacket::Opcode::DAILY_GIFT_ACTION)
		{
			write_byte(0);
		}
	};

	// 领取第 day(1..28) 天奖励：byte action(1=CLAIM) + byte day。
	// 仅在客户端判定该格「可领」时发送；真实结果由 0x3722/respType=1 驱动 UI。
	class DailyGiftClaimPacket : public OutPacket
	{
	public:
		explicit DailyGiftClaimPacket(int16_t day)
			: OutPacket(OutPacket::Opcode::DAILY_GIFT_ACTION)
		{
			write_byte(1);
			write_byte(static_cast<uint8_t>(day));
		}
	};
}
