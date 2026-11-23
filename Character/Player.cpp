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

#include <nlnx/nx.hpp>

#include "CombatPanelStats.h"
#include "PlayerStates.h"
#include "SkillId.h"

#include "../Data/SkillData.h"
#include "../Data/WeaponData.h"
#include "../Data/SetItemData.h"
#include "../Data/SkillData.h"
#include "../IO/UI.h"
#include "../Gameplay/Stage.h"

#include "../IO/UITypes/UIStatsInfo.h"
#include "../Net/Packets/CommonPackets.h"
#include "../Net/Packets/GameplayPackets.h"
#include "../Net/Packets/InventoryPackets.h"
#include "../Net/Packets/PetPackets.h"
#include "../Configuration.h"
#include "../Constants.h"
#include "../MapleStory.h"

#include <algorithm>
#include <vector>

namespace ms
{
	namespace
	{
		// ⑤ 套装 statKey → EquipStat::Id（Data/SetItem.txt 的 TIER 行）。
		EquipStat::Id set_stat_id(const std::string& key)
		{
			if (key == "STR") return EquipStat::Id::STR;
			if (key == "DEX") return EquipStat::Id::DEX;
			if (key == "INT") return EquipStat::Id::INT;
			if (key == "LUK") return EquipStat::Id::LUK;
			if (key == "HP") return EquipStat::Id::HP;
			if (key == "MP") return EquipStat::Id::MP;
			if (key == "WATK") return EquipStat::Id::WATK;
			if (key == "MAGIC") return EquipStat::Id::MAGIC;
			if (key == "WDEF") return EquipStat::Id::WDEF;
			if (key == "MDEF") return EquipStat::Id::MDEF;
			if (key == "ACC") return EquipStat::Id::ACC;
			if (key == "AVOID") return EquipStat::Id::AVOID;
			if (key == "HANDS") return EquipStat::Id::HANDS;
			if (key == "SPEED") return EquipStat::Id::SPEED;
			if (key == "JUMP") return EquipStat::Id::JUMP;
			return EquipStat::Id::LENGTH;
		}

		// ⑦ 095 强化加成（Round 15）：星之力 / 火花 / 宝珠。
		// 口径说明（避免臆造数值）：
		//   · 星之力：官方是「每星 +固定四维/攻击」的阶梯表，本端无 StarForce.img
		//     数据源，因此采用**线性保守模型**：每星 STR/DEX/INT/LUK +1、
		//     WATK/MAGIC +1（25 星上限，与 get_starforce 的 0..25 一致）。
		//     数值可由后续接入真实表替换，不改结构。
		//   · 火花 / 宝珠：值是词条 id，本端无 AdditionalOption/SoulOption 表，
		//     故**只做展示不做数值加成**（在 tooltip 出行），避免凭空造属性。
		//     这与「潜能」同口径 —— 潜能也只展示，数值待词条表落地。
		void apply_enchant_bonuses(CharStats& stats, const Inventory& inventory)
		{
			// 官方穿戴位（枚举值即 BodyPart 绝对值，见 EquipSlot.h 不变量）
			static const EquipSlot::Id wear_slots[] = {
				EquipSlot::Id::HAT, EquipSlot::Id::FACE, EquipSlot::Id::EYEACC,
				EquipSlot::Id::EARACC, EquipSlot::Id::TOP, EquipSlot::Id::BOTTOM,
				EquipSlot::Id::SHOES, EquipSlot::Id::GLOVES, EquipSlot::Id::CAPE,
				EquipSlot::Id::SHIELD, EquipSlot::Id::WEAPON,
				EquipSlot::Id::RING1, EquipSlot::Id::RING2,
				EquipSlot::Id::RING3, EquipSlot::Id::RING4,
				EquipSlot::Id::RING5, EquipSlot::Id::RING6,
				EquipSlot::Id::PENDANT1, EquipSlot::Id::PENDANT2,
				EquipSlot::Id::SHOULDER, EquipSlot::Id::POCKET,
				EquipSlot::Id::MEDAL, EquipSlot::Id::BELT,
				EquipSlot::Id::BADGE, EquipSlot::Id::TOTEM1, EquipSlot::Id::TOTEM2,
				EquipSlot::Id::TOTEM3, EquipSlot::Id::TOTEM4,
				EquipSlot::Id::EMBLEM, EquipSlot::Id::ANDROID_SLOT, EquipSlot::Id::HEART
			};

			for (EquipSlot::Id slot : wear_slots)
			{
				auto oequip = inventory.get_equip(InventoryType::Id::EQUIPPED, slot);

				if (!oequip)
					continue;

				const Equip& equip = *oequip;

				uint8_t sf = equip.get_starforce();

				if (sf == 0)
					continue;

				if (sf > 25)
					sf = 25;

				stats.add_value(EquipStat::Id::STR, sf);
				stats.add_value(EquipStat::Id::DEX, sf);
				stats.add_value(EquipStat::Id::INT, sf);
				stats.add_value(EquipStat::Id::LUK, sf);
				stats.add_value(EquipStat::Id::WATK, sf);
				stats.add_value(EquipStat::Id::MAGIC, sf);
			}
		}

