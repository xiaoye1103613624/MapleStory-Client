//////////////////////////////////////////////////////////////////////////////////
// 本文件为原生 v83 死亡复活弹窗组件的 Windows 客户端移植（命名空间 ms）。
// 骨架照搬 MinMS/src/client/IO/UITypes/UIDeathNotice.cpp；本端接口差异逐项对齐：
//   - nl::nx::ui → nl::nx::UI（本端大写根，见 UINotice.cpp:44 / UILoginNotice.cpp:47）
//   - Button::PRESSED → Button::State::PRESSED（本端 Button.h:28-36）
//   - KeyAction::RETURN → KeyAction::Id::RETURN（本端 KeyAction.h:101）
//   - Constants::VIEWWIDTH/viewwidth() → Constants::Constants::get().get_viewwidth()/get_viewheight()
//   - 新增 update() 10 分钟超时（Angel v83 CUIRevive::Revive）与 get_type()（本端纯虚）
//////////////////////////////////////////////////////////////////////////////////
#include "UIDeathNotice.h"

#include "../Components/MapleButton.h"
#include "../KeyAction.h"

#include "../../Constants.h"

#include <cstdio>
#include <utility>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// DIALOG 286x146 = Notice/0、Notice/1 的真实 CANVAS 尺寸（orange-wz 实证）。
		constexpr int16_t DIALOG_W = 286;
		constexpr int16_t DIALOG_H = 146;
		// 经典 BtOK2/BtCancel2 宽 65；灰色页脚自 y≈104 起，24px 按钮居中于该条。
		constexpr int16_t BTN_W = 65;
		constexpr int16_t BTN_Y = 110;
		constexpr int16_t BTN_GAP = 10;
	}

	UIDeathNotice::UIDeathNotice(Mode in_mode, std::function<void(bool)> in_handler)
		: UIElement(Point<int16_t>(0, 0), Point<int16_t>(DIALOG_W, DIALOG_H), true),
		  mode(in_mode),
		  handler(std::move(in_handler)),
		  countdown(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE)
	{
		// 原生 Notice/1（双钮）与 Notice/0（单钮）。根访问对齐本端大写 nl::nx::UI。
		nl::node notice = nl::nx::UI["UIWindow.img"]["Notice"];
		background = Texture(notice[mode == WHEEL ? "1" : "0"]);

		// 位图真身存在时以真实尺寸为准（防止硬编码 286x146 与资源实际尺寸偏差）。
		if (background.width() > 0 && background.height() > 0)
		{
			dimension = Point<int16_t>(background.width(), background.height());
		}

		nl::node basic = nl::nx::UI["Basic.img"];

		if (mode == WHEEL)
		{
			// 一对经典「确定 / 取消」，在页脚水平居中。
			const int16_t pair_w = static_cast<int16_t>(BTN_W * 2 + BTN_GAP);
			const int16_t ok_x = static_cast<int16_t>((dimension.x() - pair_w) / 2);
			const int16_t cancel_x = static_cast<int16_t>(ok_x + BTN_W + BTN_GAP);

			buttons[BT_OK] = std::make_unique<MapleButton>(
				basic["BtOK2"], Point<int16_t>(ok_x, BTN_Y));
			buttons[BT_CANCEL] = std::make_unique<MapleButton>(
				basic["BtCancel2"], Point<int16_t>(cancel_x, BTN_Y));
		}
		else
		{
			const int16_t ok_x = static_cast<int16_t>((dimension.x() - BTN_W) / 2);
			buttons[BT_OK] = std::make_unique<MapleButton>(
				basic["BtOK2"], Point<int16_t>(ok_x, BTN_Y));
		}

		refresh_countdown();
		update_screen(
			Constants::Constants::get().get_viewwidth(),
			Constants::Constants::get().get_viewheight());
	}

	void UIDeathNotice::draw(float alpha) const
	{
		background.draw(position);
		UIElement::draw(alpha);

		// 倒计时叠在按钮行上方（y≈96，按钮自 110 起，不遮挡）。
		countdown.draw(DrawArgument(
			Point<int16_t>(
				static_cast<int16_t>(position.x() + dimension.x() / 2),
				static_cast<int16_t>(position.y() + BTN_Y - 14)),
			alpha));
	}

	void UIDeathNotice::update()
	{
		// 必须先链基类：本端 UIElement::update() 推进 sprites/buttons（UIElement.cpp:304-312），
		// MinMS 原版无 update()，但本端跳过基类会让按钮四态动画停摆。
		UIElement::update();

		if (!active)
			return;

		// 10 分钟自动回城：Angel v83 CUIRevive::Update → CUIRevive::Revive（600000ms）。
		// 每 tick +TIMESTEP（Constants.h:29，8ms），与 UIBuffList/UIClock 同口径。
		elapsed_ms += Constants::TIMESTEP;
		if (elapsed_ms >= AUTO_REVIVE_MS)
		{
			finish(false);
			return;
		}

		refresh_countdown();
	}

	void UIDeathNotice::update_screen(int16_t new_width, int16_t new_height)
	{
		// 照搬 MinMS：按传入视口居中，static_cast 防 int16_t 溢出。
		// 本端 Constants 单例替代 MinMS 的 Constants::VIEWWIDTH / viewwidth()。
		const int16_t view_w = new_width > 0 ? new_width : Constants::Constants::get().get_viewwidth();
		const int16_t view_h = new_height > 0 ? new_height : Constants::Constants::get().get_viewheight();
		position = Point<int16_t>(
			static_cast<int16_t>(view_w / 2 - dimension.x() / 2),
			static_cast<int16_t>(view_h / 2 - dimension.y() / 2));
	}

	void UIDeathNotice::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed)
			return;

		if (escape)
		{
			// Esc 在双钮窗等同「取消」（回城）；在单钮窗等同唯一「确定」（回城）。
			finish(false);
			return;
		}

		if (keycode == KeyAction::Id::RETURN)
		{
			// 回车：双钮窗=原地复活术，单钮窗=回城。
			finish(mode == WHEEL);
		}
	}

	Button::State UIDeathNotice::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case BT_OK:
			finish(mode == WHEEL);
			break;
		case BT_CANCEL:
			finish(false);
			break;
		default:
			break;
		}

		// 本端 Button.h:28-36 是带作用域的 State 枚举，不是 MinMS 的 Button::PRESSED。
		return Button::State::PRESSED;
	}

	UIElement::Type UIDeathNotice::get_type() const
	{
		return TYPE;
	}

	void UIDeathNotice::refresh_countdown()
	{
		if (!active)
			return;

		const int32_t remain_ms = AUTO_REVIVE_MS - elapsed_ms;
		const int32_t secs = remain_ms > 0 ? (remain_ms + 999) / 1000 : 0;
		if (secs == last_countdown_second)
			return;
		last_countdown_second = secs;

		char buf[16];
		std::snprintf(buf, sizeof(buf), "%02d:%02d", static_cast<int>(secs / 60), static_cast<int>(secs % 60));
		countdown.change_text(buf);
	}

	void UIDeathNotice::finish(bool use_wheel)
	{
		// active=false 先行：超时与用户点击可能同帧抵达，保证 handler 只被有效调用一次。
		if (!active)
			return;
		active = false;
		if (handler)
			handler(use_wheel);
	}
}
