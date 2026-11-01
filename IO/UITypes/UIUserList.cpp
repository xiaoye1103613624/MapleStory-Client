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
#include "UIMessenger.h"
#include "UIPartySearch.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../IO/Components/MapleButton.h"
#include "../../IO/Components/TwoSpriteButton.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/PartySearchPackets.h"
#include "../../Net/Packets/GuildPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
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

		if (Main["backgrnd"])
			sprites.emplace_back(Main["backgrnd"]);

		if (close)
			buttons[Buttons::BT_CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(244, 7));

		nl::node taben = Main["Tab"]["enabled"];
		nl::node tabdis = Main["Tab"]["disabled"];

		if (tabdis["0"] && taben["0"])
			buttons[Buttons::BT_TAB_FRIEND] = std::make_unique<TwoSpriteButton>(tabdis["0"], taben["0"]);
		if (tabdis["1"] && taben["1"])
			buttons[Buttons::BT_TAB_PARTY] = std::make_unique<TwoSpriteButton>(tabdis["1"], taben["1"]);
		if (tabdis["2"] && taben["2"])
			buttons[Buttons::BT_TAB_BOSS] = std::make_unique<TwoSpriteButton>(tabdis["2"], taben["2"]);
		if (tabdis["3"] && taben["3"])
			buttons[Buttons::BT_TAB_BLACKLIST] = std::make_unique<TwoSpriteButton>(tabdis["3"], taben["3"]);

		// Party Tab
		nl::node Party = Main["Party"];
		nl::node PartySearch = Party["PartySearch"];

		party_tab = Tab::PARTY_MINE;
		party_title = Party["title"];

		for (size_t i = 0; i <= 4; i++)
			party_mine_grid[i] = UserList["Sheet2"][i];

		party_mine_name = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "none", 0);

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

		friends_cur_location = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, "My Location - " + get_cur_location(), 0);
		friends_name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "none", 0);
		friends_group_name = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "Default Group (0/0)", 0);

		try_btn(Buttons::BT_FRIEND_ADD, Friend["BtAddFriend"]);
		try_btn(Buttons::BT_FRIEND_ADD_GROUP, Friend["BtAddGroup"]);
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

		// Boss tab
		nl::node Boss = Main["Boss"];

		if (Boss["base"])
			boss_sprites.emplace_back(Boss["base"]);
		if (Boss["base3"])
			boss_sprites.emplace_back(Boss["base3"]);
		if (Boss["base2"])
			boss_sprites.emplace_back(Boss["base2"]);

		auto try_two = [&](uint16_t id, nl::node dis, nl::node en)
		{
			if (dis && en)
				buttons[id] = std::make_unique<TwoSpriteButton>(dis, en);
		};

		try_two(Buttons::BT_BOSS_0, Boss["BossList"]["0"]["icon"]["disabled"]["0"], Boss["BossList"]["0"]["icon"]["normal"]["0"]);
		try_two(Buttons::BT_BOSS_1, Boss["BossList"]["1"]["icon"]["disabled"]["0"], Boss["BossList"]["1"]["icon"]["normal"]["0"]);
		try_two(Buttons::BT_BOSS_2, Boss["BossList"]["2"]["icon"]["disabled"]["0"], Boss["BossList"]["2"]["icon"]["normal"]["0"]);
		try_two(Buttons::BT_BOSS_3, Boss["BossList"]["3"]["icon"]["disabled"]["0"], Boss["BossList"]["3"]["icon"]["normal"]["0"]);
		try_two(Buttons::BT_BOSS_4, Boss["BossList"]["4"]["icon"]["disabled"]["0"], Boss["BossList"]["4"]["icon"]["normal"]["0"]);
		try_btn(Buttons::BT_BOSS_L, Boss["BtArrow"]["Left"]);
		try_btn(Buttons::BT_BOSS_R, Boss["BtArrow"]["Right"]);
		try_btn(Buttons::BT_BOSS_DIFF_L, Boss["BtArrow2"]["Left"]);
		try_btn(Buttons::BT_BOSS_DIFF_R, Boss["BtArrow2"]["Right"]);
		try_btn(Buttons::BT_BOSS_GO, Boss["BtEntry"]);
		deactivate(Buttons::BT_BOSS_0);
		deactivate(Buttons::BT_BOSS_1);
		deactivate(Buttons::BT_BOSS_2);
		deactivate(Buttons::BT_BOSS_3);
		deactivate(Buttons::BT_BOSS_4);
		deactivate(Buttons::BT_BOSS_L);
		deactivate(Buttons::BT_BOSS_R);
		deactivate(Buttons::BT_BOSS_DIFF_L);
		deactivate(Buttons::BT_BOSS_DIFF_R);
		deactivate(Buttons::BT_BOSS_GO);

		// Blacklist tab
		nl::node BlackList = Main["BlackList"];

		blacklist_title = BlackList["base"];

		for (size_t i = 0; i < 3; i++)
			blacklist_grid[i] = UserList["Sheet6"][i];

		blacklist_name = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "none", 0);

		try_btn(Buttons::BT_BLACKLIST_ADD, BlackList["BtAdd"]);
		try_btn(Buttons::BT_BLACKLIST_DELETE, BlackList["BtDelete"]);
		try_btn(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL, BlackList["TapShowIndividual"]);
		try_btn(Buttons::BT_TAB_BLACKLIST_GUILD, BlackList["TapShowGuild"]);
		deactivate(Buttons::BT_BLACKLIST_ADD);
		deactivate(Buttons::BT_BLACKLIST_DELETE);
		deactivate(Buttons::BT_TAB_BLACKLIST_INDIVIDUAL);
		deactivate(Buttons::BT_TAB_BLACKLIST_GUILD);

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

		if (tab == Buttons::BT_TAB_PARTY)
		{
			party_title.draw(position);

			if (party_tab == Buttons::BT_TAB_PARTY_MINE)
			{
				party_mine_grid[0].draw(position + Point<int16_t>(10, 115));
				party_mine_grid[4].draw(position + Point<int16_t>(10, 133));
				party_mine_name.draw(position + Point<int16_t>(27, 130));
			}
			else if (party_tab == Buttons::BT_TAB_PARTY_SEARCH)
			{
				party_search_grid[0].draw(position);
				party_search_grid[1].draw(position);
				party_search_grid[2].draw(position);
				party_slider.draw(position);
			}
		}
		else if (tab == Buttons::BT_TAB_FRIEND)
		{
			for (Sprite sprite : friend_sprites)
				sprite.draw(position, alpha);

			friends_online_text.draw(position + Point<int16_t>(211, 62));
			friends_cur_location.draw(position + Point<int16_t>(9, 279));
			friend_grid[0].draw(position + Point<int16_t>(10, 116));
			friend_grid[2].draw(position + Point<int16_t>(10, 135));
			friends_name.draw(position + Point<int16_t>(24, 134));
			friends_group_name.draw(position + Point<int16_t>(29, 114));
			friends_slider.draw(position);
		}
		else if (tab == Buttons::BT_TAB_BOSS)
		{
			for (Sprite sprite : boss_sprites)
				sprite.draw(position, alpha);
		}
		else if (tab == Buttons::BT_TAB_BLACKLIST)
		{
			blacklist_title.draw(position + Point<int16_t>(24, 104));
			blacklist_grid[0].draw(position + Point<int16_t>(24, 134));
			blacklist_name.draw(position + Point<int16_t>(24, 134));
		}

		UIElement::draw_buttons(alpha);
	}

	void UIUserList::update()
	{
		UIElement::update();

		if (tab == Buttons::BT_TAB_FRIEND)
			for (Sprite sprite : friend_sprites)
				sprite.update();

		if (tab == Buttons::BT_TAB_BOSS)
			for (Sprite sprite : boss_sprites)
				sprite.update();
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
				uint16_t new_tab = tab;

				if (new_tab < Buttons::BT_TAB_BLACKLIST)
					new_tab++;
				else
					new_tab = Buttons::BT_TAB_FRIEND;

				change_tab(new_tab);
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
		switch (buttonid)
		{
			case Buttons::BT_CLOSE:
				deactivate();
				break;
			case Buttons::BT_TAB_FRIEND:
			case Buttons::BT_TAB_PARTY:
			case Buttons::BT_TAB_BOSS:
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
		background = tabid == Buttons::BT_TAB_BOSS ? Main["Boss"]["backgrnd3"] : Main["backgrnd2"];

		if (oldtab != tab)
			set_st(Buttons::BT_TAB_FRIEND + oldtab, Button::State::NORMAL);

		set_st(Buttons::BT_TAB_FRIEND + tab, Button::State::PRESSED);

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

		if (tab == Buttons::BT_TAB_BOSS)
		{
			set_act(Buttons::BT_BOSS_0, true);
			set_act(Buttons::BT_BOSS_1, true);
			set_act(Buttons::BT_BOSS_2, true);
			set_act(Buttons::BT_BOSS_3, true);
			set_act(Buttons::BT_BOSS_4, true);
			set_act(Buttons::BT_BOSS_L, true);
			set_act(Buttons::BT_BOSS_R, true);
			set_act(Buttons::BT_BOSS_DIFF_L, true);
			set_act(Buttons::BT_BOSS_DIFF_R, true);
			set_act(Buttons::BT_BOSS_GO, true);
			set_st(Buttons::BT_BOSS_L, Button::State::DISABLED);
			set_st(Buttons::BT_BOSS_R, Button::State::DISABLED);
			set_st(Buttons::BT_BOSS_GO, Button::State::DISABLED);
			set_st(Buttons::BT_BOSS_DIFF_L, Button::State::DISABLED);
			set_st(Buttons::BT_BOSS_DIFF_R, Button::State::DISABLED);
		}
		else
		{
			set_act(Buttons::BT_BOSS_0, false);
			set_act(Buttons::BT_BOSS_1, false);
			set_act(Buttons::BT_BOSS_2, false);
			set_act(Buttons::BT_BOSS_3, false);
			set_act(Buttons::BT_BOSS_4, false);
			set_act(Buttons::BT_BOSS_L, false);
			set_act(Buttons::BT_BOSS_R, false);
			set_act(Buttons::BT_BOSS_DIFF_L, false);
			set_act(Buttons::BT_BOSS_DIFF_R, false);
			set_act(Buttons::BT_BOSS_GO, false);
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

	std::string UIUserList::get_cur_location()
	{
		return "Henesys Market";
	}
}