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

#include "Sprite.h"

#include "../Constants.h"

#include <map>
#include <list>
#include <algorithm>

namespace ms
{
	// A list of animations. Animations will be removed after all frames were displayed.
	class EffectLayer
	{
	public:
		void drawbelow(Point<int16_t> position, float alpha) const;
		void drawabove(Point<int16_t> position, float alpha) const;
		// movement = 本帧 owner 移动距离；follow=false 的粒子据此留在生成点（世界锚定）。
		void update(Point<int16_t> movement = {});
		void add(const Animation& effect, const DrawArgument& args, int8_t z, float speed, uint16_t delay_ms = 0);
		void add(const Animation& effect, const DrawArgument& args, int8_t z);
		void add(const Animation& effect, const DrawArgument& args);
		void add(const Animation& effect);

		// ---- MinMS 现代化特效族（三客户端标注：非 083 原生复刻）----
		// 下落/粒子：从 start_offset 起，按 fall_delta 在 duration_ms 内线性位移。
		void add_fall(const Animation& effect, Point<int16_t> start_offset,
			Point<int16_t> fall_delta, uint16_t delay_ms, uint16_t duration_ms,
			bool flip, int8_t z = 0, bool follow = true, float angle = 0.0f);
		// 粒子 = mirror 的 FallEffect（WZ 矩形内生成点随朝向镜像）。
		void add_particle(const Animation& effect, Point<int16_t> start_offset,
			Point<int16_t> fall_delta, uint16_t life_ms, int8_t z,
			bool flip, bool follow, float angle = 0.0f);
		// 固定偏移 + 延时爆发（Big Bang 类）。
		void add_offset(const Animation& effect, Point<int16_t> offset,
			uint16_t delay_ms, bool flip);

	private:
		class Effect
		{
		public:
			Effect(const Animation& a, const DrawArgument& args, float s, uint16_t delay = 0)
				: sprite(a, args), speed(s), wait_ms(delay) {}

			void draw(Point<int16_t> position, float alpha) const
			{
				if (wait_ms > 0)
					return;
				sprite.draw(position, alpha);
			}

			bool update()
			{
				if (wait_ms > 0)
				{
					wait_ms = wait_ms > Constants::TIMESTEP
						? wait_ms - Constants::TIMESTEP : 0;
					return false;
				}
				return sprite.update(
					static_cast<uint16_t>(Constants::TIMESTEP * speed)
				);
			}

		private:
			Sprite sprite;
			float speed;
			uint16_t wait_ms;
		};

		// 下落/粒子特效（对齐 MinMS EffectLayer.h:125-207）。
		class FallEffect
		{
		public:
			FallEffect(const Animation& a, Point<int16_t> start, Point<int16_t> delta,
				uint16_t delay, uint16_t duration, bool f, int8_t zz,
				bool follow, bool mirror, float angle)
				: animation(a), start_offset(start), fall_delta(delta),
				  delay_ms(delay), duration_ms(duration), elapsed_ms(0),
				  flip(f), z(zz), follow(follow), mirror_offset(mirror), angle(angle) {}

			void draw(Point<int16_t> position, float alpha) const
			{
				if (elapsed_ms < delay_ms)
					return;
				float t = duration_ms > 0
					? std::min(1.0f, static_cast<float>(elapsed_ms - delay_ms) / duration_ms)
					: 1.0f;
				Point<int16_t> off = start_offset + Point<int16_t>(
					static_cast<int16_t>(fall_delta.x() * t + 0.5f),
					static_cast<int16_t>(fall_delta.y() * t + 0.5f));
				if (mirror_offset && flip)
					off = Point<int16_t>(static_cast<int16_t>(-off.x()), off.y());
				animation.draw(DrawArgument(angle, position + off, flip, 1.0f), alpha);
			}

			bool update(Point<int16_t> movement)
			{
				animation.update();
				if (!follow)
					start_offset -= movement; // 世界锚定
				elapsed_ms = static_cast<uint16_t>(elapsed_ms + Constants::TIMESTEP);
				return elapsed_ms >= static_cast<uint16_t>(delay_ms + duration_ms);
			}

			int8_t get_z() const { return z; }

		private:
			Animation animation;
			Point<int16_t> start_offset;
			Point<int16_t> fall_delta;
			uint16_t delay_ms;
			uint16_t duration_ms;
			uint16_t elapsed_ms;
			bool flip;
			int8_t z;
			bool follow;
			bool mirror_offset;
			float angle;
		};

		// 固定偏移 + 延时爆发（对齐 MinMS EffectLayer.h:209-251）。
		class OffsetEffect
		{
		public:
			OffsetEffect(const Animation& a, Point<int16_t> off, uint16_t delay, bool f)
				: animation(a), offset(off), delay_ms(delay), elapsed_ms(0), flip(f), started(false) {}

			void draw(Point<int16_t> position, float alpha) const
			{
				if (!started)
					return;
				animation.draw(DrawArgument(position + offset, flip), alpha);
			}

			bool update()
			{
				elapsed_ms = static_cast<uint16_t>(elapsed_ms + Constants::TIMESTEP);
				if (!started)
				{
					if (elapsed_ms < delay_ms)
						return false;
					started = true;
					elapsed_ms = 0;
				}
				return animation.update();
			}

		private:
			Animation animation;
			Point<int16_t> offset;
			uint16_t delay_ms;
			uint16_t elapsed_ms;
			bool flip;
			bool started;
		};

		std::map<int8_t, std::list<Effect>> effects;
		std::list<FallEffect> falls;
		std::list<OffsetEffect> offsets;

		// 同屏已排队特效的总条数（各 z 层相加），供「画质调整」的密度上限判断。
		size_t count() const;
	};
}