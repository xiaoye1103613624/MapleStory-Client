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

#include "../../Character/Char.h"

#include <memory>
#include <vector>

namespace ms
{
	// Interface for skill effects
	class SkillUseEffect
	{
	public:
		virtual ~SkillUseEffect() {}

		virtual void apply(Char& target) const = 0;

	protected:
		class Effect
		{
		public:
			Effect(nl::node src)
			{
				animation = src;
				z = src["z"];
			}

			void apply(Char& target) const
			{
				target.show_attack_effect(animation, z);
			}

		private:
			Animation animation;
			int8_t z;
		};
	};

	// No animation
	class NoUseEffect : public SkillUseEffect
	{
	public:
		void apply(Char&) const override {}
	};

	// An effect which displays an animation over the character's position
	class SingleUseEffect : public SkillUseEffect
	{
	public:
		SingleUseEffect(nl::node src);

		void apply(Char& target) const override;

	private:
		Effect effect;
	};

	// 从 Effect/BasicEff.img 直接按节点名加载施放特效。
	// Angel 原生：TryRegisterTeleport@0x957b74 播 BasicEff.img/Teleport，
	// DoActiveSkill_BoundJump@0x96be38 播 BasicEff.img/Flying —— 两者都不在
	// Skill.img 的 effect 节点里，故不能复用 SingleUseEffect（它取 src["effect"]）。
	// 沿用 Effect 基类：apply 走 Char::show_attack_effect，天然尊重
	// SysOptSkillEffects「技能使用时允许画面效果」画面开关，与原生一致。
	class BasicEffUseEffect : public SkillUseEffect
	{
	public:
		// path: BasicEff.img 下的节点名（如 "Teleport" / "Flying"）
		BasicEffUseEffect(const std::string& path);

		void apply(Char& target) const override;

	private:
		Effect effect;
	};

	// An effect which displays an animation over the character's position
	// The effect changes based on whether the character uses a two-handed weapon
	class TwoHandedUseEffect : public SkillUseEffect
	{
	public:
		TwoHandedUseEffect(nl::node src);

		void apply(Char& target) const override;

	private:
		BoolPair<Effect> effects;
	};

	// An effect which displays multiple animations over the character's position
	class MultiUseEffect : public SkillUseEffect
	{
	public:
		MultiUseEffect(nl::node src);

		void apply(Char& target) const override;

	private:
		std::vector<Effect> effects;
	};

	// The animation changes with the character level
	class ByLevelUseEffect : public SkillUseEffect
	{
	public:
		ByLevelUseEffect(nl::node src);

		void apply(Char& target) const override;

	private:
		std::map<uint16_t, Effect> effects;
	};

	// 第二轮补齐：技能 WZ 中除 effect/hit 外还有 prepare/keydown/keydownend/
	// screen/special/tile 六类特效节点（orange-wz 跨 img 实证：1412/1510/000/2211.img
	// 的对应节点是 CANVAS 帧动画，可 Animation(src) 直接播放）。本类把这些节点
	// 作为「角色锚点施放特效」播，复用 Effect 基类 → Char::show_attack_effect，
	// 天然受 SysOptSkillEffects 画面开关约束。receiver/finish 两类全库无源，不接。
	// 注意：MinMS 的 special 雨滴/tile 落点需 EffectLayer delay/粒子系统（本端无），
	// 这里只做「原地逐帧播放」的可见近似，不做位移/延时物理，属诚实降级。
	class ExtraNodeUseEffect : public SkillUseEffect
	{
	public:
		ExtraNodeUseEffect(nl::node src);
		void apply(Char& target) const override;
	private:
		Effect effect;
	};

	// 一次施放需要同时播多个额外节点（如 14121004 的 prepare+keydown+tile）。
	class CompositeUseEffect : public SkillUseEffect
	{
	public:
		void add(std::unique_ptr<SkillUseEffect> e);
		void apply(Char& target) const override;
	private:
		std::vector<std::unique_ptr<SkillUseEffect>> effects;
	};

	// Use effect for Iron Body
	class IronBodyUseEffect : public SkillUseEffect
	{
	public:
		void apply(Char& target) const override;
	};
}