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
#include "Combat.h"

#include "../../Character/SkillId.h"
#include "../../IO/Messages.h"

#include "../../Net/Packets/AttackAndSkillPackets.h"
#include "../../Net/Packets/AranPackets.h"
#include "../../Net/Packets/GameplayPackets.h"

namespace ms
{
	namespace
	{
		// 职业门照抄服务端 GameConstants.isAran（GameConstants.java:513-515）。
		// 本 fork 的战神 id 是 2000 / 2100..2112，技能 id 也是 21xxxxxx 前缀
		// （Aran.java:29 COMBO_ABILITY = 21000000），不是标准 v83 的 22xx——
		// 客户端若按 22xx 判定，服务端 AranComboHandler 会整包忽略，连击永远不涨。
		bool is_arleg(uint16_t job)
		{
			return job == 2000 || (job >= 2100 && job <= 2112);
		}

		// s9 RangedAttackHandler:97-105：Combo Smash / Fenrir / Tempest 命中时服务端
		// 直接 setCombo(0) 清空连击，这三个技能自己不能再补发计数包。
		bool is_combo_finisher(int32_t skillid)
		{
			return skillid == 21100004 || skillid == 21110004 || skillid == 21120006;
		}
	}

	Combat::Combat(Player& in_player, MapChars& in_chars, MapMobs& in_mobs, MapReactors& in_reactors) : player(in_player), chars(in_chars), mobs(in_mobs), reactors(in_reactors),
		attackresults([&](const AttackResult& attack) { apply_attack(attack); }),
		bulleteffects([&](const BulletEffect& effect) { apply_bullet_effect(effect); }),
		damageeffects([&](const DamageEffect& effect) { apply_damage_effect(effect); }) {}

	void Combat::draw(double viewx, double viewy, float alpha) const
	{
		for (auto& be : bullets)
			be.bullet.draw(viewx, viewy, alpha);

		for (auto& dn : damagenumbers)
			dn.draw(viewx, viewy, alpha);
	}

	void Combat::update()
	{
		attackresults.update();
		bulleteffects.update();
		damageeffects.update();

		bullets.remove_if(
			[&](BulletEffect& mb)
			{
				int32_t target_oid = mb.damageeffect.target_oid;

				if (mobs.contains(target_oid))
				{
					mb.target = mobs.get_mob_head_position(target_oid);
					bool apply = mb.bullet.update(mb.target);

					if (apply)
						apply_damage_effect(mb.damageeffect);

					return apply;
				}
				else
				{
					return mb.bullet.update(mb.target);
				}
			}
		);

		damagenumbers.remove_if(
			[](DamageNumber& dn)
			{
				return dn.update();
			}
		);
	}

	void Combat::use_move(int32_t move_id)
	{
		if (!player.can_attack())
			return;

		const SpecialMove& move = get_move(move_id);
		SpecialMove::ForbidReason reason = player.can_use(move);
		Weapon::Type weapontype = player.get_stats().get_weapontype();

		switch (reason)
		{
		case SpecialMove::ForbidReason::FBR_NONE:
			apply_move(move);
			break;
		default:
			ForbidSkillMessage(reason, weapontype).drop();
			break;
		}
	}

