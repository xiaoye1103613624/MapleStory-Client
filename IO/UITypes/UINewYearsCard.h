//////////////////////////////////////////////////////////////////////////////////
//	NewYearsCard UI shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <string>

namespace ms
{
	class UINewYearsCard : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::NEWYEARSCARD;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UINewYearsCard();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_tip(const std::string& tip);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtOk, BtCancel, BtClose };
		void begin_compose();
		bool find_card(int16_t& slot, int32_t& itemid) const;

		bool has_assets = false;
		Texture backgrnd;
		Text tip_label;
		std::string pending_receiver;
	};
}
