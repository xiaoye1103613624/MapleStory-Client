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
#include "Mob.h"

#include "../../Util/Misc.h"
#include "../../Configuration.h"

#include "../../Net/Packets/GameplayPackets.h"

#include <algorithm>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Mob::Mob(int32_t oi, int32_t mid, int8_t mode, int8_t st, uint16_t fh, bool newspawn, int8_t tm, Point<int16_t> position,
		int8_t spawn_effect, bool parent, int32_t parent_oid) : MapObject(oi)
	{
		std::string strid = string_format::extend_id(mid, 7);
		nl::node src = nl::nx::Mob[strid + ".img"];

		nl::node info = src["info"];

		level = info["level"];
		watk = info["PADamage"];
		matk = info["MADamage"];
		wdef = info["PDDamage"];
		mdef = info["MDDamage"];
		accuracy = info["acc"];
		avoid = info["eva"];
		knockback = info["pushed"];
		speed = info["speed"];
		flyspeed = info["flySpeed"];
		touchdamage = info["bodyAttack"].get_bool();
		undead = info["undead"].get_bool();
		boss = info["boss"].get_bool();
		noflip = info["noFlip"].get_bool();
		notattack = info["notAttack"].get_bool();
		canjump = src["jump"].size() > 0;
		canfly = src["fly"].size() > 0;
		canmove = src["move"].size() > 0 || canfly;
		
		std::string linkid = info["link"];
		nl::node link_src = nl::nx::Mob[linkid + ".img"];
		nl::node link = link_src ? link_src : src;
		nl::node fly = link["fly"];

		if (canfly)
		{
			animations[Stance::STAND] = fly;
			animations[Stance::MOVE] = fly;
		}
		else
		{
			animations[Stance::STAND] = link["stand"];
			animations[Stance::MOVE] = link["move"];
		}

		animations[Stance::JUMP] = link["jump"];
		// 挨打/死亡帧并不总是叫 hit1/die1：抽样 2053 个 img 里 die1 只有 1608 个，
		// 另有 die(4)、die2(11)、dieF(9)；hit1 缺 476 个，其中有 hit(7)。
		// 空节点只会构造出「一张空白帧」的 Animation，update() 当场返回结束，
		// 表现就是怪物挨打毫无反馈、死亡时瞬间消失。
		nl::node hit = link["hit1"];

		if (hit.size() == 0)
			hit = link["hit"];

		animations[Stance::HIT] = hit;

		nl::node die = link["die1"];

		if (die.size() == 0)
		{
			for (auto& alt : { "die", "die2", "dieF" })
			{
				if (die.size() > 0)
					break;

				die = link[alt];
			}
		}

		animations[Stance::DIE] = die;

		// 攻击/技能动作帧：状态字节按段基址 + 2*下标入表（下标即服务端 castPos）。
		// 抽样 Mob.nx 2053 个 img：attack1 有 897 个、attack2 375、attack3 183，最深到 attack8；
		// skill1 460、skill2 137、skill3 53，最深 skill9。修前这些帧一张都没加载，
		// 怪物从头到尾只会站/走/跳/挨打/死，攻击动作整轮缺失。
		for (int32_t i = 0; i < ATTACK_SLOTS; i++)
		{
			nl::node attack = link["attack" + std::to_string(i + 1)];

			if (attack.size() > 0)
				animations[static_cast<Stance>(Stance::ATTACK + 2 * i)] = attack;
		}

		for (int32_t i = 0; i < SKILL_SLOTS; i++)
		{
			nl::node skill = link["skill" + std::to_string(i + 1)];

			if (skill.size() > 0)
				animations[static_cast<Stance>(Stance::SKILL + 2 * i)] = skill;
		}

		name = nl::nx::String["Mob.img"][std::to_string(mid)]["name"].get_string();

		nl::node sndsrc = nl::nx::Sound["Mob.img"][strid];

		hitsound = sndsrc["Damage"];
		diesound = sndsrc["Die"];

		speed += 100;
		speed *= 0.001f;

		flyspeed += 100;
		flyspeed *= 0.0005f;

		if (canfly)
			phobj.type = PhysicsObject::Type::FLYING;

		id = mid;
		team = tm;
		// spawn 尾段（此前这些字节被错位读掉/从没传进来）：
		// effect>0 出生特效、-3 父怪死后才出、parent 关系。波 2 播特效、
		// 波 2 接 delayed 雪藏逻辑；此处先正确存储，杜绝未初始化读。
		effect = spawn_effect;
		has_parent = parent;
		parentoid = parent_oid;
		set_position(position);
		set_control(mode);
		phobj.fhid = fh;
		phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);

		hppercent = 0;
		dying = false;
		dead = false;
		fading = false;
		skill_pending = false;
		next_skill_id = 0;
		next_skill_level = 0;
		// effect == -3（父怪在场但尚未死亡）⇒ 雪藏：不活动、不画，父怪死亡时唤醒。
		dormant = (parent && spawn_effect == -3);
		set_stance(st);
		flydirection = STRAIGHT;
		counter = 0;

		namelabel = Text(Text::Font::A13M, Text::Alignment::CENTER, Color::Name::WHITE, Text::Background::NAMETAG, name);

		if (newspawn)
		{
			fadein = true;
			opacity.set(0.0f);
		}
		else
		{
			fadein = false;
			opacity.set(1.0f);
		}

		// 雪藏怪从构造起就是非活动（MapObjects::draw 只画 active 对象，
		// update 只跑 active），父怪死亡 wake() 时才按原生淡入出现。
		if (dormant)
		{
			deactivate();
			opacity.set(0.0f);
		}

		if (control && stance == Stance::STAND)
			next_move();
	}

	void Mob::set_stance(uint8_t stancebyte)
	{
		flip = (stancebyte % 2) == 0;

		if (!flip)
			stancebyte -= 1;

		if (stancebyte < Stance::MOVE)
			stancebyte = Stance::MOVE;

		// animations 是 map，set_stance/draw/update/get_head_position 全走 .at()：
		// 状态字节只要没有对应帧（12..23、攻击段越界、60+ 这些本包拿不到的区段），
		// out_of_range 就逃过 PacketSwitch 的 PacketError 捕获直接把客户端 terminate。
		// 所以按「有没有加载到这一帧」决定能不能进这个姿态，否则退回移动/站立。
		if (animations.find(static_cast<Stance>(stancebyte)) == animations.end())
			stancebyte = animations.find(Stance::MOVE) != animations.end() ? Stance::MOVE : Stance::STAND;

		set_stance(static_cast<Stance>(stancebyte));
	}

	void Mob::set_stance(Stance newstance)
	{
		if (stance != newstance)
		{
			stance = newstance;

			animations.at(stance).reset();
		}
	}

	int8_t Mob::update(const Physics& physics)
	{
		if (!active)
			return phobj.fhlayer;

		bool aniend = animations.at(stance).update();

		if (aniend && stance == Stance::DIE)
			dead = true;

		if (fading)
		{
			opacity -= 0.025f;

			if (opacity.last() < 0.025f)
			{
				opacity.set(0.0f);
				fading = false;
				dead = true;
			}
		}
		else if (fadein)
		{
			opacity += 0.025f;

			if (opacity.last() > 0.975f)
			{
				opacity.set(1.0f);
				fadein = false;
			}
		}

		if (dead)
		{
			deactivate();

			return -1;
		}

		effects.update();
		showhp.update();

		if (!dying)
		{
			if (!canfly)
			{
				if (phobj.is_flag_not_set(PhysicsObject::Flag::TURNATEDGES))
				{
					flip = !flip;
					phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);

					if (stance == Stance::HIT)
						set_stance(Stance::STAND);
				}
			}

			switch (stance)
			{
			case Stance::MOVE:
				if (canfly)
				{
					phobj.hforce = flip ? flyspeed : -flyspeed;

					switch (flydirection)
					{
					case FlyDirection::UPWARDS:
						phobj.vforce = -flyspeed;
						break;
					case FlyDirection::DOWNWARDS:
						phobj.vforce = flyspeed;
						break;
					}
				}
				else
				{
					phobj.hforce = flip ? speed : -speed;
				}

				break;
			case Stance::HIT:
				if (canmove)
				{
					double KBFORCE = phobj.onground ? 0.2 : 0.1;
					phobj.hforce = flip ? -KBFORCE : KBFORCE;
				}

				break;
			case Stance::JUMP:
				phobj.vforce = -5.0;
				break;
			}

			physics.move_object(phobj);

			if (control)
			{
				counter++;

				bool next;

				switch (stance)
				{
				case Stance::HIT:
					next = counter > 200;
					break;
				case Stance::JUMP:
					next = phobj.onground;
					break;
				default:
					next = aniend && counter > 200;
					break;
				}

				if (next)
				{
					next_move();
					update_movement();
					counter = 0;
				}
			}
		}
		else
		{
			phobj.normalize();
			physics.get_fht().update_fh(phobj);
		}

		return phobj.fhlayer;
	}

	void Mob::next_move()
	{
		if (canmove)
		{
			// 出招优先级一：服务端 MOVE_MONSTER_RESPONSE 授权过的下一技能
			// （MoveLifeHandler.java:150 只在 canUseSkill 判定通过后携带 skillId/level）。
			// 控制器怪下一次动作上报 SKILL 段，动画槽随机取一个已加载的 skill{n}：
			// 服务端校验的是技能 id（byte 段），animation 槽只是表现。
			if (control && skill_pending)
			{
				std::vector<Stance> slots;
				for (auto& kv : animations)
					if (kv.first >= Stance::SKILL)
						slots.push_back(kv.first);

				if (!slots.empty())
				{
					set_stance(slots[randomizer.next_int<size_t>(slots.size())]);
					return; // skill_pending 在 update_movement 上报后清除
				}

				skill_pending = false;
			}

			// 出招优先级二：本地随机 attack（v83 控制器职责；服务端 canUseAttack
			// 冷却不过时会把 rawActivity 打成 -1 回放，表现为怪只走不打，安全）。
			// notAttack 怪（info.notAttack）与被动态（HIT/JUMP 中）不出招。
			// 【2026-11-03 构建修复】above() 是 Randomizer 成员，前序未提交代码
			// 漏了 randomizer. 前缀 → C3861 找不到标识符，阻塞全工程构建。
			if (control && !notattack && randomizer.above(0.82f))
			{
				std::vector<Stance> slots;
				for (auto& kv : animations)
					if (kv.first >= Stance::ATTACK && kv.first < Stance::SKILL)
						slots.push_back(kv.first);

				if (!slots.empty())
				{
					set_stance(slots[randomizer.next_int<size_t>(slots.size())]);
					return;
				}
			}

			// 攻击/技能是临时动作段，不是循环状态：播完必须回基础姿态。
			// 否则下面的 switch 匹配不到任何 case，控制器会把同一个攻击状态字节
			// 一遍遍回给服务端（服务端 canUseAttack 冷却不通过时又会把 rawActivity 打成 -1）。
			if (stance >= Stance::ATTACK)
			{
				set_stance(Stance::STAND);
				return;
			}

			switch (stance)
			{
			case Stance::HIT:
			case Stance::STAND:
				set_stance(Stance::MOVE);
				flip = randomizer.next_bool();
				break;
			case Stance::MOVE:
			case Stance::JUMP:
				if (canjump && phobj.onground && randomizer.below(0.25f))
				{
					set_stance(Stance::JUMP);
				}
				else
				{
					switch (randomizer.next_int(3))
					{
					case 0:
						set_stance(Stance::STAND);
						break;
					case 1:
						set_stance(Stance::MOVE);
						flip = false;
						break;
					case 2:
						set_stance(Stance::MOVE);
						flip = true;
						break;
					}
				}

				break;
			}

			if (stance == Stance::MOVE && canfly)
				flydirection = randomizer.next_enum(FlyDirection::NUM_DIRECTIONS);
		}
		else
		{
			set_stance(Stance::STAND);
		}
	}

	void Mob::update_movement()
	{
		// 上行 activity（参数名对应经字节仿真核对，s9 MoveLifeHandler.java:69-78：
		// skillb→pNibbles、skill0→rawActivity、skill1→skillId、skill2→skillLv、
		// skill3/4→pOption 区）。修前全 0：服务端把 0 当非法动作（canUseAttack
		// castPos 越界 → rawActivity 打成 -1），受控怪永远只走不打。
		// 现在出招时上报对应段字节（24..41 attack / 42..59 skill）；技能段同时
		// 带回服务端 MOVE_MONSTER_RESPONSE 授权过的 skillId/level（:87-95 据此结算）。
		uint8_t activity = value_of(stance, flip);

		int8_t skillid = 0;
		int8_t skilllv = 0;

		if (stance >= Stance::SKILL)
		{
			skillid = next_skill_id;
			skilllv = next_skill_level;
			skill_pending = false;
		}

		MoveMobPacket(
			oid, 1, 0, static_cast<int8_t>(activity), skillid, skilllv, 0, 0,
			get_position(),
			Movement(phobj, activity)
		).dispatch();
	}

	void Mob::set_next_skill(bool use, int8_t id, int8_t level)
	{
		// 无技能段的怪（animations 里没有 skill{n} 帧）不做画饼承诺——上报 42 段
		// 却没有动画会在 set_stance 被钳回 move，白白浪费一次节拍。
		bool hasskill = false;

		for (auto& kv : animations)
		{
			if (kv.first >= Stance::SKILL)
			{
				hasskill = true;
				break;
			}
		}

		skill_pending = use && id != 0 && hasskill;
		next_skill_id = skill_pending ? id : 0;
		next_skill_level = skill_pending ? level : 0;
	}

	bool Mob::parented() const
	{
		return has_parent;
	}

	int32_t Mob::get_parent_oid() const
	{
		return parentoid;
	}

	bool Mob::is_dormant() const
	{
		return dormant;
	}

	void Mob::wake()
	{
		if (!dormant)
			return;

		dormant = false;
		fadein = true;
		opacity.set(0.0f);
		makeactive();
	}

	namespace
	{
		////////////////////////////////////////////////////////////////////////////////
		// 「系统设置 · 怪物情报」四档 → 头顶名字 / HP 条两个独立开关（B4-6）
		//
		// 档位与顺序取自原生汉化表 ijl15.dll（BeiDou-ClientV17.7）@0x293ec 起四条以 \0
		// 紧邻分隔：不显示 / 只显示名字 / 只显示HP / 名字和HP都显示。
		//
		// 序号自 0 起：原生截帧（正确客户端截图/系统设置.png）里该下拉收起态文案是
		// 「名字和HP都显示」，同一台机器注册表 soMobInfo = 3 ⇒ 档 3 = 名字 + HP 条，
		// 且**一共四档**（0..3），不是先前按"三档"规划的那份。
		//
		// 先前「原生 exe 里 UI 文案全是密文、静态逆向已证伪」的结论作废：那查的是
		// 095 的 MapleStory.exe，而汉化文案根本不在主 exe 里，在汉化壳 ijl15.dll 里。
		////////////////////////////////////////////////////////////////////////////////
		struct MobInfoTier
		{
			bool name;
			bool hp;
		};

		constexpr MobInfoTier MOB_INFO_TIER[4] = {
			{ false, false },	// 0：不显示
			{ true,  false },	// 1：只显示名字
			{ false, true },	// 2：只显示HP
			{ true,  true }		// 3：名字和HP都显示
		};

		MobInfoTier mob_info_tier()
		{
			const uint8_t ordinal = Setting<SysOptMobInfo>::get().load();

			return MOB_INFO_TIER[ordinal < 4 ? ordinal : 3];
		}
	}

	void Mob::draw(double viewx, double viewy, float alpha) const
	{
		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);
		Point<int16_t> headpos = get_head_position(absp);

		effects.drawbelow(absp, alpha);

		if (!dead)
		{
			float interopc = opacity.get(alpha);

			animations.at(stance).draw(DrawArgument(absp, flip && !noflip, interopc), alpha);

			// 名字与 HP 条是两个独立门禁（旧实现把两者一起塞进 showhp，于是
			// 「怪物情报」无论选哪档都只在受击后 2 秒里出名字 —— 原生是常显）。
			const MobInfoTier tier = mob_info_tier();

			if (tier.name)
				namelabel.draw(absp);

			if (tier.hp && showhp && !dying && hppercent > 0)
				hpbar.draw(headpos, hppercent);
		}

		effects.drawabove(absp, alpha);
	}

	void Mob::set_control(int8_t mode)
	{
		control = mode > 0;
		aggro = mode == 2;
	}

	void Mob::refresh(int8_t mode, int8_t stancebyte, uint16_t fh, int8_t tm, bool newspawn, Point<int16_t> position,
		int8_t spawn_effect, bool parent, int32_t parent_oid)
	{
		if (mode > 0)
			set_control(mode);

		team = tm;
		effect = spawn_effect;
		has_parent = parent;
		parentoid = parent_oid;
		set_position(position);
		phobj.fhid = fh;

		// 重下发意味着服务端把它当新对象，之前的死亡/淡出流程要整体作废
		dying = false;
		dead = false;
		fading = false;
		skill_pending = false;
		// 服务端补发整包 spawn ⇒ 不再雪藏（delayed 怪被重新下发的唯一时机
		// 就是它该出现的时候）；只有新包仍带 -3 才继续雪藏。
		dormant = (parent && spawn_effect == -3);
		fadein = newspawn;
		opacity.set(newspawn ? 0.0f : 1.0f);

		set_stance(stancebyte);

		// 仍雪藏的怪不能被 makeactive 误唤醒；父怪死亡走 wake()。
		if (dormant)
		{
			deactivate();
			opacity.set(0.0f);
		}
		else
		{
			makeactive();
		}

		if (control && stance == Stance::STAND)
			next_move();
	}

	void Mob::send_movement(Point<int16_t> start, std::vector<Movement>&& in_movements)
	{
		if (control)
			return;

		set_position(start);

		movements = std::forward<decltype(in_movements)>(in_movements);

		if (movements.empty())
			return;

		const Movement& lastmove = movements.front();

		uint8_t laststance = lastmove.newstate;
		set_stance(laststance);

		phobj.fhid = lastmove.fh;
	}

	Point<int16_t> Mob::get_head_position(Point<int16_t> position) const
	{
		Point<int16_t> head = animations.at(stance).get_head();

		position.shift_x((flip && !noflip) ? -head.x() : head.x());
		position.shift_y(head.y());

		return position;
	}

	void Mob::kill(int8_t animation)
	{
		switch (animation)
		{
		case 0:
			deactivate();
			break;
		case 1:
			dying = true;

			apply_death();
			break;
		case 2:
			fading = true;
			dying = true;
			break;
		}
	}

	void Mob::show_hp(int8_t percent, uint16_t playerlevel)
	{
		if (hppercent == 0)
		{
			int16_t delta = playerlevel - level;

			if (delta > 9)
				namelabel.change_color(Color::Name::YELLOW);
			else if (delta < -9)
				namelabel.change_color(Color::Name::RED);
		}

		if (percent > 100)
			percent = 100;
		else if (percent < 0)
			percent = 0;

		hppercent = percent;
		showhp.set_for(2000);
	}

	void Mob::show_effect(const Animation& animation, int8_t pos, int8_t z, bool f)
	{
		if (!active)
			return;

		// 「系统设置 · 画面效果 · 技能使用时允许画面效果」（B4-3）的技能命中侧出口；
		// 施放侧在 Character/Char.cpp 的 show_attack_effect。两处同一开关。
		if (!Setting<SysOptSkillEffects>::get().load())
			return;

		Point<int16_t> shift;

		switch (pos)
		{
		case 0:
			shift = get_head_position(Point<int16_t>());
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		}

		effects.add(animation, DrawArgument(shift, f), z);
	}

	float Mob::calculate_hitchance(int16_t leveldelta, int32_t player_accuracy) const
	{
		float faccuracy = static_cast<float>(player_accuracy);
		float hitchance = faccuracy / (((1.84f + 0.07f * leveldelta) * avoid) + 1.0f);

		if (hitchance < 0.01f)
			hitchance = 0.01f;

		return hitchance;
	}

	double Mob::calculate_mindamage(int16_t leveldelta, double damage, bool magic) const
	{
		double mindamage =
			magic ?
			damage - (1 + 0.01 * leveldelta) * mdef * 0.6 :
			damage * (1 - 0.01 * leveldelta) - wdef * 0.6;

		return mindamage < 1.0 ? 1.0 : mindamage;
	}

	double Mob::calculate_maxdamage(int16_t leveldelta, double damage, bool magic) const
	{
		double maxdamage =
			magic ?
			damage - (1 + 0.01 * leveldelta) * mdef * 0.5 :
			damage * (1 - 0.01 * leveldelta) - wdef * 0.5;

		return maxdamage < 1.0 ? 1.0 : maxdamage;
	}

	damage::DamageLines Mob::calculate_damage(const Attack& attack)
	{
		// damage.formula_v2 = false 时完全保持旧 Fyms085 公式与数值，便于一键回滚。
		const bool v2 = Configuration::get().get_damage_formula_v2();

		double mindamage = 1.0;
		double maxdamage = 1.0;
		float hitchance = 1.0f;
		float critical = 0.0f;
		int16_t leveldelta = level - attack.playerlevel;

		if (leveldelta < 0)
			leveldelta = 0;

		const bool magic = attack.damagetype == Attack::DamageType::DMG_MAGIC;

		switch (attack.damagetype)
		{
		case Attack::DamageType::DMG_WEAPON:
		case Attack::DamageType::DMG_MAGIC:
			mindamage = calculate_mindamage(leveldelta, attack.mindamage, magic);
			maxdamage = calculate_maxdamage(leveldelta, attack.maxdamage, magic);
			hitchance = calculate_hitchance(leveldelta, attack.accuracy);
			critical = attack.critical;
			break;
		case Attack::DamageType::DMG_FIXED:
			mindamage = attack.fixdamage;
			maxdamage = attack.fixdamage;
			hitchance = 1.0f;
			critical = 0.0f;
			break;
		}

		if (v2)
		{
			// 无视防御：把防御减免中被忽略的比例加回。原生 CalcDamage 的 PDD/MDD
			// 分支见 notes/r001，这里按 0..1 比例线性还原，避免改动防御函数的既有语义。
			if (attack.ignoredef > 0.0f)
			{
				float ignored = std::min<float>(std::max<float>(attack.ignoredef, 0.0f), 1.0f);
				double defmin = magic
					? (1 + 0.01 * leveldelta) * mdef * 0.6
					: wdef * 0.6;
				double defmax = magic
					? (1 + 0.01 * leveldelta) * mdef * 0.5
					: wdef * 0.5;

				mindamage += defmin * ignored;
				maxdamage += defmax * ignored;
			}

			// BOSS 增伤：WZ info/boss 标记（构造时读取）。
			if (boss && attack.bossdamage > 0.0f)
			{
				double factor = 1.0 + attack.bossdamage;
				mindamage *= factor;
				maxdamage *= factor;
			}
		}

		if (mindamage < 1.0)
			mindamage = 1.0;

		if (maxdamage < mindamage)
			maxdamage = mindamage;

		// 暴击倍率区间：CharStats 的 mincrit/maxcrit 是「额外倍率」，故 1+x。
		// 开关关闭时固定 ×1.5，与改造前一致。
		float critical_min = v2 ? attack.critical_min : 1.5f;
		float critical_max = v2 ? attack.critical_max : 1.5f;

		damage::DamageLines result(attack.hitcount);

		std::generate(
			result.begin(), result.end(),
			[&]()
			{
				return next_damage(mindamage, maxdamage, hitchance, critical,
					critical_min, critical_max);
			}
		);

		update_movement();

		return result;
	}

	damage::DamageLine Mob::next_damage(double mindamage, double maxdamage, float hitchance,
		float critical, float critical_min, float critical_max) const
	{
		bool hit = randomizer.below(hitchance);

		if (!hit)
			return damage::DamageLine(0, false);

		double damage = randomizer.next_real(mindamage, maxdamage);
		bool iscritical = randomizer.below(critical);

		if (iscritical)
		{
			float low = std::min<float>(critical_min, critical_max);
			float high = std::max<float>(critical_min, critical_max);

			if (low < 1.0f)
				low = 1.0f;

			if (high < low)
				high = low;

			damage *= (low == high) ? low : randomizer.next_real(low, high);
		}

		if (damage < 1.0)
			damage = 1.0;

		// 唯一 clamp 点：2^53 精度上限 ∩ 配置 cap ∩ 服务端 0x3730 破功值。
		// 原生端的 199999.0 硬封顶（IDA 0xAFE8A0）按用户决议移除。
		int64_t cap = damage::effective_cap(Configuration::get().get_damage_cap());
		int64_t intdamage = damage::clamp_damage(damage, cap);

		return damage::DamageLine(intdamage, iscritical);
	}

	void Mob::apply_damage(int32_t damage, bool toleft)
	{
		hitsound.play();

		if (dying && stance != Stance::DIE)
		{
			apply_death();
		}
		else if (control && is_alive() && damage >= knockback)
		{
			flip = toleft;
			counter = 170;
			set_stance(Stance::HIT);

			update_movement();
		}
	}

	MobAttack Mob::create_touch_attack() const
	{
		if (!touchdamage)
			return MobAttack();

		int32_t minattack = static_cast<int32_t>(watk * 0.8f);
		int32_t maxattack = watk;
		int32_t attack = randomizer.next_int(minattack, maxattack);

		return MobAttack(attack, get_position(), id, oid);
	}

	void Mob::apply_death()
	{
		set_stance(Stance::DIE);
		diesound.play();
		dying = true;
	}

	bool Mob::is_alive() const
	{
		return active && !dying;
	}

	bool Mob::is_in_range(const Rectangle<int16_t>& range) const
	{
		if (!active)
			return false;

		Rectangle<int16_t> bounds = animations.at(stance).get_bounds();
		bounds.shift(get_position());

		return range.overlaps(bounds);
	}

	Point<int16_t> Mob::get_head_position() const
	{
		Point<int16_t> position = get_position();

		return get_head_position(position);
	}
}