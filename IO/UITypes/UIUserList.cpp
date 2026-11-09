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
#include "UIUserList.h"
#include "UICharInfo.h"
#include "UIAlliance.h"
#include "UIChatBar.h"
#include "UIGuildBBS.h"
#include "UIMessenger.h"
#include "UIPartySearch.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../IO/Components/MapleButton.h"
#include "../../IO/Components/TwoSpriteButton.h"
#include "../../Character/GuildModel.h"
#include "../../Character/Job.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/AlliancePackets.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/PartySearchPackets.h"
#include "../../Net/Packets/GuildPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 家族页行区（原生截图行带实测，evidence/guildpage/，tools/_measure_academy.py）：
	// 在线横幅 y111 / 列头 y139 / 行区 y157 起 2 行；离线横幅 y189 / 列头 y217 /
	// 行区 y217 起 5 行，行高 15。绘制与命中必须共用这组常量，否则点行选中会错位。
	constexpr int16_t GUILD_ROW_H = 15;
	constexpr int16_t GUILD_ONLINE_ROWS = 2;
	constexpr int16_t GUILD_OFFLINE_ROWS = 5;
	constexpr int16_t GUILD_ONLINE_Y = 157;
	constexpr int16_t GUILD_OFFLINE_Y = 217;

	UIUserList::UIUserList(uint16_t t) : UIDragElement<PosUSERLIST>(Point<int16_t>(260, 20)), tab(t)
	{
		nl::node close = UiAssetResolve::close_btn();

		UserList = UiAssetResolve::window("UserList");
		nl::node Main = UserList["Main"];

		if (!UiAssetResolve::usable(Main) && !UserList["backgrnd"])
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		if (!Main)
			Main = UserList;

		// backgrnd / backgrnd2 / backgrnd3 是三张**互斥**的底：同为 312x389、不透明占比各
		// .988、无整行全透明，原生按页签族只画其中一张。以前在这里 emplace 进 sprites，
		// 就和 change_tab() 里按页签选的 background 同点叠了两张全不透明底 —— 后画的整张
		// 盖掉先画的，页签一换看到的永远是 background 那张。逐页签实测（meanSAD，越低越像，
		// 见 evidence/r27-oracle/README.md）：好友=backgrnd3 11.6、家族联盟=backgrnd2 7.6、
		// 组员/拒绝=backgrnd。底改由 change_tab() 单张选画。

		if (close)
			buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(288, 4));

		nl::node taben = Main["Tab"]["enabled"];
		nl::node tabdis = Main["Tab"]["disabled"];

		// 页签条落位：oracle 锚 = Tab/enabled/0 在原生帧上的 (4,23)，五格相对锚
		// x = 0 / 31 / 62 / 92 / 145，y = 0 / -1 / -1 / 0 / -1。原生五格依次是
		// 好友 组员 家族 家族联盟 拒绝列表 —— 原先只接了前四格，「拒绝列表」错拿了
		// 3 号（家族联盟）的美术，第三格（家族）被当成 BossParty 页用。
		static const Point<int16_t> TAB_POS[5] = {
			Point<int16_t>(4, 23),
			Point<int16_t>(35, 22),
			Point<int16_t>(66, 22),
			Point<int16_t>(96, 23),
			Point<int16_t>(149, 22)
		};

		auto make_tab = [&](uint16_t id, uint8_t index)
		{
			if (tabdis[index] && taben[index])
				buttons[id] = std::make_unique<TwoSpriteButton>(tabdis[index], taben[index], TAB_POS[index]);
		};

		make_tab(Buttons::BT_TAB_FRIEND, 0);
		make_tab(Buttons::BT_TAB_PARTY, 1);
		make_tab(Buttons::BT_TAB_GUILD, 2);
		make_tab(Buttons::BT_TAB_UNION, 3);
		make_tab(Buttons::BT_TAB_BLACKLIST, 4);

		// Party Tab
		nl::node Party = Main["Party"];
		nl::node PartySearch = Party["PartySearch"];

		party_tab = Tab::PARTY_MINE;
		party_title = Party["title"];

		for (size_t i = 0; i <= 4; i++)
			party_mine_grid[i] = UserList["Sheet2"][i];

		party_mine_name = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "", 0);
		bind_text(party_mine_name, "userlist.none");

		nl::node party_taben = Party["Tab"]["enabled"];
		nl::node party_tabdis = Party["Tab"]["disabled"];

		auto try_btn = [&](uint16_t id, nl::node src)
		{
			if (src)
				buttons[id] = std::make_unique<MapleButton>(src);
		};

		try_btn(Buttons::BT_PARTY_CREATE, Party["BtPartyMake"]);
		try_btn(Buttons::BT_PARTY_INVITE, Party["BtPartyInvite"]);
		try_btn(Buttons::BT_PARTY_LEAVE, Party["BtPartyOut"]);
		try_btn(Buttons::BT_PARTY_SETTINGS, Party["BtPartySettings"]);

		auto deactivate = [&](uint16_t id)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(false);
		};

		deactivate(Buttons::BT_PARTY_CREATE);
		deactivate(Buttons::BT_PARTY_INVITE);
		deactivate(Buttons::BT_PARTY_LEAVE);
		deactivate(Buttons::BT_PARTY_SETTINGS);

		if (party_tabdis["0"] && party_taben["0"])
			buttons[Buttons::BT_TAB_PARTY_MINE] = std::make_unique<TwoSpriteButton>(party_tabdis["0"], party_taben["0"]);
		if (party_tabdis["1"] && party_taben["1"])
			buttons[Buttons::BT_TAB_PARTY_SEARCH] = std::make_unique<TwoSpriteButton>(party_tabdis["1"], party_taben["1"]);
		deactivate(Buttons::BT_TAB_PARTY_MINE);
		deactivate(Buttons::BT_TAB_PARTY_SEARCH);

		party_search_grid[0] = PartySearch["partyName"];
		party_search_grid[1] = PartySearch["request"];
		party_search_grid[2] = PartySearch["table"];

		try_btn(Buttons::BT_PARTY_SEARCH_LEVEL, PartySearch["BtPartyLevel"]);
		deactivate(Buttons::BT_PARTY_SEARCH_LEVEL);

		int16_t party_x = 243;
		int16_t party_y = 114;
		int16_t party_height = party_y + 168;
		int16_t party_unitrows = 6;
		int16_t party_rowmax = 6;
		party_slider = Slider(Slider::Type::DEFAULT_SILVER, Range<int16_t>(party_y, party_height), party_x, party_unitrows, party_rowmax, [](bool) {});

		// Buddy Tab
		nl::node Friend = Main["Friend"];

		friend_tab = Tab::FRIEND_ALL;
		if (Friend["title"])
			friend_sprites.emplace_back(Friend["title"]);
		if (Friend["CbCondition"]["text"])
			friend_sprites.emplace_back(Friend["CbCondition"]["text"]);
		if (UserList["line"])
			friend_sprites.emplace_back(UserList["line"], DrawArgument(Point<int16_t>(132, 115), Point<int16_t>(230, 0)));

		if (UserList["BtSheetIClose"])
			buttons[Buttons::BT_FRIEND_GROUP_0] = std::make_unique<MapleButton>(UserList["BtSheetIClose"], Point<int16_t>(13, 118));
		deactivate(Buttons::BT_FRIEND_GROUP_0);

		for (size_t i = 0; i <= 3; i++)
			friend_grid[i] = UserList["Sheet1"][i];

		std::string text = "(" + std::to_string(friend_count) + std::string("/") + std::to_string(friend_total) + std::string(")");
		friends_online_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, text, 0);

		// get_cur_location() 那半截在本窗体生命周期内恒定（换世界是重建窗体，重走 ctor），
		// 所以走带值的绑定档位，占位符留在模板里，切语言时只换前缀不动位置名。
		friends_cur_location = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, "", 0);
		bind_text(friends_cur_location, "userlist.cur_location", get_cur_location());
		friends_name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "", 0);
		bind_text(friends_name, "userlist.none");
		friends_group_name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "", 0);
		bind_text(friends_group_name, "userlist.default_group");

		// 「增加朋友 / 增加群」横排：BtAddGroup 在原生帧上实测 (68,77) agree=1.000；
		// 两张钮同为 65x18，帧上两者之间空 65..67 三列 ⇒ BtAddFriend 落 (0,77)。
		auto try_btn_at = [&](uint16_t id, nl::node src, Point<int16_t> pos)
		{
			if (src)
				buttons[id] = std::make_unique<MapleButton>(src, pos);
		};

		try_btn_at(Buttons::BT_FRIEND_ADD, Friend["BtAddFriend"], Point<int16_t>(0, 77));
		try_btn_at(Buttons::BT_FRIEND_ADD_GROUP, Friend["BtAddGroup"], Point<int16_t>(68, 77));
		try_btn(Buttons::BT_FRIEND_EXPAND, Friend["BtPlusFriend"]);
		// Friend 页无专用 Messenger 钮时：复用 UIWindow.img/Messenger/BtEnter 作开窗入口。
		{
			nl::node msg_btn = Friend["BtMessenger"];

			if (!msg_btn)
				msg_btn = Friend["BtChat"];

			if (!msg_btn)
				msg_btn = UiAssetResolve::window("Messenger")["BtEnter"];

			if (msg_btn)
			{
				buttons[Buttons::BT_FRIEND_MESSENGER] = std::make_unique<MapleButton>(
					msg_btn,
					Point<int16_t>(208, 85)
				);
			}
		}
		deactivate(Buttons::BT_FRIEND_ADD);
		deactivate(Buttons::BT_FRIEND_ADD_GROUP);
		deactivate(Buttons::BT_FRIEND_EXPAND);
		deactivate(Buttons::BT_FRIEND_MESSENGER);

		try_btn(Buttons::BT_TAB_FRIEND_ALL, Friend["TapShowAll"]);
		try_btn(Buttons::BT_TAB_FRIEND_ONLINE, Friend["TapShowOnline"]);
		deactivate(Buttons::BT_TAB_FRIEND_ALL);
		deactivate(Buttons::BT_TAB_FRIEND_ONLINE);

		int16_t friends_x = 243;
		int16_t friends_y = 115;
		int16_t friends_height = friends_y + 148;
		int16_t friends_unitrows = 6;
		int16_t friends_rowmax = 6;
		friends_slider = Slider(Slider::Type::DEFAULT_SILVER, Range<int16_t>(friends_y, friends_height), friends_x, friends_unitrows, friends_rowmax, [](bool) {});

		// Blacklist tab
		nl::node BlackList = Main["BlackList"];

		blacklist_title = BlackList["base"];

		for (size_t i = 0; i < 3; i++)
			blacklist_grid[i] = UserList["Sheet6"][i];

		blacklist_name = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "", 0);
		bind_text(blacklist_name, "userlist.none");

		try_btn(Buttons::BT_BLACKLIST_ADD, BlackList["BtAdd"]);
		try_btn(Buttons::BT_BLACKLIST_DELETE, BlackList["BtDelete"]);
		try_btn(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL, BlackList["TapShowIndividual"]);
		try_btn(Buttons::BT_TAB_BLACKLIST_GUILD, BlackList["TapShowGuild"]);
		deactivate(Buttons::BT_BLACKLIST_ADD);
		deactivate(Buttons::BT_BLACKLIST_DELETE);
		deactivate(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL);
		deactivate(Buttons::BT_TAB_BLACKLIST_GUILD);

		// Guild Tab（家族页 = 共用窗第 3 页签，2026-11-07 续作）。
		// 资源节点与坐标来自运行时 UI.nx 的 UIWindow.img/UserList/Guild/GuildInfo
		// 与原生截图行带实测（docs/features/social-loop-audit-2026-11/evidence/guildpage/）。
		// 数据统一读 GuildModel（登录时 s9 0x1A 全量下发即写入），不再依赖 UIGuild 浮窗实例。
		nl::node GI = Main["Guild"]["GuildInfo"];

		guild_notice_bar = GI["guildinfo6"];		// 282x20 公告白条 @(12,78)
		guild_online_banner = GI["guildinfo0"];	// 281x27 在线家族横幅 @(12,111)
		guild_offline_banner = GI["guildinfo1"];	// 281x27 离线家族横幅 @(12,189)
		guild_col_header = GI["guildinfo7"];		// 281x18 列头 @(12,139)/(12,217)
		guild_notice_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");
		guild_none_text = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");
		bind_text(guild_none_text, "guild.page_none");

		auto gi_btn = [&](uint16_t id, const char* name, Point<int16_t> pos)
		{
			if (GI[name])
				buttons[id] = std::make_unique<MapleButton>(GI[name], pos);
		};

		gi_btn(Buttons::BT_GUILD_NOTICE, "Btnotice", Point<int16_t>(287, 78));
		// 底部两行：原生四列 x14/84/156/232，行 y306/328；末行 y356。
		gi_btn(Buttons::BT_GUILD_INVITE, "BtInvite", Point<int16_t>(14, 306));
		gi_btn(Buttons::BT_GUILD_RANKUP, "BtUp", Point<int16_t>(14, 328));
		gi_btn(Buttons::BT_GUILD_WITHDRAW, "BtWithdraw", Point<int16_t>(84, 306));
		gi_btn(Buttons::BT_GUILD_RANKDOWN, "BtDown", Point<int16_t>(84, 328));
		gi_btn(Buttons::BT_GUILD_WHO, "BtWhere", Point<int16_t>(156, 306));
		gi_btn(Buttons::BT_GUILD_KICK, "BtKick", Point<int16_t>(156, 328));
		gi_btn(Buttons::BT_GUILD_WHISPER, "BtWhisper", Point<int16_t>(232, 306));
		gi_btn(Buttons::BT_GUILD_CHAT, "BtChat", Point<int16_t>(232, 328));
		gi_btn(Buttons::BT_GUILD_BBS, "BtGuildBBS", Point<int16_t>(120, 356));
		gi_btn(Buttons::BT_GUILD_PARTYINVITE, "BtPartyInvite", Point<int16_t>(196, 356));
		gi_btn(Buttons::BT_GUILD_INFO, "BtInfo", Point<int16_t>(268, 356));
		set_guild_buttons(false);

		// 2026-11-08：按运行时节点树补齐五页签资源与按钮（覆盖上面几处旧命名绑定）。
		bind_runtime_assets();

		change_tab(tab);

		dimension = Point<int16_t>(276, 390);
		if (Main["backgrnd"])
		{
			Point<int16_t> d = Texture(Main["backgrnd"]).get_dimensions();
			if (d.x() > 0 && d.y() > 0)
				dimension = d;
		}
	}

	void UIUserList::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		background.draw(position);

		// 五个页签共用同一个壳，按当前页签只画一套页内件（旧代码引用 Sheet1/2/6、
		// title/base 等不存在的节点，恒为空，只剩一个硬编码 Text("none")）。
		if (tab == Tab::FRIEND)
			draw_friend_tab(alpha);
		else if (tab == Tab::PARTY)
			draw_party_tab(alpha);
		else if (tab == Tab::GUILD)
			draw_guild_tab(alpha);
		else if (tab == Tab::UNION)
			draw_union_tab(alpha);
		else if (tab == Tab::BLACKLIST)
			draw_blacklist_tab(alpha);

		UIElement::draw_buttons(alpha);
	}

	void UIUserList::update()
	{
		UIElement::update();

		if (tab == Buttons::BT_TAB_FRIEND)
		{
			for (Sprite sprite : friend_sprites)
				sprite.update();
		}
		else if (tab == Buttons::BT_TAB_GUILD)
		{
			// 家族回包只写 GuildModel；页签可能在回包之后才打开，故每帧把公告
			// 同步进 Text（仅在内容变化时重建布局，避免逐帧重排字形）。
			const GuildData& g = GuildModel::get().data();
			const std::string notice = g.notice.empty()
				? Locale::text("guild.notice_none") : g.notice;

			if (guild_notice_text.get_text() != notice)
				guild_notice_text.change_text(notice);
		}
	}

	void UIUserList::set_guild_buttons(bool on)
	{
		static const uint16_t all[] = {
			Buttons::BT_GUILD_NOTICE,
			Buttons::BT_GUILD_INVITE, Buttons::BT_GUILD_WITHDRAW, Buttons::BT_GUILD_KICK,
			Buttons::BT_GUILD_WHO, Buttons::BT_GUILD_WHISPER, Buttons::BT_GUILD_CHAT,
			Buttons::BT_GUILD_RANKUP, Buttons::BT_GUILD_RANKDOWN,
			Buttons::BT_GUILD_BBS, Buttons::BT_GUILD_PARTYINVITE, Buttons::BT_GUILD_INFO
		};

		for (uint16_t id : all)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(on);
		}
	}

	int32_t UIUserList::guild_row_at(Point<int16_t> cursorpos, bool* online_list) const
	{
		const Point<int16_t> rel = cursorpos - position;

		if (rel.x() < 12 || rel.x() > 292)
			return 0;

		const GuildData& g = GuildModel::get().data();
		auto hit = [&](bool online, int16_t y0, int16_t maxrows) -> int32_t
		{
			// 可见行数受限于绘制区（与 draw_guild_tab 共用同一组常量）。
			int16_t visible = 0;

			for (const auto& m : g.members)
				if (m.online == online && visible < maxrows)
					visible++;

			if (rel.y() < y0 || rel.y() >= y0 + visible * GUILD_ROW_H)
				return 0;

			const int16_t idx = (rel.y() - y0) / GUILD_ROW_H;
			int16_t i = 0;

			for (const auto& m : g.members)
			{
				if (m.online != online || i >= maxrows)
					continue;

				if (i == idx)
					return m.id;

				i++;
			}

			return 0;
		};

		if (int32_t id = hit(true, GUILD_ONLINE_Y, GUILD_ONLINE_ROWS))
		{
			if (online_list)
				*online_list = true;

			return id;
		}

		if (int32_t id = hit(false, GUILD_OFFLINE_Y, GUILD_OFFLINE_ROWS))
		{
			if (online_list)
				*online_list = false;

			return id;
		}

		return 0;
	}

	Cursor::State UIUserList::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 成员行不是 Button：点选只更新选中态，事件仍交回基类处理按钮/拖拽。
		if (tab == Buttons::BT_TAB_GUILD && clicked)
		{
			if (int32_t cid = guild_row_at(cursorpos))
				guild_selected_cid = cid;
		}

		return UIDragElement<PosUSERLIST>::send_cursor(clicked, cursorpos);
	}

	void UIUserList::draw_guild_tab(float) const
	{
		const GuildData& g = GuildModel::get().data();

		// 米色标题条烘焙在底图上，未入会时原生在其上写「没有参加的家族」。
		if (!g.in_guild())
			guild_none_text.draw(position + Point<int16_t>(96, 58));
		else
		{
			if (guild_notice_bar)
				guild_notice_bar.draw(position + Point<int16_t>(12, 78));

			guild_notice_text.draw(position + Point<int16_t>(16, 80));
		}

		auto draw_group = [&](bool online, const Texture& banner, int16_t banner_y,
			int16_t rows_y, int16_t maxrows, const Text& count)
		{
			if (banner)
				banner.draw(position + Point<int16_t>(12, banner_y));
			count.draw(position + Point<int16_t>(240, banner_y + 5));

			if (guild_col_header)
				guild_col_header.draw(position + Point<int16_t>(12, banner_y + 28));
			if (guild_col_header2)
				guild_col_header2.draw(position + Point<int16_t>(12, banner_y + 46));
			if (guild_separator)
				guild_separator.draw(position + Point<int16_t>(12, banner_y + 64));

			int16_t y = rows_y;
			int16_t drawn = 0;

			for (const auto& m : g.members)
			{
				if (m.online != online || drawn >= maxrows)
					continue;

				// 列文字起点由 guildinfo7 列头逐像素量得：名字 27 / 职业 114 /
				// 等级 179 / 地位 222（已含底图 x12 落位偏移）。
				const Color::Name cname = (m.id == guild_selected_cid)
					? Color::Name::TORCHRED : Color::Name::BLACK;
				Text name(Text::Font::A11M, Text::Alignment::LEFT, cname, m.name);
				name.draw(position + Point<int16_t>(27, y));

				Text job(Text::Font::A11M, Text::Alignment::LEFT, cname,
					Job(static_cast<uint16_t>(m.job)).get_name());
				job.draw(position + Point<int16_t>(114, y));

				Text lv(Text::Font::A11M, Text::Alignment::LEFT, cname, std::to_string(m.level));
				lv.draw(position + Point<int16_t>(179, y));

				const std::string rank = (m.rank >= 1 && m.rank <= 5)
					? g.rankTitles[m.rank - 1] : std::string();
				Text rk(Text::Font::A11M, Text::Alignment::LEFT, cname, rank);
				rk.draw(position + Point<int16_t>(222, y));

				y += GUILD_ROW_H;
				drawn++;
			}

			// 空态：正确端两组列表都有一行 "none"。
			if (drawn == 0)
				guild_none_row_text.draw(position + Point<int16_t>(27, rows_y));
		};

		draw_group(true, guild_online_banner, 111, GUILD_ONLINE_Y, GUILD_ONLINE_ROWS,
			guild_online_count_text);
		draw_group(false, guild_offline_banner, 189, GUILD_OFFLINE_Y, GUILD_OFFLINE_ROWS,
			guild_offline_count_text);
		guild_slider.draw(position);
	}

	void UIUserList::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				deactivate();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				// 五格顺序 = 原生页签条：好友 组员 家族 家族联盟 拒绝列表。
				static const uint8_t order[] = {
					Tab::FRIEND, Tab::PARTY, Tab::GUILD, Tab::UNION, Tab::BLACKLIST
				};

				for (uint8_t i = 0; i < 5; i++)
				{
					if (order[i] != tab)
						continue;

					change_tab(order[(i + 1) % 5]);
					break;
				}
			}
			else if (keycode == 'A' || keycode == 'a')
			{
				button_pressed(Buttons::BT_FRIEND_ADD);
			}
			else if (keycode == 'G' || keycode == 'g')
			{
				button_pressed(Buttons::BT_FRIEND_ADD_GROUP);
			}
			else if (keycode == 'C' || keycode == 'c')
			{
				button_pressed(Buttons::BT_PARTY_CREATE);
			}
			else if (keycode == 'I' || keycode == 'i')
			{
				button_pressed(Buttons::BT_PARTY_INVITE);
			}
			else if (keycode == 'L' || keycode == 'l')
			{
				button_pressed(Buttons::BT_PARTY_LEAVE);
			}
			else if (keycode == 'E' || keycode == 'e')
			{
				button_pressed(Buttons::BT_PARTY_SETTINGS);
			}
			else if (keycode == 'M' || keycode == 'm')
			{
				button_pressed(Buttons::BT_FRIEND_MESSENGER);
			}
			else if (keycode == 'P' || keycode == 'p')
			{
				button_pressed(Buttons::BT_TAB_PARTY_SEARCH);
			}
			else if (keycode == 'W' || keycode == 'w')
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					std::string name = pre;
					UI::get().emplace<UISoftKey>([name](const std::string& msg) {
						if (msg.empty()) return;
						WhisperPacket(name, msg).dispatch();
						LOG(LOG_DEBUG, "UIUserList: Whisper to=" << name);
					}, []() {}, "Whisper msg");
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& name) {
						if (name.empty()) return;
						UI::get().emplace<UISoftKey>([name](const std::string& msg) {
							if (msg.empty()) return;
							WhisperPacket(name, msg).dispatch();
							LOG(LOG_DEBUG, "UIUserList: Whisper to=" << name);
						}, []() {}, "Whisper msg");
					}, []() {}, "Whisper to");
				}
			}
		}
	}

	UIElement::Type UIUserList::get_type() const
	{
		return TYPE;
	}

	Button::State UIUserList::button_pressed(uint16_t buttonid)
	{
		auto guild_hint = [](const char* key)
		{
			const std::string msg = Locale::text(key);

			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(msg.c_str(), UIChatBar::MessageType::YELLOW);
		};

		switch (buttonid)
		{
			case Buttons::BT_CLOSE:
				deactivate();
				break;
			case Buttons::BT_TAB_FRIEND:
			case Buttons::BT_TAB_PARTY:
			case Buttons::BT_TAB_GUILD:
			case Buttons::BT_TAB_BLACKLIST:
				change_tab(buttonid);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_PARTY_MINE:
				change_party_tab(buttonid);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_PARTY_SEARCH:
				change_party_tab(buttonid);
				// 顶层 UIWindow.img/PartySearch 独立壳（与页内嵌网格并存）
				if (UiAssetResolve::usable(UiAssetResolve::window("PartySearch")))
					UI::get().emplace<UIPartySearch>();
				return Button::State::PRESSED;
			case Buttons::BT_PARTY_SEARCH_LEVEL:
			case Buttons::BT_PARTY_SEARCH_INVITE:
				LOG(LOG_DEBUG, "UIUserList: PartySearch entry → toggle UIPartySearch");
				if (UiAssetResolve::usable(UiAssetResolve::window("PartySearch")))
					UI::get().emplace<UIPartySearch>();
				break;
			case Buttons::BT_TAB_FRIEND_ALL:
			case Buttons::BT_TAB_FRIEND_ONLINE:
				change_friend_tab(buttonid);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_BLACKLIST_INDIVIDUAL:
			case Buttons::BT_TAB_BLACKLIST_GUILD:
				change_blacklist_tab(buttonid);
				return Button::State::PRESSED;
			case Buttons::BT_FRIEND_MESSENGER:
			{
				LOG(LOG_DEBUG, "UIUserList: Friend → toggle UIMessenger");
				UI::get().emplace<UIMessenger>();

				if (auto m = UI::get().get_element<UIMessenger>())
					m->try_create_room();

				break;
			}
			case Buttons::BT_FRIEND_ADD:
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					BuddyAddPacket(pre, "Default Group").dispatch();
					LOG(LOG_DEBUG, "UIUserList: BuddyAdd target=" << pre);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& name) {
						if (name.empty()) return;
						BuddyAddPacket(name, "Default Group").dispatch();
						LOG(LOG_DEBUG, "UIUserList: BuddyAdd name=" << name);
					}, []() {}, "Add friend");
				}
				break;
			}
			case Buttons::BT_FRIEND_ADD_GROUP:
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					std::string name = pre;
					UI::get().emplace<UISoftKey>([name](const std::string& group) {
						if (group.empty()) return;
						BuddyAddPacket(name, group).dispatch();
						LOG(LOG_DEBUG, "UIUserList: BuddyChangeGroup name=" << name << " group=" << group);
					}, []() {}, "Friend group");
					break;
				}
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					UI::get().emplace<UISoftKey>([name](const std::string& group) {
						if (group.empty()) return;
						BuddyAddPacket(name, group).dispatch();
						LOG(LOG_DEBUG, "UIUserList: BuddyChangeGroup name=" << name << " group=" << group);
					}, []() {}, "Friend group");
				}, []() {}, "Friend name");
				break;
			}
			case Buttons::BT_PARTY_CREATE:
				PartyCreatePacket().dispatch();
				LOG(LOG_DEBUG, "UIUserList: PartyCreate");
				break;
			case Buttons::BT_PARTY_INVITE:
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					PartyInvitePacket(pre).dispatch();
					LOG(LOG_DEBUG, "UIUserList: PartyInvite target=" << pre);
					break;
				}
				// SoftKey：纯数字 → PartyJoin(partyid)；否则 → PartyInvite(name)
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					if (s.empty()) return;
					bool all_digit = true;
					for (char c : s) if (c < '0' || c > '9') { all_digit = false; break; }
					if (all_digit) {
						try {
							int32_t pid = std::stoi(s);
							if (pid > 0) {
								PartyJoinPacket(pid).dispatch();
								LOG(LOG_DEBUG, "UIUserList: PartyJoin id=" << pid);
							}
						} catch (...) {}
					} else {
						PartyInvitePacket(s).dispatch();
						LOG(LOG_DEBUG, "UIUserList: PartyInvite name=" << s);
					}
				}, []() {}, "Invite name|Join id");
				break;
			}
			case Buttons::BT_PARTY_LEAVE:
				PartyLeavePacket().dispatch();
				LOG(LOG_DEBUG, "UIUserList: PartyLeave");
				break;
			case Buttons::BT_PARTY_SETTINGS:
			{
				// CharInfo 有 cid → Expel；有名无 cid → Deny；否则 SoftKey
				int32_t from_cid = 0;
				std::string from_name;
				if (auto ci = UI::get().get_element<UICharInfo>()) {
					from_cid = ci->get_target_cid();
					from_name = ci->get_target_name();
				}
				if (from_cid > 0) {
					PartyExpelPacket(from_cid).dispatch();
					LOG(LOG_DEBUG, "UIUserList: PartyExpel cid=" << from_cid);
				} else if (!from_name.empty()) {
					DenyPartyRequestPacket(from_name).dispatch();
					LOG(LOG_DEBUG, "UIUserList: DenyParty from=" << from_name);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& s) {
						if (s.empty()) return;
						bool all_digit = true;
						for (char c : s) if (c < '0' || c > '9') { all_digit = false; break; }
						if (all_digit) {
							try {
								int32_t cid = std::stoi(s);
								if (cid > 0) {
									PartyExpelPacket(cid).dispatch();
									LOG(LOG_DEBUG, "UIUserList: PartyExpel cid=" << cid);
								}
							} catch (...) {}
						} else {
							DenyPartyRequestPacket(s).dispatch();
							LOG(LOG_DEBUG, "UIUserList: DenyParty from=" << s);
						}
					}, []() {}, "Expel cid|Deny name");
				}
				break;
			}
			case Buttons::BT_BLACKLIST_ADD:
			{
				// 无 Ignore opcode：FindPlayer 作检索代理
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					FindPlayerPacket(pre).dispatch();
					LOG(LOG_DEBUG, "UIUserList: Blacklist FindPlayer=" << pre);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& name) {
						if (name.empty()) return;
						FindPlayerPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIUserList: Blacklist FindPlayer=" << name);
					}, []() {}, "Find name");
				}
				break;
			}
			case Buttons::BT_BLACKLIST_DELETE:
			{
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					FindPlayerPacket(pre).dispatch();
					LOG(LOG_DEBUG, "UIUserList: Blacklist delete FindPlayer=" << pre);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& name) {
						if (name.empty()) return;
						FindPlayerPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIUserList: Blacklist delete FindPlayer=" << name);
					}, []() {}, "Find name");
				}
				break;
			}
			case Buttons::BT_TAB_UNION:
				change_tab(Tab::UNION);
				return Button::State::PRESSED;
			case Buttons::BT_GUILD_NOTICE:
			{
				// s9 GuildOperationHandler case 0x10：0x10 + string（限 100 字）。
				UI::get().emplace<UISoftKey>([](const std::string& notice)
				{
					if (!notice.empty())
						GuildNoticePacket(notice).dispatch();
				}, []() {}, Locale::text("guild.notice_prompt"));
				break;
			}
			case Buttons::BT_GUILD_INVITE:
			{
				std::string pre;

				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();

				if (!pre.empty())
				{
					GuildInvitePacket(pre).dispatch();
					break;
				}

				UI::get().emplace<UISoftKey>([](const std::string& name)
				{
					if (!name.empty())
						GuildInvitePacket(name).dispatch();
				}, []() {}, Locale::text("guild.invite_prompt"));
				break;
			}
			case Buttons::BT_GUILD_WITHDRAW:
			{
				// s9 case 0x07 校验 cid==自己 且 name==自己，否则判 [Hack] 丢弃。
				const auto& player = Stage::get().get_player();
				const int32_t self = player.get_oid();
				const std::string selfname = player.get_name();

				if (self > 0 && !selfname.empty())
					GuildLeavePacket(self, selfname).dispatch();

				break;
			}
			case Buttons::BT_GUILD_KICK:
			case Buttons::BT_GUILD_WHO:
			case Buttons::BT_GUILD_WHISPER:
			case Buttons::BT_GUILD_RANKUP:
			case Buttons::BT_GUILD_RANKDOWN:
			case Buttons::BT_GUILD_PARTYINVITE:
			case Buttons::BT_GUILD_INFO:
			{
				const GuildMember* m = GuildModel::get().find(guild_selected_cid);

				if (!m || m->id <= 0)
				{
					guild_hint("guild.need_select");
					break;
				}

				switch (buttonid)
				{
					case Buttons::BT_GUILD_KICK:
						GuildExpelPacket(m->id, m->name).dispatch();
						break;
					case Buttons::BT_GUILD_WHO:
						FindPlayerPacket(m->name).dispatch();
						break;
					case Buttons::BT_GUILD_WHISPER:
					{
						const std::string name = m->name;
						UI::get().emplace<UISoftKey>([name](const std::string& msg)
						{
							if (!msg.empty())
								WhisperPacket(name, msg).dispatch();
						}, []() {}, Locale::text("guild.whisper_prompt"));
						break;
					}
					case Buttons::BT_GUILD_RANKUP:
					case Buttons::BT_GUILD_RANKDOWN:
					{
						// s9 case 0x0E：newRank 是绝对职位 (2..5]，不是增减量；
						// rank<=2 只有会长可设，这里按当前职位 ±1 后夹到合法区间。
						int32_t rank = m->rank + (buttonid == Buttons::BT_GUILD_RANKUP ? -1 : 1);

						if (rank < 2)
							rank = 2;
						if (rank > 5)
							rank = 5;

						GuildChangeRankPacket(m->id, static_cast<int8_t>(rank)).dispatch();
						break;
					}
					case Buttons::BT_GUILD_PARTYINVITE:
						PartyInvitePacket(m->name).dispatch();
						break;
					case Buttons::BT_GUILD_INFO:
						UI::get().emplace<UICharInfo>(m->id);
						break;
				}

				break;
			}
			case Buttons::BT_GUILD_CHAT:
			{
				// v83 家族聊天由聊天框目标/斜杠别名路由；change_target 仍是预留空实现，
				// 故这里展开输入行并预置 /g 前缀（UIChatBar::input_text_enter_callback 已接）。
				if (auto chatbar = UI::get().get_element<UIChatBar>())
				{
					chatbar->toggle_input(true);
					chatbar->set_input("/g ");
				}

				break;
			}
			case Buttons::BT_GUILD_BBS:
			{
				auto bbs = UI::get().get_element<UIGuildBBS>();

				if (!bbs)
					bbs = UI::get().emplace<UIGuildBBS>();
				else if (!bbs->is_active())
					bbs->makeactive();

				if (bbs)
					bbs->request_list(0);

				break;
			}
			// ---- 2026-11-08 五页签新增按钮：一律复用已与 s9 核对过的协议包 ----
			case Buttons::BT_FRIEND_SHOW_ALL:
			case Buttons::BT_FRIEND_SHOW_ONLINE:
				change_friend_tab(buttonid == Buttons::BT_FRIEND_SHOW_ONLINE
					? Tab::FRIEND_ONLINE : Tab::FRIEND_ALL);
				return Button::State::PRESSED;
			case Buttons::BT_FRIEND_ADD2:
			case Buttons::BT_FRIEND_MATE:
			case Buttons::BT_FRIEND_MOD:
			case Buttons::BT_FRIEND_GROUP_MOD:
				// 节点语义（好友/分组管理）按节点名推断，统一落到 BuddyAdd/改分组。
				button_pressed(Buttons::BT_FRIEND_ADD);
				break;
			case Buttons::BT_FRIEND_PARTY:
				button_pressed(Buttons::BT_PARTY_INVITE);
				break;
			case Buttons::BT_FRIEND_WHISPER:
			case Buttons::BT_FRIEND_MESSAGE:
			case Buttons::BT_FRIEND_GROUP_WHISPER:
			case Buttons::BT_PARTY_WHISPER:
			case Buttons::BT_UNION_WHISPER:
			{
				UI::get().emplace<UISoftKey>([](const std::string& name)
				{
					if (name.empty())
						return;

					UI::get().emplace<UISoftKey>([name](const std::string& msg)
					{
						if (!msg.empty())
							WhisperPacket(name, msg).dispatch();
					}, []() {}, "Whisper msg");
				}, []() {}, "Whisper to");
				break;
			}
			case Buttons::BT_FRIEND_WHERE:
			case Buttons::BT_FRIEND_BLOCK:
			case Buttons::BT_FRIEND_UNBLOCK:
			case Buttons::BT_UNION_WHERE:
			{
				// v83 无 Ignore opcode：与既有拒绝列表一致，用 FindPlayer 作检索代理。
				UI::get().emplace<UISoftKey>([](const std::string& name)
				{
					if (!name.empty())
						FindPlayerPacket(name).dispatch();
				}, []() {}, "Find name");
				break;
			}
			case Buttons::BT_FRIEND_DELETE2:
			{
				UI::get().emplace<UISoftKey>([](const std::string& s)
				{
					try
					{
						int32_t cid = std::stoi(s);

						if (cid > 0)
							BuddyDeletePacket(cid).dispatch();
					}
					catch (...) {}
				}, []() {}, "Friend cid");
				break;
			}
			case Buttons::BT_PARTY_CREATE2:
				PartyCreatePacket().dispatch();
				break;
			case Buttons::BT_PARTY_INVITE2:
				button_pressed(Buttons::BT_PARTY_INVITE);
				break;
			case Buttons::BT_PARTY_WITHDRAW:
				PartyLeavePacket().dispatch();
				break;
			case Buttons::BT_PARTY_KICK:
			case Buttons::BT_PARTY_CHANGE_BOSS:
			{
				const bool leader = buttonid == Buttons::BT_PARTY_CHANGE_BOSS;
				UI::get().emplace<UISoftKey>([leader](const std::string& s)
				{
					try
					{
						int32_t cid = std::stoi(s);

						if (cid <= 0)
							return;

						if (leader)
							PartyChangeLeaderPacket(cid).dispatch();
						else
							PartyExpelPacket(cid).dispatch();
					}
					catch (...) {}
				}, []() {}, "Member cid");
				break;
			}
			case Buttons::BT_PARTY_SEARCH2:
				if (UiAssetResolve::usable(UiAssetResolve::window("PartySearch")))
					UI::get().emplace<UIPartySearch>();
				break;
			case Buttons::BT_PARTY_CHAT:
			case Buttons::BT_UNION_CHAT:
			{
				if (auto chatbar = UI::get().get_element<UIChatBar>())
				{
					chatbar->toggle_input(true);
					chatbar->set_input("/p ");
				}

				break;
			}
			case Buttons::BT_PARTY_HP:
			case Buttons::BT_GUILD_CHANGE:
				guild_hint("userlist.need_data");
				break;
			case Buttons::BT_UNION_INVITE:
			{
				UI::get().emplace<UISoftKey>([](const std::string& name)
				{
					if (!name.empty())
						AllianceInvitePacket(name).dispatch();
				}, []() {}, "Guild name");
				break;
			}
			case Buttons::BT_UNION_NOTICE:
			{
				UI::get().emplace<UISoftKey>([](const std::string& notice)
				{
					if (!notice.empty())
						AllianceNoticePacket(notice).dispatch();
				}, []() {}, "Alliance notice");
				break;
			}
			case Buttons::BT_UNION_WITHDRAW:
			case Buttons::BT_UNION_KICK:
			case Buttons::BT_UNION_CHANGE:
			case Buttons::BT_UNION_UP:
			case Buttons::BT_UNION_DOWN:
			case Buttons::BT_UNION_INFO:
			case Buttons::BT_UNION_PARTYINVITE:
			{
				// s9 AllianceOperationHandler 各 action 载荷不一，不臆造 action 字节：
				// 统一打开已核对过协议的 UIAlliance 窗，由该窗完成具体操作。
				auto alliance = UI::get().get_element<UIAlliance>();

				if (!alliance)
					alliance = UI::get().emplace<UIAlliance>();
				else if (!alliance->is_active())
					alliance->makeactive();

				guild_hint("userlist.use_alliance_window");
				break;
			}
			case Buttons::BT_GUILD_BOARD:
				button_pressed(Buttons::BT_GUILD_BBS);
				break;
			default:
				return Button::State::NORMAL;
			}

		return Button::State::NORMAL;
	}

	void UIUserList::change_tab(uint8_t tabid)
	{
		uint8_t oldtab = tab;
		tab = tabid;

		auto set_act = [&](uint16_t id, bool act)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(act);
		};
		auto set_st = [&](uint16_t id, Button::State st)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(st);
		};

		nl::node Main = UserList["Main"] ? UserList["Main"] : UserList;

		// 三张互斥底（312x389）：2026-11-08 用 5 张 oracle 帧逐像素 meanSAD 量出
		// 好友=backgrnd3(11.59)、家族/联盟=backgrnd2(25.06/7.61)、
		// 组员/拒绝=backgrnd(23.50/11.07)。逐页签只画一张，禁止叠底。
		if (tabid == Tab::FRIEND)
			background = Main["backgrnd3"];
		else if (tabid == Tab::GUILD || tabid == Tab::UNION)
			background = Main["backgrnd2"];
		else
			background = Main["backgrnd"];

		// 顶层页签按钮 id 与 tab 值不再线性（UNION 在枚举末尾），走显式映射。
		if (oldtab != tab)
			set_st(tab_button(oldtab), Button::State::NORMAL);

		set_st(tab_button(tab), Button::State::PRESSED);

		if (tab == Buttons::BT_TAB_PARTY)
		{
			set_act(Buttons::BT_PARTY_CREATE, true);
			set_act(Buttons::BT_PARTY_INVITE, true);
			set_act(Buttons::BT_TAB_PARTY_MINE, true);
			set_act(Buttons::BT_TAB_PARTY_SEARCH, true);

			change_party_tab(Tab::PARTY_MINE);
		}
		else
		{
			set_act(Buttons::BT_PARTY_CREATE, false);
			set_act(Buttons::BT_PARTY_INVITE, false);
			set_act(Buttons::BT_TAB_PARTY_MINE, false);
			set_act(Buttons::BT_TAB_PARTY_SEARCH, false);
			set_act(Buttons::BT_PARTY_SEARCH_LEVEL, false);
		}

		if (tab == Buttons::BT_TAB_FRIEND)
		{
			set_act(Buttons::BT_FRIEND_ADD, true);
			set_act(Buttons::BT_FRIEND_ADD_GROUP, true);
			set_act(Buttons::BT_FRIEND_EXPAND, true);
			set_act(Buttons::BT_FRIEND_MESSENGER, true);
			set_act(Buttons::BT_TAB_FRIEND_ALL, true);
			set_act(Buttons::BT_TAB_FRIEND_ONLINE, true);
			set_act(Buttons::BT_FRIEND_GROUP_0, true);

			change_friend_tab(Tab::FRIEND_ALL);
		}
		else
		{
			set_act(Buttons::BT_FRIEND_ADD, false);
			set_act(Buttons::BT_FRIEND_ADD_GROUP, false);
			set_act(Buttons::BT_FRIEND_EXPAND, false);
			set_act(Buttons::BT_FRIEND_MESSENGER, false);
			set_act(Buttons::BT_TAB_FRIEND_ALL, false);
			set_act(Buttons::BT_TAB_FRIEND_ONLINE, false);
			set_act(Buttons::BT_FRIEND_GROUP_0, false);
		}

		if (tab == Buttons::BT_TAB_BLACKLIST)
		{
			set_act(Buttons::BT_BLACKLIST_ADD, true);
			set_act(Buttons::BT_BLACKLIST_DELETE, true);
			set_act(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL, true);
			set_act(Buttons::BT_TAB_BLACKLIST_GUILD, true);
		}
		else
		{
			set_act(Buttons::BT_BLACKLIST_ADD, false);
			set_act(Buttons::BT_BLACKLIST_DELETE, false);
			set_act(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL, false);
			set_act(Buttons::BT_TAB_BLACKLIST_GUILD, false);
		}

		// 每个页签只激活自己的按钮集；家族页 12 按钮沿用上一轮的 set_guild_buttons。
		set_guild_buttons(tab == Tab::GUILD);
		set_page_buttons(tab);
	}

	uint16_t UIUserList::get_tab()
	{
		return tab;
	}

	void UIUserList::change_party_tab(uint8_t tabid)
	{
		uint8_t oldtab = party_tab;
		party_tab = tabid;

		auto set_st = [&](uint16_t id, Button::State st)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(st);
		};
		auto set_act = [&](uint16_t id, bool act)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(act);
		};

		if (oldtab != party_tab)
			set_st(Buttons::BT_TAB_FRIEND + oldtab, Button::State::NORMAL);

		set_st(Buttons::BT_TAB_FRIEND + party_tab, Button::State::PRESSED);

		set_act(Buttons::BT_PARTY_SEARCH_LEVEL, party_tab == Buttons::BT_TAB_PARTY_SEARCH);
	}

	void UIUserList::change_friend_tab(uint8_t tabid)
	{
		uint8_t oldtab = friend_tab;
		friend_tab = tabid;

		auto set_st = [&](uint16_t id, Button::State st)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(st);
		};

		if (oldtab != friend_tab)
			set_st(Buttons::BT_TAB_FRIEND + oldtab, Button::State::NORMAL);

		set_st(Buttons::BT_TAB_FRIEND + friend_tab, Button::State::PRESSED);
	}

	void UIUserList::change_blacklist_tab(uint8_t tabid)
	{
		uint8_t oldtab = blacklist_tab;
		blacklist_tab = tabid;

		auto set_st = [&](uint16_t id, Button::State st)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(st);
		};

		if (oldtab != blacklist_tab)
			set_st(Buttons::BT_TAB_FRIEND + oldtab, Button::State::NORMAL);

		set_st(Buttons::BT_TAB_FRIEND + blacklist_tab, Button::State::PRESSED);
	}

	void UIUserList::bind_runtime_assets()
	{
		// 本包没有 Main 子树：五个页签的资源都在 UserList 顶层。
		// 节点名一律取自 evidence/userlist_runtime_nodes.txt（运行时 IMG，不是打包 UI.wz）。
		nl::node Root = UserList;
		nl::node Friend = Root["Friend"];
		nl::node Party = Root["Party"];
		nl::node BlackList = Root["BlackList"];
		nl::node GI = Root["Guild"]["GuildInfo"];
		nl::node GU = Root["GuildUnion"];

		// 只接受 bitmap：容器节点传进 Texture 会得到空贴图（R5 曾踩过）。
		auto bitmap = [](nl::node parent, const char* name) -> Texture
		{
			nl::node n = parent[name];
			return (n && n.data_type() == nl::node::type::bitmap) ? Texture(n) : Texture();
		};
		// 只接受四态按钮容器，空壳节点跳过。
		auto bind_btn = [&](uint16_t id, nl::node parent, const char* name, Point<int16_t> pos)
		{
			nl::node n = parent[name];
			if (n && n.size() > 0)
				buttons[id] = std::make_unique<MapleButton>(n, pos);
		};
		// 原生底栏 4 列 x 3 行（五张 oracle 帧一致）。
		auto grid = [](uint8_t i) -> Point<int16_t>
		{
			static const int16_t xs[4] = { 14, 84, 156, 232 };
			static const int16_t ys[3] = { 306, 328, 350 };
			return Point<int16_t>(xs[i % 4], ys[(i / 4) % 3]);
		};
		struct NodeBtn { uint16_t id; const char* node; };
		auto bind_grid = [&](nl::node parent, const NodeBtn* defs, uint8_t count)
		{
			for (uint8_t i = 0; i < count; i++)
				bind_btn(defs[i].id, parent, defs[i].node, grid(i));
		};

		// 好友页 Friend（38 个直接子节点，旧代码只用了 4 个）
		friend_banner = bitmap(Friend, "friend0");
		friend_group_bar = bitmap(Friend, "friend4");
		friend_col_header = bitmap(Friend, "friend1");
		friend_col_header2 = bitmap(Friend, "friend5");
		friend_separator = bitmap(Friend, "friend3");
		friend_line = bitmap(Friend, "friend6");
		bind_btn(BT_FRIEND_ADD, Friend, "BtAddFriend", Point<int16_t>(14, 80));
		bind_btn(BT_FRIEND_ADD_GROUP, Friend, "BtAddGroup", Point<int16_t>(80, 80));
		bind_btn(BT_FRIEND_SHOW_ALL, Friend, "BtShowAll", Point<int16_t>(150, 80));
		bind_btn(BT_FRIEND_SHOW_ONLINE, Friend, "BtShowOnline", Point<int16_t>(215, 80));
		static const NodeBtn friend_btns[] = {
			{ BT_FRIEND_WHISPER, "BtWhisper" },
			{ BT_FRIEND_MATE, "BtMate" },
			{ BT_FRIEND_MOD, "BtMod" },
			{ BT_FRIEND_MESSAGE, "BtMessage" },
			{ BT_FRIEND_BLOCK, "BtBlock" },
			{ BT_FRIEND_UNBLOCK, "BtUnBlock" },
			{ BT_FRIEND_DELETE2, "BtDelete" },
			{ BT_FRIEND_PARTY, "BtParty" },
			{ BT_FRIEND_WHERE, "BtWhere" },
			{ BT_FRIEND_GROUP_WHISPER, "BtGroupWhisper" },
			{ BT_FRIEND_GROUP_MOD, "GroupMod" },
			{ BT_FRIEND_ADD2, "BtAdd" }
		};
		bind_grid(Friend, friend_btns, 12);

		// 组员页 Party（18 个直接子节点，旧代码 0 个有效）
		party_online_banner = bitmap(Party, "party0");
		party_offline_banner = bitmap(Party, "party4");
		party_col_header = bitmap(Party, "party1");
		party_col_header2 = bitmap(Party, "party5");
		party_separator = bitmap(Party, "party2");
		static const NodeBtn party_btns[] = {
			{ BT_PARTY_CREATE2, "BtCreate" },
			{ BT_PARTY_INVITE2, "BtInvite" },
			{ BT_PARTY_WITHDRAW, "BtWithdraw" },
			{ BT_PARTY_KICK, "BtKick" },
			{ BT_PARTY_CHANGE_BOSS, "BtChangeBoss" },
			{ BT_PARTY_SEARCH2, "BtSearch" },
			{ BT_PARTY_HP, "BtHP" },
			{ BT_PARTY_WHISPER, "BtWhisper" },
			{ BT_PARTY_CHAT, "BtChat" }
		};
		bind_grid(Party, party_btns, 9);

		// 拒绝列表 BlackList（旧代码只接了 2 个按钮，3 张烘焙件全漏）
		blacklist_banner = bitmap(BlackList, "blacklist0");
		blacklist_col_header = bitmap(BlackList, "blacklist1");
		blacklist_separator = bitmap(BlackList, "blacklist2");
		bind_btn(BT_BLACKLIST_ADD, BlackList, "BtAdd", Point<int16_t>(60, 352));
		bind_btn(BT_BLACKLIST_DELETE, BlackList, "BtDelete", Point<int16_t>(160, 352));

		// 家族联盟 GuildUnion（20 个直接子节点，旧代码 0 个）
		union_notice_bar = bitmap(GU, "guildinfo6");
		union_banner = bitmap(GU, "guildinfo0");
		union_col_header = bitmap(GU, "guildinfo2");
		union_col_header2 = bitmap(GU, "guildinfo3");
		union_separator = bitmap(GU, "guildinfo4");
		static const NodeBtn union_btns[] = {
			{ BT_UNION_INVITE, "BtInvite" },
			{ BT_UNION_WITHDRAW, "BtWithdraw" },
			{ BT_UNION_WHERE, "BtWhere" },
			{ BT_UNION_WHISPER, "BtWhisper" },
			{ BT_UNION_UP, "BtUp" },
			{ BT_UNION_DOWN, "BtDown" },
			{ BT_UNION_KICK, "BtKick" },
			{ BT_UNION_CHAT, "BtChat" },
			{ BT_UNION_NOTICE, "Btnotice" },
			{ BT_UNION_PARTYINVITE, "BtPartyInvite" },
			{ BT_UNION_INFO, "BtInfo" },
			{ BT_UNION_CHANGE, "BtChange" }
		};
		bind_grid(GU, union_btns, 12);

		// 家族页补齐 + 12 按钮从 3 列改回原生 4x3
		guild_col_header2 = bitmap(GI, "guildinfo3");
		guild_separator = bitmap(GI, "guildinfo4");
		guild_mark = bitmap(GI, "guildmark");
		static const NodeBtn guild_btns[] = {
			{ BT_GUILD_INVITE, "BtInvite" },
			{ BT_GUILD_WITHDRAW, "BtWithdraw" },
			{ BT_GUILD_WHO, "BtWhere" },
			{ BT_GUILD_WHISPER, "BtWhisper" },
			{ BT_GUILD_RANKUP, "BtUp" },
			{ BT_GUILD_RANKDOWN, "BtDown" },
			{ BT_GUILD_KICK, "BtKick" },
			{ BT_GUILD_CHAT, "BtChat" },
			{ BT_GUILD_BBS, "BtGuildBBS" },
			{ BT_GUILD_PARTYINVITE, "BtPartyInvite" },
			{ BT_GUILD_INFO, "BtInfo" },
			{ BT_GUILD_BOARD, "BtBoard" }
		};
		bind_grid(GI, guild_btns, 12);

		auto label = [](Color::Name c)
		{
			return Text(Text::Font::A11M, Text::Alignment::LEFT, c, "");
		};
		friend_count_text = label(Color::Name::WHITE);
		friend_group_text = label(Color::Name::BLACK);
		friend_none_text = label(Color::Name::BLACK);
		friend_location_text = label(Color::Name::BLACK);
		party_online_count_text = label(Color::Name::WHITE);
		party_offline_count_text = label(Color::Name::WHITE);
		party_none_text = label(Color::Name::BLACK);
		blacklist_count_text = label(Color::Name::WHITE);
		blacklist_none_text = label(Color::Name::BLACK);
		union_none_text = label(Color::Name::BLACK);
		guild_online_count_text = label(Color::Name::WHITE);
		guild_offline_count_text = label(Color::Name::WHITE);
		guild_none_row_text = label(Color::Name::BLACK);

		friend_count_text.change_text("(0/25)");
		friend_group_text.change_text(Locale::text("userlist.friend_group") + "(0/0)");
		friend_none_text.change_text(Locale::text("userlist.none"));
		friend_location_text.change_text(Locale::text("userlist.location") + " - " + get_cur_location());
		party_online_count_text.change_text("0/0");
		party_offline_count_text.change_text("0/0");
		party_none_text.change_text(Locale::text("userlist.none"));
		blacklist_count_text.change_text("0/26");
		blacklist_none_text.change_text(Locale::text("userlist.none"));
		union_none_text.change_text(Locale::text("userlist.union_none"));
		guild_online_count_text.change_text("[0/0]");
		guild_offline_count_text.change_text("[0/0]");
		guild_none_row_text.change_text(Locale::text("userlist.none"));

		auto slider = [](int16_t x)
		{
			return Slider(Slider::Type::DEFAULT_SILVER, Range<int16_t>(105, 285), x, 6, 6, [](bool) {});
		};
		friends_slider = slider(292);
		party_slider = slider(292);
		blacklist_slider = slider(292);
		union_slider = slider(292);
		guild_slider = slider(292);
	}

	uint16_t UIUserList::tab_button(uint8_t page)
	{
		switch (page)
		{
			case Tab::PARTY:
				return Buttons::BT_TAB_PARTY;
			case Tab::GUILD:
				return Buttons::BT_TAB_GUILD;
			case Tab::UNION:
				return Buttons::BT_TAB_UNION;
			case Tab::BLACKLIST:
				return Buttons::BT_TAB_BLACKLIST;
			default:
				return Buttons::BT_TAB_FRIEND;
		}
	}

	void UIUserList::set_page_buttons(uint8_t page)
	{
		auto act = [&](uint16_t id, bool on)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(on);
		};
		struct Group { uint16_t first; uint16_t last; uint8_t page; };
		static const Group groups[] = {
			{ BT_FRIEND_SHOW_ALL, BT_FRIEND_ADD2, Tab::FRIEND },
			{ BT_PARTY_CREATE2, BT_PARTY_CHAT, Tab::PARTY },
			{ BT_UNION_INVITE, BT_UNION_WITHDRAW, Tab::UNION },
			{ BT_GUILD_BOARD, BT_GUILD_CHANGE, Tab::GUILD }
		};

		for (const auto& g : groups)
			for (uint16_t id = g.first; id <= g.last; id++)
				act(id, page == g.page);
	}

	void UIUserList::draw_friend_tab(float) const
	{
		if (friend_banner)
			friend_banner.draw(position + Point<int16_t>(12, 52));
		friend_count_text.draw(position + Point<int16_t>(243, 56));
		if (friend_group_bar)
			friend_group_bar.draw(position + Point<int16_t>(12, 100));
		friend_group_text.draw(position + Point<int16_t>(22, 104));
		if (friend_col_header)
			friend_col_header.draw(position + Point<int16_t>(12, 119));
		if (friend_col_header2)
			friend_col_header2.draw(position + Point<int16_t>(12, 138));
		if (friend_separator)
			friend_separator.draw(position + Point<int16_t>(12, 156));
		friend_none_text.draw(position + Point<int16_t>(24, 150));
		friend_location_text.draw(position + Point<int16_t>(12, 292));
		friends_slider.draw(position);
	}

	void UIUserList::draw_party_tab(float) const
	{
		if (party_online_banner)
			party_online_banner.draw(position + Point<int16_t>(12, 52));
		party_online_count_text.draw(position + Point<int16_t>(250, 56));
		if (party_col_header)
			party_col_header.draw(position + Point<int16_t>(12, 80));
		party_none_text.draw(position + Point<int16_t>(24, 105));
		if (party_offline_banner)
			party_offline_banner.draw(position + Point<int16_t>(12, 140));
		party_offline_count_text.draw(position + Point<int16_t>(250, 144));
		if (party_col_header2)
			party_col_header2.draw(position + Point<int16_t>(12, 168));
		if (party_separator)
			party_separator.draw(position + Point<int16_t>(12, 186));
		party_slider.draw(position);
	}

	void UIUserList::draw_blacklist_tab(float) const
	{
		if (blacklist_banner)
			blacklist_banner.draw(position + Point<int16_t>(12, 52));
		blacklist_count_text.draw(position + Point<int16_t>(250, 56));
		if (blacklist_col_header)
			blacklist_col_header.draw(position + Point<int16_t>(12, 80));
		if (blacklist_separator)
			blacklist_separator.draw(position + Point<int16_t>(12, 98));
		blacklist_none_text.draw(position + Point<int16_t>(24, 105));
		blacklist_slider.draw(position);
	}

	void UIUserList::draw_union_tab(float) const
	{
		if (union_notice_bar)
			union_notice_bar.draw(position + Point<int16_t>(12, 52));
		union_none_text.draw(position + Point<int16_t>(96, 56));
		if (union_banner)
			union_banner.draw(position + Point<int16_t>(12, 84));
		if (union_col_header)
			union_col_header.draw(position + Point<int16_t>(12, 112));
		if (union_col_header2)
			union_col_header2.draw(position + Point<int16_t>(12, 130));
		if (union_separator)
			union_separator.draw(position + Point<int16_t>(12, 148));
		union_slider.draw(position);
	}

	std::string UIUserList::get_cur_location()
	{
		return "Henesys Market";
	}
}