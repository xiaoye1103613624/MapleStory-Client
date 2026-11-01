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
#include "UIChat.h"
#include "UIBuddyList.h"
#include "UICharInfo.h"
#include "UIMessenger.h"
#include "UISoftKey.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

#include <list>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIChat::UIChat() : UIDragElement<PosMAPLECHAT>()
	{
		show_weekly = Configuration::get().get_show_weekly();

		nl::node socialChatEnter = UiAssetResolve::window("socialChatEnter");

		if (!UiAssetResolve::usable(socialChatEnter))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node backgrnd = socialChatEnter["backgrnd"];
		nl::node backgrnd4 = socialChatEnter["backgrnd4"];
		nl::node backgrnd5 = socialChatEnter["backgrnd5"];

		rank_shift = Point<int16_t>(86, 130);
		name_shift = Point<int16_t>(50, 5);

		origin_left = Texture(backgrnd4).get_origin();
		origin_right = Texture(backgrnd5).get_origin();

		origin_left = Point<int16_t>(std::abs(origin_left.x()), std::abs(origin_left.y()));
		origin_right = Point<int16_t>(std::abs(origin_right.x()), std::abs(origin_right.y()));

		sprites.emplace_back(socialChatEnter["ribbon"]);
		sprites.emplace_back(backgrnd);
		sprites.emplace_back(socialChatEnter["backgrnd2"]);
		sprites.emplace_back(socialChatEnter["backgrnd3"]);
		sprites.emplace_back(backgrnd4);
		sprites.emplace_back(backgrnd5);

		if (socialChatEnter["btX"])
			buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(socialChatEnter["btX"]);
		if (socialChatEnter["duoChat"])
			buttons[Buttons::CHAT_DUO] = std::make_unique<MapleButton>(socialChatEnter["duoChat"]);
		if (socialChatEnter["groupChatFrd"])
			buttons[Buttons::CHAT_FRIEND] = std::make_unique<MapleButton>(socialChatEnter["groupChatFrd"]);
		if (socialChatEnter["groupChatRnd"])
			buttons[Buttons::CHAT_RANDOM] = std::make_unique<MapleButton>(socialChatEnter["groupChatRnd"]);

		charset = Charset(socialChatEnter["number"], Charset::Alignment::RIGHT);

		name_left = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);
		name_right = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);

		dimension = Texture(backgrnd).get_dimensions();

		if (show_weekly)
			UI::get().emplace<UIRank>();
	}

	void UIChat::draw(float inter) const
	{
		UIElement::draw(inter);

		charset.draw("0", position + origin_left + rank_shift);
		charset.draw("0", position + origin_right + rank_shift);

		name_left.draw(position + origin_left + name_shift);
		name_right.draw(position + origin_right + name_shift);
	}

	void UIChat::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { close(); return; }
		if (!pressed) return;
		if (keycode == 'F' || keycode == 'f' || keycode == 'B' || keycode == 'b')
		{
			int32_t cid = 0;
			if (auto bl = UI::get().get_element<UIBuddyList>())
				cid = bl->get_selected_cid();
			if (cid <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					cid = ci->get_target_cid();
			auto ask_msg = [](int32_t buddy_cid) {
				UI::get().emplace<UISoftKey>([buddy_cid](const std::string& text) {
					if (text.empty()) return;
					std::list<int32_t> recips;
					if (buddy_cid > 0) recips.push_back(buddy_cid);
					MultiChatPacket(MultiChatPacket::Type::BUDDY, recips, text).dispatch();
				}, []() {}, "Buddy msg");
			};
			if (cid > 0)
				ask_msg(cid);
			else
				UI::get().emplace<UISoftKey>([ask_msg](const std::string& cid_s) {
					int32_t id = 0;
					try { if (!cid_s.empty()) id = std::stoi(cid_s); } catch (...) {}
					ask_msg(id);
				}, []() {}, "Buddy cid");
		}
		else if (keycode == 'R' || keycode == 'r' || keycode == 'S' || keycode == 's')
		{
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FindPlayerPacket(pre).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					FindPlayerPacket(name).dispatch();
				}, []() {}, "Find player");
			}
		}
		else if (keycode == 'D' || keycode == 'd')
		{
			UI::get().emplace<UIMessenger>();
			if (auto m = UI::get().get_element<UIMessenger>())
				m->try_create_room();
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			UI::get().emplace<UISoftKey>([](const std::string& text) {
				if (text.empty()) return;
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::PARTY, empty, text).dispatch();
			}, []() {}, "Party msg");
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			UI::get().emplace<UISoftKey>([](const std::string& text) {
				if (text.empty()) return;
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::GUILD, empty, text).dispatch();
			}, []() {}, "Guild msg");
		}
		else if (keycode == 'A' || keycode == 'a')
		{
			UI::get().emplace<UISoftKey>([](const std::string& text) {
				if (text.empty()) return;
				std::list<int32_t> empty;
				MultiChatPacket(MultiChatPacket::Type::ALLIANCE, empty, text).dispatch();
			}, []() {}, "Alliance msg");
		}
	}

	UIElement::Type UIChat::get_type() const
	{
		return TYPE;
	}

	Button::State UIChat::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::CLOSE:
			close();
			break;
		case Buttons::CHAT_DUO:
		{
			// Duo → 打开 Messenger 并发起建房
			UI::get().emplace<UIMessenger>();
			if (auto m = UI::get().get_element<UIMessenger>())
				m->try_create_room();
			LOG(LOG_DEBUG, "UIChat: Duo → UIMessenger");
			break;
		}
		case Buttons::CHAT_FRIEND:
		{
			int32_t cid = 0;
			if (auto bl = UI::get().get_element<UIBuddyList>())
				cid = bl->get_selected_cid();
			if (cid <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					cid = ci->get_target_cid();
			auto ask_msg = [](int32_t buddy_cid) {
				UI::get().emplace<UISoftKey>([buddy_cid](const std::string& text) {
					if (text.empty()) return;
					std::list<int32_t> recips;
					if (buddy_cid > 0) recips.push_back(buddy_cid);
					MultiChatPacket(MultiChatPacket::Type::BUDDY, recips, text).dispatch();
					LOG(LOG_DEBUG, "UIChat: Friend MultiChat cid=" << buddy_cid << " len=" << text.size());
				}, []() {}, "Buddy msg");
			};
			if (cid > 0)
				ask_msg(cid);
			else
				UI::get().emplace<UISoftKey>([ask_msg](const std::string& cid_s) {
					int32_t id = 0;
					try { if (!cid_s.empty()) id = std::stoi(cid_s); } catch (...) {}
					ask_msg(id);
				}, []() {}, "Buddy cid");
			break;
		}
		case Buttons::CHAT_RANDOM:
		{
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FindPlayerPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIChat: Random → FindPlayer name=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					FindPlayerPacket(name).dispatch();
					LOG(LOG_DEBUG, "UIChat: Random → FindPlayer name=" << name);
				}, []() {}, "Find player");
			}
			break;
		}
		default:
			break;
		}

		return Button::State::NORMAL;
	}

	void UIChat::close()
	{
		deactivate();
	}

	UIRank::UIRank() : UIDragElement<PosMAPLECHAT>()
	{
		Configuration::get().set_show_weekly(false);

		nl::node socialRank = UiAssetResolve::window("socialRank");

		if (!UiAssetResolve::usable(socialRank))
		{
			active = false;
			dimension = Point<int16_t>(0, 0);
			return;
		}

		nl::node backgrnd = socialRank["backgrnd"];
		nl::node backgrnd4 = socialRank["backgrnd4"];
		nl::node backgrnd5 = socialRank["backgrnd5"];

		rank_shift = Point<int16_t>(86, 130);
		name_shift = Point<int16_t>(52, 4);

		origin_left = Texture(backgrnd4).get_origin();
		origin_right = Texture(backgrnd5).get_origin();

		origin_left = Point<int16_t>(std::abs(origin_left.x()) - 1, std::abs(origin_left.y()));
		origin_right = Point<int16_t>(std::abs(origin_right.x()), std::abs(origin_right.y()));

		sprites.emplace_back(socialRank["ribbon"]);
		sprites.emplace_back(backgrnd);
		sprites.emplace_back(socialRank["backgrnd2"]);
		sprites.emplace_back(socialRank["backgrnd3"]);
		sprites.emplace_back(backgrnd4);
		sprites.emplace_back(backgrnd5);

		if (socialRank["btX"])
			buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(socialRank["btX"]);

		charset = Charset(socialRank["number"], Charset::Alignment::RIGHT);

		name_left = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);
		name_right = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);

		dimension = Texture(backgrnd).get_dimensions();
		position = position + Point<int16_t>(211, 124);
	}

	void UIRank::draw(float inter) const
	{
		UIElement::draw(inter);

		charset.draw("0", position + origin_left + rank_shift);
		charset.draw("0", position + origin_right + rank_shift);

		name_left.draw(position + origin_left + name_shift);
		name_right.draw(position + origin_right + name_shift);
	}

	void UIRank::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { close(); return; }
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's' || keycode == 'F' || keycode == 'f')
		{
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FindPlayerPacket(pre).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (name.empty()) return;
					FindPlayerPacket(name).dispatch();
				}, []() {}, "Rank find");
			}
		}
	}

	UIElement::Type UIRank::get_type() const
	{
		return TYPE;
	}

	Button::State UIRank::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::CLOSE:
			close();
			break;
		default:
			break;
		}

		return Button::State::NORMAL;
	}

	void UIRank::close()
	{
		deactivate();
	}
}