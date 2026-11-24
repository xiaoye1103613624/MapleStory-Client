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
#include "Animation.h"

#include "../Constants.h"

#include "../Util/Misc.h"

#include <set>

namespace ms
{
	static void read_frame_timing(nl::node src, uint16_t& delay,
		std::pair<uint8_t, uint8_t>& opacities, std::pair<int16_t, int16_t>& scales)
	{
		delay = src["delay"];

		if (delay == 0)
			delay = 100;

		bool hasa0 = src["a0"].data_type() == nl::node::type::integer;
		bool hasa1 = src["a1"].data_type() == nl::node::type::integer;

		if (hasa0 && hasa1)
		{
			opacities = { src["a0"], src["a1"] };
		}
		else if (hasa0)
		{
			uint8_t a0 = src["a0"];
			opacities = { a0, 255 - a0 };
		}
		else if (hasa1)
		{
			uint8_t a1 = src["a1"];
			opacities = { 255 - a1, a1 };
		}
		else
		{
			opacities = { 255, 255 };
		}

		bool hasz0 = src["z0"].data_type() == nl::node::type::integer;
		bool hasz1 = src["z1"].data_type() == nl::node::type::integer;

		if (hasz0 && hasz1)
			scales = { src["z0"], src["z1"] };
		else if (hasz0)
			scales = { src["z0"], 0 };
		else if (hasz1)
			scales = { 100, src["z1"] };
		else
			scales = { 100, 100 };
	}

	Frame::Frame(nl::node src)
	{
		texture = src;
		bounds = src;
		head = src["head"];
		read_frame_timing(src, delay, opacities, scales);
		has_explicit_a0 = src["a0"].data_type() == nl::node::type::integer;
		has_explicit_a1 = src["a1"].data_type() == nl::node::type::integer;
	}

	Frame::Frame(nl::node meta, nl::node bitmap_src)
	{
		texture = bitmap_src;
		bounds = meta;
		head = meta["head"];
		read_frame_timing(meta, delay, opacities, scales);
		has_explicit_a0 = meta["a0"].data_type() == nl::node::type::integer;
		has_explicit_a1 = meta["a1"].data_type() == nl::node::type::integer;

		// Keep map-object anchor from the original frame, not the override bitmap.
		if (meta["origin"].data_type() == nl::node::type::vector)
		{
			Point<int16_t> desired = meta["origin"];
			Point<int16_t> current = texture.get_origin();
			texture.shift(current - desired);
		}
	}

	Frame::Frame()
	{
		delay = 0;
		opacities = { 0, 0 };
		scales = { 0, 0 };
		has_explicit_a0 = false;
		has_explicit_a1 = false;
	}

	void Frame::freeze_opacity_if_static()
	{
		// 单帧节点且只有显式 a0（无 a1 配对端点）时，read_frame_timing 生成的
		// {a0, 255-a0} 是给多帧交叉淡化用的互补端点；单帧没有下一帧接手，
		// 于是每一轮 delay 到点 opacity 都从 255-a0 骤回 a0，形成 ~1/delay
		// 的呼吸频闪（shineWood back/16: a0=235 -> 20 -> 235, ~100ms/9.4Hz）。
		//
		// 【收紧】只压平落在人眼敏感频闪带的高速 ramp（delay<=150ms，约 >=6.7Hz）。
		// delay 更长的单帧 a0 是设计好的缓慢发光/明暗呼吸（如 moltenRock
		// ani/1/10 a0=0 delay=1900ms 的 0.5Hz 岩浆渐亮渐暗），必须原样保留，
		// 否则会把官方特效压成恒定透明——那就是误杀。
		if (has_explicit_a0 && !has_explicit_a1 && delay <= 150)
		{
			uint8_t a0 = opacities.first;
			opacities = { a0, a0 };
		}
	}

	void Frame::draw(const DrawArgument& args) const
	{
		texture.draw(args);
	}

	uint8_t Frame::start_opacity() const
	{
		return opacities.first;
	}

	uint16_t Frame::start_scale() const
	{
		return scales.first;
	}

	uint8_t Frame::end_opacity() const
	{
		return opacities.second;
	}

	uint16_t Frame::end_scale() const
	{
		return scales.second;
	}

	uint16_t Frame::get_delay() const
	{
		return delay;
	}

	Point<int16_t> Frame::get_origin() const
	{
		return texture.get_origin();
	}

	Point<int16_t> Frame::get_dimensions() const
	{
		return texture.get_dimensions();
	}

	Point<int16_t> Frame::get_head() const
	{
		return head;
	}

	Rectangle<int16_t> Frame::get_bounds() const
	{
		return bounds;
	}

	float Frame::opcstep(uint16_t timestep) const
	{
		return timestep * static_cast<float>(opacities.second - opacities.first) / delay;
	}

	float Frame::scalestep(uint16_t timestep) const
	{
		return timestep * static_cast<float>(scales.second - scales.first) / delay;
	}

	Animation::Animation(nl::node src)
		: Animation(src, 0, false, false, false)
	{
		// 委托完整构造（三客户端校准后统一走一条装载路径，零行为变化）。
	}

