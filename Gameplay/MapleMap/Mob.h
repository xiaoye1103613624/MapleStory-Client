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

#include "MapObject.h"

#include "../Movement.h"
#include "MovePath.h"

#include "../Combat/Attack.h"
#include "../Combat/Bullet.h"

#include "../../Audio/Audio.h"
#include "../../Graphics/EffectLayer.h"
#include "../../Graphics/Geometry.h"
#include "../../Util/Randomizer.h"
#include "../../Util/TimedBool.h"

#include <string>
#include <vector>

namespace ms
{
	// 分裂怪跟随父怪（effect == -3）：父怪死后服务端不再补发 spawn，原生客户端
	// 雪藏该怪直至父怪死亡。本类存储 spawn_effect/parent/parent_oid（波 2 接线）。
	class Mob : public MapObject
	{
	public:
		static const size_t NUM_STANCES = 6;

		// 状态字节：2..10 是基础姿态（偶数=朝左，奇数=朝右，见 set_stance）。
		// 24 起是 v83 的攻击段、42 起是技能段：服务端用 (byte-24)/2、(byte-42)/2 取 0 基下标，
		// 动画节点名是 attack{下标+1} / skill{下标+1}。
		// 证据：BeiDou-Server_s9 net/server/channel/handlers/MoveLifeHandler.java:84-110
		//       + server/life/LifeFactory.java:485-495（"attack" + (i + 1) 循环，i 从 0）。
		enum Stance : uint8_t
		{
			MOVE = 2,
			STAND = 4,
			JUMP = 6,
			HIT = 8,
			DIE = 10,
			// 多套死亡动画（OnDie 的 rand()%v6+9 随机选 variant）。一个怪可有
			// die1..dieN（抽样 2053 个 img：die1 1608、die2 11、dieF 9）。
			// 段基址沿用 2*下标约定：die2=12、die3=14…die7=22（< ATTACK=24，
			// 不与之冲突）。精确「action 偏移」待 REA 反编译 PrepareActionLayer
			// 精修；当前降级为「按 WZ 实际存在的 die 帧随机枚举」（观感等价）。
			DIE2 = 12,
			DIE3 = 14,
			DIE4 = 16,
			DIE5 = 18,
			DIE6 = 20,
			DIE7 = 22,
			ATTACK = 24,
			SKILL = 42
		};

		// 是否任一死亡姿态（DIE..DIE7），供 update() 判定死亡动画播完置 dead。
		static bool is_death_stance(Stance s)
		{
			return s >= DIE && s <= DIE7;
		}

		// 攻击段服务端只接受下标 0..7（MonsterInformationProvider.getMobAttackInfo 越界返回 null），
		// 技能段 42..59 可到下标 8。
		static constexpr int32_t ATTACK_SLOTS = 8;
		static constexpr int32_t SKILL_SLOTS = 9;

		static std::string nameof(Stance stance)
		{
			if (stance >= SKILL)
				return "skill" + std::to_string((stance - SKILL) / 2 + 1);

			if (stance >= ATTACK)
				return "attack" + std::to_string((stance - ATTACK) / 2 + 1);

			if (stance >= DIE && stance <= DIE7)
				return "die" + std::to_string((stance - DIE) / 2 + 1);

			static const std::string stancenames[NUM_STANCES] =
			{
				"move",
				"stand",
				"jump",
				"hit1",
				"die1",
				"fly"
			};

			// 原来的 (stance - 1) / 2 对 2..10 正好得到 0..4，但一旦有越界状态就会读到数组外；
			// 这里显式钳到已知区间。
			size_t index = (stance - MOVE) / 2;

			if (index >= NUM_STANCES - 1)
				index = 0;

			return stancenames[index];
		}

		static uint8_t value_of(Stance stance, bool flip)
		{
			return flip ? stance : stance + 1;
		}

		// Construct a mob by combining data from game files with data sent by the server
		// spawn_effect/parent 来自 SPAWN_MOB 尾段（s9 PacketCreator:1449-1564）：
		// >0 是出生特效（Mob/SpecialEff.img），parent_oid 用于扎昆之手一类
		// 分裂怪跟随父怪死亡（effect == -3 时父怪死后才出现）。
		Mob(int32_t oid, int32_t mobid, int8_t mode, int8_t stance, uint16_t fhid, bool newspawn, int8_t team, Point<int16_t> position,
			int8_t spawn_effect = 0, bool parent = false, int32_t parent_oid = 0);

		// Draw the mob
		void draw(double viewx, double viewy, float alpha) const override;
		// Update movement and animations
		int8_t update(const Physics& physics) override;

		// Update who a mob is controlled by:
		// 0 - no control, 1 - control, 2 - aggro
		void set_control(int8_t mode);

