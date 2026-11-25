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
#include "Char.h"
#include "../Graphics/TintContext.h"
#include "../Audio/Audio.h"

#include "../Data/WeaponData.h"
#include "../Configuration.h"
#include "../Util/Misc.h"

#include <fstream>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// GM 隐身本地视觉提示：自身角色半透明渲染的不透明度（0=全透，1=不透明）。
	constexpr float HIDDEN_ALPHA = 0.4f;

	Char::Char(int32_t o, const CharLook& lk, const std::string& name) : MapObject(o), look(lk), look_preview(lk), namelabel(Text(Text::Font::A13M, Text::Alignment::CENTER, Color::Name::WHITE, Text::Background::NAMETAG, name)), ring_prev_position() {}

	void Char::draw(double viewx, double viewy, float alpha) const
	{
		// ColorPrism: push this character's id so draw-time tint routing (Texture::draw)
		// applies their own colours to the shared Clothing frames. RAII pops on exit.
		TintContext::Scope tint_scope(get_oid());

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);

		effects.drawbelow(absp, alpha);

		Color color;

		if (invincible)
		{
			float phi = invincible.alpha() * 30;
			float rgb = 0.9f - 0.5f * std::abs(std::sinf(phi));

			color = Color(rgb, rgb, rgb, 1.0f);
		}
		else
		{
			color = Color::Code::CWHITE;
		}

		// 原生还原（2026-11-21）：Angel CUserLocal::SetDamaged@0x9581a9 与 MinMS
		// Char.cpp:338-358 的受击视觉只有灰度乘色闪 + emotion sprite，没有独立红闪通道。
		// 已移除本端自创的 hitflash 300ms 红闪（原 Char.cpp:62-67）；保留上面的灰度闪。

		// GM 隐身本地视觉提示：隐身时自身角色整体半透明渲染（远端角色 hidden 恒 false）。
		if (hidden)
		{
			color = Color(1.0f, 1.0f, 1.0f, HIDDEN_ALPHA);
		}

		look.draw(DrawArgument(absp, color), alpha);

		// 分身残影（SHADOWPARTNER）：身后侧绘制半透明深色复制体（无额外资源，R6）。
		if (shadowpartner && state != State::DIED)
		{
			Point<int16_t> shadow_pos = absp + Point<int16_t>(facing_right ? -45 : 45, 0);
			look.draw(DrawArgument(shadow_pos, Color(0.3f, 0.3f, 0.3f, 0.6f)), alpha);
		}

		// 护盾特效（MAGIC_GUARD/HOLY_SHIELD）：循环动画居中绘制。
		for (auto& shield : shields)
			shield.second.draw(DrawArgument(absp), alpha);

		afterimage.draw(look.get_frame(), DrawArgument(absp, facing_right), alpha);

		if (ironbody)
		{
			float ibalpha = ironbody.alpha();
			float scale = 1.0f + ibalpha;
			float opacity = 1.0f - ibalpha;

			look.draw(DrawArgument(absp, scale, scale, opacity), alpha);
		}

		for (auto& pet : pets)
			if (pet.get_itemid())
				pet.draw(viewx, viewy, alpha);

		// If ever changing code for namelabel confirm placements with map 10000
		namelabel.draw(absp + Point<int16_t>(0, -4));
		chatballoon.draw(absp - Point<int16_t>(0, 85));

		effects.drawabove(absp, alpha);

		for (auto& number : damagenumbers)
			number.draw(viewx, viewy, alpha);
	}

	void Char::draw_preview(Point<int16_t> position, float alpha) const
	{
		look_preview.draw(position, false, Stance::Id::STAND1, Expression::Id::DEFAULT);
	}

	bool Char::update(const Physics& physics, float speed)
	{
		damagenumbers.remove_if(
			[](DamageNumber& number)
			{
				return number.update();
			}
		);

		// follow=false 的粒子按世界锚定：用本帧位移抵消相对偏移（MinMS Char.cpp:600-606）。
		const Point<int16_t> cur_position = get_position();
		const Point<int16_t> movement = cur_position - ring_prev_position;
		ring_prev_position = cur_position;
		effects.update(movement);
		chatballoon.update();
		invincible.update();
		ironbody.update();

		for (auto& shield : shields)
			shield.second.update();

		for (auto& pet : pets)
		{
			if (pet.get_itemid())
			{
				switch (state)
				{
				case State::LADDER:
				case State::ROPE:
					pet.set_stance(PetLook::Stance::HANG);
					break;
				case State::SWIM:
					pet.set_stance(PetLook::Stance::FLY);
					break;
				default:
					if (pet.get_stance() == PetLook::Stance::HANG || pet.get_stance() == PetLook::Stance::FLY)
						pet.set_stance(PetLook::Stance::STAND);

					break;
				}

				pet.update(physics, get_position());
			}
		}

		uint16_t stancespeed = 0;

		if (speed >= 1.0f / Constants::TIMESTEP)
			stancespeed = static_cast<uint16_t>(Constants::TIMESTEP * speed);

		afterimage.update(look.get_frame(), stancespeed);

		return look.update(stancespeed);
	}

	float Char::get_stancespeed() const
	{
		if (attacking)
			return get_real_attackspeed();

		switch (state)
		{
		case State::WALK:
			return static_cast<float>(std::abs(phobj.hspeed));
		case State::LADDER:
		case State::ROPE:
			return static_cast<float>(std::abs(phobj.vspeed));
		default:
			return 1.0f;
		}
	}

	float Char::get_real_attackspeed() const
	{
		int8_t speed = get_integer_attackspeed();

		return 1.7f - static_cast<float>(speed) / 10;
	}

	uint16_t Char::get_attackdelay(size_t no) const
	{
		uint8_t first_frame = afterimage.get_first_frame();
		uint16_t delay = look.get_attackdelay(no, first_frame);
		float fspeed = get_real_attackspeed();

		return static_cast<uint16_t>(delay / fspeed);
	}

	int8_t Char::update(const Physics& physics)
	{
		update(physics, 1.0f);

		return get_layer();
	}

	int8_t Char::get_layer() const
	{
		return is_climbing() ? 7 : phobj.fhlayer;
	}

	void Char::show_attack_effect(Animation toshow, int8_t z)
	{
		// 「系统设置 · 画面效果 · 技能使用时允许画面效果」（B4-3）。
		// 只吞技能施放/命中的装饰性特效；Char::show_effect_id 那条路径
		//（中毒/流血/无敌/被击/升级等状态可视化）**不受此开关影响**，
		// 否则关掉特效就等于关掉游戏信息，原生也不是这个语义。
		if (!Setting<SysOptSkillEffects>::get().load())
			return;

		// 技能施放特效按 WZ 自身帧 delay 播放（原生 CUser::ShowSkillEffect 取 UOL 后
		// 直接投 CAnimationDisplayer::Effect_SkillUse，不把角色攻速传给动画）。
		// 旧实现传 get_real_attackspeed()=1.7-speed/10 当 EffectLayer 动画倍速：
		// 高攻速角色 speed>=12 时倍速<0.5 让 18 帧法阵拖到 4 秒以上，speed>=17
		// 时倍速归零、timestep=0 使动画永不推进，紫圈永久残留（用户 2026-11-22
		// 龙咆哮实机现象）。怪物受击侧 Mob::show_effect 一直用固定 1.0f，两侧统一。
		effects.add(toshow, DrawArgument(facing_right), z, 1.0f);
	}

	void Char::show_falling_effect(Animation animation, Point<int16_t> start_offset,
		Point<int16_t> fall_delta, uint16_t delay_ms, uint16_t duration_ms,
		bool flip, int8_t z, bool follow, float angle)
	{
		if (!Setting<SysOptSkillEffects>::get().load())
			return;
		effects.add_fall(animation, start_offset, fall_delta, delay_ms,
			duration_ms, flip, z, follow, angle);
	}

	void Char::show_particle_effect(Animation animation, Point<int16_t> start_offset,
		Point<int16_t> fall_delta, uint16_t life_ms, int8_t z, bool flip,
		bool follow, float angle)
	{
		if (!Setting<SysOptSkillEffects>::get().load())
			return;
		effects.add_particle(animation, start_offset, fall_delta, life_ms,
			z, flip, follow, angle);
	}

	void Char::show_offset_effect(Animation animation, Point<int16_t> offset,
		uint16_t delay_ms, bool flip)
	{
		if (!Setting<SysOptSkillEffects>::get().load())
			return;
		effects.add_offset(animation, offset, delay_ms, flip);
	}

	void Char::show_effect_id(CharEffect::Id toshow)
	{
		effects.add(chareffects[toshow]);
	}

	void Char::show_iron_body()
	{
		ironbody.set_for(500);
	}

	void Char::show_damage(int64_t damage)
	{
		int16_t start_y = phobj.get_y() - 60;
		int16_t x = phobj.get_x() - 10;

		damagenumbers.emplace_back(DamageNumber::Type::TOPLAYER, damage, start_y, x);

		// 原生还原（2026-11-21）：对齐 Angel SetDamaged@0x9581a9 / MinMS
		// Char.cpp:1257-1277 —— Miss(damage<=0) 只飘伤害数字，不进警戒姿态、不闪；
		// 命中才 set_alerted(5000)，无敌窗 2000→1500ms（MinMS invincible.set_for(1500)）。
		if (damage <= 0)
			return;

		look.set_alerted(5000);
		invincible.set_for(1500);
	}

	void Char::set_shadowpartner(bool on)
	{
		shadowpartner = on;
	}

	void Char::set_hidden(bool h)
	{
		hidden = h;
	}

	bool Char::is_hidden() const
	{
		return hidden;
	}

	void Char::set_shield(int stat, int32_t skill_id)
	{
		// 护盾特效：从 Buff 下发的技能 id 动态定位 Skill/<ns>.img/skill/<strid>/effect；
		// 节点缺失（BeiDou 精简 img 常态）则跳过，不影响主流程（R6）。
		if (skill_id <= 0)
			return;

		std::string ns = string_format::skill_namespace(skill_id);
		std::string strid = (skill_id < 10000000)
			? string_format::extend_id(skill_id, 7)
			: std::to_string(skill_id);
		nl::node eff = nl::nx::Skill[ns + ".img"]["skill"][strid]["effect"];

		if (!eff)
			return;

		shields[stat] = Animation(eff);
	}

	void Char::clear_shield(int stat)
	{
		shields.erase(stat);
	}

	void Char::speak(const std::string& line)
	{
		chatballoon.change_text(line);
	}

	void Char::change_look(MapleStat::Id stat, int32_t id)
	{
		switch (stat)
		{
		case MapleStat::Id::SKIN:
			look.set_body(id);
			break;
		case MapleStat::Id::FACE:
			look.set_face(id);
			break;
		case MapleStat::Id::HAIR:
			look.set_hair(id);
			break;
		}
	}

	void Char::set_state(uint8_t statebyte)
	{
		if (statebyte % 2 == 1)
		{
			set_direction(false);

			statebyte -= 1;
		}
		else
		{
			set_direction(true);
		}

		Char::State newstate = by_value(statebyte);
		set_state(newstate);
	}

	void Char::set_expression(int32_t expid)
	{
		Expression::Id expression = Expression::byaction(expid);
		look.set_expression(expression);
	}

	void Char::attack(const std::string& action)
	{
		look.set_action(action);

		attacking = true;
		look.set_alerted(5000);
	}

	void Char::attack(Stance::Id stance)
	{
		look.attack(stance);

		attacking = true;
		look.set_alerted(5000);
	}

	void Char::attack(bool degenerate)
	{
		look.attack(degenerate);

		attacking = true;
		look.set_alerted(5000);
	}

	void Char::attack_final()
	{
		// 三端原生对齐（MinMS Char.cpp:1432-1439）：终极攻击播 OF 动作，
		// 结束 hold 态、进入 attacking、保持警戒。仅表现层，伤害由 s9 权威结算。
		look.attack_final();
		attacking = true;
		look.set_alerted(5000);
	}

	void Char::set_afterimage(int32_t skill_id)
	{
		int32_t weapon_id = look.get_equips().get_weapon();

		if (weapon_id <= 0)
			return;

		const WeaponData& weapon = WeaponData::get(weapon_id);

		std::string stance_name = Stance::names[look.get_stance()];
		int16_t weapon_level = weapon.get_equipdata().get_reqstat(MapleStat::Id::LEVEL);
		const std::string& ai_name = weapon.get_afterimage();

		afterimage = Afterimage(skill_id, ai_name, stance_name, weapon_level);
	}

	const Afterimage& Char::get_afterimage() const
	{
		return afterimage;
	}

	void Char::set_direction(bool f)
	{
		facing_right = f;
		look.set_direction(f);
	}

	void Char::set_state(State st)
	{
		state = st;

		Stance::Id stance = Stance::by_state(state);
		look.set_stance(stance);

		// 2026-11-25：原生 v83 死亡墓碑下落动画（MinMS Char.cpp:1492-1529）。
		// 由非死亡态切入 DIED 时播一次 Tomb.img/fall + TOMBSTONE 音效；
		// 复活（SetField→respawn→nullstate）会再切离 DIED，下次死亡可重播。
		if (st == State::DIED)
		{
			if (tomb_fall.get_frame_count() > 0)
				effects.add(tomb_fall);
			Sound(Sound::Name::TOMBSTONE).play();
		}
	}

	void Char::add_pet(uint8_t index, int32_t iid, const std::string& name, int32_t uniqueid, Point<int16_t> pos, uint8_t stance, int32_t fhid)
	{
		if (index > 2)
			return;

		pets[index] = PetLook(iid, name, uniqueid, pos, stance, fhid);
	}

	void Char::remove_pet(uint8_t index, bool hunger)
	{
		if (index > 2)
			return;

		pets[index] = PetLook();

		if (hunger)
		{
			// TODO: Empty
		}
	}

	void Char::move_pet(uint8_t index, Point<int16_t> pos, uint8_t stance)
	{
		if (index > 2)
			return;
		if (pets[index].get_itemid() == 0)
			return;

		pets[index].set_position(pos.x(), pos.y());
		pets[index].set_stance(stance);
	}

	void Char::rename_pet(uint8_t index, const std::string& name)
	{
		if (index > 2)
			return;
		if (pets[index].get_itemid() == 0)
			return;
		pets[index].set_name(name);
	}

	bool Char::is_invincible() const
	{
		return invincible == true;
	}

	bool Char::is_sitting() const
	{
		return state == State::SIT;
	}

	bool Char::is_climbing() const
	{
		return state == State::LADDER || state == State::ROPE;
	}

	bool Char::is_twohanded() const
	{
		return look.get_equips().is_twohanded();
	}

	Weapon::Type Char::get_weapontype() const
	{
		int32_t weapon_id = look.get_equips().get_weapon();

		if (weapon_id <= 0)
			return Weapon::Type::NONE;

		return WeaponData::get(weapon_id).get_type();
	}

	bool Char::getflip() const
	{
		return facing_right;
	}

	std::string Char::get_name() const
	{
		return namelabel.get_text();
	}

	CharLook& Char::get_look()
	{
		return look;
	}

	const CharLook& Char::get_look() const
	{
		return look;
	}

	bool Char::has_pet() const
	{
		for (const PetLook& pet : pets)
			if (pet.get_itemid() > 0)
				return true;
		return false;
	}

	const PetLook& Char::get_pet(uint8_t index) const
	{
		static PetLook empty;
		if (index >= 3)
			return empty;
		return pets[index];
	}

	PhysicsObject& Char::get_phobj()
	{
		return phobj;
	}

	void Char::init()
	{
		// [DBG-CHAR] 联调诊断桩：闭环后不再落文件（no-op），保留以便将来联调。
		auto dbg = [](const std::string&) {};

		CharLook::init();
		dbg("char:look-ok");

		// 2026-11-25：原生墓碑动画资源（Effect/Tomb.img/fall）
		tomb_fall = Animation(nl::nx::Effect["Tomb.img"]["fall"]);

		nl::node src = nl::nx::Effect["BasicEff.img"];
		dbg(std::string("char:src=") + (static_cast<bool>(src) ? "y" : "n"));

		for (auto iter : CharEffect::PATHS)
		{
			dbg(std::string("char:path ") + iter.second);
			auto resolved = src.resolve(iter.second);
			dbg(std::string("char:size ") + iter.second + " n=" + std::to_string(resolved.size()));
			chareffects.emplace(iter.first, resolved);
		}

		dbg("char:effects-done");
	}

	EnumMap<CharEffect::Id, Animation> Char::chareffects;
	Animation Char::tomb_fall;
}