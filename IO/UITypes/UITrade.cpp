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
#include "UIItemInventory.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Util/Misc.h"

namespace ms
{
	UITrade::UITrade() : UIElement(Point<int16_t>(260, 120), Point<int16_t>(360, 260)),
		started(false), locked(false), my_meso(0), partner_meso(0), add_count(0),
		title(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// 本包 UI.nx 根下没有 Trade.img（v83 真身是 UIWindow.img/TradingRoom 565x474，
		// 只有 BtTrade/BtCancel/BtReset/BtCoin 四钮，件表见 notes/asset-node-inventory.md §1.5）。
		// 换根须有 TradingRoom 内的原生坐标，目前无参考截图也无 IDA 证据 ⇒ 底衬与五钮显式预留，
		// 交易流程仍可用菜单/右键与 ESC 退出；旧写法 bn ? MapleButton(bn) : MapleButton(bn, pos)
		// 两个分支都拿不到贴图（真值分支还会把坐标丢成窗口原点），属凭空逻辑。
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
			try_maple_btn(buttons, i, trade[btnname[i]], btnpos[i], "UITrade");
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

	void UITrade::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			TradeExitPacket().dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'X' || keycode == 'x')
		{
			TradeExitPacket().dispatch();
			deactivate();
		}
		else if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's')
			prompt_meso();
		else if (keycode == 'C' || keycode == 'c' || keycode == 'T' || keycode == 't')
			prompt_chat();
		else if (keycode == 'L' || keycode == 'l')
			TradeConfirmPacket().dispatch();
		else if (keycode == 13)
			TradeTransactionPacket().dispatch();
		else if (keycode == 'I' || keycode == 'i' || keycode == 'A' || keycode == 'a')
		{
			// SoftKey：背包有选中格时只问 qty；否则 iv,pos,qty,slot
			int16_t inv_slot = 0;
			uint8_t inv_type = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				inv_slot = inv->get_selected_slot();
				if (inv_slot > 0)
					inv_type = static_cast<uint8_t>(inv->get_tab());
			}
			if (inv_slot > 0 && inv_type > 0) {
				uint8_t iv = inv_type;
				int16_t pos = inv_slot;
				UI::get().emplace<UISoftKey>([iv, pos](const std::string& s) {
					int16_t qty = 1;
					uint8_t trade_slot = 0;
					if (!s.empty()) {
						size_t c = s.find(',');
						try {
							qty = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
							if (c != std::string::npos)
								trade_slot = static_cast<uint8_t>(std::stoi(s.substr(c + 1)));
						} catch (...) { return; }
					}
					if (qty < 1) return;
					TradeAddItemPacket(iv, pos, qty, trade_slot).dispatch();
					LOG(LOG_DEBUG, "UITrade: AddItem from inv iv=" << (int)iv
						<< " pos=" << pos << " qty=" << qty);
				}, []() {}, "qty[,tradeSlot]");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int vals[4] = { 1, 1, 1, 0 };
					size_t start = 0; int vi = 0;
					while (vi < 4 && start <= s.size()) {
						size_t c = s.find(',', start);
						std::string part = (c == std::string::npos) ? s.substr(start) : s.substr(start, c - start);
						try { vals[vi] = std::stoi(part); } catch (...) {}
						++vi;
						if (c == std::string::npos) break;
						start = c + 1;
					}
					if (vals[1] > 0 && vals[2] > 0) {
						TradeAddItemPacket(
							static_cast<uint8_t>(vals[0]), static_cast<int16_t>(vals[1]),
							static_cast<int16_t>(vals[2]), static_cast<uint8_t>(vals[3])
						).dispatch();
						LOG(LOG_DEBUG, "UITrade: SoftKey AddItem " << s);
					}
				}, []() {}, "iv,pos,qty,slot");
			}
		}
	}
}
