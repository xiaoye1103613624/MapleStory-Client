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

namespace ms
{
	// Notifies the server of an attack
	// The opcode is determined by the attack type
	// Attack::CLOSE = CLOSE_ATTACK(44)
	// Attack::RANGED = RANGED_ATTACK(45)
	// Attack::MAGIC = MAGIC_ATTACK(46)
	class AttackPacket : public OutPacket
	{
	public:
		// 出站版式全部以服务端 AbstractDealDamageHandler::parseDamage 为准
		// （s9 gms-server/.../handlers/AbstractDealDamageHandler.java:681-1047）。
		// 这个解析器是「定长顺序读」：前面每少读/多读一个字段，后面 display / direction /
		// stance / speed 和整段伤害行就会整体位移，服务端拿到的是垃圾 oid 与垃圾伤害。
		AttackPacket(const AttackResult& attack) : OutPacket(opcodefor(attack.type))
		{
			// parseDamage:686-691：1 字节占位 + numAttackedAndDamage + skill(int)。
			skip(1);

			// numAttackedAndDamage 是两个 4 位段（高=怪物数，低=每怪伤害段数），所以两位都只有
			// 0-15。低段必须掩码：本客户端 Skill.nx 里 attackCount 超过 15 的只有暴怒
			// 4211006（16/18/20），它走服务端专用分支、低段被忽略，但不掩码就会把
			// 20 的 bit4 灌进高段，服务端读到的怪物数凭空 +1。
			// 怪物数一侧由 Combat 钳到 15（见 apply_move 里的说明）。
			write_byte((attack.mobcount << 4) | (attack.hitcount & 0xF));
			write_int(attack.skill);

			// parseDamage:705-709：是否再读一个 int（charge）只取决于**技能 id 命中固定表**，
			// 不是「有没有充能值」。原来这里写的是 `if (attack.charge > 0)`，而全工程没有任何
			// 地方给 AttackResult::charge 赋过值，于是这 9 个技能一个字节都不发，
			// 服务端却照读 4 字节 → 后面全体前移 4 字节。
			if (has_charge_field(attack.skill))
				write_int(attack.charge);

			// parseDamage:711-714：skip(8) + display + direction + stance。
			skip(8);

			write_byte(attack.display);
			write_byte(attack.toleft);
			write_byte(attack.stance);

			// parseDamage:751-764。近战/魔法：byte + speed + skip(4) 共 6 字节；
			// 远程：byte + speed + byte + rangedirection + skip(7) 共 11 字节。
			skip(1);
			write_byte(attack.speed);

			if (attack.type == Attack::Type::RANGED)
			{
				skip(1);
				write_byte(attack.toleft);
				skip(7);

				// parseDamage:757-759：飓风/穿透箭/速射在服务端还会多 skip(4)，
				// 少写这 4 字节会让紧随其后的伤害行整体前移 4 字节。
				if (has_sustained_field(attack.skill))
					skip(4);
			}
			else
			{
				skip(4);
			}

			// 暴怒（4211006）走 parseDamage:715-750 的专用分支：每个怪物是
			// oid(4) + pad(12) + 段数 byte + 段数×伤害(4) + tail(4)，
			// 末尾还要再跟一整块「被引爆的金币堆」列表（普通攻击没有这块）。
			// 金币堆的 drop oid 由服务端持有，客户端只按掉落物外观渲染，
			// 拿不到「哪几堆金币属于我且还在范围内」，所以这里固定上报 0 堆：
			// 技能会照常结算对怪物的伤害，但不会引爆地上的金币。
			if (attack.skill == 4211006)
			{
				for (auto& damagetomob : attack.damagelines)
				{
					write_int(damagetomob.first);
					skip(12);
					write_byte(static_cast<int8_t>(damagetomob.second.size()));

					for (auto& singledamage : damagetomob.second)
						write_int(singledamage.first);

					skip(4);
				}

				// 最后一块：服务端多读一个 oid(int)，再读段数(byte)。
				write_int(0);
				write_byte(0);
			}
			else
			{
				for (auto& damagetomob : attack.damagelines)
				{
					write_int(damagetomob.first);

					// 14 字节里含怪物的属性/状态位，服务端整段 skip（:908）。
					skip(14);

					for (auto& singledamage : damagetomob.second)
					{
						write_int(singledamage.first);
						// TODO: Add critical here
					}

					// parseDamage:1037-1039 的排除条件写成 `a != x || a != y || ...`，
					// 恒真 → 服务端**每个目标后面都会 skip(4)**。
					// 原来这里按 skill != 5221004 跳过，等于速射发包时少 4 字节，
					// 下一个目标的 oid 会从错位处读起。
					skip(4);
				}

				// parseDamage:1042-1045：毒炸弹（14111006）在所有目标之后再读
				// skip(4) + Point(x,y)，毒雾就落在服务端读到的这个点上。
				// 客户端没有鼠标选点，退化成攻击发起位置。
				if (attack.skill == 14111006)
				{
					skip(4);
					write_point(attack.origin);
				}
			}
		}

