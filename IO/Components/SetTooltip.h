//////////////////////////////////////////////////////////////////////////////////
//	SetTooltip — 套装 companion 面板（2026-11-12 装备槽位扩展 ⑤ · 展示层）
//
//	参考 E:\MXD\扩展改动\套装系统\README.md §UI：
//	    [悬停装备 tip] → [套装 tip] → [已穿比较 tip]
//	  套装 tip 在 mainX + mainW + 4；装备 tip 保持原版不改。
//
//	本端实现同语义：作为 EquipTooltip 的**第三层**独立面板，
//	复用 TooltipBackdrop（程序描边，与装备 tip 同色同透明度），
//	不改任何既有绘制路径 —— 只在 EquipTooltip::draw 末尾按需叠一层。
//
//	数据源：Data/SetItemData（本地花名册，与 Player::recalc_stats 注入同一份），
//	因此「面板显示的档位」与「实际生效的属性」永远同源，不会各说各话。
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Charset.h"
#include "TextTooltip.h"
#include "Tooltip.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <vector>

namespace ms
{
	class SetTooltip
	{
	public:
		static constexpr int16_t FRAME_WIDTH = 200;

		// item_id：悬停装备的物品 id；equipped_item_ids：当前已穿戴（算件数用）。
		// 不属于任何套装时 valid() 为 false，调用方跳过绘制。
		void set_item(int32_t item_id, const std::vector<int32_t>& equipped_item_ids);

		bool valid() const { return is_valid; }

		// 面板高度（含 padding），供调用方做视口夹取。
		int16_t height() const { return panel_height; }

		void draw(Point<int16_t> pos) const;

	private:
		struct Line
		{
			Text text;
			bool active = false;	// 已达成的档位行高亮
		};

		bool is_valid = false;
		int16_t panel_height = 0;

		Text title;			// 「套装名 (n/N)」
		std::vector<Line> tier_lines;	// 每档一行：`3件套：STR +5`

		TooltipBackdrop backdrop;
		ColorLine separator;
	};
}
