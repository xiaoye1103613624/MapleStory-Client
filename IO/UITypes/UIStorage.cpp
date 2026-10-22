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
#include "UIStorage.h"

#include <nlnx/nx.hpp>
#include <string>

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UISoftKey.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/StoragePackets.h"

namespace ms
{
	UIStorage::UIStorage() : UIElement(Point<int16_t>(200, 20), Point<int16_t>(348, 360)),
		capacity(0), meso(0),
		mesolabel(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// 背景（参考端 Login.img/Storage.img；缺失则用底板）
		nl::node storage = nl::nx::UI["Storage.img"];
		if (storage["backgrnd"])
			sprites.emplace_back(Sprite(storage["backgrnd"], Point<int16_t>(0, 0)));

		// 按钮：节点存在则用其坐标，否则退化为计算坐标（仍可点击）
		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 320),   // BtArrange
			Point<int16_t>(120, 320),  // BtMesoIn
			Point<int16_t>(220, 320),  // BtMesoOut
			Point<int16_t>(300, 20)    // BtClose
		};
		const char* btnname[] = { "BtArrange", "BtMesoIn", "BtMesoOut", "BtClose" };
		for (uint16_t i = 0; i < 4; i++)
		{
			nl::node bn = storage[btnname[i]];
			buttons[i] = bn
				? std::make_unique<MapleButton>(bn)
				: std::make_unique<MapleButton>(bn, btnpos[i]);
		}

		mesolabel.change_text("金币: 0");
	}

	void UIStorage::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		for (const auto& si : stored)
		{
			Point<int16_t> pos = position + slot_pos(si.slot);
			const Texture& icon = ItemData::get(si.itemid).get_icon(false);

			if (icon)
				icon.draw(pos);

			if (si.quantity > 1)
			{
				OutlinedText qty(Text::Font::A12M, Text::Alignment::RIGHT,
					Color::Name::WHITE, Color::Name::BLACK);
				qty.change_text(std::to_string(si.quantity));
				qty.draw(pos + Point<int16_t>(CELL - 6, CELL - 12));
			}
		}

		mesolabel.draw(position + Point<int16_t>(16, 300));

		UIElement::draw_buttons(alpha);
	}

	void UIStorage::update()
	{
	}

	void UIStorage::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t slot = slot_by_position(cursorpos - position);
		for (const auto& si : stored)
		{
			if (si.slot == slot)
			{
				StorageTakePacket(si.type, slot).dispatch();
				break;
			}
		}
	}

	UIElement::Type UIStorage::get_type() const
	{
		return TYPE;
	}

	Button::State UIStorage::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case BtArrange:
				StorageArrangePacket().dispatch();
				break;
			case BtMesoIn:
				prompt_meso(true);
				break;
			case BtMesoOut:
				prompt_meso(false);
				break;
			case BtClose:
				StorageClosePacket().dispatch();
				deactivate();
				break;
		}
		return Button::State::NORMAL;
	}

	void UIStorage::set_storage(uint8_t slots, int64_t m, std::vector<StoredItem> items)
	{
		capacity = slots;
		meso = m;
		stored = std::move(items);
		mesolabel.change_text("金币: " + std::to_string(meso));
	}

	void UIStorage::update_items(uint8_t slots, std::vector<StoredItem> items)
	{
		capacity = slots;
		stored = std::move(items);
	}

	void UIStorage::set_meso(uint8_t slots, int64_t m)
	{
		capacity = slots;
		meso = m;
		mesolabel.change_text("金币: " + std::to_string(meso));
	}

	void UIStorage::show_error(uint8_t code)
	{
		// s9 仓库错误码：0x0A 满 / 0x0B 已满 / 0x0C 未开启 / 0x11 交易中 / 0x10 其它
		LOG(LOG_WARNING, "Storage error code: 0x" << std::hex << static_cast<int>(code));
	}

	Point<int16_t> UIStorage::slot_pos(int16_t slot) const
	{
		int16_t col = slot % COLS;
		int16_t row = slot / COLS;
		return Point<int16_t>(12 + col * CELL, 40 + row * CELL);
	}

	int16_t UIStorage::slot_by_position(Point<int16_t> pos) const
	{
		int16_t col = (pos.x() - 12) / CELL;
		int16_t row = (pos.y() - 40) / CELL;
		if (col < 0 || col >= COLS || row < 0)
			return -1;
		return row * COLS + col;
	}

	void UIStorage::prompt_meso(bool deposit)
	{
		bool d = deposit;
		UI::get().emplace<UISoftKey>([d](const std::string& s)
		{
			try
			{
				int32_t amt = std::stoi(s);
				if (amt > 0)
					StorageMesoPacket(d ? -amt : amt).dispatch();
			}
			catch (...)
			{
			}
		});
	}
}
