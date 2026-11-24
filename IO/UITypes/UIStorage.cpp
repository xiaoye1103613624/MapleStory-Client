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
#include "UIDelivery.h"
#include "UISoftKey.h"
#include "UINotice.h"
#include "UIItemInventory.h"
#include "UIStoreBank.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/StoragePackets.h"
#include "../../Util/Locale.h"
#include "../../Util/Misc.h"
#include "../../Util/UiAssetResolve.h"

namespace ms
{
	UIStorage::UIStorage() : UIElement(Point<int16_t>(200, 20), Point<int16_t>(348, 360)),
		capacity(0), meso(0),
		mesolabel(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, Color::Name::BLACK)
	{
		// 四源对照 A3 修复：v83 仓库真身是 UIWindow.img/Trunk（orange-wz 实测
		// backgrnd 463×318），本包根下没有 Storage.img。按钮坐标从 Angel v83
		// CTrunkDlg::OnCreate@0x7c618e 逐字取：BtSort(154,55)/BtInCoin(236,294)/
		// BtOutCoin(5,294)。关闭钮仍共用描边 X（原生走系统 X，非 Trunk/BtExit）。
		nl::node storage = nl::nx::UI["UIWindow.img"]["Trunk"];
		if (storage["backgrnd"])
			sprites.emplace_back(Sprite(storage["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(154, 55),   // BtSort  (整理)
			Point<int16_t>(236, 294),  // BtInCoin(存钱)
			Point<int16_t>(5, 294),    // BtOutCoin(取钱)
			Point<int16_t>(300, 20)    // BtClose
		};
		const char* btnname[] = { "BtSort", "BtInCoin", "BtOutCoin" };
		for (uint16_t i = 0; i < 3; i++)
			try_maple_btn(buttons, i, storage[btnname[i]], btnpos[i], "UIStorage");

		try_maple_btn(buttons, BtClose, UiAssetResolve::close_btn(), btnpos[3], "UIStorage close");

		// mesolabel 有三个写入点（ctor 初值 + 两条包回调），全都带数据 => 走活文案，
		// 不进绑定表；三处用同一个键，切语言时靠下一次包更新或 ctor 重取自然换新。
		mesolabel.change_text(Locale::format("storage.meso", "0"));
	}

	void UIStorage::draw(float alpha) const
	{
		UIElement::draw_sprites(alpha);

		for (const auto& si : stored)
		{
			Point<int16_t> pos = position + slot_pos(si.slot);
			const Texture& icon = ItemData::get(si.itemid).get_icon(si.itemid / 10000 == 238);

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
		selected_slot = slot;
		for (const auto& si : stored)
		{
			if (si.slot == slot)
			{
				StorageTakePacket(si.type, slot).dispatch();
				break;
			}
		}
	}

	Cursor::State UIStorage::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active)
			return st;
		int16_t slot = slot_by_position(cursorpos - position);
		if (slot < 0)
			return st;
		for (const auto& si : stored)
		{
			if (si.slot == slot)
			{
				selected_slot = slot;
				LOG(LOG_DEBUG, "UIStorage: select slot=" << slot << " type=" << static_cast<int>(si.type));
				break;
			}
		}
		return st;
	}

	const StoredItem* UIStorage::selected_item() const
	{
		for (const auto& si : stored)
			if (si.slot == selected_slot)
				return &si;
		return nullptr;
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
		mesolabel.change_text(Locale::format("storage.meso", std::to_string(meso)));
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
		mesolabel.change_text(Locale::format("storage.meso", std::to_string(meso)));
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

	void UIStorage::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			StorageClosePacket().dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'X' || keycode == 'x') {
			StorageClosePacket().dispatch();
			deactivate();
			return;
		}
		if (keycode == 'A' || keycode == 'a')
			StorageArrangePacket().dispatch();
		else if (keycode == 'I' || keycode == 'i')
			prompt_meso(true);
		else if (keycode == 'O' || keycode == 'o')
			prompt_meso(false);
		else if (keycode == 'T' || keycode == 't') {
			if (const StoredItem* si = selected_item()) {
				StorageTakePacket(si->type, si->slot).dispatch();
				LOG(LOG_DEBUG, "UIStorage: Take selected slot=" << si->slot);
			} else {
				
			}
		} else if (keycode == 'P' || keycode == 'p') {
			int16_t inv_slot = 0;
			InventoryType::Id inv_tab = InventoryType::Id::NONE;
			int32_t itemid = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				inv_slot = inv->get_selected_slot();
				inv_tab = inv->get_tab();
				if (inv_slot > 0)
					itemid = Stage::get().get_player().get_inventory().get_item_id(inv_tab, inv_slot);
			}
			if (inv_slot > 0 && itemid > 0) {
				int16_t quantity = Stage::get().get_player().get_inventory().get_item_count(inv_tab, inv_slot);
				if (quantity <= 0)
					quantity = 1;
				StorageStorePacket(inv_slot, itemid, quantity).dispatch();
				LOG(LOG_DEBUG, "UIStorage: store slot=" << inv_slot << " id=" << itemid);
			}
		} else if (keycode == 'D' || keycode == 'd') {
			auto ui = UI::get().get_element<UIDelivery>();
			if (!ui) ui = UI::get().emplace<UIDelivery>();
			else if (!ui->is_active()) ui->makeactive();
		} else if (keycode == 'F' || keycode == 'f') {
			auto ui = UI::get().get_element<UIStoreBank>();
			if (!ui) ui = UI::get().emplace<UIStoreBank>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	void UIStorage::prompt_meso(bool deposit)
	{
		// s9 StorageProcessor case 7: writeLong(meso)，正数=取出、负数=存入。
		// 输入上限取当前持有金币/仓库金币的较大者，避免本地输入超过持有量。
		int64_t limit = deposit
			? Stage::get().get_player().get_inventory().get_meso()
			: meso;
		if (limit <= 0)
			limit = 1;
		UI::get().emplace<UIEnterNumber>(
			Locale::text(deposit ? "storage.deposit_qty" : "storage.withdraw_qty"),
			[deposit](int32_t n) {
				if (n <= 0)
					return;
				StorageMesoPacket(deposit ? -static_cast<int64_t>(n) : static_cast<int64_t>(n)).dispatch();
				LOG(LOG_DEBUG, "UIStorage: meso " << (deposit ? "deposit " : "withdraw ") << n);
			},
			limit > 2147483647LL ? 2147483647 : static_cast<int32_t>(limit),
			1
		);
	}
}
