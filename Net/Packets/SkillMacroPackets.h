////////////////////////////////////////////////////////////////////////////////
//	SKILL_MACRO(110) — save macros (s9 SkillMacroHandler)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"
#include "../../Character/SkillMacros.h"

#include <array>

namespace ms
{
	class ChangeSkillMacroPacket : public OutPacket
	{
	public:
		ChangeSkillMacroPacket() : OutPacket(OutPacket::Opcode::SKILL_MACRO)
		{
			const auto& macros = SkillMacros::get();
			uint8_t n = static_cast<uint8_t>(macros.size());
			if (n > SkillMacros::MAX_SLOTS)
				n = static_cast<uint8_t>(SkillMacros::MAX_SLOTS);
			write_byte(n);
			for (uint8_t i = 0; i < n; i++) {
				const auto& s = macros.get_slot(i);
				write_string(s.name);
				write_byte(s.shout);
				write_int(s.skill1);
				write_int(s.skill2);
				write_int(s.skill3);
			}
		}
	};

	// Opcode: CHANGE_QUICKSLOT(183 / 0xB7) — s9 QuickslotKeyMappedModifiedHandler
	// body：8× int（服务端逐槽 (byte)readInt 截断存 byte[8]；v83 触发键全部
	// <0x100，无撞车）。语义 = 快捷栏 8 个**触发键**（槽内技能/道具绑定走
	// KEYMAP(135) 既有通道）。下行对应 QUICKSLOT_INIT(0x9F)，存储层见
	// Keyboard::assign_quickslot（Wave 1-1.2）。改键 UI 接线待 v83 KeyConfig
	// 快捷页实体化（Wave 6），本包类先行登记防重复造。
	class ChangeQuickslotPacket : public OutPacket
	{
	public:
		explicit ChangeQuickslotPacket(const std::array<uint32_t, 8>& trigger_keys)
			: OutPacket(OutPacket::Opcode::CHANGE_QUICKSLOT)
		{
			for (uint32_t key : trigger_keys)
				write_int(static_cast<int32_t>(key));
		}
	};
}
