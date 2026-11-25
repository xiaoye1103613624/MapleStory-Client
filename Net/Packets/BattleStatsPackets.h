//////////////////////////////////////////////////////////////////////////////////
// BattleStatsPackets — 战斗统计出站包（C→S 0x3BC，对齐 192 RequestOnCalc@0x62ee00）
//
// C→S 0x3BC：3×byte(bOn, bIsNew, bClear)
//   bOn=1/bIsNew=0：请求开始/继续（打开面板）；bOn=1/bIsNew=1：新建（Start 按钮，重置数据）
//   bOn=0：结束统计（End 按钮）；bClear=1：清空运行时
// 服务端回 0x3BC 1B 许可 + 0x3BD 队伍数据（解析见 WaveAShellHandlers.cpp）。
// 与 s9 BattleStatsHandler.java 三态语义逐字节对齐。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 战斗统计控制请求：3×byte。构造即发包。
	class BattleStatsRequestPacket : public OutPacket
	{
	public:
		BattleStatsRequestPacket(bool on, bool isNew, bool clear)
			: OutPacket(OutPacket::Opcode::BATTLE_STATS_REQUEST)
		{
			write_byte(on ? 1 : 0);
			write_byte(isNew ? 1 : 0);
			write_byte(clear ? 1 : 0);
		}
	};
}
