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
#include "AttackHandlers.h"

#include "../../Character/SkillId.h"
#include "../../Gameplay/Stage.h"
#include "../../Configuration.h"
#include "../../Gameplay/Combat/Damage.h"

#include <limits>

namespace ms
{
	AttackHandler::AttackHandler(Attack::Type t)
	{
		type = t;
	}

	void AttackHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		uint8_t count = recv.read_byte();

		recv.skip(1);

		AttackResult attack;
		attack.type = type;
		attack.attacker = cid;

		attack.level = recv.read_byte();
		attack.skill = (attack.level > 0) ? recv.read_int() : 0;

		attack.display = recv.read_byte();
		attack.toleft = recv.read_bool();
		attack.stance = recv.read_byte();
		attack.speed = recv.read_byte();

		recv.skip(1);

		attack.bullet = recv.read_int();

		attack.mobcount = (count >> 4) & 0xF;
		attack.hitcount = count & 0xF;

		for (uint8_t i = 0; i < attack.mobcount; i++)
		{
			int32_t oid = recv.read_int();

			recv.skip(1);

			uint8_t length = (attack.skill == SkillId::Id::MESO_EXPLOSION) ? recv.read_byte() : attack.hitcount;

			// 与 AttackPacket 的 damage.long_lines 成对：12 字节/行 = int32 vanillaLine
			// + int64 真实值。legacy 仍是 4 字节/行，暴击用负数编码。
			const bool long_lines = Configuration::get().get_damage_long_lines();

			for (uint8_t j = 0; j < length; j++)
			{
				int32_t vanilla = recv.read_int();
				int64_t damage;

				if (long_lines)
				{
					int64_t real = recv.read_long();
					bool critical = damage::vanilla_is_critical(vanilla);
					// 真实值字段必须非负；异常包（负/超 2^53）回退到 vanilla 的绝对值，
					// 避免把垃圾值渲染成天文数字。
					// 与 legacy 一致：暴击位的绝对值取低 31 位，不能用 -vanilla
					// （0x80000000|d 取负会变成 2147483648-d）。
					int32_t fallback = static_cast<int32_t>(
						static_cast<uint32_t>(vanilla) & 0x7FFFFFFFu);
					damage = (real > 0 && real <= damage::EXACT_INT_CAP)
						? real
						: static_cast<int64_t>(fallback);
					attack.damagelines[oid].emplace_back(damage, critical);
				}
				else
				{
					// V83+：最高位标记暴击；绝对值写入飘字
					bool critical = (vanilla < 0);
					int32_t value = vanilla;

					if (critical)
						value &= 0x7FFFFFFF;

					attack.damagelines[oid].emplace_back(static_cast<int64_t>(value), critical);
				}
			}
		}
		while (recv.available())
			recv.skip_byte();

		Stage::get().get_combat().push_attack(attack);
	}
}