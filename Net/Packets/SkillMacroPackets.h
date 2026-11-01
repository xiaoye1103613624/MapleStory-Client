////////////////////////////////////////////////////////////////////////////////
//	SKILL_MACRO(110) — save macros (s9 SkillMacroHandler)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"
#include "../../Character/SkillMacros.h"

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
}
