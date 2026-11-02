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

namespace ms
{
	class UIUserList : public UIDragElement<PosUSERLIST>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::USERLIST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIUserList(uint16_t tab);

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		enum Tab
		{
			FRIEND,
			PARTY,
			GUILD,
			BLACKLIST,
			PARTY_MINE,
			PARTY_SEARCH,
			FRIEND_ALL,
			FRIEND_ONLINE
		};

		void change_tab(uint8_t tabid);
		uint16_t get_tab();

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void change_party_tab(uint8_t tabid);
		void change_friend_tab(uint8_t tabid);
		void change_blacklist_tab(uint8_t tabid);
		std::string get_cur_location();

		enum Buttons
		{
			BT_TAB_FRIEND,
			BT_TAB_PARTY,
			BT_TAB_GUILD,
			BT_TAB_BLACKLIST,
			BT_TAB_PARTY_MINE,
			BT_TAB_PARTY_SEARCH,
			BT_TAB_FRIEND_ALL,
			BT_TAB_FRIEND_ONLINE,
			BT_TAB_BLACKLIST_INDIVIDUAL,
			BT_TAB_BLACKLIST_GUILD,
			BT_CLOSE,
			BT_PARTY_CREATE,
			BT_PARTY_INVITE,
			BT_PARTY_LEAVE,
			BT_PARTY_SETTINGS,
			BT_PARTY_SEARCH_LEVEL,
			BT_PARTY_SEARCH_INVITE,
			BT_FRIEND_ADD,
			BT_FRIEND_ADD_GROUP,
			BT_FRIEND_EXPAND,
			BT_FRIEND_MESSENGER,
			BT_FRIEND_GROUP_0,
			BT_BLACKLIST_ADD,
			BT_BLACKLIST_DELETE,
			// 原生页签条第五格「家族联盟」：本包有美术、无页面内容（GuildUnion 未实现），
			// 只画不切页。排在末尾，不参与 BT_TAB_FRIEND + tab 的页签序号换算。
			BT_TAB_UNION
		};

		uint16_t tab;
		nl::node UserList;
		Texture background;

		// Party tab
		uint16_t party_tab;
		Texture party_title;
		Texture party_mine_grid[5];
		Texture party_search_grid[3];
		Text party_mine_name;
		Slider party_slider;

		// Buddy tab
		uint16_t friend_tab;
		int friend_count = 0;
		int friend_total = 50;
		std::vector<Sprite> friend_sprites;
		Texture friend_grid[4];
		Text friends_online_text;
		Text friends_cur_location;
		Text friends_name;
		Text friends_group_name;
		Slider friends_slider;

		// Blacklist tab
		uint16_t blacklist_tab;
		Texture blacklist_title;
		Texture blacklist_grid[3];
		Text blacklist_name;
	};
}