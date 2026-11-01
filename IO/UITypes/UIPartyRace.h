//////////////////////////////////////////////////////////////////////////////////
//	PartyRace HUD shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIPartyRace : public UIDragElement<PosPARTYRACE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::PARTYRACE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIPartyRace();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_stage(int32_t stage);
		void set_score(int32_t red, int32_t blue);
		void show_result(bool win);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtClose };

		bool has_assets = false;
		bool show_result_panel = false;
		bool last_win = false;
		int32_t stage = 0;
		int32_t red = 0;
		int32_t blue = 0;
		Texture stage_bg;
		Texture result_bg;
		Texture win_tex;
		Texture lose_tex;
		Text stage_label;
		Text score_label;
	};
}
