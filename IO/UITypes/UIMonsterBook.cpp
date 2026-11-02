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
#include "UISoftKey.h"
#include "../UI.h"
#include "UIMiscShells.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MonsterBookPackets.h"
#include "../../Util/Locale.h"
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
		return Locale::format("monsterbook.stub_mob", std::to_string(id));
	}

	UIMonsterBook::UIMonsterBook() :
		title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, Locale::text("monsterbook.title")),
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

		nl::node close = UiAssetResolve::close_btn();

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

		cover_label.change_text(Locale::format("monsterbook.label_cover", mb.get_cover() > 0 ? monster_name(mb.get_cover()) : std::string("-")));
		total_label.change_text(Locale::format("monsterbook.label_cards", std::to_string(cards.size())));

		for (size_t i = 0; i < cards.size(); i++)
		{
			std::string txt = Locale::format("monsterbook.label_level", std::to_string(static_cast<int>(cards[i].level))) + "  " + monster_name(cards[i].id);
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
					selected_card_id = cards[row].id;
					MonsterBookSetCoverPacket(cards[row].id).dispatch();
					cover_label.change_text("Cover: " + monster_name(cards[row].id));
				}
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIMonsterBook::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c') {
			if (selected_card_id > 0) {
				MonsterBookSetCoverPacket(selected_card_id).dispatch();
				cover_label.change_text("Cover: " + monster_name(selected_card_id));
				LOG(LOG_DEBUG, "UIMonsterBook: cover selected=" << selected_card_id);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int32_t id = std::stoi(s);
						if (id > 0) {
							selected_card_id = static_cast<int16_t>(id);
							MonsterBookSetCoverPacket(selected_card_id).dispatch();
							cover_label.change_text("Cover id=" + s);
							LOG(LOG_DEBUG, "UIMonsterBook: SoftKey cover=" << id);
						}
					} catch (...) {}
				}, []() {}, "Cover card id");
			}
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			reload();
			LOG(LOG_DEBUG, "UIMonsterBook: reload");
		}
		else if (keycode == 'S' || keycode == 's')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				if (s.empty()) return;
				for (const auto& card : cards)
				{
					std::string nm = monster_name(card.id);
					if (std::to_string(card.id) == s ||
						(!nm.empty() && nm.find(s) != std::string::npos))
					{
						cover_label.change_text(nm.empty()
							? ("#" + std::to_string(card.id))
							: nm);
						LOG(LOG_DEBUG, "UIMonsterBook: SoftKey find id=" << card.id);
						return;
					}
				}
				cover_label.change_text("Not found");
			}, []() {}, "card id|name");
		}
	}

	UIElement::Type UIMonsterBook::get_type() const
	{
		return TYPE;
	}
}
