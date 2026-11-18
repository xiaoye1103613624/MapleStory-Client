#include "UICooldownList.h"

#include "../../Constants.h"
#include "../../Util/Misc.h"

#include <algorithm>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	CooldownIcon::CooldownIcon(int32_t sid, int32_t duration_ms)
		: cover(IconCover::COOLDOWN, duration_ms),
		  secs_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, ""),
		  skill_id(sid), remaining_ms(duration_ms)
	{
#ifdef USE_NX
		const std::string strid = string_format::extend_id(skill_id, 7);
		nl::node src = nl::nx::Skill[string_format::skill_namespace(skill_id) + ".img"]["skill"][strid];
		icon = src["icon"];
#endif
		const int32_t secs = (remaining_ms + 999) / 1000;
		secs_label.change_text(std::to_string(std::max(secs, 1)));
	}

	void CooldownIcon::draw(Point<int16_t> position, float alpha) const
	{
		if (icon)
			icon.draw(position);
		cover.draw(position + Point<int16_t>(1, -31), alpha);
		secs_label.draw(position + Point<int16_t>(16, 2));
	}

	bool CooldownIcon::update()
	{
		cover.update();
		remaining_ms -= Constants::TIMESTEP;
		const int32_t secs = (remaining_ms + 999) / 1000;
		secs_label.change_text(std::to_string(std::max(secs, 0)));
		return remaining_ms < Constants::TIMESTEP;
	}

	UICooldownList::UICooldownList()
	{
		active = true;
		update_screen(
			Constants::Constants::get().get_viewwidth(),
			Constants::Constants::get().get_viewheight());
	}

	void UICooldownList::draw(float alpha) const
	{
		Point<int16_t> icpos = position;
		for (const auto& icon : icons)
		{
			icon.second.draw(icpos, alpha);
			icpos.shift_x(-32);
		}
	}

	void UICooldownList::update()
	{
		for (auto iter = icons.begin(); iter != icons.end();)
		{
			if (iter->second.update())
				iter = icons.erase(iter);
			else
				++iter;
		}
	}

	void UICooldownList::update_screen(int16_t new_width, int16_t)
	{
		// Buff 条约 y=55；CD 行在其下（MinMS y=78）。
		position = Point<int16_t>(static_cast<int16_t>(new_width - 35), 90);
		dimension = Point<int16_t>(32, 32);
	}

	void UICooldownList::add_cooldown(int32_t skill_id, int32_t duration_ms)
	{
		if (duration_ms <= 0)
		{
			icons.erase(skill_id);
			return;
		}
		icons.insert_or_assign(skill_id, CooldownIcon(skill_id, duration_ms));
	}

	void UICooldownList::remove_cooldown(int32_t skill_id)
	{
		icons.erase(skill_id);
	}

	UIElement::Type UICooldownList::get_type() const
	{
		return TYPE;
	}
}
