////////////////////////////////////////////////////////////////////////////////
//	Aran combo outbound — ARAN_COMBO_COUNTER(163 / 0xA3)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 空 payload：服务端递增连击
	// s9 RecvOpcode.ARAN_COMBO_COUNTER(0xA3) → AranComboHandler：每收一包连击 +1，
	// 距上次 >3000ms 先归零；每满 10 层按 Combo Ability 等级上 ARAN_COMBO buff。
	// 非战神（GameConstants.isAran）或没学连击技能时服务端直接忽略本包。
	// 发送点：Combat::attack —— 本次攻击命中几只怪发几包（终结技除外）。
	class AranComboPacket : public OutPacket
	{
	public:
		AranComboPacket() : OutPacket(OutPacket::Opcode::ARAN_COMBO_COUNTER)
		{
		}
	};
}
