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
#include "UITrade.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/TradePackets.h"

namespace ms
{
	UITrade::UITrade() : UIElement(Point<int16_t>(260, 120), Point<int16_t>(360, 260)),
		started(false), locked(false), my_meso(0), partner_meso(0), add_count(0),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		nl::node trade = nl::nx::UI["Trade.img"];
		if (trade["backgrnd"])
			sprites.emplace_back(Sprite(trade["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 220),    // BtAddMeso
			Point<int16_t>(100, 220),   // BtLock
			Point<int16_t>(160, 220),   // BtTrade
			Point<int16_t>(220, 220),   // BtCancel
			Point<int16_t>(300, 8)      // BtChat
		};
		const char* btnname[] = { "BtAddMeso", "BtLock", "BtTrade", "BtCancel", "BtChat" };
		for (uint16_t i = 0; i < 5; i++)
		{
			nl::node bn = trade[btnname[i]];
			buttons[i] = bn
				? std::make_unique<MapleButton>(bn)
				: std::make_unique<MapleButton>(bn, btnpos[i]);
		}
	}

	void UITrade::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text(std::string("交易") + (started ? "" : "（等待对方）"));
		title.draw(position + Point<int16_t>(10, 8));

		// 我方（左列）
		OutlinedText my(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK);
		my.change_text("金币: " + std::to_string(my_meso) + (locked ? " [锁定]" : ""));
		my.draw(position + Point<int16_t>(10, 30));

		int16_t y = 55;
		for (const auto& s : my_items)
		{
			const Texture& icon = ItemData::get(s.itemid).get_icon(false);
			if (icon)
				icon.draw(position + Point<int16_t>(10, y));
			if (s.quantity > 1)
			{
				OutlinedText q(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::WHITE, Color::Name::BLACK);
				q.change_text(std::to_string(s.quantity));
				q.draw(position + Point<int16_t>(44, y + 18));
			}
			y += 30;
		}

		// 对方（右列）
		OutlinedText partner(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK);
		partner.change_text(partner_name + " 金币: " + std::to_string(partner_meso));
		partner.draw(position + Point<int16_t>(190, 30));

		y = 55;
		for (const auto& s : partner_items)
		{
			const Texture& icon = ItemData::get(s.itemid).get_icon(false);
			if (icon)
				icon.draw(position + Point<int16_t>(190, y));
			if (s.quantity > 1)
			{
				OutlinedText q(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::WHITE, Color::Name::BLACK);
				q.change_text(std::to_string(s.quantity));
				q.draw(position + Point<int16_t>(224, y + 18));
			}
			y += 30;
		}

		// 聊天
		y = 150;
		for (const auto& c : chat_log)
		{
			OutlinedText cl(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Color::Name::BLACK);
			cl.change_text(c);
			cl.draw(position + Point<int16_t>(10, y));
			y += 14;
		}

		UIElement::draw_buttons(alpha);
	}

	void UITrade::update()
	{
	}

	UIElement::Type UITrade::get_type() const
	{
		return TYPE;
	}

	void UITrade::start_trade()
	{
		started = true;
	}

	void UITrade::partner_joined(const std::string& name)
	{
		partner_name = name;
	}

	void UITrade::set_meso(uint8_t number, int32_t meso)
	{
		if (number == 0)
			my_meso = meso;
		else
			partner_meso = meso;
	}

	void UITrade::add_item(uint8_t number, uint8_t slot, int32_t itemid, int16_t quantity)
	{
		TradeSlot s;
		s.slot = slot;
		s.itemid = itemid;
		s.quantity = quantity;
		if (number == 0)
			my_items.push_back(s);
		else
			partner_items.push_back(s);
	}

	void UITrade::set_locked()
	{
		locked = true;
	}

	void UITrade::result(uint8_t number, uint8_t op)
	{
		// op: 0x01 完成 / 0x02 取消 等；关闭窗口
		deactivate();
	}

	void UITrade::add_chat(const std::string& msg)
	{
		chat_log.push_back(msg);
		if (chat_log.size() > 5)
			chat_log.erase(chat_log.begin());
	}

	uint8_t UITrade::next_slot() const
	{
		return static_cast<uint8_t>(my_items.size() + 1);
	}

	Button::State UITrade::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtAddMeso:
				prompt_meso();
				break;
			case BtLock:
				TradeConfirmPacket().dispatch();
				break;
			case BtTrade:
				TradeTransactionPacket().dispatch();
				break;
			case BtCancel:
				TradeExitPacket().dispatch();
				deactivate();
				break;
			case BtChat:
				prompt_chat();
				break;
		}
		return Button::State::NORMAL;
	}

	void UITrade::prompt_meso()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			try
			{
				int32_t amt = std::stoi(s);
				if (amt > 0)
					TradeSetMesoPacket(amt).dispatch();
			}
			catch (...)
			{
			}
		});
	}

	void UITrade::prompt_chat()
	{
		UI::get().emplace<UISoftKey>([this](const std::string& s)
		{
			if (!s.empty())
				TradeChatPacket(s).dispatch();
		});
	}
}