	Animation::Animation(nl::node src, int16_t start_frame, bool force_zigzag,
		bool settle_static, bool keep_order)
		: settle_static(settle_static), settled(false), delay_scale(1.0f)
	{
		bool istexture = src.data_type() == nl::node::type::bitmap;

		if (istexture)
		{
			frames.push_back(src);
		}
		else if (keep_order)
		{
			// 按 WZ 子节点原始顺序逐帧装载，不做 set 去重/排序（MinMS Animation.cpp:212-225）。
			// 用于 uol/重复帧名表达的回摆 clip；三客户端资源实证当前样本无重复，但保留接口。
			for (auto sub : src)
			{
				if (sub.data_type() == nl::node::type::bitmap)
					frames.push_back(sub);
			}
			if (frames.empty())
				frames.push_back(Frame());
		}
		else
		{
			std::set<int16_t> frameids;

			for (auto sub : src)
			{
				if (sub.data_type() == nl::node::type::bitmap)
				{
					int16_t fid = string_conversion::or_default<int16_t>(sub.name(), -1);

					if (fid >= start_frame)
						frameids.insert(fid);
				}
			}

			for (auto& fid : frameids)
			{
				auto sub = src[std::to_string(fid)];
				frames.push_back(sub);
			}

			if (frames.empty())
				frames.push_back(Frame());
		}

		animated = frames.size() > 1;

		// 单帧静态背景：把「仅 a0」的互补 ramp 压平成恒定 alpha，
		// 消除 shineWood 类背景每 ~100ms 呼吸一次的频闪。
		if (frames.size() == 1)
		{
			frames[0].freeze_opacity_if_static();
		}

		zigzag = force_zigzag || src["zigzag"].get_bool();

		reset();
	}

	Animation::Animation(nl::node src, nl::node texture_override)
		: settle_static(false), settled(false), delay_scale(1.0f)
	{
		bool istexture = src.data_type() == nl::node::type::bitmap;
		bool has_override = texture_override.data_type() == nl::node::type::bitmap;

		if (istexture)
		{
			if (has_override)
				frames.push_back(Frame(src, texture_override));
			else
				frames.push_back(src);
		}
		else
		{
			std::set<int16_t> frameids;

			for (auto sub : src)
			{
				if (sub.data_type() == nl::node::type::bitmap)
				{
					int16_t fid = string_conversion::or_default<int16_t>(sub.name(), -1);

					if (fid >= 0)
						frameids.insert(fid);
				}
			}

			for (auto& fid : frameids)
			{
				auto sub = src[std::to_string(fid)];

				if (has_override)
					frames.push_back(Frame(sub, texture_override));
				else
					frames.push_back(sub);
			}

			if (frames.empty())
				frames.push_back(Frame());
		}

		animated = frames.size() > 1;

		if (frames.size() == 1)
		{
			frames[0].freeze_opacity_if_static();
		}

		zigzag = src["zigzag"].get_bool();

		reset();
	}

	Animation::Animation()
		: settle_static(false), settled(false), delay_scale(1.0f)
	{
		animated = false;
		zigzag = false;

		frames.push_back(Frame());

		reset();
	}

	void Animation::reset()
	{
		frame.set(0);
		opacity.set(frames[0].start_opacity());
		xyscale.set(frames[0].start_scale());
		delay = frames[0].get_delay();
		framestep = 1;
		settled = false;
		delay_scale = 1.0f;
	}

	void Animation::draw(const DrawArgument& args, float alpha) const
	{
		int16_t interframe = frame.get(alpha);
		float interopc = opacity.get(alpha) / 255;
		float interscale = xyscale.get(alpha) / 100;

		bool modifyopc = interopc != 1.0f;
		bool modifyscale = interscale != 1.0f;

		if (modifyopc || modifyscale)
			frames[interframe].draw(args + DrawArgument(interscale, interscale, interopc));
		else
			frames[interframe].draw(args);
	}

	bool Animation::update()
	{
		return update(Constants::TIMESTEP, true);
	}

	bool Animation::update(uint16_t timestep)
	{
		return update(timestep, true);
	}

