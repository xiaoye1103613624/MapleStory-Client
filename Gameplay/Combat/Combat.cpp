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
#include "../../Data/SkillData.h"
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

		// 攻击包 display 字段（s9 PacketCreator.addAttackBody 原样广播给其他客户端）。
		// MinMS Combat.cpp:174-190：组合动作走 body_action_by_name（alert2→41…），
		// 基本站姿走 body_action(stance)。旧实现恒 0 → 他人看到固定默认姿态。
		void fill_attack_wire_action(AttackResult& result, const Player& player)
		{
			result.stance = static_cast<uint8_t>(player.get_look().get_stance());
			const std::string& action = player.get_look().get_action_name();
			if (!action.empty())
			{
				if (uint8_t wire = Stance::body_action_by_name(action))
				{
					result.display = wire;
					return;
				}
			}
			result.display = Stance::body_action(
				result.stance != 0 ? Stance::by_id(result.stance) : Stance::STAND1);
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
		// H1 宏释放队列泵：每帧最多尝试放一个待释放的宏技能；
		// player.can_attack()（动画硬直）没结束就原地等，间隔至少
		// MACRO_GAP=8ms 一帧，避免同 tick 连发被服务端合并。
		if (!macro_pending.empty())
		{
			if (macro_cooldown > 0)
				--macro_cooldown;
			else if (player.can_attack())
			{
				int32_t next = macro_pending.front();
				macro_pending.erase(macro_pending.begin());
				use_move(next);
				// 放完一个后至少隔 2 帧再试下一个，让动画/服务端时序分开。
				macro_cooldown = 2;
			}
		}

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

	void Combat::use_macro(int32_t skill1, int32_t skill2, int32_t skill3)
	{
		// Angel CMacroSysMan::Update：整行宏按 1→2→3 顺序入队，不跳过空位后
		// 提前收口（空位 = 该槽无技能，直接不入队）。
		macro_pending.clear();
		for (int32_t sid : { skill1, skill2, skill3 })
			if (sid > 0)
				macro_pending.push_back(sid);
		macro_cooldown = 1;
	}

	void Combat::use_move(int32_t move_id)
	{
		if (player.get_char_state() == Char::State::DIED || player.is_attacking())
			return;

		const SpecialMove& move = get_move(move_id);

		// MinMS Combat::use_move：仅普攻/攻击技能走 can_attack / can_cast_attack；
		// 瞬移/二段跳等非攻击位移技能不拦武器与落地，否则空中二段跳永远发不出去。
		if (!move.is_skill())
		{
			if (!player.can_attack())
				return;
		}
		else if (move.is_attack())
		{
			if (!player.can_cast_attack())
				return;
		}

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
			fill_attack_wire_action(result, player);

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
				{
					// s9 ReactorHitHandler：oid + charPos + stance + pad4 + skillid。
					// stance/skill 取本次 AttackResult（旧恒 0 会使姿态/技能敏感脚本偏）。
					DamageReactorPacket(
						reactor->get_oid(),
						player.get_position(),
						static_cast<int16_t>(result.stance),
						result.skill).dispatch();
				}
		}
		else
		{
			move.apply_useeffects(player);
			move.apply_actions(player, Attack::Type::MAGIC);

			// 2026-11-22：瞬移/二段跳等非攻击位移技能走这条分支。旧实现只播特效
			// + 发 UseSkillPacket，从不调用 apply_use_movement（该函数仅攻击分支 :200
			// 调用），所以技能"发出去了但人不动"。在这里补上本地位移。
			apply_use_movement(move);

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
		// 2026-11-22：原四个 case 全是空语句 —— 瞬移/二段跳发包正常但客户端
		// 本地零位移，用户报「学来的技能用不了」。接 MinMS Combat::apply_use_movement
		// / apply_teleport 同源的 Player 位移；MP/最终坐标仍由 s9 权威结算。
		const int32_t skillid = move.get_id();
		const int32_t level = player.get_skills().get_level(skillid);

		switch (skillid)
		{
		case SkillId::Id::FLASH_JUMP:
		case SkillId::Id::FLASH_JUMP_NW:
		case SkillId::Id::FLASH_JUMP_DUAL:
			player.apply_flash_jump(skillid, level);
			break;
		case SkillId::Id::TELEPORT_FP:
		case SkillId::Id::IL_TELEPORT:
		case SkillId::Id::PRIEST_TELEPORT:
		case SkillId::Id::TELEPORT_FP_CYGNUS:
		case SkillId::Id::TELEPORT_EVAN:
		{
			// 瞬移距离取技能 hrange*100（v83 基础约 130），缺省 130。
			int16_t range = 130;
			const auto& stats = SkillData::get(skillid).get_stats(level > 0 ? level : 1);
			if (stats.hrange > 0.0f)
				range = static_cast<int16_t>(stats.hrange * 100.0f);
			player.apply_teleport(skillid, range);
			break;
		}
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
		case SkillId::Id::RECOIL_SHOT:
			// 激退射杀：施放即后跃（空挥也退），落点纯客户端表现，不动服务端。
			player.apply_recoil(player.get_skills().get_level(SkillId::Id::RECOIL_SHOT));
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
		Point<int16_t> head_position = mobs.get_mob_head_position(effect.target_oid);
		damagenumbers.push_back(effect.number);
		// 多段伤害的末段到达时怪物可能已被击杀回收，此时 get_mob_head_position
		// 返回 (0,0)，照旧 set_x(0) 会把数字甩到地图左侧固定位置。
		// 仅在怪物仍存在时跟随其最新位置；否则保留 place_numbers 锁定的 x。
		if (head_position.x() != 0 || head_position.y() != 0)
			damagenumbers.back().set_x(head_position.x());

		while (damagenumbers.size() > MAX_DAMAGE_NUMBERS)
			damagenumbers.pop_front();

		const SpecialMove& move = get_move(effect.move_id);
		mobs.apply_damage(effect.target_oid, effect.damage, effect.toleft, effect.user, move);
	}

	void Combat::show_mob_damage(int32_t oid, int64_t damage, int32_t skin)
	{
		if (damage <= 0)
			return;

		Point<int16_t> head = mobs.get_mob_head_position(oid);

		if (head.x() == 0 && head.y() == 0)
			return;

		damagenumbers.emplace_back(DamageNumber::Type::NORMAL, damage, head.y(), 0, skin);
		damagenumbers.back().set_x(head.x());

		while (damagenumbers.size() > MAX_DAMAGE_NUMBERS)
			damagenumbers.pop_front();
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
		Point<int16_t> headpos = mobs.get_mob_head_position(oid);
		int16_t head = headpos.y();

		for (auto& line : damagelines)
		{
			int64_t amount = line.first;
			bool critical = line.second;
			DamageNumber::Type type = critical ? DamageNumber::Type::CRITICAL : DamageNumber::Type::NORMAL;
			// 生成时就锁定怪物头部 x：后续延迟派发若遇怪物回收，位置不会退化成 0。
			numbers.emplace_back(type, amount, head, headpos.x(), player.get_damage_skin());

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