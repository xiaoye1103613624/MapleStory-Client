//////////////////////////////////////////////////////////////////////////////////
//	EnergyBar HUD — ENERGY_CHARGE gauge (UIWindow.img/EnergyBar)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"
#include "../../Graphics/Texture.h"

namespace ms
{
	class UIEnergyBar : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ENERGYBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIEnergyBar();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

	private:
		static constexpr int16_t MAX_ENERGY = 10000;
		static constexpr int16_t BAR_INNER_W = 70;

		bool has_assets = false;
		bool visible = false;
		int16_t energy = 0;
		Texture bar_w;
		Texture bar_c;
		Texture bar_e;
		Texture gage;
	};
}
