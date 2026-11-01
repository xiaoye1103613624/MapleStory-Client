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

#include "../UIDragElement.h"

#include "../Components/Textfield.h"
// Q1 定案的共享框体件（半透明黑底 + 1px 灰边），聊天历史框与 tooltip 同判据。
#include "../Components/TextTooltip.h"

// TODO: Change these?
#include "../Messages.h"

namespace ms
{
	class UIChatBar : public UIDragElement<PosCHAT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHATBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIChatBar();

		void draw(float inter) const override;
		void update() override;

		Button::State button_pressed(uint16_t buttonid) override;

		bool is_in_range(Point<int16_t> cursor_position) const override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		enum MessageGroup : uint8_t
		{
			ALL,
			BATTLE,
			PARTY,
			FRIEND,
			GUILD,
			ALLIANCE,
			SPOUSE,
			GROUP,
			LENGTH
		};

		enum MessageType : uint8_t
		{
			UNK0,
			WHITE,
			RED,
			BLUE,
			YELLOW
		};

		bool has_input() const;
		bool is_classic() const;
		void toggle_view();
		// UIStateGame 的 TOGGLECHAT 分支要在输入行收起时把它打开（classic 的 Enter 是
		// 唯一入口，展开/缩放钮只管历史框），故必须公开。
		void toggle_input(bool enabled);
		void show_message(const char* message, MessageType type);
		void focus_whisper();
		void focus_whisper(const std::string& target);

		// TODO: Change these?
		void change_target(int32_t action) {}

	private:
		static constexpr int16_t INPUT_TEXT_HEIGHT = 18;
		static constexpr int16_t MIN_HEIGHT = 12;
		static constexpr int16_t MAX_HEIGHT = 467;

		bool indragrange(Point<int16_t> cursor_position) const override;
		bool intoprange(Point<int16_t> cursor_position) const;
		bool inbottomrange(Point<int16_t> cursor_position) const;
		bool inleftrange(Point<int16_t> cursor_position)  const;
		bool inrightrange(Point<int16_t> cursor_position) const;
		bool intopleftrange(Point<int16_t> cursor_position)  const;
		bool inbottomrightrange(Point<int16_t> cursor_position) const;
		bool intoprightrange(Point<int16_t> cursor_position)  const;
		bool inbottomleftrange(Point<int16_t> cursor_position) const;

		Point<int16_t> get_position() const;
		Point<int16_t> get_dragarea_position() const;
		Point<int16_t> get_input_position() const;
		Point<int16_t> get_input_text_position();
		void toggle_view(bool max, bool pressed);
		void update_view(bool pressed);
		void input_text_enter_callback(std::string message);
		void input_text_escape_callback();
		void close_input();
		void change_message(bool up);

		enum Buttons
		{
			BtMax,
			BtMin,
			BtChat,
			BtHelp,
			BtItemLink,
			BtChatEmoticon,
			BtOutChat,
			BtWhisper
		};

		enum DragDirection
		{
			NONE,
			DOWN,
			LEFT,
			DOWNLEFT
		};

		Texture drag;
		std::vector<Texture> max_textures;
		std::vector<Texture> min_textures;
		std::vector<Texture> input_textures;

		int16_t min_x;
		int16_t max_x;
		int16_t top_y;
		int16_t center_y;
		int16_t bottom_y;
		int16_t btMin_x;
		int16_t user_view_x;
		int16_t user_view_y;
		int16_t temp_view_x;
		int16_t temp_view_y;
		int16_t min_view_y;
		bool view_max;
		bool view_input;
		bool view_adjusted;
		bool position_adjusted;
		DragDirection drag_direction;
		Point<int16_t> temp_position;

		int16_t input_bg_x;
		int16_t input_bg_y;
		int16_t input_max_x;
		int16_t input_origin_x;
		int16_t input_origin_y;
		int16_t input_btns_x;
		Textfield input_text;

		struct Message
		{
			MessageGroup group;
			MessageType type;
			Text text;

