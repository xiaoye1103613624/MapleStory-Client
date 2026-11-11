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
#include "Player.h"

#include "PlayerStates.h"

#include "../Data/WeaponData.h"
#include "../IO/UI.h"

#include "../IO/UITypes/UIStatsInfo.h"
#include "../Net/Packets/CommonPackets.h"
#include "../Net/Packets/GameplayPackets.h"
#include "../Net/Packets/InventoryPackets.h"
#include "../Net/Packets/PetPackets.h"
#include "../Configuration.h"
#include "../Constants.h"
#include "../MapleStory.h"

#include <algorithm>

namespace ms
{
	const PlayerNullState nullstate;

	const PlayerState* get_state(Char::State state)
	{
		static PlayerStandState standing;
		static PlayerWalkState walking;
		static PlayerFallState falling;
		static PlayerProneState lying;
		static PlayerClimbState climbing;
		static PlayerSitState sitting;
		static PlayerFlyState flying;

		switch (state)
		{
			case Char::State::STAND:
				return &standing;
			case Char::State::WALK:
				return &walking;
			case Char::State::FALL:
				return &falling;
			case Char::State::PRONE:
				return &lying;
			case Char::State::LADDER:
			case Char::State::ROPE:
				return &climbing;
			case Char::State::SIT:
				return &sitting;
			case Char::State::SWIM:
				return &flying;
			default:
				return nullptr;
		}
	}

	Player::Player(const CharEntry& entry) : Char(entry.id, entry.look, entry.stats.name), stats(entry.stats)
	{
		attacking = false;
		underwater = false;

		set_state(Char::State::STAND);
		set_direction(true);
	}

	Player::Player() : Char(0, {}, "") {}

	void Player::respawn(Point<int16_t> pos, bool uw)
	{
		set_position(pos.x(), pos.y());
		underwater = uw;
		keysdown.clear();
		attacking = false;
		ladder = nullptr;
		nullstate.update_state(*this);
	}

	void Player::send_action(KeyAction::Id action, bool down)
	{
		const PlayerState* pst = get_state(state);

		if (pst)
			pst->send_action(*this, action, down);

		keysdown[action] = down;
	}

	void Player::recalc_stats(bool equipchanged)
	{
		Weapon::Type weapontype = get_weapontype();

		stats.set_weapontype(weapontype);
		stats.init_totalstats();

		if (equipchanged)
			inventory.recalc_stats(weapontype);

		for (auto stat : EquipStat::values)
		{
			int32_t inventory_total = inventory.get_stat(stat);
			stats.add_value(stat, inventory_total);
		}

		auto passive_skills = skillbook.collect_passives();

		for (auto& passive : passive_skills)
		{
			int32_t skill_id = passive.first;
			int32_t skill_level = passive.second;

			passive_buffs.apply_buff(stats, skill_id, skill_level);
		}

		for (const Buff& buff : buffs.values())
			active_buffs.apply_buff(stats, buff.stat, buff.value);

		stats.close_totalstats();

		if (auto statsinfo = UI::get().get_element<UIStatsInfo>())
			statsinfo->update_all_stats();
	}

	void Player::change_equip(int16_t slot)
	{
		if (int32_t itemid = inventory.get_item_id(InventoryType::Id::EQUIPPED, slot))
			look.add_equip(itemid);
		else
			look.remove_equip(EquipSlot::by_id(slot));
	}

	void Player::use_item(int32_t itemid)
	{
		InventoryType::Id type = InventoryType::by_item_id(itemid);

		if (int16_t slot = inventory.find_item(type, itemid))
			if (type == InventoryType::Id::USE)
				UseItemPacket(slot, itemid).dispatch();
	}