		void apply_set_item_bonuses(CharStats& stats, const std::vector<int32_t>& equipped_item_ids)
		{
			// 按套装分组，避免同一件装备属于多套时重复计数。
			std::vector<int32_t> seen_sets;

			for (int32_t item_id : equipped_item_ids)
			{
				int32_t set_id = SetItemData::set_of(item_id);

				if (set_id <= 0)
					continue;

				if (std::find(seen_sets.begin(), seen_sets.end(), set_id) != seen_sets.end())
					continue;

				seen_sets.push_back(set_id);

				int16_t count = SetItemData::count_equipped(set_id, equipped_item_ids);

				if (count <= 0)
					continue;

				for (const SetItemData::Tier& tier : SetItemData::active_tiers(set_id, count))
				{
					const std::string& key = tier.stat_key;

					if (key == "ALLSTAT")
					{
						stats.add_value(EquipStat::Id::STR, tier.value);
						stats.add_value(EquipStat::Id::DEX, tier.value);
						stats.add_value(EquipStat::Id::INT, tier.value);
						stats.add_value(EquipStat::Id::LUK, tier.value);
					}
					else if (key == "DAMAGE")
					{
						// 服务端 0x175 synced 后 damR/fdR 以 CombatPanelStats 为准，避免双计。
						if (!CombatPanelStats::get().synced)
							stats.add_damagepercent(tier.value / 100.0f);
					}
					else if (key == "BOSS")
					{
						if (!CombatPanelStats::get().synced)
							stats.add_bossdmg(tier.value / 100.0f);
					}
					else if (key == "IGNOREDEF")
					{
						if (!CombatPanelStats::get().synced)
							stats.add_ignoredef(tier.value / 100.0f);
					}
					// 基础百分比（按裸属性，服务端 0x176 tier 下发）：走 add_percent。
					else if (key == "HP_RATE")
					{
						stats.add_percent(EquipStat::Id::HP, tier.value / 100.0f);
					}
					else if (key == "MP_RATE")
					{
						stats.add_percent(EquipStat::Id::MP, tier.value / 100.0f);
					}
					else if (key == "STR_RATE")
					{
						stats.add_percent(EquipStat::Id::STR, tier.value / 100.0f);
					}
					else if (key == "DEX_RATE")
					{
						stats.add_percent(EquipStat::Id::DEX, tier.value / 100.0f);
					}
					else if (key == "INT_RATE")
					{
						stats.add_percent(EquipStat::Id::INT, tier.value / 100.0f);
					}
					else if (key == "LUK_RATE")
					{
						stats.add_percent(EquipStat::Id::LUK, tier.value / 100.0f);
					}
					else if (EquipStat::Id id = set_stat_id(key); id != EquipStat::Id::LENGTH)
					{
						stats.add_value(id, tier.value);
					}
				}
			}
		}
	}

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

	void Player::use_sandbox_fht(const FootholdTree* tree)
	{
		sandbox_fht = tree;
	}

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

		// ⑤ 套装加成注入（2026-11-12）：
		// 数据源 Data/SetItem.txt，按已穿戴 itemid 统计件数 → 达标档位累加。
		// 权威口径 E:\project\BeiDou-Server\...\SetItemManager.java 与
		// E:\MXD\扩展改动\套装系统\README.md；本端不移植 xy 的 combat.stat 框架
		//（s9 无 CombatStatProfile/SetBonusStatProvider），改为本地结算。
		// 唯一注入点：所有 recalc_stats 调用方（InventoryHandlers/SetField/PlayerHandlers/
		// CashShopParser/Player.cpp）自动继承，不在 Combat.cpp 里绕圈（伤害由服务端回包）。
		apply_set_item_bonuses(stats, inventory.equipped_item_ids());

