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

#include "../UIElement.h"

#include "../Components/IconCover.h"
#include "../../Graphics/Text.h"

#include <string>
#include <vector>

namespace ms
{
	class BuffIcon
	{
	public:
		BuffIcon(int32_t buff, int32_t dur, bool show_timer);

		void draw(Point<int16_t> position, float alpha) const;
		bool update();

	private:
		static const uint16_t FLASH_TIME = 3'000;
		// 参考端（ijl15!FormatBuffTime / DrawBuffTimeLabelOnCanvas）把倒计时画在
		// 32x32 的图标画布上：文字顶边 y=21，水平居中，黄色正文 + 黑色 1px 描边。
		static constexpr int16_t ICON_SIZE = 32;
		static constexpr int16_t LABEL_TOP = 21;

		// 剩余时间格式化，规则与参考端一致：
		//   >= 1h -> "%dh%dm"；>= 1m -> "%dm%ds"；否则 "%ds"；秒数向上取整且最小为 1。
		static std::string format_time(int32_t milliseconds);

		// 仅在秒数变化时重排文本，避免每帧重建 layout。
		void refresh_label();

		Texture icon;
		IconCover cover;
		int32_t buffid;
		int32_t duration;
		int32_t shown_seconds;
		Linear<float> opacity;
		float opcstep;
		bool show_timer;
		Text timelabel;
		Text timeoutline;
	};


	class UIBuffList : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::BUFFLIST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIBuffList();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;

		Cursor::State send_cursor(bool pressed, Point<int16_t> position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		// Wave 3：右键点掉增益图标 = 取消该 buff（s9 CancelBuffHandler 只认
		// int sourceid=技能id，回 s2c CANCEL_BUFF(33) 后列表自然移除）。
		// 命中判定必须自算：基类 is_in_range 用 (position, position+dimension)，
		// dimension=(position.x,32) 使矩形从条带**右缘**起向屏外延伸 ——
		// 图标所在条带 [x-32(n-1), x+32) 的左段永远打不中本窗，
		// 右键全落进地图 ContextMenu。
		bool is_in_range(Point<int16_t> cursor_position) const override;
		void rightclick(Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

		void add_buff(int32_t buffid, int32_t duration);
		void remove_buff(int32_t buffid);

	private:
		std::unordered_map<int32_t, BuffIcon> icons;
		// draw() 迭代顺序快照（draw 是 const → mutable）。unordered_map 在
		// update() 擦除过期项后迭代序可变，右键命中必须按"这一帧实际画出来
		// 的顺序"索引，不能重拼迭代器。
		mutable std::vector<int32_t> draw_order;
		// ui.buff_timer：是否在图标上叠加剩余时间文本。
		bool show_timer;
	};
}