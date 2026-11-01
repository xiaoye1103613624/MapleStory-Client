//////////////////////////////////////////////////////////////////////////////////
//	Minigame lobby shell (Omok / MatchCard via PLAYER_INTERACTION — wire later)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIMinigame : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MINIGAME;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIMinigame();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_tip(const std::string& tip);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtReady, BtStart, BtExit };

		bool has_assets = false;
		Texture info;
		Text tip_label;
	};
}
