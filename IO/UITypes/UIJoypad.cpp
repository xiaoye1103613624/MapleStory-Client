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
#include "UIJoypad.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Util/Misc.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// TODO: Add combo boxes nl::nx::ui["Basic.img"]["ComboBox"] / ["ComboBox5"];
	UIJoypad::UIJoypad() : UIDragElement<PosJOYPAD>()
	{
		// ComboBox 未接；SoftKey A 切换 classic/alternative 背景
		alternative_settings = false;

		nl::node JoyPad = nl::nx::UI["UIWindow.img"]["JoyPad"];
		nl::node Basic = nl::nx::UI["Basic.img"];

		backgrnd[true] = JoyPad["backgrnd_alternative"];
		backgrnd[false] = JoyPad["backgrnd_classic"];

		buttons[Buttons::DEFAULT] = std::make_unique<MapleButton>(JoyPad["BtDefault"]);
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(Basic["BtCancel4"], Point<int16_t>(124, 303));
		buttons[Buttons::OK] = std::make_unique<MapleButton>(Basic["BtOK4"], Point<int16_t>(82, 303));

		for (Text& text : key_text)
			text = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "None");

		dimension = backgrnd[alternative_settings].get_dimensions();
	}

	void UIJoypad::draw(float inter) const
	{
		backgrnd[alternative_settings].draw(position);

		int16_t x = 79;
		int16_t y = 24;
		int16_t y_adj = 18;

		for (size_t i = 0; i < Setting::SETTING_NUM; i++)
		{
			if (i == 0)
				key_text[i].draw(position + Point<int16_t>(x, y));
			else if (i > 0 && i < 4)
				key_text[i].draw(position + Point<int16_t>(x - 16, y + 44 + y_adj * (i - 1)));
			else
				key_text[i].draw(position + Point<int16_t>(x - 16, y + 123 + y_adj * (i - 4)));
		}

		UIElement::draw(inter);
	}

	void UIJoypad::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				cancel();
			else if (keycode == KeyAction::Id::RETURN)
				save();
			else if (keycode == 'A' || keycode == 'a')
			{
				alternative_settings = !alternative_settings;
				if (backgrnd[alternative_settings])
					dimension = backgrnd[alternative_settings].get_dimensions();
				LOG(LOG_DEBUG, "UIJoypad: layout="
					<< (alternative_settings ? "alternative" : "classic"));
			}
			else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
				|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n')
			{
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) return;
					std::string t = tip;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t == "y" || t == "yes" || t == "ok" || t == "save" || t == "1")
						save();
					else if (t == "n" || t == "no" || t == "0" || t == "cancel")
						cancel();
					else if (t == "alt" || t == "alternative") {
						alternative_settings = true;
						if (backgrnd[true]) dimension = backgrnd[true].get_dimensions();
					} else if (t == "classic" || t == "c") {
						alternative_settings = false;
						if (backgrnd[false]) dimension = backgrnd[false].get_dimensions();
					}
				}, []() {}, "save|cancel|alt|classic");
			}
		}
	}

	UIElement::Type UIJoypad::get_type() const
	{
		return TYPE;
	}

	Button::State UIJoypad::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::DEFAULT:
				for (Text& text : key_text)
					text.change_text("None");
				LOG(LOG_DEBUG, "UIJoypad: DEFAULT reset bindings label");
				break;
			case Buttons::CANCEL:
				cancel();
				break;
			case Buttons::OK:
				save();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	void UIJoypad::cancel()
	{
		deactivate();
	}

	void UIJoypad::save()
	{
		cancel();
	}
}