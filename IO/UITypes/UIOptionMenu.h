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

#include "../UIDragElement.h"

#include "../../Template/Rectangle.h"
#include "../Components/MapleComboBox.h"

namespace ms
{
	class UIOptionMenu : public UIDragElement<PosOPTIONMENU>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::OPTIONMENU;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIOptionMenu();

		void draw(float inter) const override;

		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void change_tab(uint16_t tabid);

		enum Buttons : uint16_t
		{
			TAB0,
			TAB1,
			TAB2,
			TAB3,
			TAB4,
			CANCEL,
			OK,
			UIRESET,
			SELECT_RES
		};

		uint16_t selected_tab;
		Texture tab_background[Buttons::CANCEL];
	};

	////////////////////////////////////////////////////////////////////////////////
	// v83「游戏设置」/「系统设置」两扇独立窗。
	//
	// 为什么要另立两型：本包（BeiDou v83）没有 StatusBar3.img/OptionMenu，
	// 上面的 UIOptionMenu 只剩「画一张底板」的退化壳；而 UIWindow.img/GameOpt 与
	// SysOpt 的底板将**全部标题与行名烘焙进位图**（nx_inspect 实测两节点只有
	// backgrnd 一个子），原生截图里可见的勾选框 / 滑条 / 下拉全是运行时叠加的
	// Basic.img 控件。于是「两颗按钮都开同一扇空壳、一个勾选框也没有」成为用户
	// 实测反馈的缺陷。版式常数全部来自
	// docs/features/ui-v83-window-restore/evidence/w7-option-settings/_settings_map2.txt
	// （原生 游戏设置.png / 系统设置.png 逐格模板 SAD 测量，SAD=0.00 才算命中）。
	////////////////////////////////////////////////////////////////////////////////

	// 横向设置滑条（Basic.img/Slider 的 left/mid/right + thumbNormal）。
	// 不是 Button 子类：Button 派发只有「悬浮→点击」一档，给不了按住拖动，
	// 由宿主窗体在自己的 send_cursor 里直接驱动。
	class SettingsSlider
	{
	public:
		SettingsSlider() {}

		// track_x0/x1：轨道左右端（画布坐标，含）；row_y：滑钮顶边行。
		// origin 为 Slider 件的 (0,-2)，故轨道件的绘制 y = row_y + 1。
		void init(int16_t track_x0, int16_t track_x1, int16_t row_y);
		// value 归一到 [0, 100]。
		void set_value(uint8_t value);
		uint8_t get_value() const;

		void draw(Point<int16_t> parent) const;
		// 命中轨道则按 x 取值并返回 true（窗体据此吞掉这次光标事件）。
		// pressed=false 时清拖动状态。
		bool apply_cursor(Point<int16_t> parent, Point<int16_t> cursor, bool pressed);

		Rectangle<int16_t> bounds(Point<int16_t> parent) const;

		// Basic.img/Slider/thumbNormal 的实测宽度；滑钮左沿可在
		// [track_x0, track_x1 - THUMB_W + 1] 之间行走。
		static constexpr int16_t THUMB_W = 22;

	private:
		int16_t thumb_x() const;

		int16_t track_x0 = 0;
		int16_t track_x1 = 0;
		int16_t row_y = 0;
		uint8_t value = 0;
		bool dragging = false;

		Texture track_left;
		Texture track_mid;
		Texture track_right;
		Texture thumb;
	};

	// v83「游戏设置」：12 行同意/拒绝勾选 + 确定 / 取消。
	class UIGameOpt : public UIDragElement<PosGAMEOPT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GAMEOPT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIGameOpt();

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

		// 底板自上而下的 12 行（标题烘焙在 GameOpt/backgrnd 位图里，顺序实测自
		// 原生截图：悄悄话 / 好友聊天 / 聊天邀请 / 交易申请 / 组队邀请 / 寻找组队 /
		// 游戏邀请 / 家族聊天 / 家族邀请 / 联盟聊天 / 联盟邀请 / 学院招待）。
		enum Option : uint8_t
		{
			WHISPER,
			BUDDY_CHAT,
			CHAT_INVITE,
			TRADE_APPLY,
			PARTY_APPLY,
			PARTY_SEARCH,
			GAME_INVITE,
			FAMILY_CHAT,
			FAMILY_APPLY,
			UNION_CHAT,
			UNION_APPLY,
			ACADEMY_INVITE,
			OPTION_COUNT
		};

		// 第 option 行是否「同意」。邀请拦截点应查此函数（消费方见 .cpp 顶部预留说明）。
		static bool accepts(Option option);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			OPTION0,
			OPTIONCANCEL = OPTION0 + OPTION_COUNT,
			OPTIONOK
		};

		static constexpr int16_t CHECK_X = 130;
		static constexpr int16_t CHECK_TOP = 34;
		static constexpr int16_t CHECK_PITCH = 18;
		// 底部两颗橙色钮（47x19）：确定 = Basic.img/BtOK2，取消 = Basic.img/BtCancel2。
		static constexpr int16_t BTN_Y = 262;
		static constexpr int16_t OK_X = 34;
		static constexpr int16_t CANCEL_X = 84;

		void commit();

		uint16_t mask;
		bool dirty;
		Texture backgrnd;
		Texture cb_off;
		Texture cb_on;
	};

	// v83「系统设置」：6 条滑条 + 2 个下拉 + 5 个勾选 + 确定 / 取消。
	class UISysOpt : public UIDragElement<PosSYSOPT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::SYSOPT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UISysOpt();

		void draw(float inter) const override;
		void update() override;

		void send_key(int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

		UIElement::Type get_type() const override;

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		enum Buttons : uint16_t
		{
			MUTE_BGM,
			MUTE_SFX,
			SKILL_EFFECTS,
			WINDOW_MODE,
			ALT_ENTER,
			MOB_INFO,
			SCREENSHOT_PATH,
			CANCEL,
			OK
		};

		enum Sliders : uint8_t
		{
			SL_QUALITY,
			SL_BGM,
			SL_SFX,
			SL_MOUSE,
			SL_HP,
			SL_MP,
			SL_COUNT
		};

		void load_values();
		void commit();
		// 静音格与音量滑条的联动：静音即压 0，取消回到静音前的值。
		void mute_state(uint8_t& stash, SettingsSlider& slider);
		void toggle_mute(uint8_t& stash, SettingsSlider& slider);
		// 下拉收起态：Basic.img/ComboBox 的 5 + rwidth + 18 三段 + 当前项文字。
		void draw_combo(int16_t y, const std::string& option) const;

		// 轨道左右端与滑钮行：均为 _settings_map2.txt 的实测值。
		static constexpr int16_t TRACK_X0 = 99;
		static constexpr int16_t TRACK_X1 = 245;
		// 6 条滑条的滑钮顶边行（原生逐格 SAD=0.00）。
		static constexpr int16_t SLIDER_ROW[SL_COUNT] = { 40, 70, 100, 160, 190, 220 };
		// 5 个勾选框的画布落位（x, y）。
		static constexpr int16_t CHECK_BOX[5][2] = {
			{ 241, 71 },	// 背景音乐 · 静音
			{ 241, 101 },	// 效果音响 · 静音
			{ 64, 251 },	// 画面效果
			{ 64, 304 },	// 画面模式 · 窗口化
			{ 64, 315 }		// 画面模式 · ALT+回车可切换
		};
		// 下拉框：盒体左上 + 中段宽度（右端箭头 18x17 实测落在 x=222）。
		static constexpr int16_t COMBO_X = 157;
		static constexpr int16_t COMBO_W = 60;
		static constexpr int16_t COMBO_Y_PATH = 127;
		static constexpr int16_t COMBO_Y_MOB = 278;
		// 底部两颗橙色钮（47x19），与「游戏设置」同一件，只是落位不同。
		static constexpr int16_t BTN_Y = 342;
		static constexpr int16_t OK_X = 194;
		static constexpr int16_t CANCEL_X = 244;

		Texture backgrnd;
		Texture cb_off;
		Texture cb_on;
		SettingsSlider sliders[SL_COUNT];
		// 静音 = 该路音量被压到 0；再次点击恢复到静音前的值（原生即此语义）。
		// 不另立配置项：ui.bgm/sfx.volume 已经是消费方读的那一份。
		uint8_t bgm_stash;
		uint8_t sfx_stash;
		bool skill_effects;
		bool window_mode;
		bool alt_enter;
		uint8_t mob_info;
		uint8_t screenshot_path;
	};
}