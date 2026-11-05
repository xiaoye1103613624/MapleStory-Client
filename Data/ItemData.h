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

#include "../Graphics/Texture.h"
#include "../Template/BoolPair.h"
#include "../Template/Cache.h"

namespace ms
{
	// Class that represents an item loaded from the game's files
	// Contains all shared data between concrete items
	class ItemData : public Cache<ItemData>
	{
	public:
		// Returns whether the item was loaded correctly
		bool is_valid() const;
		// Returns whether the item is tradable or not
		bool is_untradable() const;
		// Returns whether the item is a one-of-a-kind item or not
		bool is_unique() const;
		// Returns whether the item is able to be sold or not
		bool is_unsellable() const;
		// Returns whether the item is a cash item or not
		bool is_cashitem() const;
		// Returns whether the item was loaded correctly
		explicit operator bool() const;

		// Returns the item id
		int32_t get_id() const;
		// Returns the item price
		int32_t get_price() const;
		// Returns the item's gender based on item id
		int8_t get_gender() const;
		// Returns the item's name loaded from the String file
		const std::string& get_name() const;
		// Returns the item's description loaded from the String file
		const std::string& get_desc() const;
		// Return the item category (Also the node name)
		const std::string& get_category() const;
		// Returns one of the item's icons
		// For each item there is a 'raw' icon and an icon with a drop shadow
		const Texture& get_icon(bool raw) const;
		// Wave 2a：召唤袋判定 —— 与 s9 ItemInformationProvider.getSummonMobs
		// 同规则（Item 数据 info 节点下存在 mob 子表 = 刷怪袋；s9 无前缀规则；
		// 2022 段的 useType==2022 判定属于"即时传送类"，不可混用）。
		// 构造期解析一次存位，避免每次使用路由现查 nx。
		bool is_summon_bag() const;
		// W3：info/script 节点存在 = 脚本道具（对账 s9 getScriptedItemInfo）
		bool is_scripted_item() const;

	private:
		// Allow the cache to use the constructor
		friend Cache<ItemData>;
		// Creates an item from the game's Item file with the specified id
		ItemData(int32_t itemid);

		std::string get_eqcategory(int32_t itemid) const;
		int32_t get_prefix(int32_t itemid) const;
		int32_t get_item_prefix(int32_t itemid) const;
		int8_t get_item_gender(int32_t itemid) const;

		BoolPair<Texture> icons;
		int32_t itemid;
		int32_t price;
		int8_t gender;
		std::string name;
		std::string desc;
		std::string category;

		bool valid;
		bool untradable;
		bool unique;
		bool unsellable;
		bool cashitem;
		// Wave 2a：Item info 节点存在 mob 子表 = 召唤袋（构造期解析一次）。
		bool summonbag;
		bool scripted = false;
	};
}