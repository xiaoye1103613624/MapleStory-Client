//////////////////////////////////////////////////////////////////////////////////
//	Field Clock / Timer HUD (CLOCK / STOP_CLOCK packets)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"

#include <string>

namespace ms
{
	class UIClock : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CLOCK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIClock();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		/// type 1 = wall clock text, type 2 = countdown seconds, else free text
		void set_clock(int8_t type, const std::string& text, int32_t seconds);
		void stop();

	private:
		bool visible = false;
		int8_t mode = 0;
		int32_t remain_ms = 0;
		int32_t seconds = 0;
		Text label;
	};
}
