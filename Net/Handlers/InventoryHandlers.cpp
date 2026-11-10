//////////////////////////////////////////////////////////////////////////////////
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
#include "InventoryHandlers.h"

#include "Helpers/ItemParser.h"

#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"

#include "../../IO/UITypes/UIEquipInventory.h"
#include "../../IO/UITypes/UIItemInventory.h"
#include "../../IO/UITypes/UISkillBook.h"
#include "../../IO/UITypes/UIKeyConfig.h"
#include "../../IO/UITypes/UIShop.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

namespace ms
{
	namespace {
		void inv_tip(const std::string& tip, Color::Name color = Color::Name::YELLOW)
		{
			// chat 优先；无 ChatBar 才 SM；不强制 FloatNotice
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		}
	}

	void GatherResultHandler::handle(InPacket& recv) const
	{
		// Angel 083 CWvsContext::OnGatherItemResult @ 0xa1e943：
		// Decode1(); ti=Decode1(); CUIItem::SetArrangeButton(ui, ti, 1) → 显示 BtSort
		// （旧注释曾引另一个地址，在 083 IDB 里是 ZAllocEx 构造器，非本函数，已剔除。）
		recv.read_byte();
		InventoryType::Id type = InventoryType::by_value(recv.read_byte());

		if (auto iteminventory = UI::get().get_element<UIItemInventory>())
		{
			if (!iteminventory->is_active())
				iteminventory->makeactive();
			iteminventory->set_arrange_button(type, true);
		}
	}

	void SortResultHandler::handle(InPacket& recv) const
	{
		// Angel 083 CWvsContext::OnSortItemResult @ 0xa1e96d：
		// Decode1(); ti=Decode1(); CUIItem::SetArrangeButton(ui, ti, 0) → 显示 BtGather
		// （旧注释曾引另一个地址，083 IDB 里同为 ZAllocEx 构造器，已剔除。）
		recv.read_byte();
		InventoryType::Id type = InventoryType::by_value(recv.read_byte());

		if (auto iteminventory = UI::get().get_element<UIItemInventory>())
		{
			if (!iteminventory->is_active())
				iteminventory->makeactive();
			iteminventory->set_arrange_button(type, false);
			iteminventory->clear_new();
		}
	}

	void ModifyInventoryHandler::handle(InPacket& recv) const
	{
		recv.read_bool(); // 'updatetick'

		Inventory& inventory = Stage::get()
			.get_player()
			.get_inventory();

		struct Mod
		{
			int8_t mode;
			InventoryType::Id type;
			int16_t pos;
			int16_t arg;
		};

		std::vector<Mod> mods;

		int8_t size = recv.read_byte();

		for (int8_t i = 0; i < size; i++)
		{
			Mod mod;
			mod.mode = recv.read_byte();
			mod.type = InventoryType::by_value(recv.read_byte());
			mod.pos = recv.read_short();

			mod.arg = 0;

			switch (mod.mode)
			{
			case Inventory::Modification::ADD:
				ItemParser::parse_item(recv, mod.type, mod.pos, inventory);

				if (auto keyconfig = UI::get().get_element<UIKeyConfig>())
				{
					int16_t count_now = inventory.get_item_count(mod.type, mod.pos);
					keyconfig->update_item_count(mod.type, mod.pos, count_now);
				}

				break;
			case Inventory::Modification::CHANGECOUNT:
			{
				mod.arg = recv.read_short();

				int16_t count_before = inventory.get_item_count(mod.type, mod.pos);
				int16_t count_now = mod.arg;

				inventory.modify(mod.type, mod.pos, mod.mode, mod.arg, Inventory::Movement::MOVE_NONE);

				if (auto keyconfig = UI::get().get_element<UIKeyConfig>())
					keyconfig->update_item_count(mod.type, mod.pos, count_now - count_before);

				if (count_before < count_now)
					mod.mode = Inventory::Modification::ADDCOUNT;
			}
			break;
			case Inventory::Modification::SWAP:
				mod.arg = recv.read_short();
				break;
			case Inventory::Modification::REMOVE:
				if (auto keyconfig = UI::get().get_element<UIKeyConfig>())
				{
					int16_t count_before = inventory.get_item_count(mod.type, mod.pos);
					keyconfig->update_item_count(mod.type, mod.pos, -1 * count_before);
				}

				inventory.modify(mod.type, mod.pos, mod.mode, mod.arg, Inventory::Movement::MOVE_INTERNAL);
				break;
			}

			mods.push_back(mod);
		}

		Inventory::Movement move = (recv.available() > 0) ?
			Inventory::movementbyvalue(recv.read_byte()) :
			Inventory::Movement::MOVE_INTERNAL;

		for (const Mod& mod : mods)
		{
			if (mod.mode == 2)
				inventory.modify(mod.type, mod.pos, mod.mode, mod.arg, move);

			if (auto shop = UI::get().get_element<UIShop>())
				shop->modify(mod.type);

			auto eqinvent = UI::get().get_element<UIEquipInventory>();
			auto itinvent = UI::get().get_element<UIItemInventory>();

			switch (move)
			{
			case Inventory::Movement::MOVE_INTERNAL:
				switch (mod.type)
				{
				case InventoryType::Id::EQUIPPED:
					if (eqinvent)
						eqinvent->modify(mod.pos, mod.mode, mod.arg);

					Stage::get().get_player().change_equip(-mod.pos);
					Stage::get().get_player().change_equip(-mod.arg);
					break;
				case InventoryType::Id::EQUIP:
				case InventoryType::Id::USE:
				case InventoryType::Id::SETUP:
				case InventoryType::Id::ETC:
				case InventoryType::Id::CASH:
					if (itinvent)
						itinvent->modify(mod.type, mod.pos, mod.mode, mod.arg);

					break;
				}

				break;
			case Inventory::Movement::MOVE_EQUIP:
			case Inventory::Movement::MOVE_UNEQUIP:
				if (mod.pos < 0)
				{
					if (eqinvent)
						eqinvent->modify(-mod.pos, 3, 0);

					if (itinvent)
						itinvent->modify(InventoryType::Id::EQUIP, mod.arg, mod.mode, 0);

					Stage::get().get_player().change_equip(-mod.pos);
				}
				else if (mod.arg < 0)
				{
					if (eqinvent)
						eqinvent->modify(-mod.arg, 0, 0);

					if (itinvent)
						itinvent->modify(InventoryType::Id::EQUIP, mod.pos, Inventory::Modification::REMOVE, 0);

					Stage::get().get_player().change_equip(-mod.arg);
				}

				break;
			}
		}

		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().recalc_stats(true);
		UI::get().enable();
	}

