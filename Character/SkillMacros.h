//////////////////////////////////////////////////////////////////////////////////
//	Skill macro slots (SKILL_MACROS / MACRO_SYS_DATA_INIT)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <string>

namespace ms
{
	struct SkillMacroSlot
	{
		std::string name;
		uint8_t shout = 0;
		int32_t skill1 = 0;
		int32_t skill2 = 0;
		int32_t skill3 = 0;
	};

	class SkillMacros
	{
	public:
		static constexpr size_t MAX_SLOTS = 5;

		static SkillMacros& get()
		{
			static SkillMacros instance;
			return instance;
		}

		void clear()
		{
			for (auto& s : slots)
				s = {};
			count = 0;
		}

		void set_slot(size_t i, SkillMacroSlot slot)
		{
			if (i >= MAX_SLOTS)
				return;
			slots[i] = std::move(slot);
			if (i + 1 > count)
				count = i + 1;
		}

		// Append skill id into first empty skill1/2/3 across slots (creates slot 0 if empty).
		bool assign_skill(int32_t skill_id)
		{
			if (skill_id <= 0)
				return false;
			if (count == 0) {
				SkillMacroSlot s;
				s.name = "Macro1";
				s.skill1 = skill_id;
				set_slot(0, std::move(s));
				return true;
			}
			for (size_t i = 0; i < count && i < MAX_SLOTS; i++) {
				auto& s = slots[i];
				if (s.skill1 == 0) { s.skill1 = skill_id; return true; }
				if (s.skill2 == 0) { s.skill2 = skill_id; return true; }
				if (s.skill3 == 0) { s.skill3 = skill_id; return true; }
			}
			if (count < MAX_SLOTS) {
				SkillMacroSlot s;
				s.name = "Macro" + std::to_string(count + 1);
				s.skill1 = skill_id;
				set_slot(count, std::move(s));
				return true;
			}
			return false;
		}

		const SkillMacroSlot& get_slot(size_t i) const
		{
			static SkillMacroSlot empty;
			return i < MAX_SLOTS ? slots[i] : empty;
		}

		size_t size() const { return count; }

	private:
		SkillMacros() = default;
		std::array<SkillMacroSlot, MAX_SLOTS> slots{};
		size_t count = 0;
	};
}
