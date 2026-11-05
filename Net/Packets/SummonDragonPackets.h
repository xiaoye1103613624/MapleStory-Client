////////////////////////////////////////////////////////////////////////////////
//	W2 召唤兽 / 战神龙 c2s — 对账 s9 MoveSummon / SummonDamage / DamageSummon /
//	Beholder / MoveDragon Handler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MovementPacket.h"

#include <vector>

namespace ms
{
	// Opcode: MOVE_SUMMON(175 / 0xAF)
	// MoveSummonHandler: oid, short x, short y, parseMovement
	class MoveSummonPacket : public MovementPacket
	{
	public:
		MoveSummonPacket(int32_t oid, Point<int16_t> start, const Movement& movement)
			: MovementPacket(OutPacket::Opcode::MOVE_SUMMON)
		{
			write_int(oid);
			write_point(start);
			write_byte(1);
			writemovement(movement);
		}
	};

	// Opcode: SUMMON_ATTACK(176 / 0xB0)
	// SummonDamageHandler: oid, skip4, direction, numAttacked, skip8,
	//   ×{ monsterOid, skip18, damage }
	struct SummonAttackHit
	{
		int32_t monster_oid;
		int32_t damage;
	};

	class SummonAttackPacket : public OutPacket
	{
	public:
		SummonAttackPacket(int32_t summon_oid, int8_t direction,
			const std::vector<SummonAttackHit>& hits)
			: OutPacket(OutPacket::Opcode::SUMMON_ATTACK)
		{
			write_int(summon_oid);
			write_int(0);
			write_byte(direction);
			write_byte(static_cast<int8_t>(hits.size() > 15 ? 15 : hits.size()));
			write_long(0); // skip8
			size_t n = hits.size() > 15 ? 15 : hits.size();
			for (size_t i = 0; i < n; ++i)
			{
				write_int(hits[i].monster_oid);
				for (int k = 0; k < 18; ++k)
					write_byte(0);
				write_int(hits[i].damage);
			}
		}
	};

	// Opcode: DAMAGE_SUMMON(177 / 0xB1)
	// DamageSummonHandler: oid, skip1(-1), damage, monsterIdFrom
	class DamageSummonPacket : public OutPacket
	{
	public:
		DamageSummonPacket(int32_t oid, int32_t damage, int32_t monster_id_from)
			: OutPacket(OutPacket::Opcode::DAMAGE_SUMMON)
		{
			write_int(oid);
			write_byte(-1);
			write_int(damage);
			write_int(monster_id_from);
		}
	};

	// Opcode: BEHOLDER(178 / 0xB2)
	// BeholderHandler: oid, skillId, [short|byte]
	class BeholderPacket : public OutPacket
	{
	public:
		BeholderPacket(int32_t oid, int32_t skill_id)
			: OutPacket(OutPacket::Opcode::BEHOLDER)
		{
			write_int(oid);
			write_int(skill_id);
			// AURA_OF_BEHOLDER 读 short；其它读 byte——写 short 0 兼容两侧
			write_short(0);
		}
	};

	// Opcode: MOVE_DRAGON(181 / 0xB5)
	// MoveDragonHandler: short x, short y, parseMovement
	class MoveDragonPacket : public MovementPacket
	{
	public:
		MoveDragonPacket(Point<int16_t> start, const Movement& movement)
			: MovementPacket(OutPacket::Opcode::MOVE_DRAGON)
		{
			write_point(start);
			write_byte(1);
			writemovement(movement);
		}
	};
}
