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
#include "UIChatBar.h"
#include "UIAllianceGrade.h"
#include "UIGuildBBS.h"
#include "UIMessenger.h"
#include "UINotice.h"
#include "UIPartySearch.h"
#include "../UI.h"
#include "../../IO/Components/MapleButton.h"
#include "../../IO/Components/TwoSpriteButton.h"
#include "../../Configuration.h"
#include "../../Character/AllianceModel.h"
#include "../../Character/BuddyListModel.h"
#include "../../Character/GuildModel.h"
#include "../../Character/Job.h"
#include "../../Character/PartyModel.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/AlliancePackets.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/PartySearchPackets.h"
#include "../../Net/Packets/GuildPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/Locale.h"

#include <algorithm>
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// P0 好友/组员行区（相对 UserList 窗；复用家族页行高经验，像素精修挂后续）。
	static constexpr int16_t FRIEND_ROW_Y = 130;	// r3 oracle：分组行 112 + 行距 → 首行 130
	static constexpr int16_t FRIEND_ROW_H = 18;
	static constexpr int16_t FRIEND_MAX_ROWS = 7;
	static constexpr int16_t PARTY_ONLINE_Y = 106;	// r3 oracle：none 文本顶 y106
	static constexpr int16_t PARTY_OFFLINE_Y = 187;	// r3 oracle：none 文本顶 y187
	static constexpr int16_t PARTY_ROW_H = 18;
	static constexpr int16_t PARTY_ONLINE_ROWS = 3;
	static constexpr int16_t PARTY_OFFLINE_ROWS = 5;

	// 家族页行区（原生截图行带实测，evidence/guildpage/，tools/_measure_academy.py）：
	// 在线横幅 y111 / 列头 y139 / 行区 y157 起 2 行；离线横幅 y189 / 列头 y217 /
	// 行区须在列头之下（旧 GUILD_OFFLINE_Y=217 与列头同 y，成员字压在列头上）。
	// 2026-11-11：离线行起点改为 235（列头 217 + 行高约 18）。
	constexpr int16_t GUILD_ROW_H = 15;
	constexpr int16_t GUILD_ONLINE_ROWS = 2;
	constexpr int16_t GUILD_OFFLINE_ROWS = 5;
	constexpr int16_t GUILD_ONLINE_Y = 157;
	constexpr int16_t GUILD_OFFLINE_Y = 235;

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

		load_blacklist(); // v83 CConfig::LoadBlackList：恢复本地拒绝列表

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

		// r3：Angel 083 好友页没有组折叠按钮控件（OnCreate 无创建；组展开/收起
		// 在 CTabFriend::OnClick 里翻 CTabFriend+20 标志位），不再绑定 img 残留节点。
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

		// oracle / 好友列表框.png：BtAddFriend mae=0 @(1,77)，BtAddGroup @(68,77)。
		try_btn_at(Buttons::BT_FRIEND_ADD, Friend["BtAddFriend"], Point<int16_t>(1, 77));
		try_btn_at(Buttons::BT_FRIEND_ADD_GROUP, Friend["BtAddGroup"], Point<int16_t>(68, 77));
		// r3：Angel 083 好友页仅 11 钮（OnCreate 立即数 @0x919b73）：顶部
		// BtAddFriend(5,81)/BtAddGroup(72,81)/BtShowOnline(232,81) + 底栏 8 钮。
		// BtPlusFriend/BtMessenger/BtSheetIClose/TapShowAll/TapShowOnline 均为
		// img 残留节点、原生不创建（R6：节点存在 ≠ 原生使用），一律不绑定。
		deactivate(Buttons::BT_FRIEND_EXPAND);
		deactivate(Buttons::BT_FRIEND_MESSENGER);
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
		// 底栏将被 bind_runtime_assets() 的 guild_grid(6/77/156/226 × 316/333/360) 覆盖。
		gi_btn(Buttons::BT_GUILD_INVITE, "BtInvite", Point<int16_t>(6, 316));
		gi_btn(Buttons::BT_GUILD_RANKUP, "BtUp", Point<int16_t>(6, 333));
		gi_btn(Buttons::BT_GUILD_WITHDRAW, "BtWithdraw", Point<int16_t>(77, 316));
		gi_btn(Buttons::BT_GUILD_RANKDOWN, "BtDown", Point<int16_t>(77, 333));
		gi_btn(Buttons::BT_GUILD_WHO, "BtWhere", Point<int16_t>(156, 316));
		gi_btn(Buttons::BT_GUILD_KICK, "BtKick", Point<int16_t>(156, 333));
		gi_btn(Buttons::BT_GUILD_WHISPER, "BtWhisper", Point<int16_t>(226, 316));
		gi_btn(Buttons::BT_GUILD_CHAT, "BtChat", Point<int16_t>(226, 333));
		gi_btn(Buttons::BT_GUILD_BBS, "BtGuildBBS", Point<int16_t>(77, 360));
		gi_btn(Buttons::BT_GUILD_PARTYINVITE, "BtPartyInvite", Point<int16_t>(156, 360));
		gi_btn(Buttons::BT_GUILD_INFO, "BtInfo", Point<int16_t>(226, 360));
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

		if (tab == Tab::FRIEND)
		{
			for (Sprite sprite : friend_sprites)
				sprite.update();

			// P0：好友计数/分组文案从 BuddyListModel 同步（handler 只写模型）。
			const BuddyListData& bl = BuddyListModel::get().data();
			const int32_t cap = bl.capacity > 0 ? bl.capacity : 25;
			const std::string cnt = "(" + std::to_string(bl.buddies.size())
				+ "/" + std::to_string(cap) + ")";
			if (friend_count_text.get_text() != cnt)
				friend_count_text.change_text(cnt);

			int online_n = 0;
			for (const auto& b : bl.buddies)
				if (b.online)
					online_n++;
			const std::string grp = Locale::text("userlist.friend_group")
				+ "(" + std::to_string(online_n) + "/" + std::to_string(bl.buddies.size()) + ")";
			if (friend_group_text.get_text() != grp)
				friend_group_text.change_text(grp);

			// 「目前位置 - <地图名>」随换图刷新（仅 mapid 变化时查一次 String.nx）。
			const int32_t mapid = Stage::get().get_mapid();
			if (mapid != friend_location_mapid)
			{
				friend_location_mapid = mapid;
				const std::string loc = Locale::text("userlist.location") + " - " + get_cur_location();
				if (friend_location_text.get_text() != loc)
					friend_location_text.change_text(loc);
			}
		}
		else if (tab == Tab::PARTY)
		{
			const Party& p = PartyModel::get().data();
			int online_n = 0;
			int total_n = 0;
			for (const auto& m : p.members)
			{
				if (m.id == 0)
					continue;
				total_n++;
				if (m.online)
					online_n++;
			}
			const int offline_n = total_n - online_n;
			const std::string on = std::to_string(online_n) + "/" + std::to_string(total_n);
			const std::string off = std::to_string(offline_n) + "/" + std::to_string(total_n);
			if (party_online_count_text.get_text() != on)
				party_online_count_text.change_text(on);
			if (party_offline_count_text.get_text() != off)
				party_offline_count_text.change_text(off);
		}
		else if (tab == Tab::GUILD)
		{
			// 家族回包只写 GuildModel；页签可能在回包之后才打开，故每帧把公告
			// 同步进 Text（仅在内容变化时重建布局，避免逐帧重排字形）。
			const GuildData& g = GuildModel::get().data();
			const std::string notice = g.notice.empty()
				? Locale::text("guild.notice_none") : g.notice;

			if (guild_notice_text.get_text() != notice)
				guild_notice_text.change_text(notice);
		}
		else if (tab == Tab::BLACKLIST)
		{
			// r3：拒绝列表计数（容量 26，oracle 帧「0/26」）。
			const std::string cnt =
				std::to_string(blacklist_names.size()) + "/26";
			if (blacklist_count_text.get_text() != cnt)
				blacklist_count_text.change_text(cnt);
		}
		else if (tab == Tab::UNION)
		{
			const AllianceData& a = AllianceModel::get().data();
			const std::string notice = a.notice.empty()
				? Locale::text("guild.notice_none") : a.notice;
			if (union_notice_text.get_text() != notice)
				union_notice_text.change_text(notice);

			int online_n = 0;
			for (const auto& m : a.members)
				if (m.online)
					online_n++;
			const std::string cnt = "[" + std::to_string(online_n) + "/"
				+ std::to_string(a.members.size()) + "]";
			if (union_count_text.get_text() != cnt)
				union_count_text.change_text(cnt);
		}
	}

	void UIUserList::load_blacklist()
	{
		// v83 CConfig::LoadBlackList@0x49e303：从本地配置读回，空格/非法名过滤。
		const std::string raw = Setting<BlackList>::get().load();
		blacklist_names.clear();
		size_t start = 0;
		while (start < raw.size() && blacklist_names.size() < BLACKLIST_CAPACITY)
		{
			const size_t comma = raw.find(',', start);
			const std::string name = raw.substr(start, comma == std::string::npos
				? std::string::npos : comma - start);
			if (!name.empty() && name.size() <= 12)
				blacklist_names.push_back(name);
			if (comma == std::string::npos)
				break;
			start = comma + 1;
		}
	}

	void UIUserList::save_blacklist() const
	{
		// v83 CConfig::SaveBlackList 对应：逗号分隔写回本地配置。
		std::string raw;
		for (size_t i = 0; i < blacklist_names.size(); ++i)
		{
			if (i) raw += ',';
			raw += blacklist_names[i];
		}
		Setting<BlackList>::get().save(raw);
	}

	int32_t UIUserList::self_guild_rank() const
	{
		// 玩家自身公会职位：从成员列表按 oid 取（s9 guildRank 1=会长,2=副会长,5=最低级；
		// GuildOperationHandler 管理操作要求 rank<=2）。找不到返回 6（视为无权限）。
		const int32_t self = Stage::get().get_player().get_oid();
		if (const GuildMember* m = GuildModel::get().find(self))
			return m->rank;
		return 6;
	}

	int32_t UIUserList::self_alliance_rank() const
	{
		// 联盟职位（s9 allianceRank 1=盟主,2=副盟主,5=最低级；
		// AllianceOperationHandler:68 要求 rank<=2）。找不到返回 6。
		const int32_t self = Stage::get().get_player().get_oid();
		if (const AllianceMember* m = AllianceModel::get().find(self))
			return m->alliance_rank;
		return 6;
	}

	void UIUserList::set_guild_buttons(bool on)
	{
		// r3：Angel 家族页空态是「4×3 按钮整组灰显仍可见」（oracle 家族帧），
		// 旧实现 set_active(false) 直接消失。灰显 = DISABLED（img 四态 disabled/0
		// 贴图齐全，UIElement::send_cursor 对 DISABLED 不派发 hover/click = 不可操作）。
		// Btnotice（右上笔形钮）不在灰显组 —— oracle 家族/联盟帧上常亮。
		static const uint16_t all[] = {
			Buttons::BT_GUILD_INVITE, Buttons::BT_GUILD_WITHDRAW, Buttons::BT_GUILD_KICK,
			Buttons::BT_GUILD_WHO, Buttons::BT_GUILD_WHISPER, Buttons::BT_GUILD_CHAT,
			Buttons::BT_GUILD_RANKUP, Buttons::BT_GUILD_RANKDOWN,
			Buttons::BT_GUILD_BBS, Buttons::BT_GUILD_PARTYINVITE, Buttons::BT_GUILD_INFO
		};

		for (uint16_t id : all)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(on ? Button::State::NORMAL : Button::State::DISABLED);
		}

		// 功能前置（s9 GuildOperationHandler 权威）：管理类按钮仅 rank<=2
		// （会长/副会长）可操作，普通成员灰掉，避免"点了被服务端拒"。
		// 邀请/踢人/升降职/公告属管理；退出/位置/悄悄话/对话/留言板/组队邀请/资料
		// 对所有成员开放。
		if (on)
		{
			const bool can_manage = self_guild_rank() <= 2;
			static const uint16_t manage_btns[] = {
				Buttons::BT_GUILD_INVITE, Buttons::BT_GUILD_KICK,
				Buttons::BT_GUILD_RANKUP, Buttons::BT_GUILD_RANKDOWN
			};
			for (uint16_t id : manage_btns)
			{
				auto it = buttons.find(id);
				if (it != buttons.end() && it->second)
					it->second->set_state(can_manage ? Button::State::NORMAL : Button::State::DISABLED);
			}
		}

		// Btnotice 笔形钮：会长/副会长可改公告（s9 case 0x0D），普通成员灰掉。
		if (auto it = buttons.find(Buttons::BT_GUILD_NOTICE); it != buttons.end() && it->second)
			it->second->set_state(on && self_guild_rank() <= 2 ? Button::State::NORMAL : Button::State::DISABLED);
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

	int32_t UIUserList::friend_row_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x()),
			static_cast<int16_t>(cursorpos.y() - position.y()));

		if (rel.x() < 12 || rel.x() > 280)
			return 0;
		if (rel.y() < FRIEND_ROW_Y)
			return 0;

		const int16_t row = static_cast<int16_t>((rel.y() - FRIEND_ROW_Y) / FRIEND_ROW_H);
		if (row < 0 || row >= FRIEND_MAX_ROWS)
			return 0;

		const BuddyListData& bl = BuddyListModel::get().data();
		const bool show_online_only = (friend_tab == Tab::FRIEND_ONLINE);
		int16_t drawn = 0;

		for (const auto& b : bl.buddies)
		{
			if (show_online_only && !b.online)
				continue;
			if (drawn == row)
				return b.cid;
			if (++drawn >= FRIEND_MAX_ROWS)
				break;
		}

		return 0;
	}

	int32_t UIUserList::party_row_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x()),
			static_cast<int16_t>(cursorpos.y() - position.y()));

		if (rel.x() < 12 || rel.x() > 280)
			return 0;

		const Party& p = PartyModel::get().data();

		auto hit = [&](bool online, int16_t rows_y, int16_t maxrows) -> int32_t
		{
			if (rel.y() < rows_y)
				return 0;

			const int16_t row = static_cast<int16_t>((rel.y() - rows_y) / PARTY_ROW_H);
			if (row < 0 || row >= maxrows)
				return 0;

			int16_t drawn = 0;
			for (const auto& m : p.members)
			{
				if (m.id == 0 || m.online != online)
					continue;
				if (drawn == row)
					return m.id;
				if (++drawn >= maxrows)
					break;
			}
			return 0;
		};

		if (int32_t id = hit(true, PARTY_ONLINE_Y, PARTY_ONLINE_ROWS))
			return id;
		return hit(false, PARTY_OFFLINE_Y, PARTY_OFFLINE_ROWS);
	}

	Cursor::State UIUserList::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 成员行不是 Button：点选只更新选中态，事件仍交回基类处理按钮/拖拽。
		// W1：顶层 tab 存 Tab::*（UNION=3 / BLACKLIST=4），禁止再跟 Buttons::BT_TAB_*
		// 比较——BT_TAB_BLACKLIST 仍为 3，会与 Tab::UNION 撞值导致拒绝列表行命中失效。
		if (clicked)
		{
			if (tab == Tab::GUILD)
			{
				if (int32_t cid = guild_row_at(cursorpos))
					guild_selected_cid = cid;
			}
			else if (tab == Tab::FRIEND)
			{
				if (int32_t cid = friend_row_at(cursorpos))
				{
					friend_selected_cid = cid;
					page_selected_row = cid;
					// 「对群」用当前选中好友的分组名（Angel StatusBar+3180）。
					if (const Buddy* b = BuddyListModel::get().find(cid))
						BuddyListModel::get().set_active_group(b->group);
					LOG(LOG_DEBUG, "UIUserList: friend select cid=" << cid);
					// r3：行选中变化 → 立即重算底栏行操作钮（Angel OnClick 同款联动）。
					update_friend_button_states();
				}
			}
			else if (tab == Tab::PARTY)
			{
				if (int32_t cid = party_row_at(cursorpos))
				{
					party_selected_cid = cid;
					page_selected_row = cid;
					LOG(LOG_DEBUG, "UIUserList: party select cid=" << cid);
				}
			}
			else if (tab == Tab::BLACKLIST)
			{
				if (int32_t row = blacklist_row_at(cursorpos))
				{
					blacklist_selected = (row == blacklist_selected) ? 0 : row;
					update_blacklist_button_states();
				}
			}
			else if (tab == Tab::UNION)
			{
				if (int32_t cid = union_row_at(cursorpos))
				{
					union_selected_cid = cid;
					page_selected_row = cid;
					update_union_button_states();
					LOG(LOG_DEBUG, "UIUserList: union select cid=" << cid);
				}
			}
		}

		return UIDragElement<PosUSERLIST>::send_cursor(clicked, cursorpos);
	}

	void UIUserList::draw_guild_tab(float) const
	{
		const GuildData& g = GuildModel::get().data();

		// 米色标题条烘焙在底图上，未入会时原生在其上写「没有参加的家族」。
		if (!g.in_guild())
			guild_none_text.draw(position + Point<int16_t>(115, 58));	// r3 oracle：文字起点 x115
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

			// 只画一张列头（guildinfo7）。guildinfo3/4 是第二套列头/分隔，
			// 叠在同行区上会「多画一层」（2026-11-11 用户家族页乱观感）。
			if (guild_col_header)
				guild_col_header.draw(position + Point<int16_t>(12, banner_y + 28));

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

		// r3 oracle：在线家族横幅顶 y103（像素扫描 blue rows 103-130）。
		draw_group(true, guild_online_banner, 103, GUILD_ONLINE_Y, GUILD_ONLINE_ROWS,
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
				// Angel SetWhisperTarget+StartChat：有名 → ChatBar 密语频道；无名 → 提示用物理键盘。
				// 禁止 emplace SoftKeyboard.img 软键盘弹窗。
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (pre.empty())
				{
					if (const Buddy* b = BuddyListModel::get().find(friend_selected_cid))
						pre = b->name;
				}
				if (!pre.empty())
				{
					if (auto chat = UI::get().get_element<UIChatBar>())
						chat->set_whisper_target(pre);
					else
						UI::get().emplace<UIChatBar>()->set_whisper_target(pre);
				}
				else
				{
					auto chatbar = UI::get().get_element<UIChatBar>();
					if (!chatbar)
						chatbar = UI::get().emplace<UIChatBar>();
					if (chatbar)
					{
						chatbar->toggle_input(true);
						chatbar->show_message(Locale::text("whisper.hint_target").c_str(),
							UIChatBar::MessageType::YELLOW);
					}
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
		// SoftKey 物理键口径：缺名/公告时展开 ChatBar + Locale 提示，禁止 SoftKeyboard 弹窗。
		auto guild_hint = [](const char* key)
		{
			const std::string msg = Locale::text(key);
			auto chatbar = UI::get().get_element<UIChatBar>();

			if (!chatbar)
				chatbar = UI::get().emplace<UIChatBar>();

			if (chatbar)
			{
				chatbar->toggle_input(true);
				chatbar->show_message(msg.c_str(), UIChatBar::MessageType::YELLOW);
			}
		};

		switch (buttonid)
		{
			case Buttons::BT_CLOSE:
				deactivate();
				break;
			case Buttons::BT_TAB_FRIEND:
				change_tab(Tab::FRIEND);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_PARTY:
				change_tab(Tab::PARTY);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_GUILD:
				change_tab(Tab::GUILD);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_UNION:
				change_tab(Tab::UNION);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_BLACKLIST:
				change_tab(Tab::BLACKLIST);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_PARTY_MINE:
				change_party_tab(Tab::PARTY_MINE);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_PARTY_SEARCH:
				change_party_tab(Tab::PARTY_SEARCH);
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
				change_friend_tab(Tab::FRIEND_ALL);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_FRIEND_ONLINE:
				change_friend_tab(Tab::FRIEND_ONLINE);
				return Button::State::PRESSED;
			case Buttons::BT_TAB_BLACKLIST_INDIVIDUAL:
			case Buttons::BT_TAB_BLACKLIST_GUILD:
				change_blacklist_tab(buttonid == Buttons::BT_TAB_BLACKLIST_INDIVIDUAL ? 0 : 1);
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
					// Angel CUtilDlgEx 输名 → UIEnterString（物理键 Textfield，禁 SoftKeyboard）。
					UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
						[](std::string name) {
							BuddyAddPacket(name, "Default Group").dispatch();
							LOG(LOG_DEBUG, "UIUserList: BuddyAdd target=" << name);
						}, 12);
				}
				break;
			}
			case Buttons::BT_FRIEND_ADD_GROUP:
			{
				// s9 BuddylistModify 无独立「建组」opcode；组名仅作本地筛选 + 后续加友 group 字段。
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_group"),
					[](std::string group) {
						BuddyListModel::get().set_active_group(group);
						LOG(LOG_DEBUG, "UIUserList: active buddy group=" << group);
					}, 13, true);
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
				if (pre.empty())
				{
					if (const Buddy* b = BuddyListModel::get().find(friend_selected_cid))
						pre = b->name;
				}
				if (!pre.empty()) {
					PartyInvitePacket(pre).dispatch();
					LOG(LOG_DEBUG, "UIUserList: PartyInvite target=" << pre);
					break;
				}
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						PartyInvitePacket(name).dispatch();
						LOG(LOG_DEBUG, "UIUserList: PartyInvite target=" << name);
					}, 12);
				break;
			}
			case Buttons::BT_PARTY_LEAVE:
				PartyLeavePacket().dispatch();
				LOG(LOG_DEBUG, "UIUserList: PartyLeave");
				break;
			case Buttons::BT_PARTY_SETTINGS:
			{
				// CharInfo 有 cid → Expel；有名无 cid → Deny；否则提示选中/物理键输入。
				int32_t from_cid = 0;
				std::string from_name;
				if (auto ci = UI::get().get_element<UICharInfo>()) {
					from_cid = ci->get_target_cid();
					from_name = ci->get_target_name();
				}
				if (from_cid <= 0 && party_selected_cid > 0)
					from_cid = party_selected_cid;
				if (from_cid > 0) {
					PartyExpelPacket(from_cid).dispatch();
					LOG(LOG_DEBUG, "UIUserList: PartyExpel cid=" << from_cid);
				} else if (!from_name.empty()) {
					DenyPartyRequestPacket(from_name).dispatch();
					LOG(LOG_DEBUG, "UIUserList: DenyParty from=" << from_name);
				} else {
					UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
						[](std::string name) {
							DenyPartyRequestPacket(name).dispatch();
							LOG(LOG_DEBUG, "UIUserList: DenyParty from=" << name);
						}, 12);
				}
				break;
			}
			case Buttons::BT_BLACKLIST_ADD:
			{
				// r3：按 Angel CTabBlackList::OnAdd 全流程做本地闭环——输入(≤12) →
				// 非自己 → 非重复 → 按名排序插入（容量 26）→ 计数联动。s9 无黑名单
				// 协议（BuddylistModifyHandler 仅 mode 1/2/3），FindPlayer 仍作检索
				// 代理（P3 红线：禁止发明拉黑包）。
				auto add_local = [this](const std::string& name)
				{
					if (name.empty() || name.size() > 12)
						return;

					if (name == Stage::get().get_player().get_name())
					{
						// Angel SP_726「YOU MAY NOT ENTER YOURSELF IN THE BLACKLIST」
						if (auto chatbar = UI::get().get_element<UIChatBar>())
							chatbar->show_message("Blacklist: cannot add yourself.", UIChatBar::MessageType::YELLOW);
						return;
					}

					for (const auto& existing : blacklist_names)
						if (_stricmp(existing.c_str(), name.c_str()) == 0)
						{
							// Angel SP_728「THIS NAME HAS ALREADY BEEN REGISTERED」
							if (auto chatbar = UI::get().get_element<UIChatBar>())
								chatbar->show_message(("Blacklist: " + name + " already registered.").c_str(),
									UIChatBar::MessageType::YELLOW);
							return;
						}

					if (blacklist_names.size() >= BLACKLIST_CAPACITY)
						return;

					auto pos = std::lower_bound(blacklist_names.begin(), blacklist_names.end(), name,
						[](const std::string& a, const std::string& b) { return _stricmp(a.c_str(), b.c_str()) < 0; });
					blacklist_names.insert(pos, name);
					save_blacklist(); // v83 CConfig::SaveBlackList

					if (auto chatbar = UI::get().get_element<UIChatBar>())
						chatbar->show_message(("Blacklist: " + name).c_str(), UIChatBar::MessageType::YELLOW);
				};

				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					add_local(pre);
					FindPlayerPacket(pre).dispatch();
				} else {
					UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
						[add_local](std::string name) {
							add_local(name);
							FindPlayerPacket(name).dispatch();
						}, 12);
				}
				break;
			}
			case Buttons::BT_BLACKLIST_DELETE:
			{
				// r3：本地删除选中行（1 基行号 → 0 基下标）。
				if (blacklist_selected > 0
					&& static_cast<size_t>(blacklist_selected) <= blacklist_names.size())
				{
					blacklist_names.erase(blacklist_names.begin() + (blacklist_selected - 1));
					save_blacklist(); // v83 CConfig::SaveBlackList
					blacklist_selected = 0;
					update_blacklist_button_states();
				}
				break;
			}
			case Buttons::BT_GUILD_NOTICE:
			{
				// s9 GuildOperationHandler case 0x10：0x10 + string（限 100 字）。
				// Angel CUtilDlgEx INPUT_STR → UIEnterString（禁 SoftKeyboard）。
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_notice"),
					[](std::string notice) {
						GuildNoticePacket(notice).dispatch();
						LOG(LOG_DEBUG, "UIUserList: GuildNotice len=" << notice.size());
					}, 100, true);
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

				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						GuildInvitePacket(name).dispatch();
						LOG(LOG_DEBUG, "UIUserList: GuildInvite target=" << name);
					}, 12);
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
						// Angel SetWhisperTarget+StartChat → ChatBar 密语频道（物理键盘输入正文）。
						const std::string name = m->name;
						if (auto chat = UI::get().get_element<UIChatBar>())
							chat->set_whisper_target(name);
						else
							UI::get().emplace<UIChatBar>()->set_whisper_target(name);
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
				// r3：Angel BtShowOnline（2013）是单钮 toggle（全部↔仅在线），非双页签。
				change_friend_tab(friend_tab == Tab::FRIEND_ALL
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
			case Buttons::BT_FRIEND_CHAT:
				// Angel BtChat(2016) → Messenger::TryNew + SendInviteMsg
				button_pressed(Buttons::BT_FRIEND_MESSENGER);
				break;
			case Buttons::BT_FRIEND_WHISPER:
			case Buttons::BT_FRIEND_MESSAGE:
			case Buttons::BT_FRIEND_GROUP_WHISPER:
			case Buttons::BT_PARTY_WHISPER:
			case Buttons::BT_UNION_WHISPER:
			{
				// Angel SetWhisperTarget+StartChat → ChatBar::set_whisper_target；
				// 无选中名：展开 ChatBar + 提示，物理键盘输入对象（禁 SoftKey 弹窗）。
				std::string pre;
				if (buttonid == Buttons::BT_PARTY_WHISPER)
				{
					if (const PartyMember* m = PartyModel::get().find(party_selected_cid))
						pre = m->name;
				}
				else if (buttonid == Buttons::BT_UNION_WHISPER)
				{
					if (const AllianceMember* m = AllianceModel::get().find(union_selected_cid))
						pre = m->name;
				}
				else if (const Buddy* b = BuddyListModel::get().find(friend_selected_cid))
				{
					pre = b->name;
				}

				if (!pre.empty())
				{
					if (auto chat = UI::get().get_element<UIChatBar>())
						chat->set_whisper_target(pre);
					else
						UI::get().emplace<UIChatBar>()->set_whisper_target(pre);
					break;
				}

				{
					auto chatbar = UI::get().get_element<UIChatBar>();
					if (!chatbar)
						chatbar = UI::get().emplace<UIChatBar>();
					if (chatbar)
					{
						chatbar->toggle_input(true);
						chatbar->show_message(Locale::text("whisper.hint_target").c_str(),
							UIChatBar::MessageType::YELLOW);
					}
				}
				break;
			}
			case Buttons::BT_FRIEND_WHERE:
			case Buttons::BT_FRIEND_BLOCK:
			case Buttons::BT_FRIEND_UNBLOCK:
			case Buttons::BT_UNION_WHERE:
			{
				// P3 ⊘ 闭环：s9 BuddylistModifyHandler 仅 mode 1/2/3（add/accept/delete），
				// 无 Ignore/黑名单 opcode → 保持 FindPlayer 检索代理；禁止发明拉黑协议。
				if (buttonid == Buttons::BT_FRIEND_WHERE)
				{
					if (const Buddy* b = BuddyListModel::get().find(friend_selected_cid))
					{
						FindPlayerPacket(b->name).dispatch();
						break;
					}
				}
				if (buttonid == Buttons::BT_UNION_WHERE)
				{
					if (const AllianceMember* m = AllianceModel::get().find(union_selected_cid))
					{
						FindPlayerPacket(m->name).dispatch();
						break;
					}
					guild_hint("guild.need_select");
					break;
				}

				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						FindPlayerPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIUserList: FindPlayer target=" << name);
					}, 12);
				break;
			}
			case Buttons::BT_FRIEND_DELETE2:
			{
				if (friend_selected_cid > 0)
				{
					BuddyDeletePacket(friend_selected_cid).dispatch();
					LOG(LOG_DEBUG, "UIUserList: BuddyDelete selected cid=" << friend_selected_cid);
					break;
				}

				guild_hint("guild.need_select");
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
				if (party_selected_cid > 0)
				{
					if (leader)
						PartyChangeLeaderPacket(party_selected_cid).dispatch();
					else
						PartyExpelPacket(party_selected_cid).dispatch();
					break;
				}
				guild_hint("guild.need_select");
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
			{
				// r3：BtHP（Angel 2207 sub_90E51C）＝组员页血条显示开关（CUIPartyHP
				// 独立悬浮窗的共用窗内实现）。
				party_hp_visible = !party_hp_visible;
				break;
			}
			case Buttons::BT_GUILD_CHANGE:
				guild_hint("userlist.need_data");
				break;
			case Buttons::BT_UNION_INVITE:
			{
				// Angel sub_914CFE @0x914cfe：CUtilDlgEx 输公会名 → Encode1(3)+EncodeStr。
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_guild_name"),
					[](std::string gname) {
						AllianceInvitePacket(gname).dispatch();
						LOG(LOG_DEBUG, "UIUserList: AllianceInvite guild=" << gname);
					}, 14, true);
				break;
			}
			case Buttons::BT_UNION_NOTICE:
			{
				// Angel sub_915214 @0x915214：Encode1(10)+EncodeStr（限 100）。
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_notice"),
					[](std::string notice) {
						AllianceNoticePacket(notice).dispatch();
						LOG(LOG_DEBUG, "UIUserList: AllianceNotice len=" << notice.size());
					}, 100, true);
				break;
			}
			case Buttons::BT_UNION_WITHDRAW:
			{
				// s9 / UIAlliance BtLeave：AllianceActionPacket(0x02) 退盟。
				if (!AllianceModel::get().data().in_alliance())
				{
					guild_hint("userlist.need_data");
					break;
				}
				AllianceActionPacket(0x02).dispatch();
				LOG(LOG_DEBUG, "UIUserList: Alliance leave (0x02)");
				break;
			}
			case Buttons::BT_UNION_PARTYINVITE:
			{
				// Angel CTabGuildAlliance::OnPartyInvite @0x9153dc → SendJoinPartyMsg。
				if (const AllianceMember* m = AllianceModel::get().find(union_selected_cid))
				{
					if (!m->name.empty())
					{
						PartyInvitePacket(m->name).dispatch();
						break;
					}
				}
				guild_hint("guild.need_select");
				break;
			}
			case Buttons::BT_UNION_INFO:
			{
				// Angel CUIUserList OnButtonClicked case 0x809 @0x91d29d：
				// ToggleAllianceGrade（位图 GuildUnion/BtInfo @≈(238,363)）。
				// 与 BtChange(0x80C ChangeMaster) / BtUp·BtDown(0x09 成员职级) 不同。
				if (!AllianceModel::get().data().in_alliance())
				{
					guild_hint("userlist.union_none");
					break;
				}
				if (auto existing = UI::get().get_element<UIAllianceGrade>())
				{
					existing->deactivate();
					break;
				}
				UI::get().emplace<UIAllianceGrade>();
				LOG(LOG_DEBUG, "UIUserList: ToggleAllianceGrade open");
				break;
			}
			case Buttons::BT_UNION_KICK:
			{
				// Angel sub_914EB4 + s9 0x06：guildid + allianceid（驱逐选中成员所属公会）。
				const AllianceMember* m = AllianceModel::get().find(union_selected_cid);
				const AllianceData& a = AllianceModel::get().data();
				if (!m || m->guild_id <= 0 || !a.in_alliance())
				{
					guild_hint("guild.need_select");
					break;
				}
				if (m->id == Stage::get().get_player().get_oid())
				{
					guild_hint("userlist.need_data");
					break;
				}
				AllianceKickPacket(m->guild_id, a.id).dispatch();
				LOG(LOG_DEBUG, "UIUserList: AllianceKick guild=" << m->guild_id
					<< " alliance=" << a.id);
				break;
			}
			case Buttons::BT_UNION_CHANGE:
			{
				// Angel sub_915983 / s9 0x07：Encode4(cid) 转让盟主。
				const AllianceMember* m = AllianceModel::get().find(union_selected_cid);
				if (!m || m->id <= 0)
				{
					guild_hint("guild.need_select");
					break;
				}
				if (m->id == Stage::get().get_player().get_oid())
				{
					guild_hint("userlist.need_data");
					break;
				}
				AllianceChangeMasterPacket(m->id).dispatch();
				LOG(LOG_DEBUG, "UIUserList: AllianceChangeMaster cid=" << m->id);
				break;
			}
			case Buttons::BT_UNION_UP:
			case Buttons::BT_UNION_DOWN:
			{
				// Angel sub_91505B / s9 0x09：cid + raise(1=升/0=降)。
				const AllianceMember* m = AllianceModel::get().find(union_selected_cid);
				if (!m || m->id <= 0)
				{
					guild_hint("guild.need_select");
					break;
				}
				if (m->id == Stage::get().get_player().get_oid())
				{
					guild_hint("userlist.need_data");
					break;
				}
				const bool raise = (buttonid == Buttons::BT_UNION_UP);
				AllianceGradeChangePacket(m->id, raise).dispatch();
				LOG(LOG_DEBUG, "UIUserList: AllianceGrade "
					<< (raise ? "up" : "down") << " cid=" << m->id);
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

		// W1.5：切入联盟页且家族已挂 allianceId、模型尚空 → 用既有 0x01 拉全量。
		if (tabid == Tab::UNION
			&& !AllianceModel::get().data().in_alliance()
			&& GuildModel::get().data().allianceId > 0)
		{
			AllianceActionPacket(0x01).dispatch();
			LOG(LOG_DEBUG, "UIUserList: UNION tab request AllianceInfo (0x01)");
		}

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

		if (tab == Tab::PARTY)
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

		if (tab == Tab::FRIEND)
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

		if (tab == Tab::BLACKLIST)
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
		auto party_sub_btn = [](uint8_t t) -> uint16_t
		{
			return (t == Tab::PARTY_SEARCH)
				? Buttons::BT_TAB_PARTY_SEARCH
				: Buttons::BT_TAB_PARTY_MINE;
		};

		if (oldtab != party_tab)
			set_st(party_sub_btn(oldtab), Button::State::NORMAL);

		set_st(party_sub_btn(party_tab), Button::State::PRESSED);
		set_act(Buttons::BT_PARTY_SEARCH_LEVEL, party_tab == Tab::PARTY_SEARCH);
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
		auto friend_sub_btn = [](uint8_t t) -> uint16_t
		{
			return (t == Tab::FRIEND_ONLINE)
				? Buttons::BT_TAB_FRIEND_ONLINE
				: Buttons::BT_TAB_FRIEND_ALL;
		};

		if (oldtab != friend_tab)
			set_st(friend_sub_btn(oldtab), Button::State::NORMAL);

		set_st(friend_sub_btn(friend_tab), Button::State::PRESSED);
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
		auto bl_sub_btn = [](uint8_t t) -> uint16_t
		{
			return (t == 1)
				? Buttons::BT_TAB_BLACKLIST_GUILD
				: Buttons::BT_TAB_BLACKLIST_INDIVIDUAL;
		};

		if (oldtab != blacklist_tab)
			set_st(bl_sub_btn(oldtab), Button::State::NORMAL);

		set_st(bl_sub_btn(blacklist_tab), Button::State::PRESSED);
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
		// 家族 / 联盟底栏：正确截图（家族联盟.png / 好友…拒绝列表.png）Tab0 对齐后
		// 量得 4 列 x≈6/77/156/226、3 行 y≈316/333/360（旧 14/84/156/232 × 316/338/360
		// 首列偏右 8px、中行偏下 5px）。
		auto guild_grid = [](uint8_t i) -> Point<int16_t>
		{
			static const int16_t xs[4] = { 6, 77, 156, 226 };
			static const int16_t ys[3] = { 316, 333, 360 };
			return Point<int16_t>(xs[i % 4], ys[(i / 4) % 3]);
		};
		struct NodeBtn { uint16_t id; const char* node; };
		auto bind_grid = [&](nl::node parent, const NodeBtn* defs, uint8_t count,
			Point<int16_t>(*pos_of)(uint8_t))
		{
			for (uint8_t i = 0; i < count; i++)
				bind_btn(defs[i].id, parent, defs[i].node, pos_of(i));
		};

		// 好友页 Friend（38 个直接子节点，旧代码只用了 4 个）
		friend_banner = bitmap(Friend, "friend0");
		friend_group_bar = bitmap(Friend, "friend4");
		friend_col_header = bitmap(Friend, "friend1");
		friend_col_header2 = bitmap(Friend, "friend5");
		friend_separator = bitmap(Friend, "friend3");
		friend_line = bitmap(Friend, "friend6");
		// r15/r27 oracle：BtAddFriend(1,77) / BtAddGroup(68,77) / BtShowOnline(228,77)。
		// 旧 (14/80/150/215, 80) 使 65px 宽钮横向重叠 9px，且整体偏下 3px。
		bind_btn(BT_FRIEND_ADD, Friend, "BtAddFriend", Point<int16_t>(1, 77));
		bind_btn(BT_FRIEND_ADD_GROUP, Friend, "BtAddGroup", Point<int16_t>(68, 77));
		// r3 live 实测补绑：BtShowOnline（2013）在线筛选单钮 toggle，
		// Angel oracle 好友帧 @228,77（与 BtAddFriend/BtAddGroup 同排）。
		bind_btn(BT_FRIEND_SHOW_ONLINE, Friend, "BtShowOnline", Point<int16_t>(228, 77));
		// r3：Angel 083 好友页无 BtShowAll（OnCreate 只创建 BtShowOnline 2013，
		// 在线筛选是单钮 toggle），img 的 BtShowAll 节点为残留、不再绑定。
		// 好友底栏（Angel OnCreate 立即数 + oracle 帧基准平移 (-4,-4)，同组员/黑名单页）：
		//   上排 y=337：BtGroupWhisper(8,341)→(4,337) BtChat(99,341)→(95,337) BtWhisper(254,341)→(250,337)
		//   下排 y=358：BtMod(8,362)→(4,358) BtDelete(49,362)→(45,358) BtParty(99,362)→(95,358)
		//              BtBlock(182,362)→(178,358) BtMessage(254,362)→(250,358)
		// BtMate/BtWhere/BtAdd/GroupMod 是 img 残留（原生好友页不创建），不绑定。
		struct FriendPos { uint16_t id; const char* node; Point<int16_t> pos; };
		static const FriendPos friend_btns[] = {
			{ BT_FRIEND_GROUP_WHISPER, "BtGroupWhisper", Point<int16_t>(4, 337) },
			{ BT_FRIEND_CHAT, "BtChat", Point<int16_t>(95, 337) },
			{ BT_FRIEND_WHISPER, "BtWhisper", Point<int16_t>(250, 337) },
			{ BT_FRIEND_MOD, "BtMod", Point<int16_t>(4, 358) },
			{ BT_FRIEND_DELETE2, "BtDelete", Point<int16_t>(45, 358) },
			{ BT_FRIEND_PARTY, "BtParty", Point<int16_t>(95, 358) },
			{ BT_FRIEND_BLOCK, "BtBlock", Point<int16_t>(178, 358) },
			{ BT_FRIEND_MESSAGE, "BtMessage", Point<int16_t>(250, 358) }
		};
		for (const auto& fb : friend_btns)
			bind_btn(fb.id, Friend, fb.node, fb.pos);
		// BtUnBlock 与 BtBlock 同位（Angel SP_2103/SP_2104 贴图切换），创建但默认不显示：
		// 本地无 buddy block 位数据（s9 BuddylistModifyHandler 无 Ignore opcode），
		// BtBlock 走检索代理，BtUnBlock 仅在拿到 block 态时接管（update_friend_button_states）。
		bind_btn(BT_FRIEND_UNBLOCK, Friend, "BtUnBlock", Point<int16_t>(178, 358));
		if (auto it = buttons.find(BT_FRIEND_UNBLOCK); it != buttons.end() && it->second)
			it->second->set_active(false);

		// 组员页：组员列表框.png（Tab0 对齐）模板 mae≈0 ——
		// Create(9,351)；2×4 格 Search/HP/Kick/Whisper @ y344、Invite/Boss/Withdraw/Chat @ y361，
		// 列 x=68/127/186/245（步长 59）。旧 4×3@(14,316) 整排上偏约 28–35px。
		party_online_banner = bitmap(Party, "party0");
		party_offline_banner = bitmap(Party, "party4");
		party_col_header = bitmap(Party, "party1");
		party_col_header2 = bitmap(Party, "party5");
		party_separator = bitmap(Party, "party2");
		struct PartyPos { uint16_t id; const char* node; Point<int16_t> pos; };
		static const PartyPos party_btns[] = {
			{ BT_PARTY_CREATE2, "BtCreate", Point<int16_t>(9, 351) },
			{ BT_PARTY_SEARCH2, "BtSearch", Point<int16_t>(68, 344) },
			{ BT_PARTY_HP, "BtHP", Point<int16_t>(127, 344) },
			{ BT_PARTY_KICK, "BtKick", Point<int16_t>(186, 344) },
			{ BT_PARTY_WHISPER, "BtWhisper", Point<int16_t>(245, 344) },
			{ BT_PARTY_INVITE2, "BtInvite", Point<int16_t>(68, 361) },
			{ BT_PARTY_CHANGE_BOSS, "BtChangeBoss", Point<int16_t>(127, 361) },
			{ BT_PARTY_WITHDRAW, "BtWithdraw", Point<int16_t>(186, 361) },
			{ BT_PARTY_CHAT, "BtChat", Point<int16_t>(245, 361) }
		};
		for (const auto& pb : party_btns)
			bind_btn(pb.id, Party, pb.node, pb.pos);

		// r3：组员页血条（083 CUIPartyHP 画法并入共用窗，资源与 UIParty 同源）：
		// GaugeBar { bar 63x7（origin -3,-3 自动内缩）/ graduation 69x13 刻度外框 }。
		{
			nl::node gaugebar = Party["PartyHP"]["GaugeBar"];
			partyhp_bar = Texture(gaugebar["bar"]);
			partyhp_graduation = Texture(gaugebar["graduation"]);
		}

		// 拒绝列表：BtAdd mae=0 @(56,351)；Delete 同高约 @(175,350)（旧 60/160,352）。
		blacklist_banner = bitmap(BlackList, "blacklist0");
		blacklist_col_header = bitmap(BlackList, "blacklist1");
		blacklist_separator = bitmap(BlackList, "blacklist2");
		// r3 oracle：增加(61,356)、删除(180,356)（黄/灰钮像素扫描）。
		bind_btn(BT_BLACKLIST_ADD, BlackList, "BtAdd", Point<int16_t>(61, 356));
		bind_btn(BT_BLACKLIST_DELETE, BlackList, "BtDelete", Point<int16_t>(180, 356));

		// 家族联盟 GuildUnion（20 个直接子节点，旧代码 0 个）
		union_notice_bar = bitmap(GU, "guildinfo6");
		union_banner = bitmap(GU, "guildinfo0");
		union_col_header = bitmap(GU, "guildinfo2");
		union_col_header2 = bitmap(GU, "guildinfo3");
		union_separator = bitmap(GU, "guildinfo4");
		// r3 oracle：联盟底栏 row1 = 邀请/出去/联盟长/悄悄话，row2 = 地位/地位/退出/对话，
		// row3 = 邀请组队@156 / 资料@226（首两槽空）；BtWhere 原生不摆、不绑定；
		// Btnotice 笔形钮 @(287,78)（与家族页 BtNotice 同位），不占 grid 格。
		static const NodeBtn union_btns[] = {
			{ BT_UNION_INVITE, "BtInvite" },
			{ BT_UNION_WITHDRAW, "BtWithdraw" },
			{ BT_UNION_CHANGE, "BtChange" },
			{ BT_UNION_WHISPER, "BtWhisper" },
			{ BT_UNION_UP, "BtUp" },
			{ BT_UNION_DOWN, "BtDown" },
			{ BT_UNION_KICK, "BtKick" },
			{ BT_UNION_CHAT, "BtChat" },
			{ BT_UNION_WHERE, "BtWhereX" },
			{ BT_UNION_PARTYINVITE, "BtPartyInvite" },
			{ BT_UNION_INFO, "BtInfo" },
			{ BT_UNION_NOTICE, "BtnoticeX" }
		};
		bind_grid(GU, union_btns, 12, guild_grid);
		bind_btn(BT_UNION_NOTICE, GU, "Btnotice", Point<int16_t>(287, 78));

		// 家族页补齐 + 12 按钮 4×3（列/行见 guild_grid）
		guild_col_header2 = bitmap(GI, "guildinfo3");
		guild_separator = bitmap(GI, "guildinfo4");
		guild_mark = bitmap(GI, "guildmark");
		// r3 oracle：第三行 = 留言板@77 / 邀请组队@156 / 资料@226，首槽 (6,360) 空
		//（BtBoard 原生不摆，不绑定；grid 序用 BOARD 占位会被 bind_btn 跳过）。
		static const NodeBtn guild_btns[] = {
			{ BT_GUILD_INVITE, "BtInvite" },
			{ BT_GUILD_WITHDRAW, "BtWithdraw" },
			{ BT_GUILD_WHO, "BtWhere" },
			{ BT_GUILD_WHISPER, "BtWhisper" },
			{ BT_GUILD_RANKUP, "BtUp" },
			{ BT_GUILD_RANKDOWN, "BtDown" },
			{ BT_GUILD_KICK, "BtKick" },
			{ BT_GUILD_CHAT, "BtChat" },
			// BtBoard 占位槽 (6,360)：oracle 该格为空，用不存在节点名使其跳过绑定。
			{ BT_GUILD_BOARD, "BtBoardX" },
			{ BT_GUILD_BBS, "BtGuildBBS" },
			{ BT_GUILD_PARTYINVITE, "BtPartyInvite" },
			{ BT_GUILD_INFO, "BtInfo" }
		};
		bind_grid(GI, guild_btns, 12, guild_grid);

		auto label = [](Color::Name c)
		{
			return Text(Text::Font::A11M, Text::Alignment::LEFT, c, "");
		};
		friend_count_text = label(Color::Name::WHITE);
		friend_group_text = label(Color::Name::MEDIUMBLUE);	// r3 oracle：分组头蓝字
		friend_none_text = label(Color::Name::BLACK);
		friend_location_text = label(Color::Name::BLACK);
		party_online_count_text = label(Color::Name::WHITE);
		party_offline_count_text = label(Color::Name::WHITE);
		party_none_text = label(Color::Name::BLACK);
		blacklist_count_text = label(Color::Name::WHITE);
		blacklist_none_text = label(Color::Name::BLACK);
		union_none_text = label(Color::Name::BLACK);
		union_notice_text = label(Color::Name::BLACK);
		union_count_text = label(Color::Name::WHITE);
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
		union_count_text.change_text("[0/0]");
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
		// r3：按页签整组切换（Angel CUIUserList::SetButton @0x91e073 的对位实现）。
		// r3 live 实测修正（2026-11-16 真机截图）：底栏按钮分属五页，必须先整组
		// 隐藏、再按当前页显示+置态 —— 旧实现只改状态不切 active，五页按钮同屏叠绘。
		static const uint16_t friend_bottom[] = {
			BT_FRIEND_GROUP_WHISPER, BT_FRIEND_CHAT, BT_FRIEND_WHISPER,
			BT_FRIEND_MOD, BT_FRIEND_DELETE2, BT_FRIEND_PARTY,
			BT_FRIEND_BLOCK, BT_FRIEND_MESSAGE
		};
		static const uint16_t friend_top[] = { BT_FRIEND_SHOW_ONLINE };
		static const uint16_t party_btns[] = {
			BT_PARTY_CREATE2, BT_PARTY_INVITE2, BT_PARTY_WITHDRAW,
			BT_PARTY_KICK, BT_PARTY_CHANGE_BOSS, BT_PARTY_SEARCH2,
			BT_PARTY_HP, BT_PARTY_WHISPER, BT_PARTY_CHAT
		};
		static const uint16_t union_btns[] = {
			BT_UNION_INVITE, BT_UNION_WITHDRAW, BT_UNION_WHERE, BT_UNION_WHISPER,
			BT_UNION_UP, BT_UNION_DOWN, BT_UNION_KICK, BT_UNION_CHAT,
			BT_UNION_PARTYINVITE, BT_UNION_INFO, BT_UNION_CHANGE, BT_UNION_NOTICE
		};
		static const uint16_t guild_btns[] = {
			BT_GUILD_INVITE, BT_GUILD_WITHDRAW, BT_GUILD_WHO, BT_GUILD_WHISPER,
			BT_GUILD_RANKUP, BT_GUILD_RANKDOWN, BT_GUILD_KICK, BT_GUILD_CHAT,
			BT_GUILD_BBS, BT_GUILD_PARTYINVITE, BT_GUILD_INFO, BT_GUILD_BOARD
		};
		static const uint16_t blacklist_btns[] = { BT_BLACKLIST_ADD, BT_BLACKLIST_DELETE };

		auto st = [&](uint16_t id, Button::State s)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(s);
		};
		auto act = [&](uint16_t id, bool a)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_active(a);
		};
		auto group = [&](const uint16_t* ids, size_t n, bool visible, Button::State s)
		{
			for (size_t i = 0; i < n; i++)
			{
				act(ids[i], visible);
				st(ids[i], s);
			}
		};

		// 1) 五页底栏全隐藏并复位 NORMAL（重显示时由下方按页重置态）。
		group(friend_bottom, std::size(friend_bottom), false, Button::State::NORMAL);
		group(friend_top, std::size(friend_top), false, Button::State::NORMAL);
		group(party_btns, std::size(party_btns), false, Button::State::NORMAL);
		group(union_btns, std::size(union_btns), false, Button::State::NORMAL);
		group(guild_btns, std::size(guild_btns), false, Button::State::NORMAL);
		group(blacklist_btns, std::size(blacklist_btns), false, Button::State::NORMAL);

		// 2) 当前页显示 + 置态（行级规则由 update_*_button_states 收窄）。
		switch (page)
		{
			case Tab::FRIEND:
				group(friend_top, std::size(friend_top), true, Button::State::NORMAL);
				group(friend_bottom, std::size(friend_bottom), true, Button::State::NORMAL);
				// 顶部钮常亮（Angel SetButton 空态下 BtAddFriend/BtAddGroup/BtShowOnline 可点）。
				// 底栏 8 钮：无选中行 → 整组 DISABLED（CTabFriend::OnClick 规则）。
				update_friend_button_states();
				break;
			case Tab::PARTY:
				// Angel 组员页无行级 disable 函数 → SetButton 语义 = 整组 enable；
				// 已有队伍时 BtCreate 置灰（update_party_button_states）。
				group(party_btns, std::size(party_btns), true, Button::State::NORMAL);
				update_party_button_states();
				break;
			case Tab::GUILD:
			{
				// 未入会整组灰（oracle 家族帧），Btnotice 常亮（set_guild_buttons 处理）。
				const bool in_guild = !GuildModel::get().data().name.empty();
				group(guild_btns, std::size(guild_btns), true,
					in_guild ? Button::State::NORMAL : Button::State::DISABLED);
				set_guild_buttons(in_guild);
				break;
			}
			case Tab::UNION:
				group(union_btns, std::size(union_btns), true, Button::State::NORMAL);
				update_union_button_states();
				break;
			case Tab::BLACKLIST:
				group(blacklist_btns, std::size(blacklist_btns), true, Button::State::NORMAL);
				update_blacklist_button_states();
				break;
		}
	}

	void UIUserList::update_friend_button_states()
	{
		// r3：Angel CTabFriend::OnClick @0x91dba9 行级 disabled 规则
		// （好友数组 index：[4]Mod [5]Delete [6]Chat [7]Whisper [8]Message [9]Party [10]Block）：
		//   在线    → 7 行钮全亮
		//   离线    → {Chat,Whisper,Party} 亮（Angel {6,7,9}），其余灰
		//   被屏蔽  → 只 {Message} 亮 + BtBlock→BtUnBlock 贴图切换（本地无 block 位，
		//             此分支仅当模型携带 block 态时生效；当前无数据，不触发）
		//   无选中  → 7 行钮全灰（oracle 好友帧空态）
		const Buddy* b = friend_selected_cid > 0
			? BuddyListModel::get().find(friend_selected_cid) : nullptr;

		auto st = [&](uint16_t id, Button::State s)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(s);
		};

		if (!b)
		{
			static const uint16_t rowbtns[] = {
				BT_FRIEND_MOD, BT_FRIEND_DELETE2, BT_FRIEND_CHAT, BT_FRIEND_WHISPER,
				BT_FRIEND_MESSAGE, BT_FRIEND_PARTY, BT_FRIEND_BLOCK
			};
			for (uint16_t id : rowbtns)
				st(id, Button::State::DISABLED);
			return;
		}

		const bool blocked = false;	// s9 无 buddy block 数据位（见 notes r1 §7-4）
		st(BT_FRIEND_UNBLOCK, Button::State::NORMAL);

		if (blocked)
		{
			st(BT_FRIEND_MESSAGE, Button::State::NORMAL);
			st(BT_FRIEND_MOD, Button::State::DISABLED);
			st(BT_FRIEND_DELETE2, Button::State::DISABLED);
			st(BT_FRIEND_CHAT, Button::State::DISABLED);
			st(BT_FRIEND_WHISPER, Button::State::DISABLED);
			st(BT_FRIEND_PARTY, Button::State::DISABLED);
			st(BT_FRIEND_BLOCK, Button::State::DISABLED);
			if (auto it = buttons.find(BT_FRIEND_UNBLOCK); it != buttons.end() && it->second)
			{
				it->second->set_active(true);
				it->second->set_state(Button::State::NORMAL);
			}
			if (auto it = buttons.find(BT_FRIEND_BLOCK); it != buttons.end() && it->second)
				it->second->set_active(false);
			return;
		}

		if (auto it = buttons.find(BT_FRIEND_UNBLOCK); it != buttons.end() && it->second)
			it->second->set_active(false);
		if (auto it = buttons.find(BT_FRIEND_BLOCK); it != buttons.end() && it->second)
			it->second->set_active(true);

		if (b->online)
		{
			st(BT_FRIEND_MOD, Button::State::NORMAL);
			st(BT_FRIEND_DELETE2, Button::State::NORMAL);
			st(BT_FRIEND_CHAT, Button::State::NORMAL);
			st(BT_FRIEND_WHISPER, Button::State::NORMAL);
			st(BT_FRIEND_MESSAGE, Button::State::NORMAL);
			st(BT_FRIEND_PARTY, Button::State::NORMAL);
			st(BT_FRIEND_BLOCK, Button::State::NORMAL);
		}
		else
		{
			// 离线（Angel {6,7,9} = Chat/Whisper/Party 仍亮）
			st(BT_FRIEND_CHAT, Button::State::NORMAL);
			st(BT_FRIEND_WHISPER, Button::State::NORMAL);
			st(BT_FRIEND_PARTY, Button::State::NORMAL);
			st(BT_FRIEND_MOD, Button::State::DISABLED);
			st(BT_FRIEND_DELETE2, Button::State::DISABLED);
			st(BT_FRIEND_MESSAGE, Button::State::DISABLED);
			st(BT_FRIEND_BLOCK, Button::State::DISABLED);
		}
	}

	void UIUserList::update_party_button_states()
	{
		// Angel 组员页无行级规则（CTabParty 无 SetButton 更新函数）→ 整组亮。
		// BtCreate 在已有队伍时置灰（创建无意义），其余保持可点（服务端校验权限）。
		const Party& p = PartyModel::get().data();

		static const uint16_t all[] = {
			BT_PARTY_INVITE2, BT_PARTY_WITHDRAW, BT_PARTY_KICK, BT_PARTY_CHANGE_BOSS,
			BT_PARTY_SEARCH2, BT_PARTY_HP, BT_PARTY_WHISPER, BT_PARTY_CHAT
		};
		for (uint16_t id : all)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(Button::State::NORMAL);
		}

		if (auto it = buttons.find(BT_PARTY_CREATE2); it != buttons.end() && it->second)
			it->second->set_state(p.in_party() ? Button::State::DISABLED : Button::State::NORMAL);
	}

	void UIUserList::update_union_button_states()
	{
		// W1.5/收口：对齐 Angel SetButton + CTabGuildAlliance 行级。
		// 未入盟：操作钮 DISABLED，笔形 Btnotice 常亮。
		// 入盟：Invite/Withdraw/Chat/Notice 常亮；Kick/Change/Up/Down 随选中亮
		// （Angel sub_914EB4/915983/91505B + s9 0x06/0x07/0x09 字段已对拍）。
		const bool in_all = AllianceModel::get().data().in_alliance();
		const bool has_sel = in_all && union_selected_cid > 0
			&& AllianceModel::get().find(union_selected_cid) != nullptr;

		auto st = [&](uint16_t id, Button::State s)
		{
			auto it = buttons.find(id);
			if (it != buttons.end() && it->second)
				it->second->set_state(s);
		};

		st(BT_UNION_INVITE, in_all ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_WITHDRAW, in_all ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_CHAT, in_all ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_WHERE, has_sel ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_WHISPER, has_sel ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_PARTYINVITE, has_sel ? Button::State::NORMAL : Button::State::DISABLED);
		// BtInfo = 职衔名窗 Toggle：入盟即亮（不依赖行选中）。
		st(BT_UNION_INFO, in_all ? Button::State::NORMAL : Button::State::DISABLED);
		// 功能前置（s9 AllianceOperationHandler:68/151）：管理类按钮需
		// allianceRank<=2（盟主/副盟主），否则即便选中行也灰掉。
		const bool can_manage = in_all && self_alliance_rank() <= 2;
		st(BT_UNION_KICK, (has_sel && can_manage) ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_CHANGE, (has_sel && can_manage) ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_UP, (has_sel && can_manage) ? Button::State::NORMAL : Button::State::DISABLED);
		st(BT_UNION_DOWN, (has_sel && can_manage) ? Button::State::NORMAL : Button::State::DISABLED);
		// 邀请新公会进盟也需管理权。
		st(BT_UNION_INVITE, can_manage ? Button::State::NORMAL : Button::State::DISABLED);

		// Btnotice 公告：盟主/副盟主可改（allianceRank<=2），普通成员灰掉。
		st(BT_UNION_NOTICE, can_manage ? Button::State::NORMAL : Button::State::DISABLED);
	}

	void UIUserList::update_blacklist_button_states()
	{
		// r3：oracle 拒绝列表帧——「增加」常亮（黄绿态），「删除」无选中行灰。
		if (auto it = buttons.find(BT_BLACKLIST_ADD); it != buttons.end() && it->second)
			it->second->set_state(Button::State::NORMAL);
		if (auto it = buttons.find(BT_BLACKLIST_DELETE); it != buttons.end() && it->second)
			it->second->set_state(blacklist_selected > 0
				? Button::State::NORMAL : Button::State::DISABLED);
	}

	void UIUserList::refresh_from_models()
	{
		// handler 收包后的统一刷新入口：按钮态按当前页签重算。
		set_page_buttons(tab);
	}

	void UIUserList::draw_friend_tab(float) const
	{
		const BuddyListData& bl = BuddyListModel::get().data();

		// r3 live 实测对齐 oracle（native_friend_tab.png 像素扫描）：
		//   横幅 (12,50)、计数 (261,59)、分组蓝字 (16,112)、虚线 (102,112) 宽123、
		//   none (16,130)、目前位置 (12,313)。
		//   friend4（离线好友横幅）是筛选态顶横幅素材，不再当分组条画在 y=100
		//   （旧实现与 Default Group 文本叠死，真机截图确认）。
		//   friend1/friend3（相同频道/不同频道列头+分隔）oracle 空态不绘制。
		if (friend_banner)
			friend_banner.draw(position + Point<int16_t>(12, 50));
		friend_count_text.draw(position + Point<int16_t>(261, 59));
		friend_group_text.draw(position + Point<int16_t>(16, 112));
		if (friend_line)
			friend_line.draw(DrawArgument(position + Point<int16_t>(102, 112), Point<int16_t>(123, 0)));

		// P0：从 BuddyListModel 绘成员行；空列表才显示 none。
		int16_t y = FRIEND_ROW_Y;
		int16_t drawn = 0;
		const bool show_online_only = (friend_tab == Tab::FRIEND_ONLINE);

		for (const auto& b : bl.buddies)
		{
			if (show_online_only && !b.online)
				continue;
			if (drawn >= FRIEND_MAX_ROWS)
				break;

			std::string info = b.name;
			if (!b.group.empty() && b.group != "All")
				info += " [" + b.group + "]";
			info += " " + (b.online
				? Locale::format("common.channel", std::to_string(b.channel + 1))
				: Locale::text("common.offline"));

			const Color::Name cname = (b.cid == friend_selected_cid)
				? Color::Name::MEDIUMBLUE : Color::Name::BLACK;
			Text row(Text::Font::A11M, Text::Alignment::LEFT, cname, info);
			row.draw(position + Point<int16_t>(24, y));
			y += FRIEND_ROW_H;
			drawn++;
		}

		if (drawn == 0)
			friend_none_text.draw(position + Point<int16_t>(16, FRIEND_ROW_Y));

		friend_location_text.draw(position + Point<int16_t>(12, 313));
		friends_slider.draw(position);
	}

	void UIUserList::draw_party_tab(float) const
	{
		const Party& p = PartyModel::get().data();

		if (party_online_banner)
			party_online_banner.draw(position + Point<int16_t>(12, 50));
		party_online_count_text.draw(position + Point<int16_t>(261, 59));
		if (party_col_header)
			party_col_header.draw(position + Point<int16_t>(12, 80));

		auto draw_party_rows = [&](bool online, int16_t rows_y, int16_t maxrows) -> int16_t
		{
			int16_t y = rows_y;
			int16_t drawn = 0;

			for (const auto& m : p.members)
			{
				if (m.id == 0 || m.online != online || drawn >= maxrows)
					continue;

				std::string info = m.name;
				if (m.id == p.leader)
					info += " *";
				info += " Lv." + std::to_string(m.level);
				if (online)
					info += " " + Locale::format("common.channel", std::to_string(m.channel + 1));
				else
					info += " " + Locale::text("common.offline");

				const Color::Name cname = (m.id == party_selected_cid)
					? Color::Name::MEDIUMBLUE : Color::Name::BLACK;
				Text row(Text::Font::A11M, Text::Alignment::LEFT, cname, info);
				row.draw(position + Point<int16_t>(24, y));

				// r3：血条（BtHP 开关；083 CUIPartyHP / MinMS GAUGE_FILL_W=63）。
				// 保持 UserList 内嵌，不拆独立窗（round68 + 计划 W-M4）。
				if (party_hp_visible && partyhp_bar && partyhp_graduation)
					if (const auto* hp = PartyModel::get().member_hp(m.id))
					{
						int32_t cur = hp->first;
						int32_t max = hp->second;

						// IMG GaugeBar/bar 实测宽 63；勿用大于 63 的外框宽当填充上限。
						constexpr int16_t GAUGE_FILL_W = 63;
						const int16_t BAR_W = partyhp_bar.width() > 0
							? std::min<int16_t>(partyhp_bar.width(), GAUGE_FILL_W)
							: GAUGE_FILL_W;
						int16_t fill_w = static_cast<int16_t>(
							BAR_W * (max > 0 ? static_cast<double>(cur) / max : 0.0));
						if (fill_w < 0) fill_w = 0;
						if (fill_w > BAR_W) fill_w = BAR_W;

						// MinMS：NAME_DX=8 文本区旁；本窗行文案锚点 24，血条同列下移。
						Point<int16_t> at = position + Point<int16_t>(24, y + 16);
						partyhp_graduation.draw(DrawArgument(at));
						Range<int16_t> crop_right(0, static_cast<int16_t>(BAR_W - fill_w));
						partyhp_bar.draw(DrawArgument(at), {}, crop_right);
					}

				y += PARTY_ROW_H + (party_hp_visible ? 14 : 0);
				drawn++;
			}

			return drawn;
		};

		const int16_t online_drawn = draw_party_rows(true, PARTY_ONLINE_Y, PARTY_ONLINE_ROWS);
		if (online_drawn == 0)
			party_none_text.draw(position + Point<int16_t>(24, PARTY_ONLINE_Y));

		if (party_offline_banner)
			party_offline_banner.draw(position + Point<int16_t>(12, 131));
		party_offline_count_text.draw(position + Point<int16_t>(261, 140));
		if (party_col_header2)
			party_col_header2.draw(position + Point<int16_t>(12, 161));

		draw_party_rows(false, PARTY_OFFLINE_Y, PARTY_OFFLINE_ROWS);
		party_slider.draw(position);
	}

	void UIUserList::draw_blacklist_tab(float) const
	{
		if (blacklist_banner)
			blacklist_banner.draw(position + Point<int16_t>(12, 55));
		// 计数文案在 update() 同步（const 方法不重建布局），容量 26（oracle 帧「0/26」）。
		blacklist_count_text.draw(position + Point<int16_t>(233, 65));
		if (blacklist_col_header)
			blacklist_col_header.draw(position + Point<int16_t>(12, 83));
		if (blacklist_separator)
			blacklist_separator.draw(position + Point<int16_t>(12, 101));

		if (blacklist_names.empty())
		{
			blacklist_none_text.draw(position + Point<int16_t>(18, BLACKLIST_ROW_Y));
		}
		else
		{
			// r3：本地拒绝列表行渲染（Angel 列表按名字排序；选中行深蓝高亮，
			// 与好友/组员页同款）。
			int16_t y = BLACKLIST_ROW_Y;

			for (size_t i = 0; i < blacklist_names.size() && i < 13; i++)
			{
				const Color::Name cname =
					(static_cast<int32_t>(i) + 1 == blacklist_selected)
					? Color::Name::MEDIUMBLUE : Color::Name::BLACK;
				Text row(Text::Font::A11M, Text::Alignment::LEFT,
					cname, blacklist_names[i]);
				row.draw(position + Point<int16_t>(24, y));
				y += BLACKLIST_ROW_H;
			}
		}

		blacklist_slider.draw(position);
	}

	int32_t UIUserList::blacklist_row_at(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x()),
			static_cast<int16_t>(cursorpos.y() - position.y()));

		if (rel.x() < 12 || rel.x() > 280)
			return 0;

		const int16_t visible =
			static_cast<int16_t>(blacklist_names.size() < 13 ? blacklist_names.size() : 13);

		if (rel.y() < BLACKLIST_ROW_Y
			|| rel.y() >= BLACKLIST_ROW_Y + visible * BLACKLIST_ROW_H)
			return 0;

		return (rel.y() - BLACKLIST_ROW_Y) / BLACKLIST_ROW_H + 1;
	}

	int32_t UIUserList::union_row_at(Point<int16_t> cursorpos) const
	{
		const AllianceData& a = AllianceModel::get().data();
		if (!a.in_alliance())
			return 0;

		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x()),
			static_cast<int16_t>(cursorpos.y() - position.y()));

		if (rel.x() < 12 || rel.x() > 280)
			return 0;

		constexpr int16_t ROW_Y = 140;
		constexpr int16_t ROW_H = 17;
		constexpr int16_t MAX_ROWS = 10;

		if (rel.y() < ROW_Y)
			return 0;

		const int16_t row = static_cast<int16_t>((rel.y() - ROW_Y) / ROW_H);
		if (row < 0 || row >= MAX_ROWS)
			return 0;

		int16_t drawn = 0;
		for (const auto& m : a.members)
		{
			if (drawn == row)
				return m.id;
			if (++drawn >= MAX_ROWS)
				break;
		}
		return 0;
	}

	void UIUserList::draw_union_tab(float) const
	{
		const AllianceData& a = AllianceModel::get().data();

		// 空态（尚未加入联盟）：只画提示条 +「还未加入联盟。」
		if (!a.in_alliance())
		{
			if (union_notice_bar)
				union_notice_bar.draw(position + Point<int16_t>(12, 52));
			union_none_text.draw(position + Point<int16_t>(104, 58));
			union_slider.draw(position);
			return;
		}

		// 入盟态：公告条 + 横幅/列头 + 成员行（Angel CTabGuildAlliance GUILDITEM）。
		if (union_notice_bar)
			union_notice_bar.draw(position + Point<int16_t>(12, 52));
		union_notice_text.draw(position + Point<int16_t>(16, 56));

		if (union_banner)
			union_banner.draw(position + Point<int16_t>(12, 78));
		union_count_text.draw(position + Point<int16_t>(240, 83));

		if (union_col_header)
			union_col_header.draw(position + Point<int16_t>(12, 110));

		constexpr int16_t ROW_Y = 140;
		constexpr int16_t ROW_H = 17;
		constexpr int16_t MAX_ROWS = 10;
		int16_t y = ROW_Y;
		int16_t drawn = 0;

		for (const auto& m : a.members)
		{
			if (drawn >= MAX_ROWS)
				break;

			const Color::Name cname = (m.id == union_selected_cid)
				? Color::Name::TORCHRED : Color::Name::BLACK;
			Text name(Text::Font::A11M, Text::Alignment::LEFT, cname, m.name);
			name.draw(position + Point<int16_t>(27, y));

			Text job(Text::Font::A11M, Text::Alignment::LEFT, cname,
				Job(static_cast<uint16_t>(m.job)).get_name());
			job.draw(position + Point<int16_t>(114, y));

			Text lv(Text::Font::A11M, Text::Alignment::LEFT, cname, std::to_string(m.level));
			lv.draw(position + Point<int16_t>(179, y));

			const std::string rank = (m.alliance_rank >= 1 && m.alliance_rank <= 5)
				? a.rank_titles[m.alliance_rank - 1] : std::string();
			Text rk(Text::Font::A11M, Text::Alignment::LEFT, cname, rank);
			rk.draw(position + Point<int16_t>(222, y));

			y = static_cast<int16_t>(y + ROW_H);
			drawn++;
		}

		if (drawn == 0)
			guild_none_row_text.draw(position + Point<int16_t>(27, ROW_Y));

		union_slider.draw(position);
	}

	std::string UIUserList::get_cur_location()
	{
		// 好友页「目前位置 - <地图名>」：动态取当前地图名（与 UIMiniMap/UIWorldMap
		// 同款 NxHelper::Map 入口），不再写死 "Henesys Market"（旧硬编码换图后不刷新）。
		// 未加载 String.nx / node 缺失时回退为空串，由调用方文案「目前位置 - 」兜底。
		const int32_t mapid = Stage::get().get_mapid();
		const std::string name = NxHelper::Map::get_map_info_by_id(mapid).name;
		return name.empty() ? std::to_string(mapid) : name;
	}
}