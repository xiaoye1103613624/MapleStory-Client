#pragma once

#include "../UIElement.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Sprite.h"

namespace ms
{
	// 本端扩展（E 桶）入口：福利中心（签到/兑换/排行/转职/传送）。
	// 083 活 IDB 全库 `*UISpecial*` / `*Fairy*` / `*Welfare*` / `*CheckIn*` / `*Attendance*` 符号命中均为 0（`tools/_ida_r93/special_family.json`），非 083 原生窗族；按 m09729「扩展功能可以不对齐」保留，零行为改动。旧注释「095 自定义中文 UI」= 依据链污染。
	// 当前以程序化背板兜底（R6 精简 img 无 Special 背景时仍能开窗口），后续补充 Special.img 资产。
	class UISpecial : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::SPECIALUI;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UISpecial();

		void draw(float inter) const override;
		UIElement::Type get_type() const override;
		Button::State button_pressed(uint16_t buttonid) override;
		bool is_in_range(Point<int16_t> cursor_position) const override;

	private:
		enum Buttons : int16_t
		{
			BT_SIGNIN = 0,
			BT_EXCHANGE = 1,
			BT_RANK = 2,
			BT_JOB = 3,
			BT_WARP = 4,
			BT_CLOSE = 5
		};

		Point<int16_t> dimension;
		bool hasbackgrnd;
		Sprite backgrnd;
		Text title;
		Text labels[5];
	};
}