	void Combat::apply_move(const SpecialMove& move)
	{
		if (move.is_attack())
		{
			Attack attack = player.prepare_attack(move.is_skill());

			move.apply_useeffects(player);
			move.apply_actions(player, attack.type);

			player.set_afterimage(move.get_id());

			move.apply_stats(player, attack);

			Point<int16_t> origin = attack.origin;
			Rectangle<int16_t> range = attack.range;
			int16_t hrange = static_cast<int16_t>(range.left() * attack.hrange);

			if (attack.toleft)
			{
				range = {
					static_cast<int16_t>(origin.x() + hrange),
					static_cast<int16_t>(origin.x() + range.right()),
					static_cast<int16_t>(origin.y() + range.top()),
					static_cast<int16_t>(origin.y() + range.bottom())
				};
			}
			else
			{
				range = {
					static_cast<int16_t>(origin.x() - range.right()),
					static_cast<int16_t>(origin.x() - hrange),
					static_cast<int16_t>(origin.y() + range.top()),
					static_cast<int16_t>(origin.y() + range.bottom())
				};
			}

			// This approach should also make it easier to implement PvP
			// 一次攻击能报给服务端的怪物数受封包格式限制：numAttackedAndDamage 的高 4 位
			// 只能写 0-15（s9 AbstractDealDamageHandler.java:688）。本客户端 Skill.nx 里
			// mobCount 最大是 30（竹林 0001009 / 10001009 / 20001009），不钳就会把怪物数写成
			// (30 << 4) & 0xFF = 0，服务端一个伤害块都不读，伤害全丢。
			uint8_t mobcount = attack.mobcount > 15 ? 15 : attack.mobcount;
			AttackResult result = attack;

			MapObjects* mob_objs = mobs.get_mobs();
			MapObjects* reactor_objs = reactors.get_reactors();

			std::vector<int32_t> mob_targets = find_closest(mob_objs, range, origin, mobcount, true);
			std::vector<int32_t> reactor_targets = find_closest(reactor_objs, range, origin, mobcount, false);

			mobs.send_attack(result, attack, mob_targets, mobcount);
			result.attacker = player.get_oid();
			extract_effects(player, move, result);

			apply_use_movement(move);
			apply_result_movement(move, result);

			// SKILL_EFFECT：白名单充能/持续技起手广播，须在伤害包之前，
			// 让他人先看到施法姿态（s9 SkillEffectHandler → skillEffect）。
			if (SkillEffectPacket::is_effect_skill(result.skill))
			{
				int32_t lv = player.get_skills().get_level(result.skill);
				int8_t dir = result.toleft ? 1 : 0;
				SkillEffectPacket(result.skill, lv, dir, static_cast<int8_t>(result.speed), dir).dispatch();
			}

			AttackPacket(result).dispatch();

			// Wave 4：手雷/毒弹投掷点上报。s9 GrenadeEffectHandler 白名单
			// GRENADE(5201002)/POISON_BOMB(14111006)，技能等级>0 时回
			// THROW_GRENADE(204) 广播落点（本端 204 已接渲染）——缺的只有
			// 这条上行。坐标用攻击 origin（世界坐标，与 s9 Point 同域）。
			if (GrenadeEffectPacket::is_grenade(result.skill))
				GrenadeEffectPacket(origin, result.skill).dispatch();

			// 战神连击由攻击命中驱动：服务端 AranComboHandler 每收到一个空的
			// ARAN_COMBO_COUNTER(0xA3) 就把连击 +1（距上次超过 3000ms 先归零，
			// 每满 10 层按 Combo Ability 等级发 ARAN_COMBO buff），计数再由
			// SHOW_COMBO(0xE1) 回推给 UIComboCounter——所以这里只发包，本地不猜数。
			// 一次攻击命中几只怪就发几包（result.damagelines 每个 oid 一条）。
			if (result.mobcount > 0 && is_arleg(player.get_stats().get_job().get_id())
				&& !is_combo_finisher(result.skill))
			{
				for (uint8_t i = 0; i < result.mobcount; i++)
					AranComboPacket().dispatch();
			}

			if (reactor_targets.size())
				if (Optional<Reactor> reactor = reactor_objs->get(reactor_targets.at(0)))
					DamageReactorPacket(reactor->get_oid(), player.get_position(), 0, 0).dispatch();
		}
		else
		{
			move.apply_useeffects(player);
			move.apply_actions(player, Attack::Type::MAGIC);

			int32_t moveid = move.get_id();
			int32_t level = player.get_skills().get_level(moveid);
			// CHAKRA / MONSTER_MAGNET 等非攻击白名单也走 SKILL_EFFECT。
			if (SkillEffectPacket::is_effect_skill(moveid))
			{
				// getflip()==facing_right；direction 用「朝左」位与攻击包 toleft 同语义。
				int8_t dir = player.getflip() ? 0 : 1;
				int8_t speed = player.get_integer_attackspeed();
				SkillEffectPacket(moveid, level, dir, speed, dir).dispatch();
			}
			UseSkillPacket(moveid, level).dispatch();
		}
	}

