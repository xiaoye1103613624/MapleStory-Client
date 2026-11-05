#include "UIGMCommandList.h"
#include "UIChatBar.h"
#include "GMCommandList.h"
#include "../UI.h"
#include "../../Configuration.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/DrawArgument.h"
#include <algorithm>

namespace ms
{
	UIGMCommandList::UIGMCommandList()
	{
		position = Point<int16_t>(60, 60);
		dimension = Point<int16_t>(WIN_W, WIN_H);

		const auto& cmds = gm_commands();
		int row = 0;
		int col = 0;
		for (const auto& c : cmds)
		{
			entries.push_back({
				Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLUE, std::string("!") + c.name),
				static_cast<int16_t>(col),
				static_cast<int16_t>(row),
				c.name
			});
			col++;
			if (col >= 2) { col = 0; row++; }
		}
	}

	void UIGMCommandList::draw(float) const
	{
		// 背景（ColorBox 纯色，硬编码坐标，无 img 依赖，遵守 R5/R6 兜底）
		ColorBox(WIN_W, WIN_H, Color::Name::BLACK, 0.7f).draw(DrawArgument(position));
		// 标题
		Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "GM 命令 (点击填充)").draw(position + Point<int16_t>(8, 4));

		// 条目（按 scroll 偏移，两列）
		for (const auto& e : entries)
		{
			int16_t r = e.row - scroll;
			if (r < 0 || r >= ROWS_PER_PAGE) continue;
			int16_t x = position.x() + 8 + e.col * COL_W;
			int16_t y = position.y() + HEADER_H + 4 + r * LINE_H;
			e.text.draw(Point<int16_t>(x, y));
		}
	}

	void UIGMCommandList::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		int16_t total_rows = static_cast<int16_t>((entries.size() + 1) / 2);
		if (keycode == KeyAction::Id::UP)
			scroll = std::max<int16_t>(0, scroll - 1);
		else if (keycode == KeyAction::Id::DOWN)
			scroll = std::min<int16_t>(total_rows - ROWS_PER_PAGE, scroll + 1);
	}

	Cursor::State UIGMCommandList::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			Point<int16_t> local(cursorpos.x() - position.x(), cursorpos.y() - position.y());
			if (local.x() < 0 || local.x() > WIN_W || local.y() < HEADER_H || local.y() > WIN_H)
				return Cursor::State::IDLE;
			int16_t r = (local.y() - HEADER_H - 4) / LINE_H + scroll;
			int16_t c = (local.x() < (8 + COL_W)) ? 0 : 1;
			for (const auto& e : entries)
			{
				if (e.row == r && e.col == c)
				{
					if (auto chat = UI::get().get_element<UIChatBar>())
						chat->set_input("!" + e.cmd);
					return Cursor::State::IDLE;
				}
			}
		}
		return Cursor::State::IDLE;
	}

	UIElement::Type UIGMCommandList::get_type() const { return TYPE; }
}
