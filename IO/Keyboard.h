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

#include "KeyAction.h"
#include "KeyType.h"

#include <array>
#include <map>

namespace ms
{
	class Keyboard
	{
	public:
		struct Mapping
		{
			KeyType::Id type;
			int32_t action;

			Mapping() : type(KeyType::Id::NONE), action(0) {}
			Mapping(KeyType::Id in_type, int32_t in_action) : type(in_type), action(in_action) {}

			bool operator==(const Mapping& other) const
			{
				return type == other.type && action == other.action;
			}

			bool operator!=(const Mapping& other) const
			{
				return type != other.type || action != other.action;
			}
		};

		Keyboard();

		void assign(uint8_t key, uint8_t type, int32_t action);
		void remove(uint8_t key);

		int32_t leftshiftcode() const;
		int32_t rightshiftcode() const;
		int32_t capslockcode() const;
		int32_t leftctrlcode() const;
		int32_t rightctrlcode() const;
		std::map<int32_t, Mapping> get_maplekeys() const;
		KeyAction::Id get_ctrl_action(int32_t keycode) const;
		Mapping get_mapping(int32_t keycode) const;
		int32_t get_mapping_index(int32_t action) const;
		Mapping get_maple_mapping(int32_t keycode) const;
		Mapping get_text_mapping(int32_t keycode, bool shift) const;

		// ---- 快捷栏绑定（Wave 1-1.2）----
		// 这条数据面原本不存在：QUICKSLOT_INIT(0x9F) 下发被旧 handler
		// 错写进普通键位表（no-op 语义），改键也从不回发 CHANGE_QUICKSLOT。
		// 存储=8 个触发键（v83 GLFW 系编码，与 s9 quickslotkeymapped
		// long 打包的字节值同域——服务端 (byte)readInt 截断，键值 >0xFF
		// 会撞车，此表全部 <0x100 安全）。
		static constexpr size_t QUICKSLOT_COUNT = 8;

		// 静默写槽（QUICKSLOT_INIT 下行用；禁止在此上行 183，否则进图会回声）。
		void set_quickslot(uint8_t slot, uint32_t key);
		// 用户改键：写槽 + 上行 CHANGE_QUICKSLOT(183) 整表 8×int。
		void assign_quickslot(uint8_t slot, uint32_t key);
		void commit_quickslots();
		void clear_quickslots();
		// 换角色清残留：触发键若等于默认表，QUICKSLOT_INIT 走 bool=false
		// 分支不下发任何键值，不清会把上一角色的键泄漏给下一角色。
		uint32_t default_quickslot_key(size_t slot);
		uint32_t get_quickslot_key(uint8_t slot) const;
		const std::array<uint32_t, QUICKSLOT_COUNT>& quickslots() const { return quickslot_keys; }
		// GLFW → v83 maple 键码（KeyConfig::Key / Keytable 下标）；0 = 不可映射。
		uint8_t maple_keycode_from_glfw(int32_t glfw_key) const;

	private:
		std::map<int32_t, Mapping> keymap;
		std::map<int32_t, Mapping> maplekeys;
		std::map<int32_t, KeyAction::Id> textactions;
		std::map<int32_t, bool> keystate;
		std::array<uint32_t, QUICKSLOT_COUNT> quickslot_keys{};
	};
}