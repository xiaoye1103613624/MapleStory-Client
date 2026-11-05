////////////////////////////////////////////////////////////////////////////////
//	W3 A 类道具 c2s — SCRIPTED / CATCH / DEATH / CANCEL_EFFECT / USE_REMOTE
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// Opcode: SCRIPTED_ITEM(78) — body 同 USE_ITEM
	class ScriptedItemPacket : public OutPacket
	{
	public:
		ScriptedItemPacket(int16_t slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::SCRIPTED_ITEM)
		{
			write_time();
			write_short(slot);
			write_int(itemid);
		}
	};

	// Opcode: USE_CATCH_ITEM(81)
	// UseCatchItemHandler: int ts, short, int itemId, int monsterOid
	class UseCatchItemPacket : public OutPacket
	{
	public:
		UseCatchItemPacket(int16_t slot, int32_t itemid, int32_t monster_oid)
			: OutPacket(OutPacket::Opcode::USE_CATCH_ITEM)
		{
			write_time();
			write_short(slot);
			write_int(itemid);
			write_int(monster_oid);
		}
	};

	// Opcode: USE_DEATHITEM(53) — int itemId
	class UseDeathItemPacket : public OutPacket
	{
	public:
		explicit UseDeathItemPacket(int32_t itemid)
			: OutPacket(OutPacket::Opcode::USE_DEATHITEM)
		{
			write_int(itemid);
		}
	};

	// Opcode: CANCEL_ITEM_EFFECT(73) — int itemId（服务端再取负）
	class CancelItemEffectPacket : public OutPacket
	{
	public:
		explicit CancelItemEffectPacket(int32_t itemid)
			: OutPacket(OutPacket::Opcode::CANCEL_ITEM_EFFECT)
		{
			write_int(itemid);
		}
	};

	// Opcode: USE_REMOTE(116) — int ticket(5451000), int gacha 0..11
	class UseRemoteGachaponPacket : public OutPacket
	{
	public:
		UseRemoteGachaponPacket(int32_t ticket_itemid, int32_t gacha_mode)
			: OutPacket(OutPacket::Opcode::USE_REMOTE)
		{
			write_int(ticket_itemid);
			write_int(gacha_mode);
		}
	};
}
