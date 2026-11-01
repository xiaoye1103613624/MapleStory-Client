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
#include "StorageHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStorage.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Character/EquipStat.h"
#include "../../Template/EnumMap.h"

#include <string>

namespace ms
{
	void StorageHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.read_byte();

		auto ensure_sm = [](Color::Name color, const std::string& tip) {
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		};
		auto ensure_open = []() -> Optional<UIStorage> {
			auto ui = UI::get().get_element<UIStorage>();
			if (!ui)
				ui = UI::get().emplace<UIStorage>();
			else if (!ui->is_active())
				ui->makeactive();
			return ui;
		};

		switch (mode)
		{
			case 0x16: // getStorage：全量下发 — 开窗
			{
				auto uistorage = ensure_open();
				if (!uistorage) return;
				recv.read_int();        // npcId
				uint8_t slots = recv.read_byte();
				recv.read_short();      // 0x7E
				recv.read_short();
				recv.read_int();
				int64_t meso = recv.read_int();
				recv.read_short();
				int16_t count = recv.read_byte();
				std::vector<StoredItem> items;
				parse_items(recv, count, items);
				recv.read_short();
				recv.read_byte();
				uistorage->set_storage(slots, meso, std::move(items));
				// 仓库 UI 已刷，不 tip
				(void)count; (void)meso;
				break;
			}
			case 0x0D: // storeStorage
			case 0x09: // takeOutStorage
			{
				uint8_t slots = recv.read_byte();
				recv.read_short();      // type bitfield / 0x7E
				recv.read_short();
				recv.read_int();
				int16_t count = recv.read_byte();
				std::vector<StoredItem> items;
				parse_items(recv, count, items);
				if (auto uistorage = ensure_open())
					uistorage->update_items(slots, std::move(items));
				(void)mode;
				break;
			}
			case 0x0F: // arrangeStorage
			{
				uint8_t slots = recv.read_byte();
				recv.read_byte();       // 124
				recv.skip(10);
				int16_t count = recv.read_byte();
				std::vector<StoredItem> items;
				parse_items(recv, count, items);
				recv.read_byte();
				if (auto uistorage = ensure_open())
					uistorage->update_items(slots, std::move(items));
				break;
			}
			case 0x13: // mesoStorage
			{
				uint8_t slots = recv.read_byte();
				recv.read_short();
				recv.read_short();
				recv.read_int();
				int64_t meso = recv.read_int();
				if (auto uistorage = ensure_open())
					uistorage->set_meso(slots, meso);
				(void)meso;
				break;
			}
			default:   // 错误码：已开 show_error；未开才 SM
			{
				if (auto uistorage = UI::get().get_element<UIStorage>())
				{
					if (uistorage->is_active())
					{
						uistorage->show_error(mode);
						break;
					}
				}
				ensure_sm(Color::Name::RED, "Storage err " + std::to_string(mode));
				break;
			}
		}
		while (recv.available())
			recv.skip_byte();
	}

	void StorageHandler::parse_items(InPacket& recv, int16_t count, std::vector<StoredItem>& out) const
	{
		for (int16_t i = 0; i < count; i++)
		{
			uint8_t type = recv.read_byte();
			int32_t iid = recv.read_int();
			InventoryType::Id invtype = static_cast<InventoryType::Id>(type);

			StoredItem si;
			si.type = invtype;
			si.itemid = iid;
			si.slot = i; // 顺序槽位（0 基）

			if (type == static_cast<uint8_t>(InventoryType::Id::EQUIP))
			{
				bool cash = recv.read_bool();
				if (cash)
					recv.skip(8);
				recv.read_long();       // expire
				recv.read_byte();       // slots
				recv.read_byte();       // level
				EnumMap<EquipStat::Id, uint16_t> stats;
				for (auto& stat : stats)
					stat.second = recv.read_short();
				recv.read_string();     // owner
				recv.read_short();      // flag
				if (cash)
					recv.skip(10);
				else
				{
					recv.read_byte();
					recv.read_byte();
					recv.read_short();
					recv.read_short();
					recv.read_int();
					recv.read_long();
				}
				recv.skip(12);
				si.quantity = 1;
			}
			else if (iid >= 5000000 && iid <= 5000102)
			{
				bool cash = recv.read_bool();
				if (cash)
					recv.skip(8);
				recv.read_long();
				recv.read_padded_string(13);
				recv.read_byte();
				recv.read_short();
				recv.read_byte();
				recv.skip(18);
				si.quantity = 1;
			}
			else
			{
				bool cash = recv.read_bool();
				if (cash)
					recv.skip(8);
				int64_t expire = recv.read_long();
				int16_t qty = recv.read_short();
				recv.read_string();     // owner
				recv.read_short();      // flag
				if ((iid / 10000 == 233) || (iid / 10000 == 207))
					recv.skip(8);
				si.quantity = qty;
			}

			out.push_back(si);
		}
	}
}
