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
//////////////////////////////////////////////////////////////////////////////////
#include "UITrade.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "UIItemInventory.h"
#include "UIStatusMessenger.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	namespace
	{
		// UIWindow.img/TradingRoom/backgrnd 565x474 的实测分区（R1/R2：运行时部署数据）：
		//  左上 0..285x125 为双方名牌区，名牌条 (20..105,105..120) / (185..270,105..120)
		//  左列 3x3 槽 x=6/45/84，右列 3x3 槽 x=169/208/247，行 y=129/166/203
		//  金币框 (20..120,243..273) / (175..280,243..273)，BtCoin 14x14 在左框左端
		//  右上 295..545x25..230 为聊天记录白框，295..490x243..270 为聊天输入条
		//  左下 2..285x290..465 为烘焙手续费表，右下 290..565x285..474 为按钮区
		constexpr int16_t SLOT_X[2][3] = { { 6, 45, 84 }, { 169, 208, 247 } };
		constexpr int16_t SLOT_Y[3] = { 129, 166, 203 };
		constexpr size_t MAX_CHAT_LINES = 13;
	}

	UITrade::UITrade() : UIElement(Point<int16_t>(260, 120), Point<int16_t>(565, 474)),
		started(false), self_locked(false), partner_locked(false), my_number(0),
		my_meso(0), partner_meso(0),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// WZ 里 MapleButton 节点不带坐标，必须显式传 Point（见 MapleButton.h）。
		nl::node trade = UiAssetResolve::window("TradingRoom");
		if (trade["backgrnd"])
			sprites.emplace_back(Sprite(trade["backgrnd"], Point<int16_t>(0, 0)));

		struct BtnDef { const char* name; uint16_t id; Point<int16_t> pos; };
		const BtnDef defs[] = {
			{ "BtCoin",   BtCoin,   Point<int16_t>(3, 242) },
			{ "BtReset",  BtReset,  Point<int16_t>(300, 438) },
			{ "BtTrade",  BtTrade,  Point<int16_t>(380, 438) },
			{ "BtCancel", BtCancel, Point<int16_t>(460, 438) },
		};
		for (const auto& d : defs)
			try_maple_btn(buttons, d.id, trade[d.name], d.pos, "UITrade");
	}

	void UITrade::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		title.change_text(Locale::text("trade.title")
			+ (self_locked ? " " + Locale::text("trade.self_locked") : std::string())
			+ (partner_locked ? " " + Locale::text("trade.partner_locked") : std::string()));
		title.draw(position + Point<int16_t>(12, 10));

		// 名牌条：白字黑描边（深蓝/蓝底）
		OutlinedText names(Text::Font::A11M, Text::Alignment::LEFT,
			Color::Name::WHITE, Color::Name::BLACK);
		names.change_text(self_name);
		names.draw(position + Point<int16_t>(22, 106));
		names.change_text(partner_name.empty() ? Locale::text("trade.waiting") : partner_name);
		names.draw(position + Point<int16_t>(187, 106));

		// 报价物品：slot 1..9 → 3 列 x 3 行，左右各一组
		auto draw_items = [this](const std::vector<TradeSlot>& list, int side)
		{
			for (const auto& s : list)
			{
				if (s.slot < 1 || s.slot > 9)
					continue;
				uint8_t idx = static_cast<uint8_t>(s.slot - 1);
				int16_t x = SLOT_X[side][idx % 3];
				int16_t y = SLOT_Y[idx / 3];
				const Texture& icon = ItemData::get(s.itemid).get_icon(false);
				if (icon)
					icon.draw(position + Point<int16_t>(x + 1, y - 2));
				if (s.quantity > 1)
				{
					OutlinedText q(Text::Font::A11M, Text::Alignment::RIGHT,
						Color::Name::WHITE, Color::Name::BLACK);
					q.change_text(std::to_string(s.quantity));
					q.draw(position + Point<int16_t>(x + 32, y + 13));
				}
			}
		};
		draw_items(my_items, 0);
		draw_items(partner_items, 1);

		// 金币框：白底用纯黑字（描边同色，避免糊成一团）
		OutlinedText meso_text(Text::Font::A11M, Text::Alignment::RIGHT,
			Color::Name::BLACK, Color::Name::BLACK);
		meso_text.change_text(std::to_string(my_meso));
		meso_text.draw(position + Point<int16_t>(116, 250));
		meso_text.change_text(std::to_string(partner_meso));
		meso_text.draw(position + Point<int16_t>(276, 250));

		// 聊天记录（右上白框）
		int16_t y = 28;
		for (const auto& c : chat_log)
		{
			OutlinedText cl(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::BLACK, Color::Name::BLACK);
			cl.change_text(c);
			cl.draw(position + Point<int16_t>(298, y));
			y += 15;
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

	void UITrade::start_trade(uint8_t number, const std::string& self, const std::string& partner)
	{
		clear_state();
		started = true;
		my_number = number;
		self_name = self;
		partner_name = partner;
	}

	void UITrade::partner_joined(const std::string& name)
	{
		partner_name = name;
		started = true;
	}

	void UITrade::set_meso(uint8_t number, int64_t meso)
	{
		if (number == 0)
			my_meso = meso;
		else
			partner_meso = meso;
	}

	void UITrade::add_item(uint8_t number, uint8_t slot, int32_t itemid, int16_t quantity)
	{
		std::vector<TradeSlot>& list = (number == 0) ? my_items : partner_items;
		for (auto& s : list)
		{
			if (s.slot == slot)
			{
				s.itemid = itemid;
				s.quantity = quantity;
				return;
			}
		}
		if (list.size() < 9)
			list.push_back({ slot, itemid, quantity });
	}

	void UITrade::set_partner_locked()
	{
		partner_locked = true;
	}

	void UITrade::result(uint8_t number, uint8_t op)
	{
		(void)number;
		(void)op;
		clear_state();
		deactivate();
	}

	void UITrade::add_chat(const std::string& msg)
	{
		chat_log.push_back(msg);
		while (chat_log.size() > MAX_CHAT_LINES)
			chat_log.erase(chat_log.begin());
	}

	uint8_t UITrade::next_slot() const
	{
		for (uint8_t slot = 1; slot <= 9; ++slot)
		{
			bool used = false;
			for (const auto& s : my_items)
			{
				if (s.slot == slot)
				{
					used = true;
					break;
				}
			}
			if (!used)
				return slot;
		}
		return 0;
	}

	void UITrade::clear_state()
	{
		started = false;
		self_locked = false;
		partner_locked = false;
		my_number = 0;
		my_meso = 0;
		partner_meso = 0;
		my_items.clear();
		partner_items.clear();
		self_name.clear();
		partner_name.clear();
		chat_log.clear();
	}

	void UITrade::confirm_trade()
	{
		if (!started || self_locked)
			return;
		self_locked = true;
		// s9 无 TRANSACTION(0x14) 分支：成交由 CONFIRM 双方各发一次完成握手。
		TradeConfirmPacket().dispatch();
	}

	Button::State UITrade::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtCoin:
				if (!self_locked)
					prompt_meso();
				break;
			case BtReset:
				// 原生有 BtReset（重置报价），但 s9 Action 枚举无 RESET，服务端不支持。
				// 最接近的等价行为是 EXIT：退还已放入的物品/金币并结束本次交易。
				TradeExitPacket().dispatch();
				clear_state();
				deactivate();
				break;
			case BtTrade:
				confirm_trade();
				break;
			case BtCancel:
				TradeExitPacket().dispatch();
				clear_state();
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
			if (self_locked)
				return;
			try
			{
				int64_t amt = std::stoll(s);
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

	void UITrade::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			TradeExitPacket().dispatch();
			clear_state();
			deactivate();
			return;
		}
		if (!pressed)
			return;
		if (keycode == 'X' || keycode == 'x')
		{
			TradeExitPacket().dispatch();
			clear_state();
			deactivate();
		}
		else if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's')
			prompt_meso();
		else if (keycode == 'C' || keycode == 'c' || keycode == 'T' || keycode == 't')
			prompt_chat();
		else if (keycode == 'L' || keycode == 'l' || keycode == 13)
			confirm_trade();
		else if (keycode == 'I' || keycode == 'i' || keycode == 'A' || keycode == 'a')
		{
			int16_t inv_slot = 0;
			uint8_t inv_type = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>())
			{
				inv_slot = inv->get_selected_slot();
				if (inv_slot > 0)
					inv_type = static_cast<uint8_t>(inv->get_tab());
			}
			uint8_t auto_slot = next_slot();
			if (auto_slot == 0)
			{
				if (auto sm = UI::get().get_element<UIStatusMessenger>())
					sm->show_status(Color::Name::YELLOW, Locale::text("trade.slots_full"));
				return;
			}
			if (inv_slot > 0 && inv_type > 0)
			{
				uint8_t iv = inv_type;
				int16_t pos = inv_slot;
				UI::get().emplace<UISoftKey>([iv, pos, auto_slot](const std::string& s)
				{
					int16_t qty = 1;
					uint8_t trade_slot = auto_slot;
					if (!s.empty())
					{
						size_t c = s.find(',');
						try
						{
							qty = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
							if (c != std::string::npos)
								trade_slot = static_cast<uint8_t>(std::stoi(s.substr(c + 1)));
						}
						catch (...)
						{
							return;
						}
					}
					if (qty < 1 || trade_slot < 1 || trade_slot > 9)
						return;
					TradeAddItemPacket(iv, pos, qty, trade_slot).dispatch();
				}, []() {}, "qty[,tradeSlot]");
			}
			else
			{
				UI::get().emplace<UISoftKey>([auto_slot](const std::string& s)
				{
					int vals[4] = { 1, 1, 1, auto_slot };
					size_t start = 0;
					int vi = 0;
					while (vi < 4 && start <= s.size())
					{
						size_t c = s.find(',', start);
						std::string part = (c == std::string::npos) ? s.substr(start) : s.substr(start, c - start);
						try { vals[vi] = std::stoi(part); } catch (...) {}
						++vi;
						if (c == std::string::npos)
							break;
						start = c + 1;
					}
					if (vals[1] > 0 && vals[2] > 0 && vals[3] >= 1 && vals[3] <= 9)
					{
						TradeAddItemPacket(
							static_cast<uint8_t>(vals[0]), static_cast<int16_t>(vals[1]),
							static_cast<int16_t>(vals[2]), static_cast<uint8_t>(vals[3])
						).dispatch();
					}
				}, []() {}, "iv,pos,qty,slot");
			}
		}
	}
}
