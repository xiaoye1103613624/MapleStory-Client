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
#include "UIGender.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../KeyAction.h"

#include "../Components/MapleButton.h"
#include "../Components/MapleComboBox.h"
#include "../UITypes/UILoginWait.h"

#include "../../Net/Packets/LoginPackets.h"
#include "../../Util/Misc.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIGender::UIGender(std::function<void()> oh) : UIElement(Point<int16_t>(0, 15), Point<int16_t>(0, 0)), okhandler(oh)
	{
		CUR_TIMESTEP = 0;

		nl::node Gender = nl::nx::UI["Login.img"]["Gender"];

		for (size_t i = 0; i < 3; i++)
			gender_sprites[i] = Gender["scroll"]["0"][i];

		sprites.emplace_back(Gender["text"]["0"], Point<int16_t>(601, 326));

		std::vector<std::string> options;
		options.push_back("Male");
		options.push_back("Female");

		uint16_t default_option = 0;

		buttons[Buttons::NO] = std::make_unique<MapleButton>(Gender["BtNo"], Point<int16_t>(650, 349));
		buttons[Buttons::YES] = std::make_unique<MapleButton>(Gender["BtYes"], Point<int16_t>(578, 349));
		buttons[Buttons::SELECT] = std::make_unique<MapleComboBox>(MapleComboBox::Type::DEFAULT, options, default_option, position, Point<int16_t>(510, 283), 65);

		dimension = Texture(gender_sprites[2]).get_dimensions();
	}

	void UIGender::draw(float inter) const
	{
		Point<int16_t> gender_pos = Point<int16_t>(355, 185);

		if (CUR_TIMESTEP == 0)
		{
			gender_sprites[0].draw(position + gender_pos);
		}
		else if (CUR_TIMESTEP == Constants::TIMESTEP * 3)
		{
			gender_sprites[1].draw(position + gender_pos);
		}
		else if (CUR_TIMESTEP >= Constants::TIMESTEP * 6)
		{
			gender_sprites[2].draw(position + gender_pos);

			UIElement::draw(inter);
		}
	}

	void UIGender::update()
	{
		UIElement::update();

		if (CUR_TIMESTEP <= Constants::TIMESTEP * 6)
			CUR_TIMESTEP += Constants::TIMESTEP;
	}

	Cursor::State UIGender::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		auto& combobox = buttons[Buttons::SELECT];

		if (combobox->is_pressed() && combobox->in_combobox(cursorpos))
			if (Cursor::State new_state = combobox->send_cursor(clicked, cursorpos))
				return new_state;

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIGender::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { button_pressed(Buttons::NO); return; }
		if (keycode == KeyAction::Id::RETURN) { button_pressed(Buttons::YES); return; }
		if (keycode == 'M' || keycode == 'm' || keycode == '0')
		{
			UI::get().emplace<UILoginWait>();
			GenderPacket(false).dispatch();
			LOG(LOG_DEBUG, "UIGender: male");
			return;
		}
		if (keycode == 'F' || keycode == 'f' || keycode == '1')
		{
			UI::get().emplace<UILoginWait>();
			GenderPacket(true).dispatch();
			LOG(LOG_DEBUG, "UIGender: female");
			return;
		}
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				bool female = false;
				if (t == "f" || t == "female" || t == "1" || t == "女")
					female = true;
				else if (t == "m" || t == "male" || t == "0" || t == "男")
					female = false;
				else
					return;
				UI::get().emplace<UILoginWait>();
				GenderPacket(female).dispatch();
				LOG(LOG_DEBUG, "UIGender: SoftKey female=" << female);
			}, []() {}, "m|f");
		}
	}

	UIElement::Type UIGender::get_type() const
	{
		return TYPE;
	}

	Button::State UIGender::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::NO:
			{
				deactivate();
				okhandler();

				return Button::State::NORMAL;
			}
			case Buttons::YES:
			{
				UI::get().emplace<UILoginWait>();

				uint16_t selected_value = buttons[Buttons::SELECT]->get_selected();
				GenderPacket(selected_value).dispatch();

				return Button::State::NORMAL;
			}
			case Buttons::SELECT:
			{
				buttons[Buttons::SELECT]->toggle_pressed();

				return Button::State::NORMAL;
			}
			default:
			{
				return Button::State::DISABLED;
			}
		}
	}
}