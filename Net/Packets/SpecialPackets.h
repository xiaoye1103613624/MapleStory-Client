#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 095 自定义中文 UI（签到/兑换/排行/转职/传送）请求包：复用 CASHSHOP_OPERATION
	// 通道，action 0x50..0x54 对应 5 个功能；服务端 CashOperationHandler 对应处理。
	class SpecialRequestPacket : public OutPacket
	{
	public:
		SpecialRequestPacket(uint8_t op) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(static_cast<int8_t>(0x50 + op));
		}
	};
}
