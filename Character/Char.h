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

#include "CharEffect.h"

#include "Inventory/Weapon.h"
#include "Look/Afterimage.h"
#include "Look/CharLook.h"
#include "Look/PetLook.h"

#include "../Graphics/EffectLayer.h"

#include "../Gameplay/Combat/DamageNumber.h"
#include "../Gameplay/MapleMap/MapObject.h"
#include "../IO/Components/ChatBalloon.h"

#include <map>

namespace ms
{
	// Base for characters, e.g. the player and other clients on the same map.
	class Char : public MapObject
	{
	public:
		// Player states which determine animation and state 
		// Values are used in movement packets (Add one if facing left)
		enum State : int8_t
		{
			WALK = 2,
			STAND = 4,
			FALL = 6,
			ALERT = 8,
			PRONE = 10,
			SWIM = 12,
			LADDER = 14,
			ROPE = 16,
			DIED = 18,
			SIT = 20
		};

		static State by_value(int8_t value)
		{
			return static_cast<State>(value);
		}

		// Draw look, NameTag, effects and chat bubble.
		void draw(double viewx, double viewy, float alpha) const override;
		// Draw look
		void draw_preview(Point<int16_t> position, float alpha) const;
		// Update look and movements
		int8_t update(const Physics& physics) override;
		// Return the current map layer, or seven if on a ladder or rope.
		int8_t get_layer() const override;

		// Check whether the character is invincible
		virtual bool is_invincible() const;
		// Return the character's level
		virtual uint16_t get_level() const = 0;
		// Return the a skill's level from the character
		virtual int32_t get_skilllevel(int32_t skillid) const = 0;
		// Return the character's base attacking speed
		virtual int8_t get_integer_attackspeed() const = 0;

		// Return the attack speed as a multiplier
		float get_real_attackspeed() const;
		// Return the delay until applying an attack
		uint16_t get_attackdelay(size_t no) const;

		// Set if the character sprite is mirrored (true = facing right)
		virtual void set_direction(bool flipped);
		// Change the character's state
		virtual void set_state(State newstate);
		// Change the character's stance to an attack action
		void attack(const std::string& action);
		// Change the character's stance to an attack stance
		void attack(Stance::Id stance);
		// Change the character's stance to it's regular attack
		void attack(bool degenerate);
		// Set the afterimage for an attack
		void set_afterimage(int32_t skill_id);
		// Return the current afterimage
		const Afterimage& get_afterimage() const;

		// Display an animation as an effect with the character
		void show_attack_effect(Animation animation, int8_t z);
		// MinMS 现代化特效入口（三端标注：非 083 原生复刻）：
		// 下落/粒子（special 类）+ 固定偏移延时爆发（Big Bang 类）。
		void show_falling_effect(Animation animation, Point<int16_t> start_offset,
			Point<int16_t> fall_delta, uint16_t delay_ms, uint16_t duration_ms,
			bool flip, int8_t z, bool follow = true, float angle = 0.0f);
		void show_particle_effect(Animation animation, Point<int16_t> start_offset,
			Point<int16_t> fall_delta, uint16_t life_ms, int8_t z, bool flip,
			bool follow, float angle = 0.0f);
		void show_offset_effect(Animation animation, Point<int16_t> offset,
			uint16_t delay_ms, bool flip);
		// Display an animation as an effect on top of the character
		void show_effect_id(CharEffect::Id toshow);
		// Display the iron body skill animation
		void show_iron_body();
		// Display damage over the characters head
		void show_damage(int64_t damage);
		// Display a chat bubble with the specified line in it
		void speak(const std::string& line);
		// Change a part of the character's look
		void change_look(MapleStat::Id stat, int32_t id);
		// Change the character's state by id
		void set_state(uint8_t statebyte);
		// Change the character's face expression by id
		void set_expression(int32_t expression);

		// 分身残影开关（SHADOWPARTNER buff 驱动，纯表现层）。
		void set_shadowpartner(bool on);

		// GM 隐身本地视觉提示开关：本地玩家 !hide 后置 true，自身角色半透明渲染。
		void set_hidden(bool h);
		// 返回角色是否处于本地隐身视觉状态。
		bool is_hidden() const;

		// 护盾特效开关（MAGIC_GUARD/HOLY_SHIELD）：按 Buff 下发的技能 id 动态加载
		// Skill/<ns>.img/skill/<strid>/effect 循环动画；clear 时移除。
		void set_shield(int stat, int32_t skill_id);
		void clear_shield(int stat);

		// Add a pet with the specified stats
		void add_pet(uint8_t index, int32_t iid, const std::string& name, int32_t uniqueid, Point<int16_t> pos, uint8_t stance, int32_t fhid);
		// Remove a pet with the specified index and reason
		void remove_pet(uint8_t index, bool hunger);
		// Update pet position/stance from MOVE_PET
		void move_pet(uint8_t index, Point<int16_t> pos, uint8_t stance);
		// Rename pet nametag from PET_NAME_CHANGE
		void rename_pet(uint8_t index, const std::string& name);

		// Return if the character is facing left
		bool getflip() const;
		// Return the name of this character
		std::string get_name() const;

		// Return if the char is in the Char::State::SIT state
		bool is_sitting() const;
		// Return if the char is in the Char::State::LADDER or Char::State::ROPE state
		bool is_climbing() const;
		// 当前移动状态（商城预览沙盒跟帧用）
		State get_char_state() const { return state; }
		// Return whether the character sprite uses stances for two-handed weapons
		bool is_twohanded() const;
		// Return the type of the currently equipped weapon
		Weapon::Type get_weapontype() const;

		// Obtain a reference to this character's look
		CharLook& get_look();
		// Obtain a constant reference to this character's look
		const CharLook& get_look() const;
		// Return a reference to this characters's physics
		PhysicsObject& get_phobj();
		// True if any of the 3 pet slots has an item
		bool has_pet() const;
		// Active pet look at index 0..2（无则 itemid==0）
		const PetLook& get_pet(uint8_t index) const;

		// Initialize character effects
		static void init();

	protected:
		Char(int32_t oid, const CharLook& look, const std::string& name);

		// Update the character's animation with the given speed
		bool update(const Physics& physics, float speed);
		// Get a speed modifier for the current stance
		float get_stancespeed() const;

		CharLook look;
		CharLook look_preview;
		PetLook pets[3];

		State state;
		bool attacking;
		bool facing_right;

	private:
		Text namelabel;
		ChatBalloon chatballoon;
		EffectLayer effects;
		// follow=false 的世界锚定粒子：上一帧位置，用于算出本帧 movement 抵消。
		Point<int16_t> ring_prev_position;
		Afterimage afterimage;
		TimedBool invincible;
		TimedBool ironbody;
		// 原生还原 2026-11-21：移除自创 hitflash 红闪成员（Angel/MinMS 无此通道）。
		std::list<DamageNumber> damagenumbers;
		// 分身残影开关（SHADOWPARTNER），绘制滞后半透明复制体。
		bool shadowpartner = false;
		// GM 隐身本地视觉状态：仅本地玩家置位，远端角色恒 false。
		bool hidden = false;
		// 护盾特效动画表（key = Buffstat::Id 的 int 值）。
		std::map<int, Animation> shields;

		static EnumMap<CharEffect::Id, Animation> chareffects;
		// 2026-11-25：原生 v83 死亡墓碑下落动画（Effect/Tomb.img/fall），
		// 切 DIED 时经 effects.add 播放，与 MinMS Char.cpp:1808-1811 同源。
		static Animation tomb_fall;
	};
}