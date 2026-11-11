////////////////////////////////////////////////////////////////////////////////
//	D1 / 分号键「勋章」窗（v83 MEDAL，Angel UI_Open:260 sub_840170(-1)）
//
//	版式真源 = 正确客户端截图/勋章框.png（255x372）：标题「勋章」，
//	5 页签 基本/职业/一般/挑战/活动（基本红字选中），「获得过的勋章个数:0个」，
//	空列表 + 右侧滚动条。
//
//	资源现状（orange-wz 门禁实测）：UIWindow.img 与 UIWindow2.img 全搜 Medal
//	matches:[]；win:Medal / MedalQuest / MedalList 两根全「节点路径不存在」；
//	E:/MXD/BeiDou-Client_S9/Data/UI/ 24 个 .img 无 Medal.img。
//	→ 按门禁 18「主路径已尽仍缺关键节点」走无资源复用件路径：
//	  ColorBox 纯色框/页签/列表/滚动条 + Text 文案 + UiAssetResolve::close_btn()。
//	客户端无勋章数据源（原生截帧本身就是 0 个），列表忠实为空，不造假数据。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Color.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIMedal : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MEDAL;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMedal();

		void draw(float inter) const override;
		void update() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtClose
		};

		static constexpr int16_t FRAME_W = 255;
		static constexpr int16_t FRAME_H = 372;
		static constexpr int16_t TAB_COUNT = 5;
		static constexpr int16_t TAB_W = 47;
		static constexpr int16_t TAB_H = 18;
		static constexpr int16_t TAB_X0 = 8;
		static constexpr int16_t TAB_Y = 30;
		static constexpr int16_t TAB_GAP = 2;

		void select_tab(int16_t index);

		int16_t active_tab = 0;
		ColorBox frame_back;
		ColorBox frame_edge;
		ColorBox list_back;
		ColorBox list_inner;
		ColorBox scroll_track;
		ColorBox scroll_thumb;
		std::vector<ColorBox> tab_plate;
		std::vector<Text> tab_label;
		Text title;
		Text count_label;
		Text active_tab_label;
		Text empty_hint;
	};
}
