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
#include "UIGuild.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "UICharInfo.h"
#include "UIGuildBBS.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/BbsPackets.h"
#include "../../Net/Packets/GuildPackets.h"
#include "UIAlliance.h"
#include "UIFamily.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	UIGuild::UIGuild() : UIElement(Point<int16_t>(300, 40), Point<int16_t>(300, 380)),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK),
		notice_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Color::Name::BLACK),
		bbs_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, Color::Name::BLACK)
	{
		nl::node guild = nl::nx::UI["GuildUI.img"];
		if (guild["backgrnd"])
			sprites.emplace_back(Sprite(guild["backgrnd"], Point<int16_t>(0, 0)));

		// round76 Angel 083 复核（REA→IDA→orange-wz→.nx→运行时 open_merge，只读取证）：
		// 083 全库无独立 CUIGuild 浮窗类，公会主体 = CTabGuild::Draw @0x90f17c 挂在
		// CUIUserList 共用窗第 3 页签（UserList/Guild/backgrnd 233x304 + 8 子节点），
		// 独立看板 = CUIGuildBBS（ctor @0x812fb4 / OnCreate @0x8133a9，
		// GuildBBS.img/GuildBBS/backgrnd 734x526 + 14 子节点）。
		// 本行 GuildUI.img 是 083 不存在的虚构根：活 IDB search_text 0 命中 / 物理 Data/UI
		// 无此文件 / orange-wz IMG_MISSING / .nx NOT FOUND / 运行时 open_merge valid=0（五证）。
		// 主路径已按 083 修好：IO/UIStateGame.cpp:361-363 G(17) 路由 UIUserList::Tab::GUILD；
		// 本浮窗仅是底栏社区钮保留的本端旁路，全文字自绘 + 300x380 自定 dimension 属显式
		// 预留，不是按 Angel 布局修复过的窗口。是否并入共用窗 / 换真资源属架构级决策点 D-G，
		// 交用户裁决；本轮零实施，不动 s9 协议 / GuildModel / 五按钮行为。
		// 旧写法 `bn ? MapleButton(bn) : MapleButton(bn, pos)` 两个分支都拿不到贴图、
		// 真值分支把坐标丢成窗口原点，已改为 close_btn() 单点守卫（见下方）。
		nl::node close = UiAssetResolve::close_btn();
		if (close)
			buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(280, 8));
	}

	void UIGuild::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text(guild().name.empty() ? Locale::text("guild.title_plain")
			: Locale::format("guild.title_named", guild().name));
		title.draw(position + Point<int16_t>(10, 8));

		notice_label.change_text(guild().notice);
		notice_label.draw(position + Point<int16_t>(10, 28));

		int16_t y = 48;
		if (!bbs_lines.empty())
		{
			bbs_label.change_text("[BBS]");
			bbs_label.draw(position + Point<int16_t>(10, y));
			y = static_cast<int16_t>(y + 14);
			for (size_t i = 0; i < bbs_lines.size() && i < 5; ++i)
			{
				bbs_label.change_text(bbs_lines[i]);
				bbs_label.draw(position + Point<int16_t>(14, y));
				y = static_cast<int16_t>(y + 14);
			}
			y = static_cast<int16_t>(y + 4);
		}
		else
		{
			y = 55;
		}

		for (const auto& m : guild().members)
		{
			OutlinedText row(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, Color::Name::BLACK);
			// 行首空格留在代码里：加载器会 trim 掉 ini 值两端空白，值写成 ` 职位{}` 也留不住那格。
			std::string info = m.name + " J" + std::to_string(m.job)
				+ " Lv" + std::to_string(m.level)
				+ " " + Locale::format("guild.member_rank", std::to_string(m.rank))
				+ " " + Locale::text(m.online ? "guild.member_online" : "guild.member_offline");
			row.change_text(info);
			row.draw(position + Point<int16_t>(10, y));
			y += ROW_H;
		}

		UIElement::draw_buttons(alpha);
	}

	void UIGuild::update()
	{
	}

	void UIGuild::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(guild().members.size()))
		{
			const GuildMember& m = guild().members[row];
			selected_cid = m.id;
			GuildExpelPacket(m.id, m.name).dispatch();
		}
	}

	Cursor::State UIGuild::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;
		int16_t row = row_by_position(cursorpos - position);
		if (row >= 0 && row < static_cast<int16_t>(guild().members.size()))
		{
			selected_cid = guild().members[row].id;
			LOG(LOG_DEBUG, "UIGuild: select cid=" << selected_cid);
		}
		return st;
	}

	const GuildMember* UIGuild::selected_member() const
	{
		for (const auto& m : guild().members)
			if (m.id == selected_cid)
				return &m;
		return nullptr;
	}

	UIElement::Type UIGuild::get_type() const
	{
		return TYPE;
	}

	void UIGuild::set_guild(const GuildData& g)
	{
		GuildModel::get().set(g);
	}

	void UIGuild::clear_guild()
	{
		GuildModel::get().clear();
	}

	void UIGuild::add_member(const GuildMember& m)
	{
		GuildModel::get().add_member(m);
	}

	void UIGuild::remove_member(int32_t id)
	{
		if (selected_cid == id)
			selected_cid = 0;
		GuildModel::get().remove_member(id);
	}

	void UIGuild::set_member_rank(int32_t id, int32_t rank)
	{
		GuildModel::get().set_member_rank(id, rank);
	}

	void UIGuild::set_member_online(int32_t id, bool online)
	{
		GuildModel::get().set_member_online(id, online);
	}

	void UIGuild::set_bbs_summary(const std::vector<std::string>& lines)
	{
		bbs_lines = lines;
	}

	void UIGuild::set_notice(const std::string& notice)
	{
		GuildModel::get().mutable_data().notice = notice;
	}

	int16_t UIGuild::row_by_position(Point<int16_t> pos) const
	{
		int16_t row = (pos.y() - 55) / ROW_H;
		if (pos.x() < 10 || pos.x() > 290 || row < 0)
			return -1;
		return row;
	}

	Button::State UIGuild::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtCreate:
				prompt_create();
				break;
			case BtInvite:
				prompt_invite();
				break;
			case BtNotice:
				prompt_notice();
				break;
			case BtAlliance:
				UI::get().emplace<UIAlliance>();
				break;
			case BtClose:
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIGuild::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'D' || keycode == 'd')
			prompt_deny();
		else if (keycode == 'L' || keycode == 'l') {
			// 默认本机离开（跳过 SoftKey）；需改目标时走 SoftKey `l` 空输入旁路已不需
			int32_t self = Stage::get().get_player().get_oid();
			std::string selfname = Stage::get().get_player().get_name();
			if (self > 0 && !selfname.empty()) {
				GuildLeavePacket(self, selfname).dispatch();
				LOG(LOG_DEBUG, "UIGuild: Leave self cid=" << self);
			} else {
				UI::get().emplace<UISoftKey>([self, selfname](const std::string& s) {
					int32_t cid = self;
					std::string name = selfname;
					if (!s.empty()) {
						size_t c = s.find(',');
						if (c != std::string::npos) {
							try { cid = std::stoi(s.substr(0, c)); } catch (...) {}
							name = s.substr(c + 1);
						} else {
							name = s;
						}
					}
					if (!name.empty()) {
						GuildLeavePacket(cid, name).dispatch();
						LOG(LOG_DEBUG, "UIGuild: Leave cid=" << cid << " name=" << name);
					}
				}, []() {}, "cid,name|name (self)");
			}
		} else if (keycode == 'J' || keycode == 'j') {
			// SoftKey guild id → char id（CharInfo/self 可跳过第二步）
			UI::get().emplace<UISoftKey>([](const std::string& gid_s) {
				int32_t gid = 0;
				try { gid = std::stoi(gid_s); } catch (...) { return; }
				if (gid <= 0) return;
				int32_t cid = 0;
				if (auto ci = UI::get().get_element<UICharInfo>())
					cid = ci->get_target_cid();
				if (cid <= 0)
					cid = Stage::get().get_player().get_oid();
				if (cid > 0) {
					GuildJoinPacket(gid, cid).dispatch();
					LOG(LOG_DEBUG, "UIGuild: Join gid=" << gid << " cid=" << cid);
				} else {
					UI::get().emplace<UISoftKey>([gid](const std::string& cid_s) {
						try {
							int32_t id = std::stoi(cid_s);
							if (id > 0) {
								GuildJoinPacket(gid, id).dispatch();
								LOG(LOG_DEBUG, "UIGuild: Join gid=" << gid << " cid=" << id);
							}
						} catch (...) {}
					}, []() {}, "Char id");
				}
			}, []() {}, "Guild id");
		} else if (keycode == 'C' || keycode == 'c') {
			prompt_create();
		} else if (keycode == 'I' || keycode == 'i') {
			prompt_invite();
		} else if (keycode == 'N' || keycode == 'n') {
			prompt_notice();
		} else if (keycode == 'E' || keycode == 'e') {
			if (const GuildMember* m = selected_member()) {
				GuildExpelPacket(m->id, m->name).dispatch();
				LOG(LOG_DEBUG, "UIGuild: Expel cid=" << m->id << " name=" << m->name);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					if (s.empty()) return;
					size_t c = s.find(',');
					int32_t cid = 0;
					std::string name = s;
					if (c != std::string::npos) {
						try { cid = std::stoi(s.substr(0, c)); } catch (...) {}
						name = s.substr(c + 1);
					}
					if (!name.empty()) {
						selected_cid = cid;
						GuildExpelPacket(cid, name).dispatch();
						LOG(LOG_DEBUG, "UIGuild: Expel cid=" << cid << " name=" << name);
					}
				}, []() {}, "cid,name|name");
			}
		} else if (keycode == 'A' || keycode == 'a') {
			auto ui = UI::get().get_element<UIAlliance>();
			if (!ui) ui = UI::get().emplace<UIAlliance>();
			else if (!ui->is_active()) ui->makeactive();
		} else if (keycode == 'F' || keycode == 'f') {
			auto ui = UI::get().get_element<UIFamily>();
			if (!ui) ui = UI::get().emplace<UIFamily>();
			else if (!ui->is_active()) ui->makeactive();
		} else if (keycode == 'B' || keycode == 'b') {
			// 打开原生 GuildBBS.img 窗并请求第 0 页；高级 SoftKey 命令仍可用 Shift+B
			auto bbs = UI::get().get_element<UIGuildBBS>();
			if (!bbs)
				bbs = UI::get().emplace<UIGuildBBS>();
			else if (!bbs->is_active())
				bbs->makeactive();
			if (bbs)
				bbs->request_list(0);
			LOG(LOG_INFO, "UIGuild: open GuildBBS list");
		}
	}

	void UIGuild::prompt_create()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (s.length() >= 3 && s.length() <= 12)
				GuildCreatePacket(s).dispatch();
		});
	}

	void UIGuild::prompt_invite()
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			GuildInvitePacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIGuild: Invite target=" << pre);
			return;
		}
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				GuildInvitePacket(s).dispatch();
		});
	}

	void UIGuild::prompt_notice()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty() && s.length() <= 100)
				GuildNoticePacket(s).dispatch();
		});
	}

	void UIGuild::prompt_deny()
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			DenyGuildRequestPacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIGuild: DenyGuild from=" << pre);
			return;
		}
		UI::get().emplace<UISoftKey>([](const std::string& from)
		{
			if (from.empty()) return;
			DenyGuildRequestPacket(from).dispatch();
			LOG(LOG_DEBUG, "UIGuild: DenyGuild from=" << from);
		}, []() {}, "Deny guild from");
	}
}