		// MOVE_MONSTER_RESPONSE 携带的下一可用技能（s9 moveMonsterResponse
		// PacketCreator.java:1643-1651：bool useSkills + short mp + byte skillId + byte level）。
		// 控制器怪下一次动作时消费它，上报 SKILL 段状态字节 + skillId/level。
		void set_next_skill(bool use, int8_t id, int8_t level);

		// 分裂怪跟随父怪（spawn 尾段 -3）：父怪死亡前雪藏。
		// 【2026-11-03 构建修复】原声明名 has_parent() 与下方同作用域存储字段
		// `bool has_parent = false;` 重名（C2365 重定义，全工程 18 处 TU 编译失败）。
		// 字段名不动（Mob.cpp spawn/refresh 两处赋值在用），访问器改名 parented()。
		bool parented() const;
		int32_t get_parent_oid() const;
		bool is_dormant() const;
		void wake();
		// 同一个 oid 被重新下发（服务端换控制器发的是整包 spawn）时，把新包里的落位、
		// 姿态、队伍和淡入状态套到已有对象上；只 makeactive() 会把新坐标整包丢掉。
		void refresh(int8_t mode, int8_t stance, uint16_t fh, int8_t tm, bool newspawn, Point<int16_t> position,
			int8_t spawn_effect = 0, bool parent = false, int32_t parent_oid = 0);
		// Send movement to the mob. The optional skill header (s9 moveMonster) makes
		// the mob play the corresponding skill{n} animation locally and remembers
		// skillId/level for the next upstream update_movement.
		// poption carries the s9 moveMonster short pOption; no client behaviour yet (P-D, reserved).
		void send_movement(Point<int16_t> start, std::vector<Movement>&& movements,
			uint8_t skillbyte = 0, int8_t skillid = 0, int8_t skilllevel = 0, int16_t poption = 0);
		// Kill the mob with the appropriate type:
		// 0 - make inactive 1 - death animation 2 - fade out
		void kill(int8_t killtype);
		// Display the hp percentage above the mob
		// Use the playerlevel to determine color of NameTag
		void show_hp(int8_t percentage, uint16_t playerlevel);
		// Show an effect at the mob's position
		void show_effect(const Animation& animation, int8_t pos, int8_t z, bool flip,
			uint16_t delay_ms = 0);
		// 三端原生对齐（MinMS Mob.cpp:7890-7925，Angel TryDoingMeleeAttack@0x952453）：
		// 忍者冲击按概率击退（距离有符号）、回马把命中怪聚到角色身侧固定距离。
		void push_back(int32_t distance);
		void gather(Point<int16_t> anchor, int32_t distance);
		// 受击火花（Angel ShowHitEffect / MinMS show_hit_spark）：CoolHit @ 原点上方 15px
		void show_hit_spark();

		// Calculate the damage to this mob with the specified attack
		damage::DamageLines calculate_damage(const Attack& attack);
		// Apply damage to the mob
		void apply_damage(int32_t damage, bool toleft, bool lethal = false);

		// Create a touch damage attack to the player
		MobAttack create_touch_attack() const;

		// Check if this mob collides with the specified rectangle
		bool is_in_range(const Rectangle<int16_t>& range) const;
		// Check if this mob is still alive
		bool is_alive() const;
		// Check if the death sequence has finished (used by MapMobs for explicit removal)
		bool is_dead() const;
		// noRevive 守卫（P-A）：WZ info/noRevive 标记该怪不参与服务端驱动的通用复活。
		// 本版只作预留，分裂怪雪藏唤醒不依赖此值。
		bool can_revive() const;
		// Return the head position
		Point<int16_t> get_head_position() const;

	private:
		enum FlyDirection
		{
			STRAIGHT,
			UPWARDS,
			DOWNWARDS,
			NUM_DIRECTIONS
		};

		// Set the stance by byte value
		void set_stance(uint8_t stancebyte);
		// Set the stance by enumeration value
		void set_stance(Stance newstance);
		// Start the death animation
		void apply_death();
		// 死亡震屏（OnDie Effect_Tremble 降级：boss 怪死亡统一触发；精确明文
		// mob id 列表待 REA 反编译 OnDie 取出后精修为指定 id 集合）。
		void trigger_death_tremble();
		// 控制怪移动生成（Angel v83 `CMovePath::GenerateMovePath` 适配移植）：
		// 动作选择对齐 move/jump/stand + 服务端驱动的 attack/skill 分支与 foothold
		// 边界（PhysicsObject::TURNATEDGES 在平台边缘翻转）；本地物理与
		// update_movement 单段上报保持不变（服务端 MOVE_MONSTER opcode=188）。
		// 注：Angel 9KB 的 CVecCtrlMob 多段路径/逐 foothold 采样在本引擎无等价体，
		// 且当前无法实机验证，故用「动作选择 + foothold 边界」适配版替代逐字节
		// 物理复刻（精确多段路径待 REA 反编译 + 引擎层 CVecCtrlMob 等价体后精修）。
		void generate_move_path();
		// Send the current position and state to the server
		void update_movement();
		// P-C：施法侧特效（GetEffectUOL 等价）。查 MobSkill.img[id]/level[lv] 的
		// effect/hit/tile 节点；节点缺失（BeiDou 精简 img 常态）则优雅跳过（R6）。
		void play_mob_skill_effect(int8_t id, int8_t lv);