	std::vector<int32_t> Combat::find_closest(MapObjects* objs, Rectangle<int16_t> range, Point<int16_t> origin, uint8_t objcount, bool use_mobs) const
	{
		std::multimap<uint16_t, int32_t> distances;

		for (auto& mmo : *objs)
		{
			if (use_mobs)
			{
				const Mob* mob = static_cast<const Mob*>(mmo.second.get());

				if (mob && mob->is_alive() && mob->is_in_range(range))
				{
					int32_t oid = mob->get_oid();
					uint16_t distance = mob->get_position().distance(origin);
					distances.emplace(distance, oid);
				}
			}
			else
			{
				// Assume Reactor
				const Reactor* reactor = static_cast<const Reactor*>(mmo.second.get());

				if (reactor && reactor->is_hittable() && reactor->is_in_range(range))
				{
					int32_t oid = reactor->get_oid();
					uint16_t distance = reactor->get_position().distance(origin);
					distances.emplace(distance, oid);
				}
			}
		}

		std::vector<int32_t> targets;

		for (auto& iter : distances)
		{
			if (targets.size() >= objcount)
				break;

			targets.push_back(iter.second);
		}

		return targets;
	}

	void Combat::apply_use_movement(const SpecialMove& move)
	{
		switch (move.get_id())
		{
		case SkillId::Id::TELEPORT_FP:
		case SkillId::Id::IL_TELEPORT:
		case SkillId::Id::PRIEST_TELEPORT:
		case SkillId::Id::FLASH_JUMP:
		default:
			break;
		}
	}

	void Combat::apply_result_movement(const SpecialMove& move, const AttackResult& result)
	{
		switch (move.get_id())
		{
		case SkillId::Id::RUSH_HERO:
		case SkillId::Id::RUSH_PALADIN:
		case SkillId::Id::RUSH_DK:
			apply_rush(result);
			break;
		default:
			break;
		}
	}

	void Combat::apply_rush(const AttackResult& result)
	{
		if (result.mobcount == 0)
			return;

		Point<int16_t> mob_position = mobs.get_mob_position(result.last_oid);
		int16_t targetx = mob_position.x();
		player.rush(targetx);
	}

	void Combat::apply_bullet_effect(const BulletEffect& effect)
	{
		bullets.push_back(effect);

		if (bullets.back().bullet.settarget(effect.target))
		{
			apply_damage_effect(effect.damageeffect);
			bullets.pop_back();
		}
	}

	void Combat::apply_damage_effect(const DamageEffect& effect)
	{
		// 伤害数字总量钳制（Wave P）：AoE 群怪/多段技能瞬时可堆几百个数字，
		// 每个都在 update/draw 上花时间。超过 160 就从最老的丢——最老的
		// 本就接近淡出尾声，观感无损，尖峰负载有界。
		constexpr size_t MAX_DAMAGE_NUMBERS = 160;

		Point<int16_t> head_position = mobs.get_mob_head_position(effect.target_oid);
		damagenumbers.push_back(effect.number);
		damagenumbers.back().set_x(head_position.x());

		while (damagenumbers.size() > MAX_DAMAGE_NUMBERS)
			damagenumbers.pop_front();

		const SpecialMove& move = get_move(effect.move_id);
		mobs.apply_damage(effect.target_oid, effect.damage, effect.toleft, effect.user, move);
	}

	void Combat::push_attack(const AttackResult& attack)
	{
		attackresults.push(400, attack);
	}

