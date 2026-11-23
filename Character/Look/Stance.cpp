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
#include "Stance.h"

#include "../../Util/Misc.h"

#include <iostream>
#include <unordered_map>

namespace ms
{
	Stance::Id Stance::by_state(int8_t state)
	{
		int8_t index = (state / 2) - 1;

		if (index < 0 || index > 10)
			return WALK1;

		constexpr Id statevalues[10] =
		{
			WALK1,
			STAND1,
			JUMP,
			ALERT,
			PRONE,
			FLY,
			LADDER,
			ROPE,
			DEAD,
			SIT
		};

		return statevalues[index];
	}

	Stance::Id Stance::by_id(uint8_t id)
	{
		if (id <= NONE || id >= LENGTH)
			return NONE;

		return static_cast<Stance::Id>(id);
	}

	Stance::Id Stance::by_string(const std::string& name)
	{
		for (auto iter : names)
			if (iter.second == name)
				return iter.first;

		single_console::log_message("[Stance::by_string] Unknown Id name: [" + name + "]");

		return NONE;
	}

	bool Stance::is_climbing(Id value)
	{
		return value == LADDER || value == ROPE;
	}

	Stance::Id Stance::baseof(Id value)
	{
		switch (value)
		{
			case STAND2:
				return STAND1;
			case WALK2:
				return WALK1;
			default:
				return value;
		}
	}

	Stance::Id Stance::secondof(Id value)
	{
		switch (value)
		{
			case STAND1:
				return STAND2;
			case WALK1:
				return WALK2;
			default:
				return value;
		}
	}

	const EnumMap<Stance::Id, std::string> Stance::names =
	{
		"",
		"alert",
		"dead",
		"fly",
		"heal",
		"jump",
		"ladder",
		"prone",
		"proneStab",
		"rope",
		"shot",
		"shoot1",
		"shoot2",
		"shootF",
		"sit",
		"stabO1",
		"stabO2",
		"stabOF",
		"stabT1",
		"stabT2",
		"stabTF",
		"stand1",
		"stand2",
		"swingO1",
		"swingO2",
		"swingO3",
		"swingOF",
		"swingP1",
		"swingP2",
		"swingPF",
		"swingT1",
		"swingT2",
		"swingT3",
		"swingTF",
		"walk1",
		"walk2"
	};

	uint8_t Stance::body_action(Id value)
	{
		// 顺序对齐角色 Body.img / 官方 CActionMan 动作表（MinMS Stance.cpp:127）。
		switch (value)
		{
		case WALK1:     return 0;
		case WALK2:     return 1;
		case STAND1:    return 2;
		case STAND2:    return 3;
		case ALERT:     return 4;
		case SWINGO1:   return 5;
		case SWINGO2:   return 6;
		case SWINGO3:   return 7;
		case SWINGOF:   return 8;
		case SWINGT1:   return 9;
		case SWINGT2:   return 10;
		case SWINGT3:   return 11;
		case SWINGTF:   return 12;
		case SWINGP1:   return 13;
		case SWINGP2:   return 14;
		case SWINGPF:   return 15;
		case STABO1:    return 16;
		case STABO2:    return 17;
		case STABOF:    return 18;
		case STABT1:    return 19;
		case STABT2:    return 20;
		case STABTF:    return 21;
		case SHOOT1:    return 22;
		case SHOOT2:    return 23;
		case SHOOTF:    return 24;
		case PRONESTAB: return 25;
		case PRONE:     return 26;
		case HEAL:      return 27;
		case FLY:       return 28;
		case JUMP:      return 29;
		case SIT:       return 30;
		case LADDER:    return 31;
		case ROPE:      return 32;
		case DEAD:      return 33;
		case SHOT:      return 22; // 手枪复用 shoot1 的 wire 动作号
		default:        return 2;  // stand1
		}
	}

	uint8_t Stance::body_action_by_name(const std::string& name)
	{
		if (name.empty())
			return 0;
		// PC 组合动作的 wire id 与 NX 子节点顺序不同（MinMS/Angel 实证）。
		static const std::unordered_map<std::string, uint8_t> pc_overrides = {
			{ "alert2", 41 },
			{ "avenger", 56 },
			{ "assaulter", 36 },
			{ "assassination", 37 },
		};
		auto it = pc_overrides.find(name);
		if (it != pc_overrides.end())
			return it->second;
		for (const auto& kv : names)
			if (kv.second == name)
				return body_action(kv.first);
		return 0;
	}
}