		// ⑦ 095 强化加成（Round 15）：星之力 / 火花 / 宝珠。
		// 与套装同一注入点（close_totalstats 之前），6 个 recalc_stats 调用方自动继承。
		apply_enchant_bonuses(stats, inventory);

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

	void Player::sync_mount_look() const
	{
		// 骑乘 buff + 已装备坐骑 → 加载 TamingMob 帧；否则清空。
		if (has_buff(Buffstat::Id::MONSTER_RIDING))
		{
			int32_t mid = inventory.get_item_id(InventoryType::Id::EQUIPPED,
				static_cast<int16_t>(EquipSlot::Id::TAMEDMOB));
			if (mid > 0)
			{
				mount_look.set_itemid(mid);
				const bool moving = (state == Char::State::WALK || state == Char::State::FALL
					|| state == Char::State::SWIM);
				mount_look.set_moving(moving);
				return;
			}
		}
		mount_look.clear();
	}

	void Player::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		if (layer != get_layer())
			return;

		sync_mount_look();
		if (mount_look.is_valid())
		{
			Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
			mount_look.draw(absp, !facing_right, alpha);
		}
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

			sync_mount_look();
			if (mount_look.is_valid())
				mount_look.update();

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

		// 商城预览沙盒：本地井内位移，禁止上报 Move*（MinMS sandbox_fht）。
		if (!sandbox_fht)
		{
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
		}

		climb_cooldown.update();

		// 技能 CD 毫秒递减（供 has_cooldown / UICooldownList）。
		for (auto it = cooldowns.begin(); it != cooldowns.end();)
		{
			it->second -= Constants::TIMESTEP;
			if (it->second <= 0)
				it = cooldowns.erase(it);
			else
				++it;
		}

		// 瞬移本地节流递减（与技能 CD 同一时基，按 TIMESTEP=8ms/帧）。
		// 不依赖服务端 add_cooldown（use_move 路径从不下发），纯本地守门。
		if (teleport_throttle > 0)
		{
			teleport_throttle -= Constants::TIMESTEP;
			if (teleport_throttle < 0)
				teleport_throttle = 0;
		}

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

	void Player::die()
	{
		// HP 归零强制死亡：attacking 时不阻碍，先清攻击态再切 DIED。
		// 走 Char::set_state 直接落状态 + dead stance（Stance.cpp names[DIED]="dead"），
		// 不经过 Player::set_state 的 attacking 门禁，避免血0还站着放完攻击。
		if (state == Char::State::DIED)
			return;

		attacking = false;
		Char::set_state(Char::State::DIED);
	}

	bool Player::is_attacking() const
	{
		return attacking;
	}

	bool Player::can_cast_attack() const
	{
		return !attacking && state != Char::State::DIED && !is_sitting()
			&& look.get_equips().has_weapon();
	}

	bool Player::can_attack() const
	{
		return can_cast_attack() && !is_climbing();
	}