	void InventoryGrowHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		int8_t limit = recv.available() ? recv.read_byte() : 0;
		InventoryType::Id inv = InventoryType::by_value(type);
		LOG(LOG_DEBUG, "InventoryGrow type=" << (int)type << " limit=" << (int)limit);
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().get_inventory().set_slotmax(inv, static_cast<uint8_t>(limit));
		auto invui = UI::get().get_element<UIItemInventory>();
		if (!invui)
			invui = UI::get().emplace<UIItemInventory>(Stage::get().get_player().get_inventory());
		else if (!invui->is_active())
			invui->makeactive();
		if (invui)
			return;
		if (auto eq = UI::get().get_element<UIEquipInventory>())
		{
			if (eq->is_active())
				return;
		}
		inv_tip("Inventory slots → " + std::to_string(limit));
	}

	void SkillLearnItemResultHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_byte();
		int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t maxlevel = recv.available() >= 4 ? recv.read_int() : 0;
		bool canuse = recv.available() ? recv.read_bool() : false;
		bool success = recv.available() ? recv.read_bool() : false;
		LOG(LOG_DEBUG, "SkillLearn cid=" << cid << " skill=" << skill
			<< " max=" << maxlevel << " can=" << canuse << " ok=" << success);
		while (recv.available())
			recv.skip_byte();
		if (canuse && success && skill > 0)
		{
			auto& player = Stage::get().get_player();
			int32_t cur = player.get_skills().get_level(skill);
			if (cur < 1) cur = 0;
			player.change_skill(skill, cur + 1, maxlevel > 0 ? maxlevel : (cur + 1), 0);
		}
		bool skillbook_open = false;
		auto sb = UI::get().get_element<UISkillBook>();
		if (!sb && canuse && success && skill > 0)
		{
			auto& player = Stage::get().get_player();
			sb = UI::get().emplace<UISkillBook>(player.get_stats(), player.get_skills());
		}
		else if (sb && !sb->is_active() && canuse && success && skill > 0)
			sb->makeactive();
		if (sb && sb->is_active())
		{
			skillbook_open = true;
			if (canuse && success && skill > 0)
				sb->update_skills(skill);
		}
		// SkillBook 已开：成功/失败自显，不 tip
		if (skillbook_open)
			return;
		if (!canuse)
			inv_tip("Cannot use skill book", Color::Name::RED);
		else if (!success)
			inv_tip("Skill book failed", Color::Name::RED);
		else if (skill > 0)
		{
			// SkillBook 未开：SkillUp 浮层（不叠 SM tip）
			std::string tip = "Skill " + std::to_string(skill) + " learned";
			auto ui = UI::get().get_element<UISkillUp>();
			if (!ui)
				ui = UI::get().emplace<UISkillUp>();
			else if (!ui->is_active())
				ui->makeactive();
			if (ui)
				ui->set_tip(tip);
			else
				inv_tip("Skill learned #" + std::to_string(skill));
		}
		(void)cid;
		(void)maxlevel;
	}
}