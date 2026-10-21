//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"

namespace ms
{
	/// Classic StatusBar「帮助」窗：轻量说明，避免误开 Quest 导致卡死。
	class UIHelp : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::NOTICE;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIHelp();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		void update_screen(int16_t new_width, int16_t new_height) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BT_OK
		};

		Text title;
		Text body;
	};
}
