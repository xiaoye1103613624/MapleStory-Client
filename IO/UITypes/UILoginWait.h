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
//	You should have received a copy of the GNU Affero General Public License		//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Texture.h"

#include <cstdint>
#include <functional>
#include <vector>

namespace ms
{
	class UILoginWait : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::LOGINWAIT;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UILoginWait();
		UILoginWait(std::function<void()> okhandler);

		UIElement::Type get_type() const override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		void update() override;
		void draw(float inter) const override;

		// 请求结束等待：进度条必须先走完 0→100%（本轮未满则补满），
		// 再由 update() 收尾（deactivate + 执行续场逻辑）。
		// 本端 Journey 设计：满条后才切页；未满就拆窗会「没到 100% 就跳页」。
		// 083 无独立 Loading 窗类（等待态在 CLogin::Update @0x5f4c16 的 10s 超时链），
		// 此行为不据 083 背书；10s 自动回标题是否补齐属决策点 D-D。
		// 无进度条素材时立即执行续场逻辑（退化为旧行为）。
		void begin_finish(std::function<void()> oncomplete);

		void close();
		std::function<void()> get_handler();

	protected:
		Button::State button_pressed(uint16_t id) override;

	private:
		enum Buttons : uint16_t
		{
			BtCancel
		};

		// 进度条一帧：Login.img/Notice/Loading/bar/0..10 真位图 109x8，每帧多点亮
		// 一格（0%..100%）；.nx 实测 0..9 帧 delay=100ms、10 帧缺省 100ms。
		struct BarFrame
		{
			Texture texture;
			uint16_t delay = 100;	// 帧延时（ms），WZ 缺省与 Animation 一致取 100
		};

		std::vector<BarFrame> bar_frames;
		Point<int16_t> bar_pos;
		uint16_t fill_elapsed = 0;	// 已播放毫秒数
		uint16_t fill_total = 0;	// 满条总毫秒数（各帧延时之和）
		std::function<void()> oncomplete;	// 满条后的续场逻辑
		bool finish_pending = false;
		bool finished = false;

		std::function<void()> okhandler;
	};
}
