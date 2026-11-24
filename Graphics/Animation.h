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

#include "Texture.h"

#include "../Template/Interpolated.h"
#include "../Template/Rectangle.h"

#include <vector>

namespace ms
{
	// A single frame within an animation.
	class Frame
	{
	public:
		Frame(nl::node src);
		// Timing / opacity from meta; bitmap from bitmap_src; origin kept from meta.
		Frame(nl::node meta, nl::node bitmap_src);
		Frame();

		void draw(const DrawArgument& args) const;

		uint8_t start_opacity() const;
		uint16_t start_scale() const;
		// MinMS 现代化：终值（oneshot 冻结时用到，对应 opacities/scales 的第二元素）。
		uint8_t end_opacity() const;
		uint16_t end_scale() const;
		uint16_t get_delay() const;
		Point<int16_t> get_origin() const;
		Point<int16_t> get_dimensions() const;
		Point<int16_t> get_head() const;
		Rectangle<int16_t> get_bounds() const;
		float opcstep(uint16_t timestep) const;
		float scalestep(uint16_t timestep) const;
		// 单帧节点里 a0 是静态 alpha；没有 a1 作为配对端点时，把
		// read_frame_timing 生成的 {a0, 255-a0} 互补 ramp 压平成 {a0, a0}，
		// 避免静态背景每 ~100ms 呼吸一次造成 ~9-10Hz 频闪（如 shineWood back/16）。
		void freeze_opacity_if_static();

	private:
		Texture texture;
		uint16_t delay;
		std::pair<uint8_t, uint8_t> opacities;
		std::pair<int16_t, int16_t> scales;
		Rectangle<int16_t> bounds;
		Point<int16_t> head;
		bool has_explicit_a0;
		bool has_explicit_a1;
	};

	// Class which consists of multiple textures to make an Animation.
	class Animation
	{
	public:
		Animation(nl::node source);
		// Same frame timing as source, but every frame draws texture_override.
		Animation(nl::node source, nl::node texture_override);
		// 三客户端校准后补齐的构造能力（对齐 MinMS Animation.cpp:194-259）：
		// start_frame=起始帧；force_zigzag=强制往返；settle_static=播一次后停末帧；
		// keep_order=按 WZ 子节点原始顺序装载、绕过 std::set 去重（保 uol/重复帧名）。
		Animation(nl::node source, int16_t start_frame, bool force_zigzag = false,
			bool settle_static = false, bool keep_order = false);
		Animation();

		bool update();
		bool update(uint16_t timestep);
		// loop=false：末帧（非 zigzag）/回到 0 帧（zigzag）时冻结并保持，不再回绕。
		bool update(uint16_t timestep, bool loop);
		void reset();

		void draw(const DrawArgument& arguments, float alpha) const;

		uint16_t get_delay(int16_t frame) const;
		uint16_t getdelayuntil(int16_t frame) const;

		// ---- MinMS 现代化动画 API（三客户端标注：非 083 原生复刻）----
		uint16_t total_delay() const;         // 单向逐帧 delay 之和
		int16_t get_frame_index() const;      // 当前帧下标
		size_t get_frame_count() const;       // 帧数
		bool is_zigzag() const;
		void set_zigzag(bool enabled);
		// 冻结到指定帧（delay=0xFFFF 哨兵），供「卷曲/蓄力峰值保持」类效果使用。
		void freeze_at(int16_t frame);
		// 解除冻结并反向播放（回摆/展开）。
		void thaw_and_reverse();
		void set_delay_scale(float scale);   // 慢放/快放（MinMS Papulatus 展开用）
		int32_t get_delay_left() const;      // 当前剩余 delay（冻结时 0xFFFF）
		int16_t get_framestep() const;
		Point<int16_t> get_origin() const;
		Point<int16_t> get_dimensions() const;
		Point<int16_t> get_head() const;
		Rectangle<int16_t> get_bounds() const;

	private:
		const Frame& get_frame() const;

		std::vector<Frame> frames;
		bool animated;
		bool zigzag;

		Nominal<int16_t> frame;
		Linear<float> opacity;
		Linear<float> xyscale;

		uint16_t delay;
		int16_t framestep;
		float opcstep;

		// MinMS 现代化状态（非 083 原生）：oneshot 冻结、慢放。
		bool settle_static;
		bool settled;
		float delay_scale;
	};
}