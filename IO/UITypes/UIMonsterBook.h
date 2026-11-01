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
#pragma once

#include "../UIElement.h"

#include "../../Character/MonsterBook.h"
#include "../../Graphics/Text.h"

#include <map>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 怪物图鉴窗口：展示已收集卡片（封面 + 卡片列表），点击卡片可设封面。
	// 数据来自 Stage::get_player().get_monsterbook()（登录时已随角色包下发）。
	class UIMonsterBook : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MONSTERBOOK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMonsterBook();

		void draw(float inter) const override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		// 封面/卡片数据变化后由 handler 调用刷新
		void reload();
		int16_t get_selected_card_id() const { return selected_card_id; }

	private:
		struct CardEntry
		{
			int16_t id;
			int8_t level;
		};

		void load_cards();

		enum Buttons : uint16_t
		{
			BtClose
		};

		Sprite background;
		bool has_background = false;
		Text title;
		Text cover_label;
		Text total_label;
		std::vector<CardEntry> cards;
		std::vector<Text> card_labels;
		Point<int16_t> list_origin;
		int16_t row_h = 18;
		int16_t selected_card_id = 0;
	};
}
