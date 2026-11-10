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
	// 083 权威依据（round71 活 IDB 现场复核，见 notes/wave2-optionmenu-083-review-20261111.md）：
	//   CUIGameOpt::OnCreate @0x993a45 —— 12 个 CCtrlCheckBox x=128、y=33+18*i（33..231），
	//     OK Basic.img/BtOK2 (34,262) / CANCEL BtCancel2 (84,262)；
	//   CUISysOpt::OnCreate @0x99417a —— 6 个 CCtrlSlider (99, 40/70/100/160/190/220)、
	//     OK (194,342) / CANCEL (244,342)，2 个下拉（3 条截图路径 / 4 条怪情），**无 9 档分辨率下拉**；
	//   CConfig::ApplyGameOpt @0x49ea1c / ApplySysOpt @0x49ea33。
	// 本端 GameOpt 常量与 083 几乎逐字对齐；SysOpt +28/+30、9 档分辨率、BTN_Y=372 是以截图/
	// rs.cpp 插件为参照的本端扩展，是否回退 083 属决策点 D-B（待用户裁决 + 五屏截图）。
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

		// 第 option 行是否「同意」。邀请/频道入站 handler 查此函数（见 gameopt-invite-mask）。
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

		// 083 CUIGameOpt::OnCreate：勾选位图 12x12 落 x=128、y=33+18*i（步长 18）。
		// 本端 130/34 是印刷行内 +2/+1px 的截图测量偏差：round71 v2 像素检测把运行时
		// GameOpt/backgrnd 拆成 12 个独立洋红格（y32..47/50..65/…/230..245）与 11 个真实间隙，
		// 083 y=33+18i 与当前端 y=34+18i 各精确落在第 0..11 格——同一批印刷行、视觉无感。
		// 按「数值重合≠语义重合」不照抄 083 常量返工（GameOpt 已按 Angel 布局对齐）。
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
			OK,
			// 两个下拉展开后的条目钮；后缀数字就是原生注册表里存的那个序号。
			MOB_OPT0,
			MOB_OPT1,
			MOB_OPT2,
			MOB_OPT3,
			PATH_OPT0,
			PATH_OPT1,
			// 多分辨率：第三个下拉（窗口大小）的盒体与 9 个档位条目。
			RESOLUTION,
			RES_OPT0,
			RES_OPT1,
			RES_OPT2,
			RES_OPT3,
			RES_OPT4,
			RES_OPT5,
			RES_OPT6,
			RES_OPT7,
			RES_OPT8
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
		// 展开态：条目列表画在盒体正下方，画 count 行并高亮当前序号那一行。
		void draw_combo_list(int16_t y, uint8_t count, const char* key_prefix, uint8_t selected) const;
		// 条目命中区随展开态开关；全关时两个下拉都只响应盒体本身。
		void set_combo_rows(bool show_mob, bool show_path);
		// 多分辨率：第三个下拉的收起态 / 展开态绘制与 9 个条目命中区开关。
		void draw_resolution_combo() const;
		void draw_resolution_list() const;
		void set_res_rows(bool show);

		// 轨道左右端与滑钮行：均为 _settings_map2.txt 的实测值。
		// TRACK_X1 取参考图滑条轨道右端 x≈229；滑块右沿 = 229+1 = 230，
		// 须严格小于右侧「大」字左缘 x≈231，避免滑块压住文字。
		static constexpr int16_t TRACK_X0 = 99;
		static constexpr int16_t TRACK_X1 = 229;
		// 6 条滑条的滑钮顶边行。原值 40/70/100/160/190/220 取自旧 365 布局（_settings_map2.txt，
		// 原生逐格 SAD=0.00）；新 299x396 底图＝旧布局整体下移 +28px（10 行中心 40→68 … 310→338，
		// 等距 30px），故统一 +28 对齐到新底图行中心 68/98/128/188/218/248。
		// 083 CUISysOpt::OnCreate 的 6 条滑条原值 40/70/100/160/190/220（无 +28）。
		// 本端 +28 与 CHECK_BOX/COMBO 的 +28/+30 来自「旧 299x365 底图 → 新 299x396 底图整体下移」
		// 的截图推断；但 round71 运行时 open_merge 探针实测 SysOpt/backgrnd 是 299x396，
		// 与旧 .nx 缓存的 299x365 是**两张完全不同的图**（逐像素 0/365 命中），不能用行偏移推算对齐。
		// 浅色内容带 y27..307 对 083 与当前端两组 y 都 6/6 命中、无区分度 ⇒ 是否回退 083 常量
		// 属决策点 D-B（待用户裁决 + 五屏实机截图），本轮不改任何行为字面量。
		static constexpr int16_t SLIDER_ROW[SL_COUNT] = { 68, 98, 128, 188, 218, 248 };
		// 5 个勾选框的画布落位（x, y）。y 随底图整体下移 +28 对齐（x 不变）。
		static constexpr int16_t CHECK_BOX[5][2] = {
			{ 241, 99 },	// 背景音乐 · 静音
			{ 241, 129 },	// 效果音响 · 静音
			{ 64, 279 },	// 画面效果
			{ 64, 332 },	// 画面模式 · 窗口化
			{ 64, 343 }		// 画面模式 · ALT+回车可切换
		};
		// 下拉框：盒体左上 + 中段宽度（右端箭头 18x17 实测落在 x=222）。
		static constexpr int16_t COMBO_X = 157;
		static constexpr int16_t COMBO_W = 60;
		// 两个原生下拉盒体顶边 y：随底图整体下移 +28（127→155 截图保存位置；278→306 怪物情报）。
		static constexpr int16_t COMBO_Y_PATH = 155;
		static constexpr int16_t COMBO_Y_MOB = 306;
		// 盒体高 = ComboBox 三段贴图的公共高（5x17 / 5x17 / 18x17）；列表行同宽同高。
		static constexpr int16_t COMBO_H = 17;
		static constexpr int16_t COMBO_TOTAL_W = 5 + COMBO_W + 18;
		// 条目数：原生汉化表里怪物情报紧邻四条、截图保存位置紧邻两条（见 .cpp 的取证注释）。
		static constexpr uint8_t MOB_INFO_COUNT = 4;
		static constexpr uint8_t SCREENSHOT_PATH_COUNT = 2;
		// 多分辨率新增行（窗口大小）：插件 rs.cpp 参考 Combo(76,338,166,18)，9 档见 rs_tier_dims。
		// 实测新底图：该行被夹在「画面模式」(row10, y≈338) 与 OK/Cancel 分隔线(y≈369)之间，
		// 盒体顶边取 343（18px 高 → 343~361，上不压 row10、下不压分隔线/OK_Cancel）。
		// 列表向上展开（9*18=162px，从 y=181 到 343，完整落在 299x396 底板内）。
		static constexpr int16_t RES_COMBO_X = 76;
		static constexpr int16_t RES_COMBO_Y = 343;
		static constexpr int16_t RES_COMBO_W = 166;
		static constexpr int16_t RES_COMBO_H = 18;
		static constexpr uint8_t RES_COUNT = 9;
		static constexpr int16_t RES_LIST_Y =
			static_cast<int16_t>(RES_COMBO_Y - RES_COUNT * RES_COMBO_H);
		// 底部两颗橙色钮（47x19）：插件 rs.cpp L3313 把 OK/Cancel 从 342 下移到 372，
		// 给新增的「窗口大小」行腾出空间。
		// 083 CUISysOpt::OnCreate OK/CANCEL 在 y=342（194,342）/(244,342)。
		// 本端 372（OK_X=194/CANCEL_X=244）落在运行时 299x396 底板底部深色空白条
		// （浅色占比 y370..395=0.00），为「窗口大小」行让位的本端扩展，见决策点 D-B。
		static constexpr int16_t BTN_Y = 372;
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