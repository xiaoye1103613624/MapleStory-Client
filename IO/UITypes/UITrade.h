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
#pragma once

#include "../UIElement.h"
#include "../Components/Icon.h"

#include "../../Graphics/SpecialText.h"

#include <string>
#include <vector>

namespace ms
{
	struct TradeSlot
	{
		uint8_t slot;
		int32_t itemid;
		int16_t quantity;
	};

	// 实时面交窗（Type::TRADE），由 TradeHandler 按 s9 PLAYER_INTERACTION 包驱动。
	// 底图/按钮取自 UIWindow.img/TradingRoom；与 UIMiscShells 里的 UITradingRoom
	// （DIY 调试壳，Type::TRADINGROOM）不是同一个窗。
	class UITrade : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TRADE;
		// Angel CTradingRoomDlg 为模态交互窗；聚焦以便快捷键落到本窗。
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UITrade();

		void draw(float alpha) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		// 背包物品拖入交易槽：反解 SLOT_X/SLOT_Y → TradeAddItemPacket。
		bool send_icon(const Icon& icon, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

		// getTradeStart(ROOM/3)：number=0 发起人 / 1 被邀请人；partner 名可为空。
		void start_trade(uint8_t number, const std::string& self_name, const std::string& partner_name);
		void partner_joined(const std::string& name);
		void set_meso(uint8_t number, int64_t meso);
		void add_item(uint8_t number, uint8_t slot, int32_t itemid, int16_t quantity);
		// 收到 CONFIRM(0x11) 表示"对方已锁定"，不是自己锁定。
		void set_partner_locked();
		void result(uint8_t number, uint8_t op);
		void add_chat(const std::string& msg);

		// 背包双击时取下一个空闲目标槽（1..9）；0 表示已满。
		uint8_t next_slot() const;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtCoin,    // Angel OnCreate id 1003 @ (170,244)
			BtReset,   // id 2 @ (116,71) — s9 无 RESET，等价 EXIT
			BtTrade,   // id 1002 @ (116,49)
			BtCancel,  // 部署资源 BtCancel；Angel 多用 Esc/Leave
			BtEnter    // Angel BtEnter @ (520,244) 发聊天（无独立 BtChat）
		};

		void prompt_meso();
		void prompt_chat();
		void confirm_trade();
		void clear_state();

		bool started;
		bool self_locked;
		bool partner_locked;
		uint8_t my_number;
		int64_t my_meso;
		int64_t partner_meso;
		std::vector<TradeSlot> my_items;
		std::vector<TradeSlot> partner_items;
		std::string self_name;
		std::string partner_name;
		std::vector<std::string> chat_log;
		mutable OutlinedText title;
		OutlinedText result_text;   // 交易结果提示（成功/取消/失败）
		int32_t result_timer = 0;   // 结果提示剩余帧数，update 递减
	};
}
