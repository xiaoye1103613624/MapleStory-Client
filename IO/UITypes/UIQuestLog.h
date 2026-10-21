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
#pragma once

#include "../UIDragElement.h"

#include "../Components/Slider.h"
#include "../Components/Textfield.h"

#include "../../Character/QuestLog.h"
#include "../../Graphics/Text.h"

namespace ms
{
	class UIQuestLog : public UIDragElement<PosQUEST>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::QUESTLOG;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIQuestLog(const QuestLog& questLog);

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicking, Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

		void refresh();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void change_tab(uint16_t tabid);
		void rebuild_list();
		void select_row(size_t index);
		Point<int16_t> get_search_pos();
		Point<int16_t> get_search_dim();
		Rectangle<int16_t> list_area() const;

		enum Buttons : uint16_t
		{
			TAB0,
			TAB1,
			TAB2,
			CLOSE,
			SEARCH,
			ALL_LEVEL,
			MY_LOCATION,
			GIVEUP
		};

		const QuestLog& questlog;

		uint16_t tab;
		std::vector<Sprite> notice_sprites;
		Textfield search;
		Text placeholder;
		Slider slider;
		Texture search_area;

		bool flat_layout;
		bool has_search;
		int16_t list_offset;
		int16_t selected_index;
		std::vector<int16_t> listed_ids;
		std::vector<Text> listed_names;
		Text detail_title;
		Text detail_progress;
	};
}
