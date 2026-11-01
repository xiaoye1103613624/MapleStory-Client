//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "UITermsOfService.h"

#include "UILoginWait.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/MapleButton.h"

#include "../../MapleStory.h"
#include "../../Net/Packets/LoginPackets.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// Login.img/TOS/0 = 431x426，origin (0,0)。下列偏移相对面板左上（局部坐标）。
		// 烘焙按钮槽约 (268,400)/(342,400)；BtOk/BtCancel 为 75x32，顶边取 388。
		constexpr int16_t TOS_TEXT_X = 36;
		constexpr int16_t TOS_TEXT_Y = 50;
		constexpr int16_t TOS_TEXT_W = 340;
		constexpr int16_t TOS_CLIP_MIN = 50;
		constexpr int16_t TOS_CLIP_LEN = 316;
		constexpr int16_t TOS_SLIDER_X = 384;
		constexpr int16_t TOS_SLIDER_Y0 = 55;
		constexpr int16_t TOS_SLIDER_Y1 = 360;
		constexpr int16_t TOS_BT_OK_X = 268;
		constexpr int16_t TOS_BT_CANCEL_X = 342;
		constexpr int16_t TOS_BT_Y = 388;
	}

	UITermsOfService::UITermsOfService(std::function<void()> oh) : okhandler(oh), offset(0), unit_rows(1)
	{
		nl::node Login = nl::nx::UI["Login.img"];
		nl::node TOS = Login["TOS"];

		// 面板锚在局部 (0,0)；屏幕位置由 center_on_screen 按视口计算，适配多分辨率。
		sprites.emplace_back(TOS);

		buttons[Buttons::OK] = std::make_unique<MapleButton>(
			Login["BtOk"],
			Point<int16_t>(TOS_BT_OK_X, TOS_BT_Y)
		);
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(
			Login["BtCancel"],
			Point<int16_t>(TOS_BT_CANCEL_X, TOS_BT_Y)
		);

		EULA = nl::nx::String["TrialEULA.img"]["EULA"];
		max_rows = EULA.size();

		text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "", TOS_TEXT_W, true, 2);

		slider = Slider(
			Slider::Type::LINE_PUNGA,
			Range<int16_t>(TOS_SLIDER_Y0, TOS_SLIDER_Y1),
			TOS_SLIDER_X,
			unit_rows,
			max_rows,
			[&](bool upwards)
			{
				int16_t shift = upwards ? -1 : 1;
				bool above = offset + shift >= 0;
				bool below = offset + shift <= max_rows - unit_rows;

				if (above && below)
				{
					offset += shift;
					update_accept(offset);
				}
			}
		);

		update_accept(offset);

		dimension = Texture(TOS).get_dimensions();
		center_on_screen();

		LOG(LOG_INFO, "UITermsOfService open dim=" << dimension.x() << "x" << dimension.y()
			<< " pos=(" << position.x() << "," << position.y() << ")");
	}

	void UITermsOfService::draw(float inter) const
	{
		UIElement::draw(inter);

		text.draw(
			position + Point<int16_t>(TOS_TEXT_X, TOS_TEXT_Y),
			Range<int16_t>(TOS_CLIP_MIN, TOS_CLIP_MIN + TOS_CLIP_LEN)
		);
		slider.draw(position);
	}

	void UITermsOfService::update_screen(int16_t, int16_t)
	{
		center_on_screen();
	}

	Cursor::State UITermsOfService::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursoroffset = cursorpos - position;

		if (slider.isenabled())
		{
			Cursor::State state = slider.send_cursor(cursoroffset, clicked);

			if (state != Cursor::State::IDLE)
				return state;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UITermsOfService::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { button_pressed(Buttons::CANCEL); return; }
		if (keycode == KeyAction::Id::RETURN) { button_pressed(Buttons::OK); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "y" || t == "yes" || t == "ok" || t == "1" || t == "agree")
					button_pressed(Buttons::OK);
				else if (t == "n" || t == "no" || t == "0" || t == "cancel")
					button_pressed(Buttons::CANCEL);
			}, []() {}, "ok|cancel");
		}
	}

	UIElement::Type UITermsOfService::get_type() const
	{
		return TYPE;
	}

	Button::State UITermsOfService::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::OK:
				UI::get().emplace<UILoginWait>();

				TOSPacket().dispatch();
				break;
			case Buttons::CANCEL:
				UI::get().quit();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	void UITermsOfService::update_accept(uint16_t offset)
	{
		if (offset == max_rows - unit_rows)
			buttons[Buttons::OK]->set_state(Button::State::NORMAL);
		else
			buttons[Buttons::OK]->set_state(Button::State::DISABLED);

		std::string shownText = "";

		for (nl::node text : EULA)
		{
			std::string name = text.name();
			name = name.substr(4, 6);

			int32_t i = std::stoi(name);

			if (i >= offset && i <= offset + 5)
				shownText += text.get_string();
		}

		text.change_text(shownText);
	}
}
