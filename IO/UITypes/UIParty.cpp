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

#include "../../Graphics/Geometry.h" // DrawArgument / Range（真血条 Texture::draw 裁剪）
#include "../../Graphics/Texture.h"  // 083 CUIPartyHP GaugeBar 真血条位图（替代自造血条）

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UIPartyRace.h"
#include "UIPartySearch.h"
#include "UISoftKey.h"
#include "UICharInfo.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	UIParty::UIParty() : UIElement(Point<int16_t>(520, 20), Point<int16_t>(312, 389)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// 按 Angel 083 CUIUserList::OnCreate @0x919b73 + CUIPartyHP::Create @0x91f934 重修
		// （round68）：083 组队/玩家列表共用窗资源根 UIWindow.img/UserList，
		// 背景 backgrnd=312x389（.nx 实测）；组队页按钮在 UserList/Party/Bt*（四态全）。
		// 旧实现写的 Party.img 全库 0 命中 → 窗口无背景、四钮空壳（第三/四例同族病）。
		nl::node userlist = nl::nx::UI["UIWindow.img"]["UserList"];
		if (UiAssetResolve::usable(userlist["backgrnd"]))
			sprites.emplace_back(Sprite(userlist["backgrnd"], Point<int16_t>(0, 0)));

		// 083 D8 组队血条真位图：CUIPartyHP 用 UserList/Party/PartyHP/GaugeBar
		// {bar 63x7 填充(origin -3,-3 自动内缩) / gauge 1x7 / graduation 69x13 刻度外框}（.nx 实测）。
		// 旧实现在 draw 里 ColorBox 手绘暗底+30% 转红是「找错资源后自造」——已清除。
		nl::node gaugebar = userlist["Party"]["PartyHP"]["GaugeBar"];
		partyhp_bar = Texture(gaugebar["bar"]);
		partyhp_graduation = Texture(gaugebar["graduation"]);

		// 四功能钮绑到 083 组队页真位图（语义最近映射），坐标取 OnCreate 立即数：
		// BtInvite 2200 (13,355) / BtWithdraw 2202 (190,344) / BtChangeBoss 2205 (249,345) /
		// BtKick 2201 (72,365)。083 组队页完整 9 钮属架构级决策点，本轮只接现有四功能。
		// id 必须显式列出：枚举里 BtClose=2 夹在 BtLeave 与 BtLeader 之间。
		nl::node partysrc = userlist["Party"];
		const char* btnname[] = { "BtInvite", "BtWithdraw", "BtChangeBoss", "BtKick" };
		const uint16_t btnid[] = { BtInvite, BtLeave, BtLeader, BtExpel };
		const Point<int16_t> btnpos[] = {
			Point<int16_t>(13, 355),   // BtInvite 2200
			Point<int16_t>(190, 344),  // BtWithdraw 2202
			Point<int16_t>(249, 345),  // BtChangeBoss 2205
			Point<int16_t>(72, 365)    // BtKick 2201
		};
		for (uint16_t i = 0; i < 4; i++)
			try_maple_btn(buttons, btnid[i], partysrc[btnname[i]], btnpos[i], "UIParty");

		// 关闭钮：v83 真按钮，右上角（312 宽 - 12 - 6）。
		try_maple_btn(buttons, BtClose, UiAssetResolve::close_btn(),
			Point<int16_t>(294, 6), "UIParty close");
	}

	void UIParty::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		// 队伍 id 是包里的数据 => 活文案，不入绑定表。
		title.change_text(Locale::format("party.title", std::to_string(party.id)));
		title.draw(position + Point<int16_t>(10, 8));

		int16_t y = 40;
		for (const auto& m : party.members)
		{
			if (m.id == 0)
				continue;

			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);

			std::string info = m.name + " (Lv." + std::to_string(m.level) + " J" + std::to_string(m.job) + ")";
			// 与好友窗共用 common.* 这两个状态词：同一句话在两个窗里长得不一样，
			// 取证时就要改两处，不如一开始只有一个真源。
			if (m.online)
				info += " " + Locale::format("common.channel", std::to_string(m.channel + 1));
			else
				info += " " + Locale::text("common.offline");
			if (m.id == party.leader)
				info += " ★";

			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));

			// D8：按 083 CUIPartyHP 真位图画血条（round68 重修）。
			// 083 用 UserList/Party/PartyHP/GaugeBar：graduation 69x13 刻度外框 +
			// bar 63x7 按 cur/max 裁右（与 UIStatusBar classic 三联量规同一画法）。
			// 旧实现 ColorBox 手绘暗底+30% 转红是「找错 Party.img 后自造」——已清除。
			// s9 UPDATE_PARTYMEMBER_HP 下发 cid/curhp/maxhp，仅在收到过血量时绘（同族 6 人）。
			auto hp_it = memberhp.find(m.id);
			if (hp_it != memberhp.end() && hp_it->second.second > 0
				&& partyhp_bar && partyhp_graduation)
			{
				int32_t cur = hp_it->second.first;
				int32_t max = hp_it->second.second;
				if (cur < 0) cur = 0;
				if (cur > max) cur = max;

				const int16_t BAR_X = 10;
				const int16_t BAR_Y = y + 16;
				Point<int16_t> at = position + Point<int16_t>(BAR_X, BAR_Y);

				// 外框（origin 0,0）。
				partyhp_graduation.draw(DrawArgument(at));

				// 内条：bar origin (-3,-3)，Texture::draw 自动相对外框内缩 3px；
				// 63 宽满条按百分比裁掉右侧未填充部分（horizontal Range 裁右）。
				const int16_t BAR_W = partyhp_bar.width();
				int16_t fill_w = static_cast<int16_t>(BAR_W * (max > 0 ? static_cast<double>(cur) / max : 0.0));
				if (fill_w < 0) fill_w = 0;
				if (fill_w > BAR_W) fill_w = BAR_W;
				Range<int16_t> crop_right(0, static_cast<int16_t>(BAR_W - fill_w));
				partyhp_bar.draw(DrawArgument(at), {}, crop_right);
			}

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
		// 083 共用窗 312 宽，成员行横向可用区随之放宽（旧 190 绑定 200 宽）。
		if (rel.x() >= 10 && rel.x() <= 302 && rel.y() >= 40 && rel.y() < 250)
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
