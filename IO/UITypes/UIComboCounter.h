//////////////////////////////////////////////////////////////////////////////////
//	Aran SHOW_COMBO HUD
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	class UIComboCounter : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::COMBOCOUNTER;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIComboCounter();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		UIElement::Type get_type() const override;

		void set_combo(int32_t count);

	private:
		int32_t combo = 0;
		int32_t hide_ms = 0;
		Text label;
		Texture backgrnd;
	};
}