	void Combat::apply_attack(const AttackResult& attack)
	{
		if (Optional<OtherChar> ouser = chars.get_char(attack.attacker))
		{
			OtherChar& user = *ouser;
			user.update_skill(attack.skill, attack.level);
			user.update_speed(attack.speed);

			const SpecialMove& move = get_move(attack.skill);
			move.apply_useeffects(user);

			if (Stance::Id stance = Stance::by_id(attack.stance))
				user.attack(stance);
			else
				move.apply_actions(user, attack.type);

			user.set_afterimage(attack.skill);

			extract_effects(user, move, attack);
		}
	}

	void Combat::extract_effects(const Char& user, const SpecialMove& move, const AttackResult& result)
	{
		AttackUser attackuser = {
			user.get_skilllevel(move.get_id()),
			user.get_level(),
			user.is_twohanded(),
			!result.toleft
		};

		if (result.bullet)
		{
			Bullet bullet{
				move.get_bullet(user, result.bullet),
				user.get_position(),
				result.toleft
			};

			for (auto& line : result.damagelines)
			{
				int32_t oid = line.first;

				if (mobs.contains(oid))
				{
					std::vector<DamageNumber> numbers = place_numbers(oid, line.second);
					Point<int16_t> head = mobs.get_mob_head_position(oid);

					size_t i = 0;

					for (auto& number : numbers)
					{
						DamageEffect effect{
							attackuser,
							number,
							damage::legacy_line(line.second[i].first),
							result.toleft,
							oid,
							move.get_id()
						};

						bulleteffects.emplace(user.get_attackdelay(i), std::move(effect), bullet, head);
						i++;
					}
				}
			}

			if (result.damagelines.empty())
			{
				int16_t xshift = result.toleft ? -400 : 400;
				Point<int16_t> target = user.get_position() + Point<int16_t>(xshift, -26);

				for (uint8_t i = 0; i < result.hitcount; i++)
				{
					DamageEffect effect{ attackuser, {}, 0, false, 0, 0 };
					bulleteffects.emplace(user.get_attackdelay(i), std::move(effect), bullet, target);
				}
			}
		}
		else
		{
			for (auto& line : result.damagelines)
			{
				int32_t oid = line.first;

				if (mobs.contains(oid))
				{
					std::vector<DamageNumber> numbers = place_numbers(oid, line.second);

					size_t i = 0;

					for (auto& number : numbers)
					{
						damageeffects.emplace(
							user.get_attackdelay(i),
							attackuser,
							number,
							damage::legacy_line(line.second[i].first),
							result.toleft,
							oid,
							move.get_id()
							);

						i++;
					}
				}
			}
		}
	}

	std::vector<DamageNumber> Combat::place_numbers(int32_t oid, const damage::DamageLines& damagelines)
	{
		std::vector<DamageNumber> numbers;
		int16_t head = mobs.get_mob_head_position(oid).y();

		for (auto& line : damagelines)
		{
			int64_t amount = line.first;
			bool critical = line.second;
			DamageNumber::Type type = critical ? DamageNumber::Type::CRITICAL : DamageNumber::Type::NORMAL;
			numbers.emplace_back(type, amount, head);

			head -= DamageNumber::rowheight(critical);
		}

		return numbers;
	}

	void Combat::show_buff(int32_t cid, int32_t skillid, int8_t level)
	{
		if (Optional<OtherChar> ouser = chars.get_char(cid))
		{
			OtherChar& user = *ouser;
			user.update_skill(skillid, level);

			const SpecialMove& move = get_move(skillid);
			move.apply_useeffects(user);
			move.apply_actions(user, Attack::Type::MAGIC);
		}
	}

	void Combat::show_player_buff(int32_t skillid)
	{
		get_move(skillid).apply_useeffects(player);
	}

	const SpecialMove& Combat::get_move(int32_t move_id)
	{
		if (move_id == 0)
			return regularattack;

		auto iter = skills.find(move_id);

		if (iter == skills.end())
			iter = skills.emplace(move_id, move_id).first;

		return iter->second;
	}
}