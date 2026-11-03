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
#include "UIBuddyList.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "UICharInfo.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	UIBuddyList::UIBuddyList() : UIElement(Point<int16_t>(740, 20), Point<int16_t>(200, 320)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		nl::node buddy = nl::nx::UI["BuddyList.img"];
		if (buddy["backgrnd"])
			sprites.emplace_back(Sprite(buddy["backgrnd"], Point<int16_t>(0, 0)));

		// 本包 UI.nx 根下没有 BuddyList.img（nx_inspect 实测），也没有 Basic.img/BtAdd
		// ⇒ 只有关闭钮能落地，且必须与其它功能窗共用同一颗描边 X
		// （UiAssetResolve::close_btn() → Basic.img/BtClose2）。
		// 旧写法 `bn ? MapleButton(bn) : MapleButton(bn, pos)` 两个分支都拿不到贴图，
		// 真值分支还会把坐标丢成窗口原点，属于凭空写出来的逻辑。
		nl::node close = UiAssetResolve::close_btn();
		if (close)
			buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(180, 8));
	}

	void UIBuddyList::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		// 标题每个数都会变（好友数/上限随包更新）=> 按绑定判据不许上表，走活文案：
		// 数字整段当 `{}` 的值喂进模板，两种语言各自决定括号和空格放哪儿。
		title.change_text(Locale::format("buddylist.title",
			std::to_string(list.buddies.size()) + "/" + std::to_string(list.capacity)));
		title.draw(position + Point<int16_t>(10, 8));

		int16_t y = 40;
		for (const auto& b : list.buddies)
		{
			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);

			std::string info = b.name;
			if (!b.group.empty() && b.group != "All")
				info += " [" + b.group + "]";
			// 「频道」「离线」是全客户端共用的状态词（好友窗与队伍窗同一份键），所以放
			// common.*；空格留在代码里拼，ini 加载器会 trim 值两端空白，写进模板会丢。
			info += " " + (b.online
				? Locale::format("common.channel", std::to_string(b.channel + 1))
				: Locale::text("common.offline"));
			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += ROW_H;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIBuddyList::update()
	{
	}

	void UIBuddyList::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(list.buddies.size()))
		{
			selected_cid = list.buddies[row].cid;
			BuddyDeletePacket(selected_cid).dispatch();
		}
	}

	Cursor::State UIBuddyList::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(list.buddies.size()))
		{
			selected_cid = list.buddies[row].cid;
			LOG(LOG_DEBUG, "UIBuddyList: select cid=" << selected_cid);
		}
		return st;
	}

	const Buddy* UIBuddyList::selected_buddy() const
	{
		for (const auto& b : list.buddies)
			if (b.cid == selected_cid)
				return &b;
		return nullptr;
	}

	UIElement::Type UIBuddyList::get_type() const
	{
		return TYPE;
	}

	void UIBuddyList::update_buddies(const BuddyListData& data)
	{
		list = data;
		bool still = false;
		for (const auto& b : list.buddies)
			if (b.cid == selected_cid) { still = true; break; }
		if (!still)
			selected_cid = 0;
	}

	void UIBuddyList::set_capacity(int32_t cap)
	{
		list.capacity = cap;
	}

	int32_t UIBuddyList::get_capacity() const
	{
		return list.capacity;
	}

	void UIBuddyList::update_buddy_channel(int32_t cid, int32_t channel)
	{
		for (auto& b : list.buddies)
		{
			if (b.cid == cid)
			{
				b.channel = channel;
				b.online = (channel != -2 && channel != -1);
				break;
			}
		}
	}

	Button::State UIBuddyList::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtAdd:
				prompt_add();
				break;
			case BtClose:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	int16_t UIBuddyList::row_by_position(Point<int16_t> pos) const
	{
		int16_t row = (pos.y() - 40) / ROW_H;
		if (pos.x() < 10 || pos.x() > 190 || row < 0)
			return -1;
		return row;
	}

	void UIBuddyList::prompt_add()
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			std::string name = pre;
			UI::get().emplace<UISoftKey>([name](const std::string& group)
			{
				BuddyAddPacket(name, group.empty() ? "All" : group).dispatch();
			}, []() {}, "Group (All)");
			return;
		}
		UI::get().emplace<UISoftKey>([](const std::string& name)
		{
			if (name.empty()) return;
			UI::get().emplace<UISoftKey>([name](const std::string& group)
			{
				BuddyAddPacket(name, group.empty() ? "All" : group).dispatch();
			}, []() {}, "Group (All)");
		}, []() {}, "Buddy name");
	}

	void UIBuddyList::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a')
			prompt_add();
		else if (keycode == 'G' || keycode == 'g') {
			const Buddy* sel = selected_buddy();
			std::string name;
			if (sel)
				name = sel->name;
			else if (auto ci = UI::get().get_element<UICharInfo>())
				name = ci->get_target_name();
			if (!name.empty()) {
				UI::get().emplace<UISoftKey>([name](const std::string& group) {
					if (group.empty()) return;
					BuddyAddPacket(name, group).dispatch();
					LOG(LOG_DEBUG, "UIBuddyList: ChangeGroup name=" << name << " g=" << group);
				}, []() {}, "Group");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					UI::get().emplace<UISoftKey>([name](const std::string& group) {
						if (group.empty()) return;
						BuddyAddPacket(name, group).dispatch();
						LOG(LOG_DEBUG, "UIBuddyList: ChangeGroup name=" << name << " g=" << group);
					}, []() {}, "Group");
				}, []() {}, "Buddy name");
			}
		} else if (keycode == 'Y' || keycode == 'y') {
			if (selected_cid <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					selected_cid = ci->get_target_cid();
			if (selected_cid > 0) {
				BuddyAcceptPacket(selected_cid).dispatch();
				LOG(LOG_DEBUG, "UIBuddyList: Accept cid=" << selected_cid);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) {
							selected_cid = cid;
							BuddyAcceptPacket(cid).dispatch();
							LOG(LOG_DEBUG, "UIBuddyList: Accept cid=" << cid);
						}
					} catch (...) {}
				}, []() {}, "Accept cid");
			}
		} else if (keycode == 'D' || keycode == 'd') {
			if (selected_cid <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					selected_cid = ci->get_target_cid();
			if (selected_cid > 0) {
				BuddyDeletePacket(selected_cid).dispatch();
				LOG(LOG_DEBUG, "UIBuddyList: Delete cid=" << selected_cid);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) {
							selected_cid = cid;
							BuddyDeletePacket(cid).dispatch();
							LOG(LOG_DEBUG, "UIBuddyList: Delete cid=" << cid);
						}
					} catch (...) {}
				}, []() {}, "Delete cid");
			}
		} else if (keycode == 'R' || keycode == 'r') {
			if (selected_cid <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					selected_cid = ci->get_target_cid();
			if (selected_cid > 0) {
				BuddyDeletePacket(selected_cid).dispatch();
				LOG(LOG_DEBUG, "UIBuddyList: Reject cid=" << selected_cid);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) {
							selected_cid = cid;
							BuddyDeletePacket(cid).dispatch();
							LOG(LOG_DEBUG, "UIBuddyList: Reject cid=" << cid);
						}
					} catch (...) {}
				}, []() {}, "Reject cid");
			}
		} else if (keycode == 'W' || keycode == 'w') {
			const Buddy* sel = selected_buddy();
			std::string to;
			if (sel)
				to = sel->name;
			else if (auto ci = UI::get().get_element<UICharInfo>())
				to = ci->get_target_name();
			if (!to.empty()) {
				UI::get().emplace<UISoftKey>([to](const std::string& msg) {
					if (msg.empty()) return;
					WhisperPacket(to, msg).dispatch();
					LOG(LOG_DEBUG, "UIBuddyList: SoftKey whisper to=" << to);
				}, []() {}, "Whisper msg");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& to) {
					if (to.empty()) return;
					UI::get().emplace<UISoftKey>([to](const std::string& msg) {
						if (msg.empty()) return;
						WhisperPacket(to, msg).dispatch();
						LOG(LOG_DEBUG, "UIBuddyList: SoftKey whisper to=" << to);
					}, []() {}, "Whisper msg");
				}, []() {}, "Whisper to");
			}
		}
	}
}
