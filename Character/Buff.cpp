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
#include "Buff.h"

namespace ms
{
	namespace Buffstat
	{
		const std::unordered_map<Id, uint64_t> first_codes =
		{
			{ Id::DASH2, 0x8000000000000L },
			{ Id::DASH, 0x10000000000000L },
			{ Id::ELEMENTAL_RESET, 0x200000000L },
			{ Id::MAGIC_SHIELD, 0x400000000L },
			{ Id::MAGIC_RESISTANCE, 0x800000000L },
			{ Id::ARAN_COMBO, 0x1000000000L },
			{ Id::COMBO_DRAIN, 0x2000000000L },
			{ Id::COMBO_BARRIER, 0x4000000000L },
			{ Id::BODY_PRESSURE, 0x8000000000L },
			{ Id::SMART_KNOCKBACK, 0x10000000000L },
			{ Id::PYRAMID_PQ, 0x20000000000L },
			{ Id::ENERGY_CHARGE, 0x4000000000000L },
			{ Id::MONSTER_RIDING, 0x20000000000000L },
			{ Id::SPEED_INFUSION, 0x40000000000000L },
			{ Id::HOMING_BEACON, 0x80000000000000L }
		};

		const std::unordered_map<Id, uint64_t> second_codes =
		{
			{ Id::NONE, 0x0 },
			{ Id::MORPH, 0x2 },
			{ Id::RECOVERY, 0x4 },
			{ Id::MAPLE_WARRIOR, 0x8 },
			{ Id::STANCE, 0x10 },
			{ Id::SHARP_EYES, 0x20 },
			{ Id::MANA_REFLECTION, 0x40 },
			{ Id::SHADOW_CLAW, 0x100 },
			{ Id::INFINITY_, 0x200 },
			{ Id::HOLY_SHIELD, 0x400 },
			{ Id::HAMSTRING, 0x800 },
			{ Id::BLIND, 0x1000 },
			{ Id::CONCENTRATE, 0x2000 },
			{ Id::PUPPET, 0x4000 },
			{ Id::ECHO_OF_HERO, 0x8000 },
			{ Id::GHOST_MORPH, 0x20000 },
			{ Id::AURA, 0x40000 },
			{ Id::CONFUSE, 0x80000 },
			{ Id::BERSERK_FURY, 0x8000000 },
			{ Id::DIVINE_BODY, 0x10000000 },
			{ Id::SPARK, 0x20000000L },
			{ Id::FINALATTACK, 0x80000000L },
			{ Id::WATK, 0x100000000L },
			{ Id::WDEF, 0x200000000L },
			{ Id::MATK, 0x400000000L },
			{ Id::MDEF, 0x800000000L },
			{ Id::ACC, 0x1000000000L },
			{ Id::AVOID, 0x2000000000L },
			{ Id::HANDS, 0x4000000000L },
			{ Id::SPEED, 0x8000000000L },
			{ Id::JUMP, 0x10000000000L },
			{ Id::MAGIC_GUARD, 0x20000000000L },
			{ Id::DARKSIGHT, 0x40000000000L },
			{ Id::BOOSTER, 0x80000000000L },
			{ Id::POWERGUARD, 0x100000000000L },
			{ Id::HYPERBODYHP, 0x200000000000L },
			{ Id::HYPERBODYMP, 0x400000000000L },
			{ Id::INVINCIBLE, 0x800000000000L },
			{ Id::SOULARROW, 0x1000000000000L },
			{ Id::STUN, 0x2000000000000L },
			{ Id::POISON, 0x4000000000000L },
			{ Id::SEAL, 0x8000000000000L },
			{ Id::DARKNESS, 0x10000000000000L },
			{ Id::COMBO, 0x20000000000000L },
			{ Id::WK_CHARGE, 0x40000000000000L },
			{ Id::DRAGONBLOOD, 0x80000000000000L },
			{ Id::HOLY_SYMBOL, 0x100000000000000L },
			{ Id::MESOUP, 0x200000000000000L },
			{ Id::SHADOWPARTNER, 0x400000000000000L },
			{ Id::PICKPOCKET, 0x800000000000000L },
			{ Id::MESOGUARD, 0x1000000000000000L },
			{ Id::WEAKEN, 0x4000000000000000L }
		};

		// 表与 s9 client/BuffStat.java 逐位对齐后修的四处置位：
		//  1) PUPPET 服务端是 0x4000，客户端原挂在 0x800000000000000（那是 PICKPOCKET 位），
		//     且与 PICKPOCKET 同值——同一位匹配到两个枚举就会把 10 字节数据块读两遍，
		//     之后所有 buff 全体错位。同理删掉与 HANDS 同值的 SHOWDASH、
		//     与 COMBO 同值的 SUMMON（服务端 COMBO=SUMMON=0x20000000000000 本就撞位）。
		//  2) BATTLESHIP 的 0xA00000040 是「多位的合成值」，服务端从不这样写；
		//     留在表里会让任何带 MANA_REFLECTION/WDEF/MDEF 的包被误判成额外一块。
		//  3) SPEED_INFUSION 服务端为 0x40000000000000，客户端多左移了两位。
		//  4) 补上服务端存在而客户端缺位的 MAGIC_SHIELD / MAGIC_RESISTANCE。
		// 缺位比错位更隐蔽：服务端照样写数据块，客户端不认就会少读 10 字节，
		// 因此解析改成「按掩码位消费」，表只负责把位翻译成枚举（见 BuffHandler::handle）。
		Id by_code(uint64_t code, bool first)
		{
			const auto& codes = first ? first_codes : second_codes;

			for (const auto& iter : codes)
				if (iter.second == code)
					return iter.first;

			return Id::NONE;
		}
	}
}