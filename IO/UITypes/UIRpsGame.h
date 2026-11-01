//////////////////////////////////////////////////////////////////////////////////
//	RPS Game UI
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIRpsGame : public UIDragElement<PosNOTICE>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::RPSGAME;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIRpsGame();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void set_status(const std::string& msg);
		void set_round_result(uint8_t selection, uint8_t answer);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t {
			BtRock, BtPaper, BtScissor, BtStart, BtContinue, BtRetry, BtExit
		};

		bool has_assets = false;
		Texture backgrnd;
		Text status;
	};
}
