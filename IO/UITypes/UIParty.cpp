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
#include "UIParty.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UIPartyRace.h"
#include "UIPartySearch.h"
#include "UISoftKey.h"
#include "UICharInfo.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	UIParty::UIParty() : UIElement(Point<int16_t>(520, 20), Point<int16_t>(200, 300)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// 本包 UI.nx 根下没有 Party.img，v83 也没有独立队伍窗根（UIWindow.img/UserInfo 是
		// 好友/信息窗，件表见 notes/asset-node-inventory.md §1.5，不能拿来当队伍窗底衬）。
		// ⇒ 底衬与 邀请/离开/队长/开除 四钮显式预留（队伍操作仍有 SoftKey L/E 与菜单入口）。
		// 关闭钮与其它功能窗共用同一颗描边 X（UiAssetResolve::close_btn() → BtClose2）。
		nl::node party = nl::nx::UI["Party.img"];
		if (party["backgrnd"])
			sprites.emplace_back(Sprite(party["backgrnd"], Point<int16_t>(0, 0)));

		// id 必须显式列出：枚举里 BtClose=2 夹在 BtLeave 与 BtLeader 之间，
		// 直接拿循环下标当 id 会把「队长」钮注册成关闭钮。
		const char* btnname[] = { "BtInvite", "BtLeave", "BtLeader", "BtExpel" };
		const uint16_t btnid[] = { BtInvite, BtLeave, BtLeader, BtExpel };
		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 250),   // BtInvite
			Point<int16_t>(100, 250),  // BtLeave
			Point<int16_t>(20, 275),   // BtLeader
			Point<int16_t>(100, 275)   // BtExpel
		};
		for (uint16_t i = 0; i < 4; i++)
			try_maple_btn(buttons, btnid[i], party[btnname[i]], btnpos[i], "UIParty");

		try_maple_btn(buttons, BtClose, UiAssetResolve::close_btn(), Point<int16_t>(180, 8), "UIParty close");
	}

	void UIParty::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text("队伍 #" + std::to_string(party.id));
		title.draw(position + Point<int16_t>(10, 8));

		int16_t y = 40;
		for (const auto& m : party.members)
		{
			if (m.id == 0)
				continue;

			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);

			std::string info = m.name + " (Lv." + std::to_string(m.level) + " J" + std::to_string(m.job) + ")";
			if (m.online)
				info += " 频道" + std::to_string(m.channel + 1);
			else
				info += " 离线";
			if (m.id == party.leader)
				info += " ★";

			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += 30;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIParty::update()
	{
	}

	UIElement::Type UIParty::get_type() const
	{
		return TYPE;
	}

	void UIParty::update_party(const Party& p)
	{
		party = p;
		bool still = false;
		for (const auto& m : party.members)
			if (m.id == selected_cid) { still = true; break; }
		if (!still)
			selected_cid = 0;
	}

	void UIParty::set_leader(int32_t cid)
	{
		party.leader = cid;
	}

	void UIParty::update_member_hp(int32_t cid, int32_t curhp, int32_t maxhp)
	{
		memberhp[cid] = { curhp, maxhp };
	}

	Button::State UIParty::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtInvite:
				prompt_invite();
				break;
			case BtLeave:
				PartyLeavePacket().dispatch();
				deactivate();
				break;
			case BtClose:
				deactivate();
				break;
			case BtLeader:
				prompt_leader();
				break;
			case BtExpel:
				prompt_expel();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIParty::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'L' || keycode == 'l')
			prompt_leader();
		else if (keycode == 'E' || keycode == 'e')
			prompt_expel();
		else if (keycode == 'I' || keycode == 'i')
			prompt_invite();
		else if (keycode == 'C' || keycode == 'c') {
			PartyCreatePacket().dispatch();
			LOG(LOG_DEBUG, "UIParty: Create");
		} else if (keycode == 'X' || keycode == 'x') {
			PartyLeavePacket().dispatch();
			LOG(LOG_DEBUG, "UIParty: Leave");
			deactivate();
		} else if (keycode == 'J' || keycode == 'j') {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t pid = std::stoi(s);
					if (pid > 0) {
						PartyJoinPacket(pid).dispatch();
						LOG(LOG_DEBUG, "UIParty: Join partyid=" << pid);
					}
				} catch (...) {}
			}, []() {}, "Party id");
		} else if (keycode == 'S' || keycode == 's' || keycode == 'P' || keycode == 'p') {
			auto ui = UI::get().get_element<UIPartySearch>();
			if (!ui) ui = UI::get().emplace<UIPartySearch>();
			else if (!ui->is_active()) ui->makeactive();
		} else if (keycode == 'R' || keycode == 'r') {
			auto ui = UI::get().get_element<UIPartyRace>();
			if (!ui) ui = UI::get().emplace<UIPartyRace>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	void UIParty::prompt_invite()
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			PartyInvitePacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIParty: Invite target=" << pre);
			return;
		}
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				PartyInvitePacket(s).dispatch();
		});
	}

	int32_t UIParty::member_at_row(int16_t row) const
	{
		int16_t i = 0;
		for (const auto& m : party.members)
		{
			if (m.id == 0)
				continue;
			if (i == row)
				return m.id;
			++i;
		}
		return 0;
	}

	Cursor::State UIParty::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;
		Point<int16_t> rel = cursorpos - position;
		if (rel.x() >= 10 && rel.x() <= 190 && rel.y() >= 40 && rel.y() < 250)
		{
			int16_t row = static_cast<int16_t>((rel.y() - 40) / 30);
			int32_t cid = member_at_row(row);
			if (cid > 0)
			{
				selected_cid = cid;
				LOG(LOG_DEBUG, "UIParty: select cid=" << cid);
			}
		}
		return st;
	}

	void UIParty::prompt_leader()
	{
		if (selected_cid <= 0)
			if (auto ci = UI::get().get_element<UICharInfo>())
				selected_cid = ci->get_target_cid();
		if (selected_cid > 0)
		{
			PartyChangeLeaderPacket(selected_cid).dispatch();
			LOG(LOG_DEBUG, "UIParty: ChangeLeader cid=" << selected_cid);
			return;
		}
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			try {
				int32_t cid = std::stoi(s);
				if (cid > 0) {
					selected_cid = cid;
					PartyChangeLeaderPacket(cid).dispatch();
					LOG(LOG_DEBUG, "UIParty: ChangeLeader cid=" << cid);
				}
			} catch (...) {}
		}, []() {}, "Leader cid");
	}

	void UIParty::prompt_expel()
	{
		if (selected_cid <= 0)
			if (auto ci = UI::get().get_element<UICharInfo>())
				selected_cid = ci->get_target_cid();
		if (selected_cid > 0)
		{
			PartyExpelPacket(selected_cid).dispatch();
			LOG(LOG_DEBUG, "UIParty: Expel cid=" << selected_cid);
			return;
		}
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			try {
				int32_t cid = std::stoi(s);
				if (cid > 0) {
					selected_cid = cid;
					PartyExpelPacket(cid).dispatch();
					LOG(LOG_DEBUG, "UIParty: Expel cid=" << cid);
				}
			} catch (...) {}
		}, []() {}, "Expel cid");
	}
}
