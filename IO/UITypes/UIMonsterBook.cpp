////////////////////////////////////////////////////////////////////////////////
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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#include "UIMonsterBook.h"

#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MonsterBookPackets.h"
#include "../../Util/UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	static std::string monster_name(int32_t id)
	{
#ifdef USE_NX
		nl::node n = nl::nx::String["Mob.img"][std::to_string(id)]["name"];
		if (n)
			return n.get_string();
#endif
		return "Mob #" + std::to_string(id);
	}

	UIMonsterBook::UIMonsterBook() :
		title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, "Monster Book"),
		cover_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, ""),
		total_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "")
	{
		dimension = Point<int16_t>(400, 480);

#ifdef USE_NX
		nl::node panel = nl::nx::UI["UIWindow.img"]["MonsterBook"]["backgrnd"];
		if (panel)
		{
			background = panel;
			has_background = true;
			dimension = background.get_dimensions();
		}

		nl::node close = nl::nx::UI["Basic.img"]["BtClose3"];
		if (!close)
			close = nl::nx::UI["Basic.img"]["BtClose2"];

		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(dimension.x() - 21, 6));
#endif

		list_origin = Point<int16_t>(20, 90);
		load_cards();
	}

	void UIMonsterBook::load_cards()
	{
		cards.clear();
		card_labels.clear();

		const MonsterBook& mb = Stage::get().get_player().get_monsterbook();
		for (const auto& kv : mb.get_cards())
			cards.push_back({ kv.first, kv.second });

		cover_label.change_text("Cover: " + (mb.get_cover() > 0 ? monster_name(mb.get_cover()) : "-"));
		total_label.change_text("Cards: " + std::to_string(cards.size()));

		for (size_t i = 0; i < cards.size(); i++)
		{
			std::string txt = "Lv." + std::to_string(static_cast<int>(cards[i].level)) + "  " + monster_name(cards[i].id);
			card_labels.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, txt);
		}
	}

	void UIMonsterBook::reload()
	{
		load_cards();
	}

	void UIMonsterBook::draw(float inter) const
	{
		if (has_background)
			background.draw(position, inter);

		title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
		cover_label.draw(position + Point<int16_t>(20, 50));
		total_label.draw(position + Point<int16_t>(20, 70));

		for (size_t i = 0; i < card_labels.size(); i++)
			card_labels[i].draw(position + list_origin + Point<int16_t>(0, static_cast<int16_t>(i) * row_h));

		for (const auto& b : buttons)
			if (b.second)
				b.second->draw(position);
	}

	Button::State UIMonsterBook::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UIMonsterBook::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			Point<int16_t> local = cursorpos - position - list_origin;
			if (local.x() >= 0 && local.x() < dimension.x() - 40 && local.y() >= 0)
			{
				int16_t row = local.y() / row_h;
				if (row >= 0 && row < static_cast<int16_t>(cards.size()))
				{
					MonsterBookSetCoverPacket(cards[row].id).dispatch();
					cover_label.change_text("Cover: " + monster_name(cards[row].id));
				}
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UIMonsterBook::get_type() const
	{
		return TYPE;
	}
}
