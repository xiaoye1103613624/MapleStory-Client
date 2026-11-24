////////////////////////////////////////////////////////////////////////////////
//	ENHANCE_ACTION(0x371D) — 192 UI_004 强化族通用请求包
//	协议契约见 docs/features/ui004-enhancement-ui-port-2026-11/CONTRACT.md
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// byte action + short cashSlot + int cashId + short equipSlot + short matSlot + byte mode
	class EnhanceActionPacket : public OutPacket
	{
	public:
		EnhanceActionPacket(uint8_t action, int16_t cash_slot, int32_t cash_id,
			int16_t equip_slot, int16_t mat_slot, uint8_t mode)
			: OutPacket(OutPacket::Opcode::ENHANCE_ACTION)
		{
			write_byte(action);
			write_short(cash_slot);
			write_int(cash_id);
			write_short(equip_slot);
			write_short(mat_slot);
			write_byte(mode);
		}
	};
}