		// Calculate the hit chance
		float calculate_hitchance(int16_t leveldelta, int32_t accuracy) const;
		// Calculate the minimum damage
		double calculate_mindamage(int16_t leveldelta, double mindamage, bool magic) const;
		// Calculate the maximum damage
		double calculate_maxdamage(int16_t leveldelta, double maxdamage, bool magic) const;
		// Calculate a random damage line based on the specified values.
		// 单段伤害在这里统一饱和到 2^53（或配置/破功值上限），是唯一 clamp 点。
		damage::DamageLine next_damage(double mindamage, double maxdamage, float hitchance,
			float critical, float critical_min, float critical_max) const;

		// Return the current 'head' position
		Point<int16_t> get_head_position(Point<int16_t> position) const;

		std::map<Stance, Animation> animations;
	// 该怪实际加载到的死亡姿态键集合（die1..dieN / 回退 die、dieF），apply_death
	// 从中随机选一播放；空时（极端缺帧）退化到 DIE。
	std::vector<Stance> death_stances;
		std::string name;
		// spawn 尾段存储（见 Spawn.h MobSpawn 注释）：
		// effect：>0 出生特效 id（Mob/SpecialEff），-2 淡入 marker，-3 等父怪死，-1 无；
		// has_parent/parentoid：父怪关系（扎昆之手、分裂怪）。
		int8_t effect = 0;
		bool has_parent = false;
		int32_t parentoid = 0;
		Sound hitsound;
		Sound diesound;
		uint16_t level;
		float speed;
		float flyspeed;
		uint16_t watk;
		uint16_t matk;
		uint16_t wdef;
		uint16_t mdef;
		uint16_t accuracy;
		uint16_t avoid;
		uint16_t knockback;
		// WZ info/maxHP：客户端估算剩余 HP，供一击必杀跳过 HIT 击退（Angel OnHit
		// 在 HP<=0 时不走 GenerateMovePath）。show_hp 百分比会回校。
		int32_t maxhp = 1;
		int32_t hp_est = 1;
		bool undead;
		// 四源对照 A1：WZ info/elemAttr（字母+强度码成对，如 "F3"=火弱点×1.5），
		// 供技能元素克制客户端表现层判定（s9 服务端权威结算，不改实际数字）。
		std::string elemattr;
		// v2：WZ info/boss 标记，用于 BOSS 增伤词缀（原生 boss 判定资源节点）。
		bool boss;
		bool touchdamage;
		bool noflip;
		bool notattack;
		bool canmove;
		bool canjump;
		bool canfly;

		EffectLayer effects;
		Text namelabel;
		// 等级名牌「Lv:N」：与名称同构的独立 NAMETAG 框，底部并排显示在名称前方（左侧）。
		// 独立于 namelabel，便于各自染色/隐藏，不与名称拼成一段文本。
		Text levellabel;
		MobHpBar hpbar;
		Randomizer randomizer;

		TimedBool showhp;

		// 服务端移动路径回放器（Angel CMovePath 等价）；非控制怪位置完全由它驱动。
		MovePath movepath_;
		uint16_t counter;

		int32_t id;
		int8_t team;
		bool dying;
		bool dead;
		bool control;
		bool aggro;
		// MOVE_MONSTER_RESPONSE 下发的下一可用技能槽（s9 moveMonsterResponse
		// PC:1643-1651）：下一次 update_movement 上报 SKILL 段时消费。
		bool skill_pending;
		int8_t next_skill_id;
		int8_t next_skill_level;
		// spawn 尾段 effect==-3 的雪藏态：父怪（parentoid）死亡前不活动。
		bool dormant;
		Stance stance;
		bool flip;
		FlyDirection flydirection;
		float walkforce;
		int8_t hppercent;
		bool fading;
		bool fadein;
		Linear<float> opacity;
		// —— parity 缺口补齐字段（对齐 Angel，全走缺失默认/跳过，维系 R6 精简 img 兼容）——
		// P-A：WZ info/noRevive（守卫，本版预留）、info/regena（复活淡入动画名，预留）。
		bool noRevive_ = false;
		std::string regena_;
		// P-E：per-mob 血条颜色（WZ info/hpTagColor）与隐藏开关（WZ info/HPgaugeHide）。
		Color hptagcolor_ = Color::Name::LIGHTGREEN;
		bool hpgauguehide_ = false;
		// P-D：s9 moveMonster 透传的 pOption，当前仅存储（无对应表现，预留）。
		int16_t mob_poption_ = 0;
	};
}