//////////////////////////////////////////////////////////////////////////////////
//	SetTooltip — 套装 companion 面板
//	布局：[悬停装备 tip] → [套装 tip] → [成长 tip?] → [已穿比较 tip]
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TextTooltip.h"
#include "Tooltip.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <vector>

namespace ms
{
	class SetTooltip
	{
	public:
		static constexpr int16_t FRAME_WIDTH = 220;

		// hover_level = 悬停装备的强化等级（Equip::get_level），用于成员名前缀 Lv:。
		void set_item(int32_t item_id, const std::vector<int32_t>& equipped_item_ids,
			uint8_t hover_level = 0);

		bool valid() const { return is_valid; }
		int16_t height() const { return panel_height; }
		void draw(Point<int16_t> pos) const;

	private:
		struct Line
		{
			Text left;
			Text right;		// 部位标签（可空）
			Texture icon;	// 2026-11-20 高版本：成员名前的装备图标（多选武器用 X 标记兜底）
			bool has_icon = false;
			bool multi_weapon = false;	// OR 组多选武器：参考图用一个「X」标记代替单件图标
			bool has_right = false;
			int16_t indent = 0;	// 2026-11-21：属性行相对阶段标题的额外左缩进（像素，不靠前导空格）
			// 2026-11-21：属性行「标签 : +值」拆两段，值对齐到统一列（冒号中轴）。
			bool split_value = false;
			Text value;
			bool active = false;
		};

		bool is_valid = false;
		int16_t panel_height = 0;

		Text title;
		std::vector<Line> lines;

		TooltipBackdrop backdrop;
		ColorLine separator;
	};
}