			Message() : group(MessageGroup::ALL), type(MessageType::UNK0), text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "") {}
			Message(std::string text) : group(MessageGroup::ALL), type(MessageType::UNK0), text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, text) {}
			Message(MessageGroup group, MessageType type, Text text) : group(group), type(type), text(text) {}
		};

		std::vector<Message> message_history;
		std::vector<std::string> user_message_history;
		size_t user_message_history_index;

		bool classic_mode;
		Texture classic_input_bg;
		Texture classic_chat_target;

		// classic 聊天区布局。参考端底栏截帧（1282x142）与 StatusBar.img 逐件 SAD
		// 配准（SAD≤5，见 evidence/w4-chatbar/coords.md §1'）给出的**相对**几何：
		//   base/backgrnd(817x71) 顶 = 底栏顶，白输入带是它自身烘焙的一条，
		//     带顶 = 底栏顶 + 4、带内高 26（带下沿再往下 2px 深色分隔线）；
		//   base/chatTarget(81x20) 压在带内左上 (底栏左沿, 底栏顶 + 8)；
		//   base/chat(566x5) 在底栏顶 + 32（带下方那条半透明细条）；
		//   Basic.img/BtMin|BtMax(12x12) 在屏幕 (536, 661) = 行内 (536, 8)。
		// UIStatusBar 把 backgrnd 画在 (0, vh-71)，所以本行必须落在带内。旧公式
		// `vh-71-INPUT_ROW_H` 把整行抬到底栏上方 28px，正是用户看到的
		// 「对所有人显示在白色输入框上面、展开按钮也在上面」。
		//   输入行内含 chatTarget 目标框(81x20) + BtWhisper 切换箭头(12x19，压在框左沿)
		//   + 带下细条(base/chat 566x5) + 输入文本 + BtMax/BtMin(12x12)；
		//   历史框位于输入行上方（user_view_y 高）。
		static constexpr int16_t CLASSIC_BAR_H = 71;
		static constexpr int16_t INPUT_BAND_TOP = 4;
		static constexpr int16_t INPUT_ROW_H = 26;
		static constexpr int16_t MAX_ROW_W = 560;
		static constexpr int16_t CHAT_TARGET_W = 81;
		static constexpr int16_t CHAT_TARGET_H = 20;
		static constexpr int16_t TARGET_BT_W = 12;
		// chatTarget 与 BtWhisper(12x19) 同顶（行内 y=4 → 屏幕 657）
		static constexpr int16_t TARGET_BOX_Y = 4;
		// 目标文字「对所有人」的行内**墨迹顶** = 8（框内区 4..24、字高 11 ⇒ 居中），
		// 绘制点由 Text::y_for_ink_top 按当前对象名反解（R27-B1b）。
		// 上一版把 bitmap_top 手抄成 11（PIL 口径，SimHei 11px 的 CJK 实测 9~10）
		// ⇒ 常量写成 draw_y=2，字实际低 2~3px。
		static constexpr int16_t TARGET_TEXT_INK_TOP = 8;
		// 展开/缩放钮(12x12)：参考端实测屏幕 (536, 661)，行内即 (536, 8)。底栏是定宽
		// 817 的贴图，所以原生用的是固定左偏移，这里跟着用固定值而不是右锚。
		static constexpr int16_t LOG_BT_X = 536;
		static constexpr int16_t LOG_BT_Y = 8;
		// base/chat 带下细条：底栏顶 +32 = 行内 28 = INPUT_ROW_H + 2
		static constexpr int16_t UNDERLINE_Y = INPUT_ROW_H + 2;
		// 参考端目标文字左对齐、紧跟箭头右侧（框内 x≈6，箭头占 0..12）
		static constexpr int16_t TARGET_TEXT_X = 14;

		// 聊天目标：BtWhisper 箭头钮 + chatTarget 框体本身都可点，循环切换
		// （对所有人→密语→组队→工会）；历史框展开/收起用 Basic.img 的
		// BtMax(+) / BtMin(-) 真按钮同位互斥显示。
		bool classic_log_open = false;
		TooltipBackdrop classic_log_backdrop;
		Text classic_target_text;
		uint8_t classic_chat_target_id = 0;
		void cycle_chat_target();
		void set_classic_log(bool open);

#if LOG_LEVEL >= LOG_UI
		ColorBox dimension_box;
		ColorBox dragarea_box;
		ColorBox top_box;
		ColorBox bottom_box;
		ColorBox left_box;
		ColorBox right_box;
		ColorBox input_box;
#endif
	};
}