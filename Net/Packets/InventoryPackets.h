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
#pragma once

#include "../OutPacket.h"

#include "../../Character/Inventory/Inventory.h"
#include "../../Data/ItemData.h"

namespace ms
{
	// Packet which requests that the inventory is sorted
	// Opcode: GATHER_ITEMS(69)
	class GatherItemsPacket : public OutPacket
	{
	public:
		GatherItemsPacket(InventoryType::Id type) : OutPacket(OutPacket::Opcode::GATHER_ITEMS)
		{
			write_time();
			write_byte(type);
		}
	};

	// Packet which requests that the inventory is sorted
	// Opcode: SORT_ITEMS(70)
	class SortItemsPacket : public OutPacket
	{
	public:
		SortItemsPacket(InventoryType::Id type) : OutPacket(OutPacket::Opcode::SORT_ITEMS)
		{
			write_time();
			write_byte(type);
		}
	};

	// Packet which requests that an item is moved
	// Opcode: MOVE_ITEM(71)
	class MoveItemPacket : public OutPacket
	{
	public:
		MoveItemPacket(InventoryType::Id type, int16_t slot, int16_t action, int16_t qty) : OutPacket(OutPacket::Opcode::MOVE_ITEM)
		{
			write_time();
			write_byte(type);
			write_short(slot);
			write_short(action);
			write_short(qty);
		}
	};

	// Packet which requests that an item is equipped
	// Opcode: MOVE_ITEM(71)
	class EquipItemPacket : public MoveItemPacket
	{
	public:
		EquipItemPacket(int16_t src, EquipSlot::Id dest) : MoveItemPacket(InventoryType::Id::EQUIP, src, -dest, 1) {}
	};

	// Packet which requests that an item is unequipped
	// Opcode: MOVE_ITEM(71)
	class UnequipItemPacket : public MoveItemPacket
	{
	public:
		UnequipItemPacket(int16_t src, int16_t dest) : MoveItemPacket(InventoryType::Id::EQUIPPED, -src, dest, 1) {}
	};

	// A packet which requests that an 'USE' item is used
	// Opcode: USE_ITEM(72)；技能书特例：USE_SKILL_BOOK(82 / 0x52)；
	// 召唤袋特例：USE_SUMMON_BAG(75 / 0x4B)
	//
	// Wave 3 收敛点：s9 对 243xxxx（技能书）有专用 handler
	// （PacketProcessor:244 registerHandler(RecvOpcode.USE_SKILL_BOOK,
	// SkillBookHandler)），走 getSkillStats→按职业 teach 表加技能等级；
	// 通用 UseItemHandler 不查 teach 表 —— 书发 72 会被当普通消耗品结算，
	// 技能学不进去。判定放这一处（itemid/10000==243，与 s9
	// ItemInformationProvider:1596 的门同规则），背包双击 / 快捷格 /
	// 聊天栏命令三个 UseItemPacket 调用点自动同语义，不在各 UI 里散判。
	//
	// Wave 2a：召唤袋（info 下有 mob 子表，ItemData::is_summon_bag）必须走
	// USE_SUMMON_BAG(75)。通用 UseItemHandler 不读 mob 表（s9 无 2022 段
	// case，落到脚本物品 useType 路径），发 72 的袋子不会刷怪。s9
	// UseSummonBagHandler body 与 USE_ITEM 逐字段同形（int ts / short slot /
	// int itemid），故同一包类三分支，仅 opcode 不同。
	//
	// 路由**下沉在本 ctor**（不在 7 个调用点散传标志位）：背包双击/快捷格/
	// 聊天命令/壳窗全部自动同语义，新增调用点零成本继承。ItemData 是缓存
	// （构造期解析 mob 位），未加载物品 is_summon_bag()=false 走默认分支，
	// 无副作用。
	class UseItemPacket : public OutPacket
	{
	public:
		static constexpr bool is_skill_book(int32_t itemid)
		{
			return itemid / 10000 == 243;
		}

		UseItemPacket(int16_t slot, int32_t itemid)
			: OutPacket(
				ItemData::get(itemid).is_summon_bag() ? Opcode::USE_SUMMON_BAG
				: (is_skill_book(itemid) ? Opcode::USE_SKILL_BOOK : Opcode::USE_ITEM))
		{
			Sound(itemid).play();

			write_time();
			write_short(slot);
			write_int(itemid);
		}
	};

	// Requests using a scroll on an equip 
	// Opcode: SCROLL_EQUIP(86)
	class ScrollEquipPacket : public OutPacket
	{
	public:
		enum Flag : uint8_t
		{
			NONE = 0x00,
			UNKNOWN = 0x01,
			WHITESCROLL = 0x02
		};

		ScrollEquipPacket(int16_t source, EquipSlot::Id target, uint8_t flags) : OutPacket(OutPacket::Opcode::SCROLL_EQUIP)
		{
			write_time();
			write_short(source);
			write_short(-target);
			write_short(flags);
		}

		ScrollEquipPacket(int16_t source, EquipSlot::Id target) : ScrollEquipPacket(source, target, 0) {}
	};
}