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

#include "../../Character/Inventory/InventoryType.h"
#include "../../Graphics/SpecialText.h"
#include "../../IO/Components/Icon.h"

#include <vector>

namespace ms
{
	// 仓库中的单件物品（仅记录渲染/交互所需字段）。
	struct StoredItem
	{
		int16_t slot;
		InventoryType::Id type;
		int32_t itemid;
		int16_t quantity;
	};

	class UIStorage : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::STORAGE;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIStorage();

		void draw(float alpha) const override;
		void update() override;
		void doubleclick(Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

		// 由 StorageHandler 填充
		void set_storage(uint8_t slots, int64_t meso, std::vector<StoredItem> items);
		void update_items(uint8_t slots, std::vector<StoredItem> items);
		void set_meso(uint8_t slots, int64_t meso);
		void show_error(uint8_t code);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BtArrange,
			BtMesoIn,
			BtMesoOut,
			BtClose
		};

		static constexpr int16_t COLS = 8;
		static constexpr int16_t CELL = 38;

		Point<int16_t> slot_pos(int16_t slot) const;
		int16_t slot_by_position(Point<int16_t> pos) const;
		void prompt_meso(bool deposit);
		const StoredItem* selected_item() const;

		uint8_t capacity;
		int64_t meso;
		int16_t selected_slot = -1;
		std::vector<StoredItem> stored;
		OutlinedText mesolabel;
	};
}
