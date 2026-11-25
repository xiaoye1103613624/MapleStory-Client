//////////////////////////////////////////////////////////////////////////////////
// 本文件为原生 v83 死亡复活弹窗组件的 Windows 客户端移植（命名空间 ms）。
// 对照源：MinMS/src/client/IO/UITypes/UIDeathNotice.{h,cpp}（命名空间 jrc）。
// 原生依据：
//   1) UIWindow.img/Notice/0 = 286x146 CANVAS（origin=(0,0)），单颗「确定」→ 回城；
//   2) UIWindow.img/Notice/1 = 286x146 CANVAS，双颗「确定/取消」→ 原地复活术 / 回城；
//   3) 10 分钟超时自动回城 = Angel v83 CUIRevive::Update 调 CUIRevive::Revive 的 600000ms。
// 本端适配：Button::State::PRESSED、KeyAction::Id::RETURN、Constants::TIMESTEP、get_type() 纯虚。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <functional>

namespace ms
{
	// 原生 v83 死亡复活弹窗。FOCUSED=true：active 期间吞掉所有输入（本端
	// UIStateGame::send_cursor 对 FOCUSED 窗无条件派发，见 UIElement.h:367-371）。
	class UIDeathNotice : public UIElement
	{
	public:
		// DEATHNOTICE 枚举由父代理在 UIElement.h 的 NUM_TYPES 前新增。
		static constexpr Type TYPE = UIElement::Type::DEATHNOTICE;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		// TOWN  = Notice/0：单钮「确定」→ 回城（use_wheel=false）。
		// WHEEL = Notice/1：双钮「确定」→ 原地复活术（use_wheel=true）、「取消」→ 回城。
		enum Mode : uint8_t
		{
			TOWN,
			WHEEL
		};

		// handler(use_wheel)：true 消耗「原地复活术」原地复活，false 回城。
		// 发包由父代理在 PlayerHandlers 注入（ChangeMapPacket），本类不碰网络层。
		UIDeathNotice(Mode mode, std::function<void(bool use_wheel)> handler);

		void draw(float alpha) const override;
		void update() override;
		void update_screen(int16_t new_width, int16_t new_height) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		// 本端 UIElement::get_type() 为纯虚（UIElement.h:373），MinMS 原版无此方法。
		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			BT_OK,
			BT_CANCEL
		};

		// active=false 后调 handler；重复触发由 active 守卫（见 cpp finish 注释）。
		void finish(bool use_wheel);
		// 倒计时秒数变化时重排文案（避免每帧 change_text）。
		void refresh_countdown();

		Mode mode;
		Texture background;
		std::function<void(bool use_wheel)> handler;

		// 10 分钟超时自动复活（Angel v83 CUIRevive::Revive，600000ms）。
		// 用 int32_t 累加（Constants::TIMESTEP=8ms/tick，600000 远超 uint16_t 上限）；
		// 与 UIBuffList / UICooldownList / UIClock 的「每 tick 减 TIMESTEP」同范式，
		// 不引 chrono，保证与游戏暂停/时钟缩放一致。
		static constexpr int32_t AUTO_REVIVE_MS = 600000;
		int32_t elapsed_ms = 0;

		// 可选增强：在按钮上方绘剩余时间（MM:SS），走本端 Text 字模而非
		// 数字贴图，避免 Basic.img 数字节点缺失导致空字（UIQuit 同用 Text）。
		Text countdown;
		int32_t last_countdown_second = -1;
	};
}
