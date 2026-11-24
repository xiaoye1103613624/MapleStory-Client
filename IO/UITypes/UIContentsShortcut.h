//////////////////////////////////////////////////////////////////////////////////
// UIContentsShortcut — 192 ShortCutUI 屏幕居中「侧栏快捷栏横向弹层」
//
// 证据：docs/features/sideicon-collapse-192-panel/PLAN.md（IDA 一手 + live probe）
//   - 面板屏幕居中：l=(screenW-w)/2, t=(screenH-h)/2，z 由 elementorder 末尾决定。
//   - 三段背景 top(353x33) / middle(353x92) x nLine / bottom(353x33)；
//     宽 = top 贴图宽 353，高 = 33 + 92*nLine + 33。
//   - 图标每行 6 个，pitch X=56 / Y=92，格内居中：
//       x = ((56-iconW)>>1) + 56*(i%6) + 10
//       y = 92*(i/6) + 33
//     图标资源 UIWindow4.img/ShortCutUI/contentsIcon/{resourcePath}/normal/0（单帧），
//     缺图标回退 contentsIcon/default。
//   - 数据源 Etc/ContentsShortcut.img（本端 C++ 静态表，字段对齐 ContentsInfo：
//     resourcePath / toolTip / openUIIndex / questIDs，从 192 XML 抠出）。
//   - 点击图标：openUIIndex>-1 → 本端 UIElement::Type（能接几个接几个；
//     无对应 Type 或任务型 → UIOk 提示，本端无 CUtilDlgEx 任务选择，不臆造），
//     点后关闭面板。
//
// 纯 UIElement（不拖拽）：面板屏幕居中、无 Setting 坐标、分辨率变化重锚 Center。
// 由 UISideIconBar 的 btMain 点击 / 悬浮 toggle 打开；ESC / btClose 关闭。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIContentsShortcut : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CONTENTSSHORTCUT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIContentsShortcut();

		void draw(float inter) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;

		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

	private:
		// ContentsInfo（192 IDA 每项 32B 字段的本端最小子集）。
		struct ContentsInfo
		{
			std::string name;          // 数据源键（BossArena/CrossHunter/...，仅日志）
			std::string resourcePath;  // contentsIcon/{resourcePath}/normal/0
			std::string toolTip;       // hover 文案（Locale 键，缺失回退原名）
			int32_t openUIIndex;       // -1 = 任务型（questIDs）
			std::vector<int32_t> questIDs;
		};

		enum Buttons : uint16_t
		{
			BT_CLOSE = 0,
			BT_ICON0 = 2000       // nID = 2000+i（IDA 一手），本端仅需不撞 BT_CLOSE
		};

		// IDA 一手布局常量。
		static constexpr int16_t PANEL_W = 353;   // top 贴图宽
		static constexpr int16_t TOP_H = 33;
		static constexpr int16_t MIDDLE_H = 92;
		static constexpr int16_t BOTTOM_H = 33;
		static constexpr int16_t ICON_PITCH_X = 56;
		static constexpr int16_t ICON_PITCH_Y = 92;
		static constexpr int16_t ICON_X_PAD = 10;
		static constexpr int16_t COLS = 6;

		static constexpr int16_t BTN_IDLE = -1;   // hover_icon 哨兵：未命中任何图标

		static std::vector<ContentsInfo> build_contents();

		void close_panel();
		void dispatch_icon(size_t index);
		int16_t icon_at(Point<int16_t> cursorpos) const;

		std::vector<ContentsInfo> contents;
		Texture tex_top;
		Texture tex_middle;
		Texture tex_bottom;
		int16_t hover_icon = BTN_IDLE;
	};
}
