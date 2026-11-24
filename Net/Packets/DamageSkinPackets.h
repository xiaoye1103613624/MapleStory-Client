//////////////////////////////////////////////////////////////////////////////////
// DamageSkinPackets — 伤害皮肤 picker 出站包（私有 C→S 0x3746/0x3747）
//
//   DAMAGE_SKIN_APPLY    (0x3746) : int skinId —— 应用已拥有皮肤
//   DAMAGE_SKIN_PURCHASE (0x3747) : int skinId —— 游戏币购买皮肤
//
// 与 s9 RecvOpcode.DAMAGE_SKIN_APPLY/PURCHASE 逐字节对齐；结果由 S→C 0x3748 驱动。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class DamageSkinApplyPacket : public OutPacket
	{
	public:
		explicit DamageSkinApplyPacket(int32_t skinId)
			: OutPacket(OutPacket::Opcode::DAMAGE_SKIN_APPLY)
		{
			write_int(skinId);
		}
	};

	class DamageSkinPurchasePacket : public OutPacket
	{
	public:
		explicit DamageSkinPurchasePacket(int32_t skinId)
			: OutPacket(OutPacket::Opcode::DAMAGE_SKIN_PURCHASE)
		{
			write_int(skinId);
		}
	};
}
