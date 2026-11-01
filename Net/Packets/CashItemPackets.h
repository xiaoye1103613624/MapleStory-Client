////////////////////////////////////////////////////////////////////////////////
//	USE_CASH_ITEM(79) — ViciousHammer / VegaSpell payloads (s9 UseCashItemHandler)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// itemType 557：unused(int) + equipSlot(int) + unused(int)
	class UseViciousHammerPacket : public OutPacket
	{
	public:
		UseViciousHammerPacket(int16_t cash_slot, int32_t cash_itemid, int16_t equip_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_int(0);
			write_int(equip_slot);
			write_int(0);
		}
	};

	// itemType 561：1 + eSlot + 2 + uSlot
	class UseVegaSpellPacket : public OutPacket
	{
	public:
		UseVegaSpellPacket(int16_t cash_slot, int32_t cash_itemid, int16_t equip_slot, int16_t use_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_int(1);
			write_int(equip_slot);
			write_int(2);
			write_int(use_slot);
		}
	};

	// 559 保护卷 / 552 宿命剪刀：short cashSlot + int cashId + int equipSlot（浅对齐）
	class UseItemProtectorPacket : public OutPacket
	{
	public:
		UseItemProtectorPacket(int16_t cash_slot, int32_t cash_itemid, int16_t equip_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_int(equip_slot);
		}
	};

	class UseKarmaScissorsPacket : public OutPacket
	{
	public:
		UseKarmaScissorsPacket(int16_t cash_slot, int32_t cash_itemid, int16_t equip_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_int(equip_slot);
		}
	};

	// 504* 缩地石：short slot + int id + byte vip(0=地图 / 1=角色名) + int mapId | string name
	class UseTeleportRockPacket : public OutPacket
	{
	public:
		UseTeleportRockPacket(int16_t cash_slot, int32_t cash_itemid, int32_t map_id)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_byte(0);
			write_int(map_id);
		}
	};

	class UseTeleportRockToPlayerPacket : public OutPacket
	{
	public:
		UseTeleportRockToPlayerPacket(int16_t cash_slot, int32_t cash_itemid, const std::string& name)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_byte(1);
			write_string(name);
		}
	};

	// 5071xxx 普通喇叭：slot + id + string；5072xxx 超级喇叭再加 whisper byte
	class UseMegaphonePacket : public OutPacket
	{
	public:
		UseMegaphonePacket(int16_t cash_slot, int32_t cash_itemid, const std::string& msg, bool whisper = false)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(msg);
			if ((cash_itemid / 1000) % 10 == 2)
				write_byte(whisper ? 1 : 0);
		}
	};

	// 5076xxx 道具喇叭：msg + whisper + hasItem + [invType + slot]
	class UseItemMegaphonePacket : public OutPacket
	{
	public:
		UseItemMegaphonePacket(int16_t cash_slot, int32_t cash_itemid, const std::string& msg,
			bool whisper, bool has_item, int8_t inv_type, int16_t inv_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(msg);
			write_byte(whisper ? 1 : 0);
			write_byte(has_item ? 1 : 0);
			if (has_item)
			{
				write_int(inv_type);
				write_int(inv_slot);
			}
		}
	};

	// s9 CouponCodeHandler：skip(2) + string code
	class CouponCodePacket : public OutPacket
	{
	public:
		explicit CouponCodePacket(const std::string& code)
			: OutPacket(OutPacket::Opcode::COUPON_CODE)
		{
			write_short(0);
			write_string(code);
		}
	};

	// s9 UseMapleLifeHandler：string proposedName
	class UseMapleLifePacket : public OutPacket
	{
	public:
		explicit UseMapleLifePacket(const std::string& name)
			: OutPacket(OutPacket::Opcode::USE_MAPLELIFE)
		{
			write_string(name);
		}
	};

	// s9 TrockAddMapHandler：byte type(1=add) + byte vip
	class TrockAddMapPacket : public OutPacket
	{
	public:
		explicit TrockAddMapPacket(bool vip)
			: OutPacket(OutPacket::Opcode::TROCK_ADD_MAP)
		{
			write_byte(1);
			write_byte(vip ? 1 : 0);
		}
	};

	// s9/HeavenMS ReportHandler：type + victim + reason + description〔+ chatlog if type==1〕
	class ReportPlayerPacket : public OutPacket
	{
	public:
		ReportPlayerPacket(const std::string& victim, const std::string& description)
			: OutPacket(OutPacket::Opcode::REPORT)
		{
			write_byte(0); // illegal program / 通用
			write_string(victim);
			write_byte(0); // reason
			write_string(description);
		}
	};

	// 5060002 孵化器：slot + id + int invType + int eggSlot
	// 仅 slot+itemid 的简易现金使用（扭蛋票等；专用 handler 字段 TBD 时浅出站）
	class UseCashItemBasicPacket : public OutPacket
	{
	public:
		UseCashItemBasicPacket(int16_t cash_slot, int32_t cash_itemid)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
		}
	};

	class UseIncubatorPacket : public OutPacket
	{
	public:
		UseIncubatorPacket(int16_t cash_slot, int32_t cash_itemid, int8_t egg_inv, int16_t egg_slot)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_int(egg_inv);
			write_int(egg_slot);
		}
	};

	// 517* 宠物取名：slot + id + string name
	class UsePetNameTagPacket : public OutPacket
	{
	public:
		UsePetNameTagPacket(int16_t cash_slot, int32_t cash_itemid, const std::string& name)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(name);
		}
	};

	// 537* 黑板 / 508* 风筝：slot + id + string
	class UseChalkboardPacket : public OutPacket
	{
	public:
		UseChalkboardPacket(int16_t cash_slot, int32_t cash_itemid, const std::string& text)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(text);
		}
	};

	class UseKitePacket : public OutPacket
	{
	public:
		UseKitePacket(int16_t cash_slot, int32_t cash_itemid, const std::string& text)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(text);
		}
	};

	// 512* 场景特效留言
	class UseMapEffectPacket : public OutPacket
	{
	public:
		UseMapEffectPacket(int16_t cash_slot, int32_t cash_itemid, const std::string& text)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(text);
		}
	};

	// 539* 情景喇叭：4 行 string + whisper byte
	class UseAvatarMegaphonePacket : public OutPacket
	{
	public:
		UseAvatarMegaphonePacket(int16_t cash_slot, int32_t cash_itemid, const std::string& line0, bool whisper = false)
			: OutPacket(OutPacket::Opcode::USE_CASH_ITEM)
		{
			write_short(cash_slot);
			write_int(cash_itemid);
			write_string(line0);
			write_string("");
			write_string("");
			write_string("");
			write_byte(whisper ? 1 : 0);
		}
	};
}