	SpecialMove::ForbidReason Player::can_use(const SpecialMove& move) const
	{
		// 死亡态禁止一切技能/攻击（含龙咆哮）：此前只拦 PRONE，血0死亡后仍能放技能。
		if (state == Char::State::DIED)
			return SpecialMove::ForbidReason::FBR_OTHER;

		if (move.is_skill() && state == Char::State::PRONE)
			return SpecialMove::ForbidReason::FBR_OTHER;

		if (move.is_attack() && (state == Char::State::LADDER || state == Char::State::ROPE))
			return SpecialMove::ForbidReason::FBR_OTHER;

		if (has_cooldown(move.get_id()))
			return SpecialMove::ForbidReason::FBR_COOLDOWN;

		// MinMS Player::can_use：二段跳仅空中且本段离地未用过；地上按键直接拒，
		// 避免仍发 USE_SKILL(91) 扣 MP / 被服务端拒包却本地零位移。
		{
			const int32_t sid = move.get_id();
			if (SkillId::is_flash_jump(sid)
				&& (phobj.onground || flash_jump_used))
				return SpecialMove::ForbidReason::FBR_OTHER;
		}

		int32_t level = skillbook.get_level(move.get_id());
		int32_t masterlevel = skillbook.get_masterlevel(move.get_id());
		if (masterlevel == 0)
			masterlevel = SkillData::get(move.get_id()).get_masterlevel();
		if (masterlevel > 0 && level > masterlevel)
			return SpecialMove::ForbidReason::FBR_OTHER;

		Weapon::Type weapon = get_weapontype();
		const Job& job = stats.get_job();
		int32_t hp = stats.get_stat(MapleStat::Id::HP);
		int32_t mp = stats.get_stat(MapleStat::Id::MP);
		uint16_t bullets = inventory.get_bulletcount();

		SpecialMove::ForbidReason reason = move.can_use(level, weapon, job, hp, mp, bullets);
		if (reason != SpecialMove::ForbidReason::FBR_NONE)
			return reason;

		// 龙咆哮(1311006)原生 HP 门禁（Angel CSkillInfo::CheckConsumeForActiveSkill@0x764256,
		// if(nSkillID==1311006) @0x7643de）：要求当前 HP > 最大HP × x/100 才可按。
		// 服务端权威：StatEffect.java:772 hpR=-x/100、:1439 hpchange += maxHP*hpR，
		// 即自损 = 最大HP×x/100；applyHpMpChange 在 nextHp<=0 且非GM时拒施（:8481）。
		// cdd=maxHP 已由服务端 getCurrentMaxHp() 坐实。
		// x 随技能等级线性递减，值来自 live Data/Skill/131.img skill/1311006/level/N/x
		// 全 30 级实测（orange-wz，2026-11-17）：x = 60 - level（1级59 … 30级30）。
		// 按等级取真实 x，避免用固定 59% 在高等级误拦本可释放的低血施放。
		if (move.get_id() == 1311006)
		{
			// 索引 0 留空，1..30 对应 WZ level 1..30 的 x 值（59..30）。
			static constexpr int32_t DRAGON_ROAR_X[31] = {
				0, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,
				49, 48, 47, 46, 45, 44, 43, 42, 41, 40,
				39, 38, 37, 36, 35, 34, 33, 32, 31, 30
			};
			int32_t idx = level < 1 ? 1 : (level > 30 ? 30 : level);
			int32_t gate = DRAGON_ROAR_X[idx];
			int32_t maxhp = stats.get_total(EquipStat::Id::HP);

			if (maxhp > 0 && hp <= static_cast<int32_t>(static_cast<int64_t>(maxhp) * gate / 100))
				return SpecialMove::ForbidReason::FBR_HPCOST;
		}

		return SpecialMove::ForbidReason::FBR_NONE;
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
		// 无视防御：装备来源以服务端 0x175 下发的官方连乘值为权威（属性驱动，无 synced 时回退本端累加）。
		// 技能自身的无视防御在 Skill.cpp 里另行 +=（近似加法，s9 无独立技能无视数据源）。
		if (CombatPanelStats::get().synced)
			attack.ignoredef = CombatPanelStats::get().ignore_pdr / 100.0f;
		else
			attack.ignoredef = stats.get_ignoredef();

		// 官方完整公式：魔法基础/暴击区间/BOSS 增伤/最终伤害统一生效，无配置开关。
		// 原生 MDamage 的 MAD 输入钳到 1999（IDA sub_791617）。
		// 魔法基础值沿用 (INT*4 + LUK)/100 * MAD，与物理 calculateMaxBaseDamage 的武器倍率 4.0 同构。
		int32_t mad = std::min<int32_t>(stats.get_total(EquipStat::Id::MAGIC), 1999);
		int32_t int_ = stats.get_total(EquipStat::Id::INT);
		int32_t luk = stats.get_total(EquipStat::Id::LUK);
		attack.magicdamage = (int_ * 4.0 + luk) / 100.0 * mad;

		// 官方暴击区间：系数 = 1 + (20~50 + 面板暴伤%)/100。
		// CharStats 的 mincrit/maxcrit 即暴伤%小数（v83 默认 0.5/0.75），故直接 1+x。
		// 下一行再叠加技能/潜能的 criticaldamage 增量（Skill.cpp 中处理）。
		attack.critical_min = 1.0f + stats.get_mincrit();
		attack.critical_max = 1.0f + stats.get_maxcrit();
		attack.bossdamage = stats.get_bossdmg();
		// 最终伤害：服务端 0x175 折算下发的连乘结果（属性驱动：无来源时服务端发 0）。
		attack.finaldamage = CombatPanelStats::get().final_damage_percent / 100.0f;

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

	void Player::apply_recoil(int32_t skill_level)
	{
		// MinMS Player.cpp:1552-1572 / Angel CUserLocal::TryDoingShootAttack@0x955537：
		// hx=±(250+40*(lv/4))、vy=-(250+20*(lv/4)) px/s，方向背离朝向；
		// WZ range 是攻击距离，不是后跃距离。服务端不产位移，纯本地表现。
		const double scale = static_cast<double>(Constants::TIMESTEP) / 1000.0;
		const int32_t q = (skill_level > 0 ? skill_level : 1) / 4;
		const double hx = (getflip() ? -1.0 : 1.0) * (250.0 + 40.0 * static_cast<double>(q));
		const double vy = -(250.0 + 20.0 * static_cast<double>(q));
		phobj.hspeed = hx * scale;
		phobj.vspeed = vy * scale;
		phobj.hforce = 0.0;
		phobj.vforce = 0.0;
		phobj.onground = false;
		phobj.fhid = 0;
		phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);
		Sound(Sound::Name::JUMP).play();
	}

