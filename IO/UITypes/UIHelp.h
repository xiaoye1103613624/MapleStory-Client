//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"
#include "../Components/Slider.h"
#include "../../Template/Range.h"

#include <string>
#include <vector>

namespace ms
{
	/// Classic StatusBar「帮助」窗：内容是真素材 Npt_GuideLine.img/NptGuide 的
	/// Text00..Text48（交易/拍卖说明，纯中文文本，本包零位图），用滚动条翻页。
	class UIHelp : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::NOTICE;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIHelp();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
		void send_scroll(double yoffset) override;
		void update_screen(int16_t new_width, int16_t new_height) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BT_CLOSE
		};

		static std::string guide_line(std::string raw);
		void rebuild();
		void on_scroll(bool upwards);

		/// UtilDlgEx 窗框：t(529x28) 顶 + c(529x20) 竖向平铺正文 + s(529x58) 底。
		/// 与 UINpcTalk 同一族窗框（`UINpcTalk.cpp:75-80` 用的就是 t/c/s/bar）。
		static constexpr int16_t FRAME_W = 529;
		static constexpr int16_t TOP_H = 28;
		static constexpr int16_t BOTTOM_H = 58;
		static constexpr int16_t BODY_H = 300;
		static constexpr int16_t TEXT_X = 16;
		static constexpr int16_t TEXT_W = 452;
		static constexpr int16_t TEXT_TOP = TOP_H + 12;
		static constexpr int16_t LINE_H = 17;
		/// 「关闭」是 UtilDlgEx/BtClose（85x19 文字钮，烤着「关闭」二字），落在
		/// 底部 s 条内：s 顶 = TOP_H+BODY_H，钮在条内 y=20（58-19=39，20 居中）。
		/// 与 UINpcTalk 的 CLOSE 位（`UINpcTalk.cpp:649` = height+48，即条内 +20）一致。
		static constexpr int16_t CLOSE_X = 9;
		static constexpr int16_t CLOSE_Y = TOP_H + BODY_H + 20;

		Texture top_frame;
		Texture bottom_frame;
		/// 原生正文底是半透明白（c 条 alpha 17..255、色值约 232,232,229），不是纯白方块。
		Texture body_fill;
		/// 每条 NptGuide/TextNN 一个 Text（maxwidth 自动换行，height 即换行后占位）。
		std::vector<Text> paragraphs;
		/// paragraphs 各自的累计纵向起点 + 总高，用于滚动与裁剪。
		std::vector<int16_t> offsets;
		int16_t content_h = 0;
		int16_t scroll = 0;
		Slider slider;
	};
}
