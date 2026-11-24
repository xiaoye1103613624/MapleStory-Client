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

#include "../Stage.h"
#include "../../Util/Misc.h"
#include "../../Configuration.h"

#include "../../Net/Packets/GameplayPackets.h"

#include <algorithm>
#include <cctype>

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
		{
			int32_t raw_maxhp = static_cast<int32_t>(info["maxHP"].get_integer());
			maxhp = raw_maxhp > 0 ? raw_maxhp : 1;
			hp_est = maxhp;
		}
		speed = info["speed"];
		flyspeed = info["flySpeed"];
		touchdamage = info["bodyAttack"].get_bool();
		undead = info["undead"].get_bool();
		// 四源对照 A1：怪物元素抗性/弱点表（WZ info/elemAttr，如 "F3"/"I2"/"L1"）。
		elemattr = info["elemAttr"].get_string();
		boss = info["boss"].get_bool();
		noflip = info["noFlip"].get_bool();
		notattack = info["notAttack"].get_bool();
		// P-A/P-E：WZ info 属性读取（缺失即默认，维系 R6 精简 img 兼容）。
		noRevive_ = info["noRevive"].get_bool();
		regena_ = info["regena"].get_string();
		hpgauguehide_ = info["HPgaugeHide"].get_bool();
		int32_t hpc = static_cast<int32_t>(info["hpTagColor"].get_integer());
		hptagcolor_ = (hpc != 0) ? Color(static_cast<uint32_t>(hpc)) : Color::Name::LIGHTGREEN;
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

		// 死亡动画：按 die1..die7 顺序加载所有存在的死亡帧（OnDie 的
		// rand()%v6+9 随机选 variant）。精确「action 偏移」待 REA 反编译
		// PrepareActionLayer 精修；当前降级为「按 WZ 实际存在的 die 帧随机枚举」。
		death_stances.clear();

		for (int32_t i = 0; i < 7; i++)
		{
			nl::node d = link["die" + std::to_string(i + 1)];

			if (d.size() > 0)
			{
				Stance ds = static_cast<Stance>(Stance::DIE + 2 * i);
				animations[ds] = d;
				death_stances.push_back(ds);
			}
		}

		// 没有任何 dieN 帧（极个别 img）：回退到 die / dieF 单帧。
		if (death_stances.empty())
		{
			for (auto& alt : { "die", "dieF" })
			{
				nl::node d = link[alt];

				if (d.size() > 0)
				{
					animations[Stance::DIE] = d;
					death_stances.push_back(Stance::DIE);
					break;
				}
			}
		}

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

		// 等级名牌：真实等级来自本构造开头读取的 WZ info/level（Mob.img/<id>/info/level），
		// 纯代码动态拼「Lv:N」，与名称同构（同字体/同 NAMETAG 底框）；底部与名称水平并排。
		levellabel = Text(Text::Font::A13M, Text::Alignment::CENTER, Color::Name::WHITE, Text::Background::NAMETAG,
			"Lv:" + std::to_string(level));

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
			generate_move_path();

		// P-B 出生特效：spawn_effect>0 播 Mob/SpecialEff.img 对应节点（出生时一次性）。
		// 节点缺失则跳过（R6）；dormant 怪（effect==-3 雪藏）尚未出现，不播。
		if (effect > 0 && !dormant)
		{
			nl::node effnode = nl::nx::Mob["SpecialEff.img"][std::to_string(effect)];
			if (effnode)
				show_effect(Animation(effnode), 0, 0, flip);
		}
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
		{
			// 回收由 MapMobs 显式驱动（对齐 Angel CMobPool::OnMobLeaveField 的
			// ZMap::RemoveKey），不再让 update() 返回 -1 成为唯一删除路径。停用
			// 对象留在当前图层，等 MapMobs 的待回收计时到时 mobs.remove(oid)。
			return phobj.fhlayer;
		}

		bool aniend = animations.at(stance).update();

		if (aniend && is_death_stance(stance))
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
			// 死亡表现已结束：停止绘制/逻辑，但不直接删对象——回收交 MapMobs
			// 显式完成（避免「update 返回 -1」与「池层显式 remove」双路径竞态）。
			deactivate();

			return phobj.fhlayer;
		}

		// 死亡兜底计时已迁移到 MapMobs（removal 队列的 MOB_REMOVAL_FRAMES），
		// 由池层显式回收，避免怪永久残留。此处不再持有兜底逻辑。

		effects.update();
		showhp.update();

		if (!dying)
		{
			if (control)
			{
				// 控制怪：本地动作驱动（generate_move_path = Angel GenerateMovePath
				// 适配移植：只选 STAND/MOVE/JUMP + 服务端驱动 attack/skill；位移交给
				// foothold 物理并用 update_movement 上报）。
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
					// Angel CMob::ShowHitEffect（IDA 0xc671e0）只 SetOneTimeAction，
					// 不在客户端叠持续 hforce。旧 KBFORCE 0.2/0.1 会在 HIT 窗口持续
					// 推怪；一击必杀时 kill 包晚到就会「die 动画 + 向后滑」。
					// 致死已在 apply_damage(lethal)/apply_death/kill 清冲量；此处不再推。
					break;
				case Stance::JUMP:
					phobj.vforce = -5.0;
					break;
				}

				physics.move_object(phobj);

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
					generate_move_path();
					update_movement();
					counter = 0;
				}
			}
			else
			{
				// 非控制怪：完全由服务端 MovePath 回放（Angel CMovePath），
				// 不做本地物理自由推进——这是「怪物越出平台」的根因修复点。
				if (movepath_.is_active())
				{
					bool ongoing = movepath_.apply(phobj);

					if (ongoing)
					{
						uint8_t ms = movepath_.get_stance();

						if (ms != 0 && static_cast<Stance>(ms) != stance)
							set_stance(static_cast<Stance>(ms));

						phobj.fhid = movepath_.get_fh();
					}
					else
					{
						if (stance != Stance::STAND && stance != Stance::HIT && stance != Stance::DIE)
							set_stance(Stance::STAND);

						phobj.normalize();
						physics.get_fht().update_fh(phobj);
					}
				}
				else
				{
					if (stance != Stance::STAND && stance != Stance::HIT && stance != Stance::DIE)
						set_stance(Stance::STAND);

					phobj.normalize();
					physics.get_fht().update_fh(phobj);
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

	// Angel v83 `CMob::GenerateMovePath` 适配移植（控制怪）：
	// 动作选择对齐 GenerateMovePath 的分支——movement（STAND/MOVE/JUMP，
	// foothold 边界由 TURNATEDGES 在平台边缘翻转）、服务端驱动的 attack/skill
	// （attack 由 s9 DoAttack 结算后下发，不再本地随机出招；skill 由 MOVE_MONSTER_
	// RESPONSE 的 skill_pending 授权）。位移由 foothold 物理约束，选完由
	// update_movement() 单段上报（服务端 MOVE_MONSTER opcode=188，同 Angel
	// COutPacket(188)）。Angel 9KB 的 CVecCtrlMob 多段路径/逐 foothold 采样在
	// 本引擎无等价体，且当前无法实机验证，故用「动作选择 + foothold 边界」适配版。
	void Mob::generate_move_path()
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

			// 攻击/技能改由服务端 MOVE_MONSTER_RESPONSE 驱动（Angel CMob 无本地随机出招，
			// 攻击由服务端 DoAttack 结算后下发），移除本地 randomizer 随机 attack 分支，
			// 避免「怪自己乱打」与服务端 castPos 不一致。
			// notAttack 怪（info.notAttack）在被动态（HIT/JUMP）中也自然不出招。

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
			// P-C 施法侧特效（GetEffectUOL 等价），节点缺失则跳过。
			play_mob_skill_effect(skillid, skilllv);
		}

		MoveMobPacket(
			oid, 1, 0, static_cast<int8_t>(activity), skillid, skilllv, 0, 0,
			get_position(),
			Movement(phobj, activity)
		).dispatch();
	}

	void Mob::play_mob_skill_effect(int8_t id, int8_t lv)
	{
		// Angel TryDoingSkill 用 GetEffectUOL 取技能特效；对应客户端资源为
		// Skill/MobSkill.img（BeiDou 部署实测在 Skill 命名空间，非 Mob，orange-wz 已验证层级）
		// 下 [id]/level[lv] 的 effect/hit/tile 节点。节点缺失则 if(node) 守卫跳过（R6）。
		nl::node skillnode = nl::nx::Skill["MobSkill.img"][std::to_string(id)]["level"][std::to_string(lv)];
		if (!skillnode)
			return;

		// 四源对照 A6：原生 effectAfter 在 Mob.img/<mobId>/info/skill/<i>
		// （字段 skill/level/action/effectAfter，单位 ms；live 实测如 8800000
		// 的 140/141 技能 effectAfter=176）。按当前 skillid+level 匹配取延迟，
		// 0 时用 1 避免施法当帧漏播（095 tStart==0→1 等价保护）。
		uint16_t delay_ms = 1;
		nl::node skillinfo = nl::nx::Mob[std::to_string(id) + ".img"]["info"]["skill"];
		if (skillinfo)
		{
			for (const auto& si : skillinfo)
			{
				if (static_cast<int32_t>(si["skill"]) != id
					|| static_cast<int32_t>(si["level"]) != lv)
					continue;
				int32_t after = static_cast<int32_t>(si["effectAfter"]);
				uint32_t capped = after > 0 ? static_cast<uint32_t>(after) : 1;
				if (capped > 65535)
					capped = 65535;
				delay_ms = static_cast<uint16_t>(capped);
				break;
			}
		}

		if (nl::node e = skillnode["effect"])
			show_effect(Animation(e), 0, 0, flip, delay_ms);
		if (nl::node h = skillnode["hit"])
			show_effect(Animation(h), 0, 0, flip, delay_ms);
		if (nl::node t = skillnode["tile"])
			show_effect(Animation(t), 0, 0, flip, delay_ms);
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

	bool Mob::can_revive() const
	{
		return !noRevive_;
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
			{
				// 2026-11-21 名牌水平居中修复（IDA 取证：CMob::MakeNameTag @0xc84f40
				// m_nNameTagPosX = totalWidth / -2；原生按"整块名牌总宽一半"挂到实体 x）。
				// 旧实现把 namelabel 单独居中在 absp.x、等级牌挂左侧，导致
				// [等级+间隙+名称] 整组相对怪物中心左偏 (levelW+GAP)/2。
				// 现在按总宽算整块左边界，再分别算两块中心，使整组水平中心落在 absp.x。
				constexpr int16_t LEVEL_TAG_GAP = 4;	// NAMETAG 底框左右各外扩约 1~3px，避免贴死
				const int16_t name_w = namelabel.width();
				const int16_t level_w = levellabel.width();
				const int16_t total_w = static_cast<int16_t>(level_w + LEVEL_TAG_GAP + name_w);
				const int16_t group_left = static_cast<int16_t>(absp.x() - total_w / 2);

				const int16_t level_x = static_cast<int16_t>(group_left + level_w / 2);
				const int16_t name_x = static_cast<int16_t>(group_left + level_w + LEVEL_TAG_GAP + name_w / 2);
				levellabel.draw(Point<int16_t>(level_x, absp.y()));
				namelabel.draw(Point<int16_t>(name_x, absp.y()));
			}

			if (tier.hp && showhp && !dying && !hpgauguehide_ && hppercent > 0)
				hpbar.draw(headpos, hppercent, hptagcolor_);
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
			generate_move_path();
	}

	void Mob::send_movement(Point<int16_t> start, std::vector<Movement>&& in_movements,
		uint8_t skillbyte, int8_t skillid, int8_t skilllevel, int16_t poption)
	{
		// 受控怪才会带 movement 列表；非受控怪走 MovePath 回放，不会触发下方。
		if (control)
			return;

		// P-D：透传 s9 moveMonster 的 pOption，当前无对应客户端表现，仅存储预留。
		mob_poption_ = poption;

		if (in_movements.empty())
			return;

		// 非控制怪：完全由服务端下发路径回放（Angel CMovePath::OnMovePacket）。
		// 不再「只 set_position 后交给物理推进」——那是怪物越出平台的根因。
		const Movement& lastmove = in_movements.back();
		uint8_t laststance = lastmove.newstate;

		// 服务端在移动包里显式指定了技能段（42..59）：把对应 skill{n} 动画槽当作当前
		// 姿态播放（帧缺失时 set_stance 会钳回 move/stand，安全），并暂存 skillId/level
		// 供 update_movement 上行回传。
		if (skillbyte >= static_cast<uint8_t>(Stance::SKILL)
			&& skillbyte < static_cast<uint8_t>(Stance::SKILL) + 2 * SKILL_SLOTS)
		{
			Stance skillstance = static_cast<Stance>(skillbyte);

			if (animations.find(skillstance) != animations.end())
			{
				set_stance(skillstance);
				next_skill_id = skillid;
				next_skill_level = skilllevel;
				// 旁观/非受控怪的施法特效：s9 不单独下发 VFX 包，纯客户端驱动。
				// 旧实现只有受控支（update_movement:548）会播，多人同屏只看得到
				// skill{n} 动画、看不到 effect/hit/tile。节点缺失时 play_ 内自有守卫。
				play_mob_skill_effect(skillid, skilllevel);
			}
			else
			{
				set_stance(static_cast<Stance>(laststance));
			}
		}
		else
		{
			set_stance(static_cast<Stance>(laststance));
		}

		set_position(start);
		movepath_.push(start, std::move(in_movements));
		phobj.fhid = movepath_.get_fh();
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
		// Angel 083 CMobPool::OnMobLeaveField @0x67961d：Decode1==0 只停用（立即消失），
		// 非 0 才进入死亡表现。s9 killMonster 默认 animation=1；2 为淡出；3 走备选
		// 死亡动画（die2/dieF）。其余非 0 字节统一走死亡，避免不认识的动画字节让
		// 怪永久留在碰撞里。死体回收不在此处做——交 MapMobs 显式完成。
		switch (animation)
		{
			case 0:
				// 立即消失：停用 + 标记 dead，由 MapMobs 显式从池删除。
				deactivate();
				dead = true;
				return;
			case 2:
				fading = true;
				break;
			case 3:
				// 备选死亡动画（die2/dieF 等）：走 apply_death 的随机 variant 选取，
				// 帧缺失安全（death_stances 为空时退化到 DIE）。
				apply_death();
				break;
			default:
				apply_death();
				break;
		}

		// boss 怪死亡震屏（OnDie Effect_Tremble 降级：boss-only；精确明文
		// mob id 列表待 REA 反编译 OnDie 取出后改为指定 id 集合）。
		if (boss)
			trigger_death_tremble();

		// 致死瞬间清 HIT 击退冲量，避免「死亡动画 + 滑行」叠播。
		phobj.hforce = 0.0;
		phobj.hspeed = 0.0;
		hp_est = 0;
		counter = 0;
		dying = true;
	}

	void Mob::show_hp(int8_t percent, uint16_t playerlevel)
	{
		if (hppercent == 0)
		{
			int16_t delta = playerlevel - level;

			if (delta > 9)
			{
				namelabel.change_color(Color::Name::YELLOW);
				levellabel.change_color(Color::Name::YELLOW);
			}
			else if (delta < -9)
			{
				namelabel.change_color(Color::Name::RED);
				levellabel.change_color(Color::Name::RED);
			}
		}

		if (percent > 100)
			percent = 100;
		else if (percent < 0)
			percent = 0;

		hppercent = percent;
		// 用服务端百分比回校本地估算 HP（一击必杀跳过击退依赖此值）。
		if (maxhp > 0)
			hp_est = static_cast<int32_t>((static_cast<int64_t>(maxhp) * percent) / 100);
		showhp.set_for(2000);
	}

	void Mob::show_effect(const Animation& animation, int8_t pos, int8_t z, bool f, uint16_t delay_ms)
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

		// 四源对照 A6：用 EffectLayer 的 delay_ms 先等再播（原生 effectAfter 延迟）。
		effects.add(animation, DrawArgument(shift, f), z, 1.0f, delay_ms);
	}

	void Mob::show_hit_spark()
	{
		// 原生还原（2026-11-21）：Angel CMob::ShowHitEffect@0x66fb4d 的受击火花语义
		// + MinMS Mob.cpp:7331-7347 —— BasicEff.img/CoolHit，绘制在怪物原点上方 15px。
		// CoolHit 经 orange-wz 实测为单帧无 delay canvas(107x39, origin 49,37)。
		// 与技能效果开关同源（SysOptSkillEffects），关掉画面效果时不显示。
		if (!active)
			return;

		if (!Setting<SysOptSkillEffects>::get().load())
			return;

		nl::node coolhit = nl::nx::Effect["BasicEff.img"]["CoolHit"];
		if (!coolhit || coolhit.size() == 0)
			return;

		constexpr int16_t MOB_HIT_EFFECT_Y_OFFSET = -15;
		// 2026-11-23 偏差修复：effects 由 Mob::draw 以屏幕坐标 absp 作父位传入
		// （drawbelow/drawabove(absp) → Sprite::stateargs+parentpos），DrawArgument
		// 必须是「相对怪物原点」的偏移。旧实现塞 phobj.get_x/get_y()（世界坐标），
		// 最终绘制 = 世界坐标 + 屏幕坐标，火花被甩到离怪几百像素。
		// MinMS Mob.cpp:7331-7347 / 本仓 show_effect:888-905 均为相对偏移。
		Point<int16_t> spark_pos(0, MOB_HIT_EFFECT_Y_OFFSET);
		effects.add(Animation(coolhit), DrawArgument(spark_pos, flip), 0);
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
		// 官方完整伤害公式（无配置开关）：属性驱动——没有来源的属性不产生乘区。
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

		// 四源对照 A1：元素克制（MinMS Mob.cpp:7438-7475 adjust_attack_damage 同源，
		// 标注为现代化增强、非 083 原生逐证）。技能 elemAttr 首字母 F/I/L/S/H/D，
		// 怪物 info/elemAttr 为「字母+强度码」成对串：1=免疫(返回1.0)、2=弱点×0.5、
		// 3=抗性×1.5。s9 服务端权威结算伤害，此处只调客户端飘字表现层。
		if (!attack.element.empty() && elemattr.size() >= 2)
		{
			const char atk = static_cast<char>(std::toupper(
				static_cast<unsigned char>(attack.element.front())));
			for (size_t i = 0; i + 1 < elemattr.size(); i += 2)
			{
				const char me = static_cast<char>(std::toupper(
					static_cast<unsigned char>(elemattr[i])));
				if (me != atk)
					continue;
				switch (elemattr[i + 1])
				{
				case '1':  // 免疫
					mindamage = maxdamage = 1.0;
					break;
				case '2':  // 弱点：伤害减半（MinMS ×0.5）
					mindamage *= 0.5;
					maxdamage *= 0.5;
					break;
				case '3':  // 抗性：伤害×1.5（MinMS ×1.5）
					mindamage *= 1.5;
					maxdamage *= 1.5;
					break;
				default:
					break;
				}
				break;
			}
		}

		// BOSS 增伤：WZ info/boss 标记（构造时读取）。
		if (boss && attack.bossdamage > 0.0f)
		{
			double factor = 1.0 + attack.bossdamage;
			mindamage *= factor;
			maxdamage *= factor;
		}

		// 最终伤害乘区：各来源独立连乘后由 0x175 折算下发（属性驱动，无来源时 finaldamage=0 → ×1）。
		if (attack.finaldamage > 0.0f)
		{
			double factor = 1.0 + attack.finaldamage;
			mindamage *= factor;
			maxdamage *= factor;
		}

		if (mindamage < 1.0)
			mindamage = 1.0;

		if (maxdamage < mindamage)
			maxdamage = mindamage;

		// 官方暴击倍率区间（基准 1.35 + cd%），由 Player/Skill 填充，无开关。
		float critical_min = attack.critical_min;
		float critical_max = attack.critical_max;

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

	void Mob::apply_damage(int32_t damage, bool toleft, bool lethal)
	{
		if (damage > 0)
			hitsound.play();

		if (dying && stance != Stance::DIE)
		{
			apply_death();
			return;
		}

		// Angel OnHit：HP<=0 时不 GenerateMovePath。本地用 WZ maxHP + show_hp 估算；
		// damage>=hp_est 或调用方 lethal=true → 跳过 HIT 滑行，等 kill 包播 die。
		if (!lethal && is_alive() && damage > 0 && hp_est > 0 && damage >= hp_est)
			lethal = true;

		if (damage > 0 && hp_est > 0)
		{
			hp_est -= damage;
			if (hp_est < 0)
				hp_est = 0;
		}

		if (lethal)
		{
			phobj.hforce = 0.0;
			phobj.hspeed = 0.0;
			show_hit_spark();
			return;
		}

		if (control && is_alive() && damage >= knockback)
		{
			// 对齐 Angel OnHit：控制怪本地受击进 HIT，但不提前触发 generate_move_path
			//（原 counter=170 会让 30 帧后立即反向行走，观感像「被打飞出」）。
			// 非控制怪的受击 HIT 由服务端 MOB_MOVED 的 HIT 段驱动，不在此强切。
			flip = toleft;
			counter = 0;
			set_stance(Stance::HIT);

			// 受击不主动发移动包（Angel 命中只 ShowHitEffect，位置仍由服务端路径决定）。

			// 原生还原（2026-11-21）：对齐 Angel CMob::ShowHitEffect@0x66fb4d 受击火花语义
			// 与 MinMS Mob.cpp:7331-7347 —— 命中时在怪物原点上方 15px 播 BasicEff/CoolHit。
			// CoolHit 经 orange-wz 实测为单帧无 delay 的 canvas(107x39, origin 49,37)，
			// 故每次新建 Animation 播放（与 FieldMarker 同口径），无需缓存成员。
			show_hit_spark();
		}
	}

	MobAttack Mob::create_touch_attack() const
	{
		// 死亡/失效的怪不再造成接触伤害：否则击杀后残留的精灵仍会咬人。
		if (!is_alive() || !touchdamage)
			return MobAttack();

		int32_t minattack = static_cast<int32_t>(watk * 0.8f);
		int32_t maxattack = watk;
		int32_t attack = randomizer.next_int(minattack, maxattack);

		return MobAttack(attack, get_position(), id, oid);
	}

	void Mob::apply_death()
	{
		// 对齐 Angel OnDie 的 rand()%v6+9：在「实际加载到的死亡帧」里随机选一套。
		// 精确 action→die 偏移待 REA 反编译 PrepareActionLayer 精修；当前按 WZ
		// 枚举（death_stances）随机，观感等价。
		if (!death_stances.empty())
			set_stance(death_stances[randomizer.next_int<size_t>(death_stances.size())]);
		else
			set_stance(Stance::DIE);

		// 死亡切姿态时清掉受击冲量，避免 HIT 击退与 die 动画叠在一起滑出。
		phobj.hforce = 0.0;
		phobj.hspeed = 0.0;
		counter = 0;

		diesound.play();
		dying = true;
	}

	void Mob::trigger_death_tremble()
	{
		// 对齐 Angel OnDie 末尾 Effect_Tremble。OnDie 用 _ZtlSecureFuse 解密 mob
		// 模板 id 后与若干常量比较触发；经 13339 反编译取证：仅 mob id 8810018 为
		// 明文可提取（Tremble 3000ms / mode1），其余比较值被加密渲染为代码地址无法
		// 读取。降级策略：boss 怪（WZ info/boss）统一震屏（语义覆盖「特殊怪」），
		// 并显式补回已确定的 8810018；参数：boss 分支 4000ms/mode1、8810018 3000ms/mode1。
		if (boss)
			Stage::get().get_camera().tremble(1, 4000);
		else if (id == 8810018)
			Stage::get().get_camera().tremble(1, 3000);
	}

	bool Mob::is_alive() const
	{
		return active && !dying;
	}

	bool Mob::is_dead() const
	{
		return dead;
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