	bool Player::apply_flash_jump(int32_t skillid, int32_t level)
	{
		// MinMS Combat::apply_use_movement :1691-1718 同源：
		// 只能在空中、本次离地未用过。WZ 只给 mpCon，位移是客户端冲量。
		// 服务端在 SPECIAL_MOVE 回包里权威结算 MP/位置，这里只给本地手感。
		if (phobj.onground || flash_jump_used)
			return false;

		const int32_t lv = level > 0 ? level : get_skills().get_level(skillid);
		const double lvval = static_cast<double>(lv < 1 ? 1 : lv);
		const double h = 3.2 + 0.12 * lvval;
		const double v = -(1.8 + 0.04 * lvval);

		phobj.hspeed = getflip() ? h : -h;
		if (phobj.vspeed > v)
			phobj.vspeed = v;

		// FALL 态并清 onground，让物理下一帧按新冲量继续飞行。
		if (state != Char::State::FALL)
			set_state(Char::State::FALL);
		phobj.onground = false;
		flash_jump_used = true;
		return true;
	}

	bool Player::apply_teleport(int32_t skillid, int32_t range)
	{
		// 对齐 Angel TryRegisterTeleport@0x957b74：120ms 本地节流 + foothold 落点吸附。
		// 单机端服务端 SPECIAL_MOVE 回包不保证到达，节流与落点校验都放本地，
		// 根治「没有间隔」「瞬移进不可站立区域卡地图」两个用户可见问题。
		(void)skillid;

		// 本地节流门禁：120ms 内只允许一次，重复施放直接丢弃（返回 false）。
		if (teleport_throttle > 0)
		{
			LOG(LOG_DEBUG, "[Player] apply_teleport throttled remain=" << teleport_throttle);
			return false;
		}

		// 方向键优先，否则沿用朝向（原生瞬时位移无滑行，靠落点吸附定位）。
		int16_t hdir = (is_key_down(KeyAction::Id::RIGHT) ? 1 : 0)
			- (is_key_down(KeyAction::Id::LEFT) ? 1 : 0);
		int16_t vdir = (is_key_down(KeyAction::Id::DOWN) ? 1 : 0)
			- (is_key_down(KeyAction::Id::UP) ? 1 : 0);

		if (hdir == 0 && vdir == 0)
			hdir = getflip() ? 1 : -1;

		int16_t dist = range > 0 ? range : 130;
		if (dist > 400)
			dist = 400;

		phobj.hspeed = 0.0;
		phobj.hforce = 0.0;

		bool moved = false;

		if (hdir != 0)
		{
			// 取当前地图 foothold 树（商城预览优先沙盒），校验目标点下方是否有可站立平台。
			const FootholdTree* fht = sandbox_fht;
			if (!fht)
				fht = &Stage::get().get_physics().get_fht();

			const int16_t from_x = phobj.get_x();
			const int16_t feet_y = phobj.get_y();
			const Range<int16_t> walls = fht->get_walls();

			// 按 1/4 递进回溯：满距离 → 3/4 → 1/2 → 1/4，取第一个有落脚点的 x。
			// resolve_spawn_point(x, 0) 内部走 get_fhid_at_x：只要 x 水平覆盖任一
			// 非墙 foothold 就回其 fhid，否则回 0 —— 判据只依赖 x 覆盖，稳定且
			// 不受 spawn 包 y/cy 偏差影响；这里仅用其 fhid 做「有无落脚」判定。
			bool landed = false;
			int16_t landed_x = from_x;
			for (int step = 4; step >= 1; --step)
			{
				int32_t cand = static_cast<int32_t>(from_x)
					+ hdir * static_cast<int32_t>(dist * step / 4);

				// 先夹到 int16 可表示范围，避免转型回绕。
				if (cand < -32768) cand = -32768;
				if (cand > 32767) cand = 32767;
				int16_t cx = static_cast<int16_t>(cand);

				// 地图边界兜底：即便 foothold 查询异常也不越出可走范围。
				if (!walls.empty() && !walls.contains(cx))
					continue;

				SpawnPoint probe = fht->resolve_spawn_point(Point<int16_t>(cx, feet_y), 0);
				if (probe.fhid != 0)
				{
					landed = true;
					landed_x = cx;
					break;
				}
			}

			if (landed)
			{
				// 落点合法：吸附到 foothold 覆盖的 x，onground=false 交物理自然落地。
				phobj.set_x(landed_x);
				phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);
				moved = true;
				LOG(LOG_DEBUG, "[Player] apply_teleport land x=" << landed_x
					<< " from=" << from_x << " dist=" << dist);
			}
			else
			{
				// 全不合法：保持原位与原 onground/state，避免卡在中间态。
				LOG(LOG_DEBUG, "[Player] apply_teleport no valid foothold, keep x=" << from_x);
				return true;
			}
		}
		else if (vdir != 0)
		{
			// 垂直瞬移不改变 x，无落点问题，沿用短冲量（最终由服务端回包校准）。
			phobj.vforce += vdir * (dist / 20.0);
			moved = true;
		}

