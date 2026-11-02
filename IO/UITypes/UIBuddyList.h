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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/SpecialText.h"

#include <string>
#include <vector>

namespace ms
{
	struct Buddy
	{
		int32_t cid;
		std::string name;
		std::string group;
		int32_t channel;
		bool online;
	};

	struct BuddyListData
	{
		int32_t capacity;
		std::vector<Buddy> buddies;
	};

	class UIBuddyList : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BUDDYLIST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIBuddyList();

		void draw(float alpha) const override;
		void update() override;
		void doubleclick(Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		void update_buddies(const BuddyListData& data);
		void set_capacity(int32_t cap);
		int32_t get_capacity() const;
		void update_buddy_channel(int32_t cid, int32_t channel);
		int32_t get_selected_cid() const { return selected_cid; }
		// 悄悄话弹层的候选名单要读好友名；只读，不改 UIBuddyList 的任何行为。
		const std::vector<Buddy>& get_buddies() const { return list.buddies; }

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtAdd,
			BtClose
		};

		static constexpr int16_t ROW_H = 24;

		int16_t row_by_position(Point<int16_t> pos) const;
		void prompt_add();
		const Buddy* selected_buddy() const;

		BuddyListData list;
		int32_t selected_cid = 0;
		mutable OutlinedText title;
	};
}
