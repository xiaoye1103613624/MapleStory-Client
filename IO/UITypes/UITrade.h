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
#pragma once

#include "../UIElement.h"

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

	class UITrade : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::TRADE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UITrade();

		void draw(float alpha) const override;
		void update() override;
		UIElement::Type get_type() const override;

		void start_trade();
		void partner_joined(const std::string& name);
		void set_meso(uint8_t number, int32_t meso);
		void add_item(uint8_t number, uint8_t slot, int32_t itemid, int16_t quantity);
		void set_locked();
		void result(uint8_t number, uint8_t op);
		void add_chat(const std::string& msg);

		// 物品栏双击时由 UIItemInventory 调用：加入交易
		uint8_t next_slot() const;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtAddMeso,
			BtLock,
			BtTrade,
			BtCancel,
			BtChat
		};

		void prompt_meso();
		void prompt_chat();

		bool started;
		bool locked;
		int32_t my_meso;
		int32_t partner_meso;
		std::vector<TradeSlot> my_items;
		std::vector<TradeSlot> partner_items;
		std::string partner_name;
		std::vector<std::string> chat_log;
		uint8_t add_count;
		mutable OutlinedText title;
	};
}