		if (moved)
		{
			phobj.onground = false;
			if (state != Char::State::FALL)
				set_state(Char::State::FALL);

			// 瞬移光圈：MinMS Combat.cpp:1783-1786（BasicEff.img/Teleport，
			// 受 SysOptSkillEffects 开关约束）。D3 已实测 live WZ 有该节点。
			static Animation tp_effect;
			static bool tp_loaded = false;
			static bool tp_ok = false;
			if (!tp_loaded)
			{
				nl::node tp = nl::nx::Effect["BasicEff.img"]["Teleport"];
				if (tp && tp.size() > 0)
				{
					tp_effect = Animation(tp);
					tp_ok = true;
				}
				tp_loaded = true;
			}
			if (tp_ok)
				show_attack_effect(tp_effect, 0);

			// 对齐 Angel 120ms：仅成功位移后置位，下一次施放需等节流走完。
			teleport_throttle = 120;
		}

		return true;
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

		// 视觉开关（纯表现层）：分身残影随 SHADOWPARTNER 生效。
		if (buff.stat == Buffstat::Id::SHADOWPARTNER)
			set_shadowpartner(true);

		// 护盾特效（MAGIC_GUARD/HOLY_SHIELD）：按技能 id 加载环绕动画。
		if (buff.stat == Buffstat::Id::MAGIC_GUARD || buff.stat == Buffstat::Id::HOLY_SHIELD)
			set_shield(static_cast<int>(buff.stat), buff.skillid);
	}

	void Player::cancel_buff(Buffstat::Id stat)
	{
		buffs[stat] = {};

		if (stat == Buffstat::Id::SHADOWPARTNER)
			set_shadowpartner(false);

		if (stat == Buffstat::Id::MAGIC_GUARD || stat == Buffstat::Id::HOLY_SHIELD)
			clear_shield(static_cast<int>(stat));
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
		// s9/Angel：cooltime 为秒；本地存毫秒并在 update 递减（对齐 MinMS）。
		if (cooltime <= 0)
		{
			cooldowns.erase(skill_id);
			return;
		}
		cooldowns[skill_id] = cooltime * 1000;
	}

	bool Player::has_cooldown(int32_t skill_id) const
	{
		auto iter = cooldowns.find(skill_id);

		if (iter == cooldowns.end())
			return false;

		return iter->second > 0;
	}

	int32_t Player::get_cooldown_remaining(int32_t skill_id) const
	{
		auto iter = cooldowns.find(skill_id);
		return iter == cooldowns.end() ? 0 : iter->second;
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