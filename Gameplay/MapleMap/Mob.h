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

#include "../Combat/Attack.h"
#include "../Combat/Bullet.h"

#include "../../Audio/Audio.h"
#include "../../Graphics/EffectLayer.h"
#include "../../Graphics/Geometry.h"
#include "../../Util/Randomizer.h"
#include "../../Util/TimedBool.h"

#include <string>

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
			ATTACK = 24,
			SKILL = 42
		};

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

		// Change this mob's control mode:
		// 0 - no control, 1 - control, 2 - aggro
		void set_control(int8_t mode);
		// 同一个 oid 被重新下发（服务端换控制器发的是整包 spawn）时，把新包里的落位、
		// 姿态、队伍和淡入状态套到已有对象上；只 makeactive() 会把新坐标整包丢掉。
		void refresh(int8_t mode, int8_t stance, uint16_t fh, int8_t tm, bool newspawn, Point<int16_t> position,
			int8_t spawn_effect = 0, bool parent = false, int32_t parent_oid = 0);
		// Send movement to the mob
		void send_movement(Point<int16_t> start, std::vector<Movement>&& movements);
		// Kill the mob with the appropriate type:
		// 0 - make inactive 1 - death animation 2 - fade out
		void kill(int8_t killtype);
		// Display the hp percentage above the mob
		// Use the playerlevel to determine color of NameTag
		void show_hp(int8_t percentage, uint16_t playerlevel);
		// Show an effect at the mob's position
		void show_effect(const Animation& animation, int8_t pos, int8_t z, bool flip);

		// Calculate the damage to this mob with the specified attack
		std::vector<std::pair<int32_t, bool>> calculate_damage(const Attack& attack);
		// Apply damage to the mob
		void apply_damage(int32_t damage, bool toleft);

		// Create a touch damage attack to the player
		MobAttack create_touch_attack() const;

		// Check if this mob collides with the specified rectangle
		bool is_in_range(const Rectangle<int16_t>& range) const;
		// Check if this mob is still alive
		bool is_alive() const;
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
		// Decide on the next state
		void next_move();
		// Send the current position and state to the server
		void update_movement();

		// Calculate the hit chance
		float calculate_hitchance(int16_t leveldelta, int32_t accuracy) const;
		// Calculate the minimum damage
		double calculate_mindamage(int16_t leveldelta, double mindamage, bool magic) const;
		// Calculate the maximum damage
		double calculate_maxdamage(int16_t leveldelta, double maxdamage, bool magic) const;
		// Calculate a random damage line based on the specified values
		std::pair<int32_t, bool> next_damage(double mindamage, double maxdamage, float hitchance, float critical) const;

		// Return the current 'head' position
		Point<int16_t> get_head_position(Point<int16_t> position) const;

		std::map<Stance, Animation> animations;
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
		bool undead;
		bool touchdamage;
		bool noflip;
		bool notattack;
		bool canmove;
		bool canjump;
		bool canfly;

		EffectLayer effects;
		Text namelabel;
		MobHpBar hpbar;
		Randomizer randomizer;

		TimedBool showhp;

		std::vector<Movement> movements;
		uint16_t counter;

		int32_t id;
		int8_t team;
		bool dying;
		bool dead;
		bool control;
		bool aggro;
		Stance stance;
		bool flip;
		FlyDirection flydirection;
		float walkforce;
		int8_t hppercent;
		bool fading;
		bool fadein;
		Linear<float> opacity;
	};
}