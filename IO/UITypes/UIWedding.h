//////////////////////////////////////////////////////////////////////////////////
//	Wedding invitation / progress shell
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIWedding : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::WEDDING;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIWedding();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_invitation(bool cathedral, const std::string& tip);
		void set_progress(uint8_t step, int32_t groom, int32_t bride);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t { BtOK };

		bool has_assets = false;
		Texture invite;
		Text tip_label;
		Text progress_label;
	};
}
