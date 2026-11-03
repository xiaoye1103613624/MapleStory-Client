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
		// 本包 UI.nx 根下没有 Storage.img（v83 真身是 UIWindow.img/Trunk 463x318，
		// 件表见 notes/asset-node-inventory.md §1.5）；换根需要 Trunk 内各钮的原生坐标，
		// 目前既无参考截图也无 IDA 证据 ⇒ 底衬与 BtArrange/BtMesoIn/BtMesoOut 显式预留。
		// 关闭钮与其它功能窗共用同一颗描边 X（UiAssetResolve::close_btn() → BtClose2）。
		nl::node storage = nl::nx::UI["Storage.img"];
		if (storage["backgrnd"])
			sprites.emplace_back(Sprite(storage["backgrnd"], Point<int16_t>(0, 0)));

		const Point<int16_t> btnpos[] = {
			Point<int16_t>(20, 320),   // BtArrange
			Point<int16_t>(120, 320),  // BtMesoIn
			Point<int16_t>(220, 320),  // BtMesoOut
			Point<int16_t>(300, 20)    // BtClose
		};
		const char* btnname[] = { "BtArrange", "BtMesoIn", "BtMesoOut" };
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
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					size_t c = s.find(',');
					if (c == std::string::npos) return;
					try {
						int type = std::stoi(s.substr(0, c));
						int16_t slot = static_cast<int16_t>(std::stoi(s.substr(c + 1)));
						if (type >= 1 && type <= 5 && slot >= 0) {
							selected_slot = slot;
							StorageTakePacket(static_cast<InventoryType::Id>(type), slot).dispatch();
							LOG(LOG_DEBUG, "UIStorage: SoftKey Take type=" << type << " slot=" << slot);
						}
					} catch (...) {}
				}, []() {}, "type,slot");
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
				int16_t slot = inv_slot;
				int32_t id = itemid;
				UI::get().emplace<UISoftKey>([slot, id](const std::string& s) {
					int16_t qty = 1;
					if (!s.empty()) {
						try { qty = static_cast<int16_t>(std::stoi(s)); } catch (...) { return; }
					}
					if (qty < 1) return;
					StorageStorePacket(slot, id, qty).dispatch();
					LOG(LOG_DEBUG, "UIStorage: Store from inv slot=" << slot << " id=" << id << " qty=" << qty);
				}, []() {}, "Store qty (selected)");
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int vals[3] = { 0, 0, 1 };
					size_t start = 0; int vi = 0;
					while (vi < 3 && start <= s.size()) {
						size_t c = s.find(',', start);
						std::string part = (c == std::string::npos) ? s.substr(start) : s.substr(start, c - start);
						try { vals[vi] = std::stoi(part); } catch (...) {}
						++vi;
						if (c == std::string::npos) break;
						start = c + 1;
					}
					if (vals[0] > 0 && vals[1] > 0) {
						StorageStorePacket(
							static_cast<int16_t>(vals[0]), vals[1],
							static_cast<int16_t>(vals[2])
						).dispatch();
						LOG(LOG_DEBUG, "UIStorage: SoftKey Store " << s);
					}
				}, []() {}, "slot,itemid,qty");
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
