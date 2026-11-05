#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"
#include <vector>
#include <string>

namespace ms
{
	// GM 命令列表窗：列出 GMCommandList 全部命令（两列），点击填充聊天框输入行。
	// 纯文本 + ColorBox 背景，硬编码坐标，无 img 依赖（遵守 R5/R6 兜底）。
	class UIGMCommandList : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GM_COMMANDLIST;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UIGMCommandList();

		void draw(float inter) const override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		UIElement::Type get_type() const override;

	private:
		struct Entry
		{
			Text text;
			int16_t col;
			int16_t row;
			std::string cmd;
		};

		std::vector<Entry> entries;
		int16_t scroll = 0;

		static constexpr int16_t WIN_W = 300;
		static constexpr int16_t WIN_H = 380;
		static constexpr int16_t LINE_H = 16;
		static constexpr int16_t COL_W = 145;
		static constexpr int16_t HEADER_H = 24;
		static constexpr int16_t ROWS_PER_PAGE = (WIN_H - HEADER_H - 8) / LINE_H;
	};
}
