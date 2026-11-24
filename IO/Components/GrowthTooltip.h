//////////////////////////////////////////////////////////////////////////////////
//	GrowthTooltip — 装备成长 companion（仿 SetTooltip，挂在套装右侧）
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TextTooltip.h"
#include "Tooltip.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <vector>

namespace ms
{
	class GrowthTooltip
	{
	public:
		static constexpr int16_t FRAME_WIDTH = 200;

		void set_item(int32_t item_id, int32_t item_level, int32_t scroll_level);
		bool valid() const { return is_valid; }
		int16_t height() const { return panel_height; }
		void draw(Point<int16_t> pos) const;

	private:
		struct Line
		{
			Text text;
			bool header = false;
		};

		bool is_valid = false;
		int16_t panel_height = 0;
		Text title;
		std::vector<Line> lines;
		TooltipBackdrop backdrop;
		ColorLine separator;
	};
}