	bool Animation::update(uint16_t timestep, bool loop)
	{
		// oneshot 冻结哨兵：settled 或 0xFFFF 已到达 → 不再推进。
		if (settled || delay == 0xFFFF)
			return true;

		const Frame& framedata = get_frame();

		opacity += framedata.opcstep(timestep);

		if (opacity.last() < 0.0f)
			opacity.set(0.0f);
		else if (opacity.last() > 255.0f)
			opacity.set(255.0f);

		xyscale += framedata.scalestep(timestep);

		if (xyscale.last() < 0.0f)
			opacity.set(0.0f);

		// delay_scale 慢放（MinMS Animation.cpp:351-369，Papulatus 展开用）。
		uint16_t eff_timestep = timestep;
		if (delay_scale > 1.0f)
		{
			uint32_t scaled = static_cast<uint32_t>(delay * delay_scale);
			if (scaled > 65000)
				scaled = 65000;
			// 逐 tick 扣除实际时间，未耗尽则本帧不推进。
			static thread_local uint32_t scaled_left = 0;
			if (scaled_left == 0)
				scaled_left = static_cast<uint32_t>(scaled);
			if (scaled_left > timestep)
			{
				scaled_left -= timestep;
				frame.normalize();
				return false;
			}
			eff_timestep = static_cast<uint16_t>(timestep - scaled_left);
			scaled_left = 0;
		}

		if (eff_timestep >= delay)
		{
			int16_t lastframe = static_cast<int16_t>(frames.size() - 1);
			int16_t nextframe;
			bool ended;

			// settle_static：播到末帧后冻结（一次性技能特效「播出即保持」）。
			if (settle_static && framestep == 1 && frame == lastframe)
			{
				opacity.set(framedata.end_opacity());
				xyscale.set(framedata.end_scale());
				settled = true;
				delay = 0xFFFF;
				return true;
			}

			if (zigzag && lastframe > 0)
			{
				if (framestep == 1 && frame == lastframe)
				{
					framestep = -framestep;
					ended = false;
				}
				else if (framestep == -1 && frame == 0)
				{
					if (!loop)
					{
						// oneshot ping-pong 完成 → 冻结在 0 帧（≈站姿）。
						opacity.set(framedata.end_opacity());
						xyscale.set(framedata.end_scale());
						delay = 0xFFFF;
						return true;
					}
					framestep = -framestep;
					ended = true;
				}
				else
				{
					ended = false;
				}

				nextframe = frame + framestep;
			}
			else
			{
				if (frame == lastframe)
				{
					if (!loop)
					{
						opacity.set(framedata.end_opacity());
						xyscale.set(framedata.end_scale());
						delay = 0xFFFF;
						return true;
					}
					nextframe = 0;
					ended = true;
				}
				else
				{
					nextframe = frame + 1;
					ended = false;
				}
			}

			uint16_t delta = eff_timestep - delay;
			float threshold = static_cast<float>(delta) / eff_timestep;
			frame.next(nextframe, threshold);

			delay = frames[nextframe].get_delay();

			if (delay >= delta)
				delay -= delta;

			opacity.set(frames[nextframe].start_opacity());
			xyscale.set(frames[nextframe].start_scale());

			return ended;
		}
		else
		{
			frame.normalize();

			delay -= eff_timestep;

			return false;
		}
	}

	uint16_t Animation::get_delay(int16_t frame_id) const
	{
		return frame_id < frames.size() ? frames[frame_id].get_delay() : 0;
	}

	uint16_t Animation::getdelayuntil(int16_t frame_id) const
	{
		uint16_t total = 0;

		for (int16_t i = 0; i < frame_id; i++)
		{
			if (i >= static_cast<int16_t>(frames.size()))
				break;

			// 修复共享 bug（MinMS Animation.cpp:675 同源）：原写 frames[frame_id]
			// 导致每帧重复加同一帧 delay。应累加 frames[i]。
			total += frames[i].get_delay();
		}

		return total;
	}

	uint16_t Animation::total_delay() const
	{
		uint16_t total = 0;
		for (const auto& f : frames)
			total += f.get_delay();
		return total;
	}

	int16_t Animation::get_frame_index() const
	{
		return frame.get();
	}

	size_t Animation::get_frame_count() const
	{
		return frames.size();
	}

	bool Animation::is_zigzag() const
	{
		return zigzag;
	}

	void Animation::set_zigzag(bool enabled)
	{
		zigzag = enabled;
	}

	void Animation::freeze_at(int16_t frame_id)
	{
		int16_t last = static_cast<int16_t>(frames.empty() ? 0 : frames.size() - 1);
		if (frame_id < 0) frame_id = 0;
		if (frame_id > last) frame_id = last;
		frame.set(frame_id);
		opacity.set(frames[frame_id].start_opacity());
		xyscale.set(frames[frame_id].start_scale());
		delay = 0xFFFF; // 冻结哨兵
		framestep = 1;
		settled = false;
	}

	void Animation::thaw_and_reverse()
	{
		if (frames.size() <= 1)
			return;
		zigzag = true;
		framestep = -1;
		settled = false;
		delay = frames[frame.get()].get_delay();
		opacity.set(frames[frame.get()].start_opacity());
		xyscale.set(frames[frame.get()].start_scale());
	}

	void Animation::set_delay_scale(float scale)
	{
		delay_scale = scale;
	}

	int32_t Animation::get_delay_left() const
	{
		return delay;
	}

	int16_t Animation::get_framestep() const
	{
		return framestep;
	}

	Point<int16_t> Animation::get_origin() const
	{
		return get_frame().get_origin();
	}

	Point<int16_t> Animation::get_dimensions() const
	{
		return get_frame().get_dimensions();
	}

	Point<int16_t> Animation::get_head() const
	{
		return get_frame().get_head();
	}

	Rectangle<int16_t> Animation::get_bounds() const
	{
		return get_frame().get_bounds();
	}

	const Frame& Animation::get_frame() const
	{
		return frames[frame.get()];
	}
}