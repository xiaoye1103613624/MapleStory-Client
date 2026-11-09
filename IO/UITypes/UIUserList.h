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
		// 家族页成员行不是按钮，须在光标事件里自行命中（选中态供底部行操作使用）。
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

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
			FRIEND_ONLINE,
			// 顶层第 4 格「家族联盟」。排在末尾避开上面四个子页签值；
			// 顶层页签 → 按钮 id 一律走 tab_button() 显式映射，不再用 BT_TAB_FRIEND + tab。
			UNION
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
		// 家族页（2026-11-07）：绘制 / 行命中 / 行操作路由
		void draw_guild_tab(float alpha) const;
		int32_t guild_row_at(Point<int16_t> cursorpos, bool* online_list = nullptr) const;
		void set_guild_buttons(bool on);
		// 2026-11-08 五页签运行时节点还原（evidence/userlist_runtime_nodes.txt 为权威）
		void bind_runtime_assets();
		void draw_friend_tab(float alpha) const;
		void draw_party_tab(float alpha) const;
		void draw_blacklist_tab(float alpha) const;
		void draw_union_tab(float alpha) const;
		void set_page_buttons(uint8_t page);
		static uint16_t tab_button(uint8_t page);

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
			// 家族页钮（GuildInfo 子树烘焙件；路由见 cpp）
			BT_GUILD_INVITE,		// BtInvite 邀请
			BT_GUILD_WITHDRAW,		// BtWithdraw 出去（离会）
			BT_GUILD_KICK,			// BtKick 删除
			BT_GUILD_WHO,			// BtWhere 位置
			BT_GUILD_WHISPER,		// BtWhisper 悄悄话
			BT_GUILD_CHAT,			// BtChat 对话（家族频道）
			BT_GUILD_RANKUP,		// BtUp 地位↑
			BT_GUILD_RANKDOWN,		// BtDown 地位↓
			BT_GUILD_NOTICE,		// Btnotice 通知
			BT_GUILD_BBS,			// BtGuildBBS 留言板
			BT_GUILD_PARTYINVITE,	// BtPartyInvite 邀请组队
			BT_GUILD_INFO,			// BtInfo 资料
			// 原生页签条第四格「家族联盟」（Tab3 贴图）：无联盟时画 disabled 态。
			BT_TAB_UNION,
			// ---- 2026-11-08 追加：按运行时 UIWindow.img/UserList 节点树绑定 ----
			// 追加在末尾，保证上面所有既有枚举值不变（旧路由与 BT_TAB_FRIEND + tab 仍可用）。
			// 好友页（Friend 子树）
			BT_FRIEND_SHOW_ALL, BT_FRIEND_SHOW_ONLINE,
			BT_FRIEND_WHISPER, BT_FRIEND_PARTY, BT_FRIEND_WHERE,
			BT_FRIEND_MATE, BT_FRIEND_MOD, BT_FRIEND_MESSAGE,
			BT_FRIEND_BLOCK, BT_FRIEND_UNBLOCK, BT_FRIEND_DELETE2,
			BT_FRIEND_GROUP_WHISPER, BT_FRIEND_GROUP_MOD, BT_FRIEND_ADD2,
			// 组员页（Party 子树）
			BT_PARTY_CREATE2, BT_PARTY_INVITE2, BT_PARTY_WITHDRAW,
			BT_PARTY_KICK, BT_PARTY_CHANGE_BOSS, BT_PARTY_SEARCH2,
			BT_PARTY_HP, BT_PARTY_WHISPER, BT_PARTY_CHAT,
			// 家族联盟页（GuildUnion 子树，与 Guild/GuildInfo 同名不同父）
			BT_UNION_INVITE, BT_UNION_KICK, BT_UNION_CHANGE,
			BT_UNION_NOTICE, BT_UNION_WHISPER, BT_UNION_PARTYINVITE,
			BT_UNION_WHERE, BT_UNION_CHAT, BT_UNION_INFO,
			BT_UNION_UP, BT_UNION_DOWN, BT_UNION_WITHDRAW,
			// 家族页补充（GuildInfo 下此前遗漏的节点）
			BT_GUILD_BOARD, BT_GUILD_CHANGE,
			BT_PAGE_LAST
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

		// Guild tab（家族页 = 083 原版「玩家列表 & 家族」共用窗第 3 页签，
		// 2026-11-07 切片；数据源 GuildModel，非 UIGuild 浮窗实例）
		// 布局 = 原生截图「好友、组员、家族、家族联盟、拒绝列表.png」行带实测
		// （tools/_measure_academy.py colband/rowband，evidence/guildpage/）。
		Texture guild_notice_bar;		// guildinfo6 282x20 公告白条 @(12,78)
		Texture guild_online_banner;	// guildinfo0 281x27 「在线家族」 @(12,111)
		Texture guild_offline_banner;	// guildinfo1 281x27 「离线家族」 @(12,189)
		Texture guild_col_header;		// guildinfo7 281x18 列头（名字/职业/等级/地位）@(12,139)
		Text guild_notice_text;			// 公告文字（白条内）
		Text guild_none_text;			// 米色标题条上的「没有参加的家族」
		int32_t guild_selected_cid = 0;

		// ---- 2026-11-08 五页签运行时资源（全部取自 UIWindow.img/UserList 真实节点） ----
		// 落位为 5 张 oracle 帧的目视初值，后续用 layout.py 逐格校准（见 notes §7）。
		// 好友页：Friend/friend0..6、icon0..5
		Texture friend_banner;			// friend0 281x27 「管理朋友」
		Texture friend_group_bar;		// friend4 281x27 分组横条
		Texture friend_col_header;		// friend1 281x19
		Texture friend_col_header2;		// friend5 281x18
		Texture friend_separator;		// friend3 281x6
		Texture friend_line;			// friend6 306x1
		Text friend_count_text;			// (0/25)
		Text friend_group_text;			// Default Group(0/0)
		Text friend_none_text;			// none
		Text friend_location_text;		// 目前位置 - xx
		// 组员页：Party/party0..5、icon0/1
		Texture party_online_banner;	// party0 281x27
		Texture party_offline_banner;	// party4 281x27
		Texture party_col_header;		// party1 279x18
		Texture party_col_header2;		// party5 279x18
		Texture party_separator;		// party2 279x4
		Text party_online_count_text;	// 0/0
		Text party_offline_count_text;	// 0/0
		Text party_none_text;
		// 拒绝列表：BlackList/blacklist0..2
		Texture blacklist_banner;		// blacklist0 281x27
		Texture blacklist_col_header;	// blacklist1 281x18
		Texture blacklist_separator;	// blacklist2 281x6
		Text blacklist_count_text;		// 0/26
		Text blacklist_none_text;
		Slider blacklist_slider;
		// 家族联盟：GuildUnion/guildinfo0,2,3,4,5,6,7 + 独立 backgrnd
		Texture union_notice_bar;		// guildinfo6 282x20
		Texture union_banner;			// guildinfo0 281x27
		Texture union_col_header;		// guildinfo2 281x18
		Texture union_col_header2;		// guildinfo3 281x18
		Texture union_separator;		// guildinfo4 281x1
		Text union_none_text;			// 还未加入联盟。
		Slider union_slider;
		// 家族页补齐
		Texture guild_col_header2;		// guildinfo3 281x18
		Texture guild_separator;		// guildinfo4 280x1
		Texture guild_mark;				// guildmark 20x20
		Text guild_online_count_text;	// [0/0]
		Text guild_offline_count_text;	// [1/1]
		Text guild_none_row_text;		// none
		Slider guild_slider;
		// 当前页签选中行（好友/组队/拒绝/联盟空态下仅占位，行数据待真机联调）
		int32_t page_selected_row = -1;
	};
}