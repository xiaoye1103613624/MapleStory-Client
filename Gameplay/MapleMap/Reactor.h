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

#include "../../Graphics/Animation.h"
#include "../../Audio/Audio.h"

#include <map>

namespace ms
{
	class Reactor : public MapObject
	{
	public:
		Reactor(int32_t oid, int32_t rid, int8_t state, Point<int16_t> position);

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics);

		void set_state(int8_t state);
		void destroy(int8_t state, Point<int16_t> position);

		bool is_hittable() const;

		// Check if this mob collides with the specified rectangle
		bool is_in_range(const Rectangle<int16_t>& range) const;

	private:
		int32_t oid;
		int32_t rid;
		int8_t state;
		// 按状态播 Sound/Reactor.img/{rid}/{state}/Hit。
		// 路径经 _tools/summon_probe 解析 Sound/Reactor.img 取证（2026-11-03，
		// 与 IDA sub_989ADD 的 "Sound/Reactor.img/%s/%s"+"Hit" 吻合）。
		void play_state_sound(int8_t st);
		// 加载当前 state 的 idle/悬浮动画（优先 state/default，否则 state 本体多帧）。
		// 依据：native-map-object-re §3.2（string id 960 "default"）；
		// 079/s9 Reactor.wz 实测无 default 子键，523+ 反应堆靠 state 下 0..N canvas 循环。
		void reload_idle();

		nl::node src;
		std::map<int8_t, Animation> animations;
		bool animation_ended;

		bool active;
		bool hittable;
		bool dead;

		Animation normal;
	};
}