	void Player::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		if (layer == get_layer())
			Char::draw(viewx, viewy, alpha);
	}

	int8_t Player::update(const Physics& physics)
	{
		const PlayerState* pst = get_state(state);

		if (pst)
		{
			pst->update(*this);
			physics.move_object(phobj);

			bool aniend = Char::update(physics, get_stancespeed());

			if (aniend && attacking)
			{
				attacking = false;
				nullstate.update_state(*this);
			}
			else
			{
				pst->update_state(*this);
			}
		}

		uint8_t stancebyte = facing_right ? state : state + 1;
		Movement newmove(phobj, stancebyte);
		bool needupdate = lastmove.hasmoved(newmove);

		if (needupdate)
		{
			MovePlayerPacket(newmove).dispatch();
			lastmove = newmove;
		}

		// W1：宠物跟随位移上报（对账 MovePetHandler）
		for (uint8_t i = 0; i < 3; ++i)
		{
			const PetLook& pet = get_pet(i);
			if (pet.get_itemid() <= 0 || pet.get_uniqueid() <= 0)
				continue;
			uint8_t petstance = static_cast<uint8_t>(pet.get_stance()) * 2;
			Movement petmove(pet.get_phobj(), petstance);
			if (lastpetmove[i].hasmoved(petmove))
			{
				MovePetPacket(pet.get_uniqueid(), petmove).dispatch();
				lastpetmove[i] = petmove;
			}
		}

		climb_cooldown.update();

		// CANCEL_DEBUFF(99)：疾病类临时状态本地倒计时到期后发空包。
		// s9 RecvOpcode.CANCEL_DEBUFF(99) 协议对齐；083 IDB 该临时状态到期检查符号 0 命中，
		// 相关 083 真符号 OnTemporaryStatSet @0xa202be / OnTemporaryStatReset @0xa2071f 仅作对照；
		// s9 CancelDebuffHandler 当前为空实现，发了无副作用，仅协议对齐。
		static constexpr Buffstat::Id k_diseases[] = {
			Buffstat::Id::STUN,
			Buffstat::Id::POISON,
			Buffstat::Id::SEAL,
			Buffstat::Id::DARKNESS,
			Buffstat::Id::WEAKEN
		};
		bool expired_disease = false;
		for (Buffstat::Id id : k_diseases)
		{
			Buff& b = buffs[id];
			if (b.value <= 0 || b.duration <= 0)
				continue;
			b.duration -= Constants::TIMESTEP;
			if (b.duration <= 0)
			{
				b = {};
				expired_disease = true;
				LOG(LOG_DEBUG, "[Player] disease expired stat=" << static_cast<int>(id));
			}
		}
		if (expired_disease)
			CancelDebuffPacket().dispatch();

		return get_layer();
	}

	int8_t Player::get_integer_attackspeed() const
	{
		int32_t weapon_id = look.get_equips().get_weapon();

		if (weapon_id <= 0)
			return 0;

		const WeaponData& weapon = WeaponData::get(weapon_id);

		int8_t base_speed = stats.get_attackspeed();
		int8_t weapon_speed = weapon.get_speed();

		return base_speed + weapon_speed;
	}

	void Player::set_direction(bool flipped)
	{
		if (!attacking)
			Char::set_direction(flipped);
	}

	void Player::set_state(State st)
	{
		if (!attacking)
		{
			Char::set_state(st);

			const PlayerState* pst = get_state(st);

			if (pst)
				pst->initialize(*this);
		}
	}

	bool Player::is_attacking() const
	{
		return attacking;
	}

	bool Player::can_attack() const
	{
		return !attacking && !is_climbing() && !is_sitting() && look.get_equips().has_weapon();
	}

	SpecialMove::ForbidReason Player::can_use(const SpecialMove& move) const
	{
		if (move.is_skill() && state == Char::State::PRONE)
			return SpecialMove::ForbidReason::FBR_OTHER;

		if (move.is_attack() && (state == Char::State::LADDER || state == Char::State::ROPE))
			return SpecialMove::ForbidReason::FBR_OTHER;

		if (has_cooldown(move.get_id()))
			return SpecialMove::ForbidReason::FBR_COOLDOWN;

		int32_t level = skillbook.get_level(move.get_id());
		Weapon::Type weapon = get_weapontype();
		const Job& job = stats.get_job();
		int32_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t mp = stats.get_stat(MapleStat::Id::MP);
		uint16_t bullets = inventory.get_bulletcount();

		return move.can_use(level, weapon, job, hp, mp, bullets);
	}

	Attack Player::prepare_attack(bool skill) const
	{
		Attack::Type attacktype;
		bool degenerate;

		if (state == Char::State::PRONE)
		{
			degenerate = true;
			attacktype = Attack::Type::CLOSE;
		}
		else
		{
			Weapon::Type weapontype;
			weapontype = get_weapontype();

			switch (weapontype)
			{
				case Weapon::Type::BOW:
				case Weapon::Type::CROSSBOW:
				case Weapon::Type::CLAW:
				case Weapon::Type::GUN:
				{
					degenerate = !inventory.has_projectile();
					attacktype = degenerate ? Attack::Type::CLOSE : Attack::Type::RANGED;
					break;
				}
				case Weapon::Type::WAND:
				case Weapon::Type::STAFF:
				{
					degenerate = !skill;
					attacktype = degenerate ? Attack::Type::CLOSE : Attack::Type::MAGIC;
					break;
				}
				default:
				{
					attacktype = Attack::Type::CLOSE;
					degenerate = false;
					break;
				}
			}
		}

		Attack attack;
		attack.type = attacktype;
		attack.mindamage = stats.get_mindamage();
		attack.maxdamage = stats.get_maxdamage();

		if (degenerate)
		{
			attack.mindamage /= 10;
			attack.maxdamage /= 10;
		}

		attack.critical = stats.get_critical();
		attack.ignoredef = stats.get_ignoredef();

		if (Configuration::get().get_damage_formula_v2())
		{
			// 原生 MDamage 的 MAD 输入钳到 1999（IDA sub_791617）。
			// 魔法基础值沿用 v83 的 (INT*4 + LUK)/100 * MAD 结构，与
			// 物理 calculateMaxBaseDamage 的武器倍率 4.0 同构。
			int32_t mad = std::min<int32_t>(stats.get_total(EquipStat::Id::MAGIC), 1999);
			int32_t int_ = stats.get_total(EquipStat::Id::INT);
			int32_t luk = stats.get_total(EquipStat::Id::LUK);
			attack.magicdamage = (int_ * 4.0 + luk) / 100.0 * mad;

			// CharStats 的 mincrit/maxcrit 是额外倍率，转成 1+x 的暴击区间。
			attack.critical_min = 1.0f + stats.get_mincrit();
			attack.critical_max = 1.0f + stats.get_maxcrit();
			attack.bossdamage = stats.get_bossdmg();
		}

		attack.accuracy = stats.get_total(EquipStat::Id::ACC);
		attack.playerlevel = stats.get_stat(MapleStat::Id::LEVEL);
		attack.range = stats.get_range();
		attack.bullet = inventory.get_bulletid();
		attack.origin = get_position();
		attack.toleft = !facing_right;
		attack.speed = get_integer_attackspeed();

		return attack;
	}

	void Player::rush(double targetx)
	{
		if (phobj.onground)
		{
			uint16_t delay = get_attackdelay(1);
			phobj.movexuntil(targetx, delay);
			phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);
		}
	}

	bool Player::is_invincible() const
	{
		if (state == Char::State::DIED)
			return true;

		if (has_buff(Buffstat::Id::DARKSIGHT))
			return true;

		return Char::is_invincible();
	}

	MobAttackResult Player::damage(const MobAttack& attack)
	{
		int64_t damage = stats.calculate_damage(attack.watk);
		show_damage(damage);

		bool fromleft = attack.origin.x() > phobj.get_x();

		bool missed = damage <= 0;
		bool immovable = ladder || state == Char::State::DIED;
		bool knockback = !missed && !immovable;

		if (knockback && randomizer.above(stats.get_stance()))
		{
			phobj.hspeed = fromleft ? -1.5 : 1.5;
			phobj.vforce -= 3.5;
		}

		uint8_t direction = fromleft ? 0 : 1;

		return { attack, damage, direction };
	}

	void Player::give_buff(Buff buff)
	{
		buffs[buff.stat] = buff;
	}

	void Player::cancel_buff(Buffstat::Id stat)
	{
		buffs[stat] = {};
	}

	bool Player::has_buff(Buffstat::Id stat) const
	{
		return buffs[stat].value > 0;
	}

	int16_t Player::get_buff_value(Buffstat::Id stat) const
	{
		return buffs[stat].value;
	}

	int32_t Player::get_buff_skillid(Buffstat::Id stat) const
	{
		return buffs[stat].skillid;
	}

	int32_t Player::get_buff_duration(Buffstat::Id stat) const
	{
		return buffs[stat].duration;
	}

	void Player::change_skill(int32_t skill_id, int32_t skill_level, int32_t masterlevel, int64_t expiration)
	{
		int32_t old_level = skillbook.get_level(skill_id);
		skillbook.set_skill(skill_id, skill_level, masterlevel, expiration);

		if (old_level != skill_level)
			recalc_stats(false);
	}

	void Player::add_cooldown(int32_t skill_id, int32_t cooltime)
	{
		cooldowns[skill_id] = cooltime;
	}

	bool Player::has_cooldown(int32_t skill_id) const
	{
		auto iter = cooldowns.find(skill_id);

		if (iter == cooldowns.end())
			return false;

		return iter->second > 0;
	}

	void Player::change_level(uint16_t level)
	{
		uint16_t oldlevel = get_level();

		if (level > oldlevel)
			show_effect_id(CharEffect::Id::LEVELUP);

		stats.set_stat(MapleStat::Id::LEVEL, level);
	}

	uint16_t Player::get_level() const
	{
		return stats.get_stat(MapleStat::Id::LEVEL);
	}

	int32_t Player::get_skilllevel(int32_t skillid) const
	{
		return skillbook.get_level(skillid);
	}

	void Player::change_job(uint16_t jobid)
	{
		show_effect_id(CharEffect::Id::JOBCHANGE);
		stats.change_job(jobid);
	}

	void Player::set_seat(Optional<const Seat> seat)
	{
		if (seat)
		{
			set_position(seat->getpos());
			set_state(Char::State::SIT);
		}
	}

	void Player::set_ladder(Optional<const Ladder> ldr)
	{
		ladder = ldr;

		if (ladder)
		{
			phobj.set_x(ldr->get_x());

			phobj.hspeed = 0.0;
			phobj.vspeed = 0.0;
			phobj.fhlayer = 7;

			set_state(ldr->is_ladder() ? Char::State::LADDER : Char::State::ROPE);
		}
	}

	void Player::set_climb_cooldown()
	{
		climb_cooldown.set_for(1000);
	}

	bool Player::can_climb()
	{
		return !climb_cooldown;
	}

	float Player::get_walkforce() const
	{
		return 0.05f + 0.11f * static_cast<float>(stats.get_total(EquipStat::Id::SPEED)) / 100;
	}

	float Player::get_jumpforce() const
	{
		return 1.0f + 3.5f * static_cast<float>(stats.get_total(EquipStat::Id::JUMP)) / 100;
	}

	float Player::get_climbforce() const
	{
		return static_cast<float>(stats.get_total(EquipStat::Id::SPEED)) / 100;
	}

	float Player::get_flyforce() const
	{
		return 0.25f;
	}

	bool Player::is_underwater() const
	{
		return underwater;
	}

	bool Player::is_key_down(KeyAction::Id action) const
	{
		return keysdown.count(action) ? keysdown.at(action) : false;
	}

	CharStats& Player::get_stats()
	{
		return stats;
	}

	const CharStats& Player::get_stats() const
	{
		return stats;
	}

	Inventory& Player::get_inventory()
	{
		return inventory;
	}

	const Inventory& Player::get_inventory() const
	{
		return inventory;
	}

	SkillBook& Player::get_skills()
	{
		return skillbook;
	}

	QuestLog& Player::get_quests()
	{
		return questlog;
	}

	TeleportRock& Player::get_teleportrock()
	{
		return teleportrock;
	}

	MonsterBook& Player::get_monsterbook()
	{
		return monsterbook;
	}

	MarriageInfo& Player::get_marriage()
	{
		return marriage;
	}

	const MarriageInfo& Player::get_marriage() const
	{
		return marriage;
	}

	Optional<const Ladder> Player::get_ladder() const
	{
		return ladder;
	}
}