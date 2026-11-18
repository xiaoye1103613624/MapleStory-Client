//////////////////////////////////////////////////////////////////////////////////
//	Cooldown icon strip (Angel skill CD row / MinMS UICooldownList).
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../Components/IconCover.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <unordered_map>

namespace ms
{
	class CooldownIcon
	{
	public:
		CooldownIcon(int32_t skill_id, int32_t duration_ms);

		void draw(Point<int16_t> position, float alpha) const;
		bool update();
		int32_t remaining() const { return remaining_ms; }

	private:
		Texture icon;
		IconCover cover;
		Text secs_label;
		int32_t skill_id;
		int32_t remaining_ms;
	};

	// 右上 buff 条下方第二行；不抢焦点。
	class UICooldownList : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::COOLDOWNLIST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UICooldownList();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		UIElement::Type get_type() const override;

		void add_cooldown(int32_t skill_id, int32_t duration_ms);
		void remove_cooldown(int32_t skill_id);

	private:
		std::unordered_map<int32_t, CooldownIcon> icons;
	};
}
