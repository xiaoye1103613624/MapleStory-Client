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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#include "UIAlliance.h"
#include "UICharInfo.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/AlliancePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAlliance::UIAlliance() :
		title(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, "Alliance"),
		name_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, ""),
		notice_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, ""),
		guild_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "")
	{
		dimension = Point<int16_t>(360, 320);

#ifdef USE_NX
		nl::node panel = nl::nx::UI["UIWindow.img"]["UserList"]["GuildUnion"]["backgrnd"];
		if (panel)
		{
			background = panel;
			has_background = true;
			dimension = background.get_dimensions();
		}

		nl::node close = UiAssetResolve::close_btn();
		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(dimension.x() - 21, 6));

		nl::node bref = nl::nx::UI["Basic.img"]["BtOK"];
		if (bref)
			buttons[Buttons::BtRefresh] = std::make_unique<MapleButton>(bref, Point<int16_t>(20, dimension.y() - 36));

		nl::node bleave = nl::nx::UI["Basic.img"]["BtCancel"];
		if (bleave)
			buttons[Buttons::BtLeave] = std::make_unique<MapleButton>(bleave, Point<int16_t>(100, dimension.y() - 36));

		// Invite / Notice：无 GuildUnion 专用钮时复用 Basic，对齐 SoftKey I/N
		nl::node binv = nl::nx::UI["Basic.img"]["BtOK"];
		if (binv)
			buttons[Buttons::BtInvite] = std::make_unique<MapleButton>(binv, Point<int16_t>(180, dimension.y() - 36));
		nl::node bnotice = nl::nx::UI["Basic.img"]["BtCancel"];
		if (bnotice)
			buttons[Buttons::BtNotice] = std::make_unique<MapleButton>(bnotice, Point<int16_t>(260, dimension.y() - 36));
