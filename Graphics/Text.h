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

#include "DrawArgument.h"

#include <map>
#include <vector>

namespace ms
{
	class Text
	{
	public:
		enum Font
		{
			A11M,
			A11B,
			A12M,
			A12B,
			A13M,
			A13B,
			A14M,
			A14B,
			A15M,
			A15B,
			A18M,
			A18B,
			NUM_FONTS
		};

		enum Alignment
		{
			LEFT,
			CENTER,
			RIGHT
		};

		enum Background
		{
			NONE,
			NAMETAG
		};

		class Layout
		{
		public:
			struct Word
			{
				size_t first;
				size_t last;
				Font font;
				Color::Name color;
			};

			struct Line
			{
				std::vector<Word> words;
				Point<int16_t> position;
			};

			Layout(const std::vector<Line>& lines, const std::vector<int16_t>& advances, int16_t width, int16_t height, int16_t endx, int16_t endy);
			Layout();

			int16_t width() const;
			int16_t height() const;
			int16_t advance(size_t index) const;
			Point<int16_t> get_dimensions() const;
			Point<int16_t> get_endoffset() const;

			// 字节 pos 布局后相对绘制原点的位置。advances 本来就是按字节索引的，
			// 缺的只是「这个字节落在哪一行」，所以按行的词区间来定位。
			// 只保证 pos 落在某行 [首词起点, 末词终点) 内时准确（取起点用）。
			Point<int16_t> position_of(size_t pos) const;

			using iterator = std::vector<Line>::const_iterator;
			iterator begin() const;
			iterator end() const;

		private:
			std::vector<Line> lines;
			std::vector<int16_t> advances;
			Point<int16_t> dimensions;
			Point<int16_t> endoffset;
		};

		Text(Font font, Alignment alignment, Color::Name color, Background background, const std::string& text = "", uint16_t maxwidth = 0, bool formatted = true, int16_t line_adj = 0);
		Text(Font font, Alignment alignment, Color::Name color, const std::string& text = "", uint16_t maxwidth = 0, bool formatted = true, int16_t line_adj = 0);
		Text();

		void draw(const DrawArgument& args) const;
		void draw(const DrawArgument& args, const Range<int16_t>& vertical) const;

		void change_text(const std::string& text);
		void change_color(Color::Name color);
		void set_background(Background background);

		bool empty() const;
		size_t length() const;
		int16_t width() const;
		int16_t height() const;
		uint16_t advance(size_t pos) const;
		Point<int16_t> dimensions() const;
		Point<int16_t> endoffset() const;
		const std::string& get_text() const;

		// 正文里某段行内可点文本（NPC 对话的 #L..#l 选项）只能从最终布局反取坐标：
		// 它不是独立控件，位置由整段正文的折行结果决定。返回相对本 Text 绘制原点。
		Point<int16_t> position_of(size_t pos) const;

		// 文本垂直落位的唯一真源（R27-B1b）。
		//
		// 渲染器画字是 char_y = draw_y + 基线偏移 - bitmap_top，一行的墨迹顶 =
		// 基线 - 本行最高字形的 bitmap_top，所以「把字画到原生帧上量到的那个位置」
		// 必须减掉这段文字自己的度量。以前每个窗口把 linespace / bitmap_top 手抄
		// 进注释再算一遍，抄错一次就整列偏低 —— 实测 SimHei 的 CJK bitmap_top 是
		// 9/10 而不是各处注释写的 11/12，能力窗、底栏职业名、金币值、聊天目标文字
		// 因此统一低了 2~3px。
		// 现在窗口侧只记「量到的墨迹顶」，换算一律走下面的实例方法（它按当前
		// text 逐字形取最大 bitmap_top，SimHei 11px 下 CJK 分 9/10 两档，
		// 「新手」的顶是「新」= 10，选类别会错 1px）。
		static int16_t linespace(Font font);
		// 本 Text 当前文字的「绘制点 y → 墨迹顶」偏移 = linespace - max(bitmap_top)。
		int16_t ink_offset() const;
		// 墨迹顶（截图/贴图坐标系）→ 本 Text::draw 的 y。多行文字取所有行的最大
		// bitmap_top，量单行时最准。
		int16_t y_for_ink_top(int16_t ink_top) const;

	private:
		void reset_layout();

		Font font;
		Alignment alignment;
		Color::Name color;
		Background background;
		Layout layout;
		uint16_t maxwidth;
		bool formatted;
		std::string text;
		int16_t line_adj;
	};
}