	private:
		static OutPacket::Opcode opcodefor(Attack::Type type)
		{
			switch (type)
			{
			case Attack::Type::CLOSE:
				return OutPacket::Opcode::CLOSE_ATTACK;
			case Attack::Type::RANGED:
				return OutPacket::Opcode::RANGED_ATTACK;
			default:
				return OutPacket::Opcode::MAGIC_ATTACK;
			}
		}

		// s9 AbstractDealDamageHandler.parseDamage:705 的技能表（常量取自
		// org/gms/constants/skills 下同名文件）。
		static bool has_charge_field(int32_t skill)
		{
			switch (skill)
			{
			case 22121000:	// Evan.ICE_BREATH
			case 22151001:	// Evan.FIRE_BREATH
			case 2121001:	// FPArchMage.BIG_BANG
			case 2221001:	// ILArchMage.BIG_BANG
			case 2321001:	// Bishop.BIG_BANG
			case 5201002:	// Gunslinger.GRENADE
			case 5101004:	// Brawler.CORKSCREW_BLOW
			case 15101003:	// ThunderBreaker.CORKSCREW_BLOW
			case 14111006:	// NightWalker.POISON_BOMB
				return true;
			default:
				return false;
			}
		}

		// s9 AbstractDealDamageHandler.parseDamage:757：远程持续/贯穿型技能多 4 字节。
		static bool has_sustained_field(int32_t skill)
		{
			switch (skill)
			{
			case 3121004:	// Bowmaster.HURRICANE
			case 13111002:	// WindArcher.HURRICANE
			case 3221001:	// Marksman.PIERCING_ARROW
			case 5221004:	// Corsair.RAPID_FIRE
				return true;
			default:
				return false;
			}
		}
	};

	// Tells the server that the player took damage
	// Opcode: TAKE_DAMAGE(48)
	class TakeDamagePacket : public OutPacket
	{
	public:
		enum From : int8_t
		{
			TOUCH = -1
		};

		TakeDamagePacket(int8_t from, uint8_t element, int32_t damage, int32_t mobid, int32_t oid, uint8_t direction) : OutPacket(OutPacket::Opcode::TAKE_DAMAGE)
		{
			write_time();
			write_byte(from);
			write_byte(element);
			write_int(damage);
			write_int(mobid);
			write_int(oid);
			write_byte(direction);
		}

		// From mob attack result
		TakeDamagePacket(const MobAttackResult& result, From from) : TakeDamagePacket(from, 0, result.damage, result.mobid, result.oid, result.direction) {}
	};

	// Packet which notifies the server of a skill usage
	// Opcode: USE_SKILL(91)
	class UseSkillPacket : public OutPacket
	{
	public:
		UseSkillPacket(int32_t skillid, int32_t level) : OutPacket(OutPacket::Opcode::USE_SKILL)
		{
			write_time();
			write_int(skillid);
			write_byte(static_cast<uint8_t>(level));

			// If monster magnet : some more bytes

			if (skillid % 10000000 == 1004)
				skip(2); // TODO: No idea what this could be

			// TODO: A point (4 bytes) could be added at the end
		}
	};
}