#endif

		// 打开即请求联盟信息（服务端回 getAllianceInfo 0x0C）
		AllianceActionPacket(0x01).dispatch();
	}

	void UIAlliance::set_alliance(int32_t id, const std::string& name, const std::vector<std::string>& ranks,
		const std::vector<int32_t>& guilds, const std::string& notice)
	{
		set_alliance(id, name, ranks, guilds, {}, notice);
	}

	void UIAlliance::set_alliance(int32_t id, const std::string& name, const std::vector<std::string>& ranks,
		const std::vector<int32_t>& guilds, const std::vector<std::string>& gnames, const std::string& notice)
	{
		alliance_id = id;
		alliance_name = name;
		alliance_notice = notice;
		this->guilds = guilds;
		this->guild_names = gnames;
		(void)ranks;

		name_label.change_text(Locale::format("alliance.name_line", name));
		notice_label.change_text(Locale::format("alliance.notice_line", notice));
		refresh_guild_label();
	}

	void UIAlliance::set_notice(const std::string& notice)
	{
		alliance_notice = notice;
		notice_label.change_text(Locale::format("alliance.notice_line", notice));
	}

	void UIAlliance::set_guild_names(const std::vector<std::string>& guild_names)
	{
		// 0x0D 只带公会块；若公会 id 列表还没到（0x0C/0x0F 未到），
		// 按块的书写次序，名称数组即可直接当 id 数组的并行展示。
		if (guilds.empty() && !guild_names.empty())
		{
			guilds.resize(guild_names.size(), 0);
		}
		this->guild_names = guild_names;
		refresh_guild_label();
	}

	void UIAlliance::set_ranks(const std::vector<std::string>& ranks)
	{
		this->rank_titles = ranks;
	}

	void UIAlliance::refresh_guild_label()
	{
		std::string txt = Locale::format("alliance.guilds_header", std::to_string(guilds.size()));
		for (size_t i = 0; i < guilds.size(); i++)
		{
			txt += " ";
			if (i < guild_names.size() && !guild_names[i].empty())
				txt += guild_names[i];
			else
				txt += std::to_string(guilds[i]);
		}
		guild_label.change_text(txt);
	}

	void UIAlliance::draw(float inter) const
	{
		if (has_background)
			background.draw(position, inter);

		title.draw(position + Point<int16_t>(dimension.x() / 2, 8));
		name_label.draw(position + Point<int16_t>(20, 48));
		notice_label.draw(position + Point<int16_t>(20, 68));
		guild_label.draw(position + Point<int16_t>(20, 88));

		for (const auto& b : buttons)
			if (b.second)
				b.second->draw(position);
	}

	Button::State UIAlliance::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BtRefresh:
				AllianceActionPacket(0x01).dispatch();
				return Button::State::NORMAL;
			case Buttons::BtLeave:
				AllianceActionPacket(0x02).dispatch();
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BtInvite:
				UI::get().emplace<UISoftKey>([](const std::string& guild) {
					if (guild.empty()) return;
					AllianceInvitePacket(guild).dispatch();
					LOG(LOG_DEBUG, "UIAlliance: BtInvite guild=" << guild);
				}, []() {}, "Invite guild");
				return Button::State::NORMAL;
			case Buttons::BtNotice:
				UI::get().emplace<UISoftKey>([this](const std::string& notice) {
					if (notice.empty()) return;
					AllianceNoticePacket(notice).dispatch();
					set_notice(notice);
					LOG(LOG_DEBUG, "UIAlliance: BtNotice len=" << notice.size());
				}, []() {}, "Alliance notice");
				return Button::State::NORMAL;
		}

		return Button::State::DISABLED;
	}

	void UIAlliance::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'N' || keycode == 'n') {
			std::string cur = get_notice();
			UI::get().emplace<UISoftKey>([this](const std::string& notice) {
				if (notice.empty()) return;
				AllianceNoticePacket(notice).dispatch();
				set_notice(notice);
				LOG(LOG_DEBUG, "UIAlliance: notice len=" << notice.size());
			}, []() {}, cur.empty() ? "Alliance notice" : ("Notice (cur=" + cur.substr(0, 24) + ")"));
		} else if (keycode == 'I' || keycode == 'i') {
			UI::get().emplace<UISoftKey>([](const std::string& guild) {
				if (guild.empty()) return;
				AllianceInvitePacket(guild).dispatch();
				LOG(LOG_DEBUG, "UIAlliance: invite guild=" << guild);
			}, []() {}, "Invite guild");
		} else if (keycode == 'D' || keycode == 'd') {
			// DENY_ALLIANCE_REQUEST：CharInfo 名作 inviter → 只问 guild
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				std::string inviter = pre;
				UI::get().emplace<UISoftKey>([inviter](const std::string& guild) {
					if (guild.empty()) return;
					DenyAllianceRequestPacket(inviter, guild).dispatch();
					LOG(LOG_DEBUG, "UIAlliance: Deny inviter=" << inviter << " guild=" << guild);
				}, []() {}, "Alliance deny guild");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& spec) {
					if (spec.empty()) return;
					size_t c = spec.find(',');
					if (c == std::string::npos) return;
					std::string inviter = spec.substr(0, c);
					std::string guild = spec.substr(c + 1);
					if (inviter.empty() || guild.empty()) return;
					DenyAllianceRequestPacket(inviter, guild).dispatch();
					LOG(LOG_DEBUG, "UIAlliance: Deny inviter=" << inviter << " guild=" << guild);
				}, []() {}, "inviter,guild");
			}
		} else if (keycode == 'R' || keycode == 'r') {
			AllianceActionPacket(0x01).dispatch();
			LOG(LOG_DEBUG, "UIAlliance: Refresh");
		} else if (keycode == 'L' || keycode == 'l') {
			AllianceActionPacket(0x02).dispatch();
			LOG(LOG_DEBUG, "UIAlliance: Leave");
			deactivate();
		}
	}

	UIElement::Type UIAlliance::get_type() const
	{
		return TYPE;
	}
}
