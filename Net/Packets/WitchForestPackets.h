//////////////////////////////////////////////////////////////////////////////////
// WitchForestPackets / BreakthroughStonePackets — 192 魔女森林 / 突破石出站包
// 对接 s9 RecvOpcode：
//   WITCH_FOREST_ACTION       0x3719 : byte action(0=OPEN,1=DRAW) + [byte drawType]
//   BREAKTHROUGH_STONE_ACTION 0x371B : byte action(0=OPEN,1=USE) + [int itemId]
// 与服务端 handler 的包结构严格一一对应，改动须同步 s9 PacketCreator/handler。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 魔女森林：请求状态 / 抽奖。drawType 0=50x1,1=50x10,2=100x1,3=100x10
	class WitchForestOpenPacket : public OutPacket
	{
	public:
		WitchForestOpenPacket() : OutPacket(OutPacket::Opcode::WITCH_FOREST_ACTION)
		{
			write_byte(0);
		}
	};

	class WitchForestDrawPacket : public OutPacket
	{
	public:
		explicit WitchForestDrawPacket(int8_t drawtype)
			: OutPacket(OutPacket::Opcode::WITCH_FOREST_ACTION)
		{
			write_byte(1);
			write_byte(static_cast<uint8_t>(drawtype));
		}
	};

	// 突破石：请求状态 / 使用突破石提升破功值
	class BreakthroughStoneOpenPacket : public OutPacket
	{
	public:
		BreakthroughStoneOpenPacket() : OutPacket(OutPacket::Opcode::BREAKTHROUGH_STONE_ACTION)
		{
			write_byte(0);
		}
	};

	class BreakthroughStoneUsePacket : public OutPacket
	{
	public:
		explicit BreakthroughStoneUsePacket(int32_t itemid)
			: OutPacket(OutPacket::Opcode::BREAKTHROUGH_STONE_ACTION)
		{
			write_byte(1);
			write_int(itemid);
		}
	};
}
