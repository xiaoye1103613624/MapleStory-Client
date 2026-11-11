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

#include "../UIElement.h"

#include "../Components/Charset.h"
#include "../Components/Gauge.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Animation.h"

#include <cstdint>

#include "../../Character/CharStats.h"
#include "../../Graphics/SpecialText.h"

#include <array>

namespace ms
{
	class UIStatusBar : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::STATUSBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		enum MenuType
		{
			MENU,
			SETTING,
			COMMUNITY,
			CHARACTER,
			EVENT
		};

		UIStatusBar(const CharStats& stats);

		void draw(float alpha) const override;
		void update() override;

		bool is_in_range(Point<int16_t> cursor_position) const override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void toggle_qs();
		void toggle_menu();
		void remove_menus();
		bool is_menu_active();
		bool is_classic() const { return classic_mode; }

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		static constexpr int16_t QUICKSLOT_MAX = 211;
		static constexpr int16_t CLASSIC_BAR_H = 71;

		float getexppercent() const;
		float gethppercent() const;
		float getmppercent() const;

		void init_modern();
		void init_classic();
		void draw_classic(float alpha) const;
		void update_classic();
		void refresh_classic_qs_icons();
		/// classic 快捷格左上角（屏幕坐标）；几何全部来自 base/quickSlot* 图集本身。
		Point<int16_t> classic_qs_cell_at(size_t slot, int16_t cols) const;
		/// 经典 quickSlot 槽格命中：返回槽下标，未命中为 -1。
		int16_t classic_qs_slot_at(Point<int16_t> cursor_position) const;
		/// 点击快捷格：按 KeyConfig 映射释放技能/使用道具（对齐参考端 OnButton 快捷栏）。
		void activate_classic_qs_slot(size_t slot);

		void toggle_qs(bool quick_slot_active);
		/// classic：按 quickslot_active 同步 QS(上箭头)/QSD(下箭头) 按钮可见性。
		void sync_classic_qs_buttons();
		/// `buttons` 是 map，用 operator[] 只读访问会插入空 unique_ptr 再解引用；
		/// classic HUD 只建 BT_CLASSIC_* 那一组，modern 的 27 个 submenu 按钮根本不存在。
		Button* button_at(uint16_t id) const;
		void set_buttons_active(std::initializer_list<uint16_t> ids, bool active);
		/// D9 · Angel 083 CUIStatusBar::SetButtonBlink @0x8dfe21：
		/// a3==0 清除 / a3!=0 加载 SP_1986..1989_..._KEY_ANI 真动画（两帧 500ms=1Hz）。
		/// 开子窗(OnButtonClicked 2000-2003 / OnChildNotify 100)与 Update(+90000ms)清除；
		/// 低等级<=10 升级 -> STAT 钮启动（OnStatChanged @0xa1fce9）。
		void start_button_blink(uint16_t id);
		void stop_button_blink(uint16_t id);
		int32_t blink_index(uint16_t id) const;
		void toggle_setting();
		void toggle_community();
		void toggle_character();
		void toggle_event();
		void remove_active_menu(MenuType type);

		Point<int16_t> get_quickslot_pos() const;

		enum Buttons : uint16_t
		{
			BT_CASHSHOP,
			BT_MENU,
			BT_OPTIONS,
			BT_CHARACTER,
			BT_COMMUNITY,
			BT_EVENT,
			BT_FOLD_QS,
			BT_EXTEND_QS,
			BT_MENU_QUEST,
			BT_MENU_MEDAL,
			BT_MENU_UNION,
			BT_MENU_MONSTER_COLLECTION,
			BT_MENU_AUCTION,
			BT_MENU_MONSTER_LIFE,
			BT_MENU_BATTLE,
			BT_MENU_ACHIEVEMENT,
			BT_MENU_FISHING,
			BT_MENU_HELP,
			BT_MENU_CLAIM,
			BT_SETTING_CHANNEL,
			BT_SETTING_OPTION,
			BT_SETTING_KEYS,
			BT_SETTING_JOYPAD,
			BT_SETTING_QUIT,
			BT_COMMUNITY_FRIENDS,
			BT_COMMUNITY_PARTY,
			BT_COMMUNITY_GUILD,
			BT_COMMUNITY_MAPLECHAT,
			BT_CHARACTER_INFO,
			BT_CHARACTER_STAT,
			BT_CHARACTER_SKILL,
			BT_CHARACTER_EQUIP,
			BT_CHARACTER_ITEM,
			BT_EVENT_SCHEDULE,
			BT_EVENT_DAILY,
			// v83 StatusBar.img direct keys
			BT_CLASSIC_SHOP,
			BT_CLASSIC_MENU,
			BT_CLASSIC_NPT,
			BT_CLASSIC_SHORT,
			BT_CLASSIC_WHISPER,
			BT_CLASSIC_CLAIM,
			BT_CLASSIC_EQUIP,
			BT_CLASSIC_INV,
			BT_CLASSIC_STAT,
			BT_CLASSIC_SKILL,
			BT_CLASSIC_KEYSET,
			BT_CLASSIC_QS,
			BT_CLASSIC_QSD
		};

		const CharStats& stats;

		Gauge expbar;
		Gauge hpbar;
		Gauge mpbar;
		Charset statset;
		Charset hpmpset;
		Charset levelset;
		Texture quickslot[2];
		Texture menutitle[5];
		Texture menubackground[3];
		OutlinedText namelabel;
		mutable OutlinedText joblabel;	// 原生底栏：LV 芯片内「职业名」一行（draw 内刷新）
		mutable OutlinedText classic_exp_text;	// 经典底栏 EXP 段：当前经验值（中文 亿/万 缩写，描边文字；位图字库无汉字且 3000 级长数字溢出段宽）
		std::vector<Sprite> hpmp_sprites;

		// W1：量规数值改画 StatusBar.img/number 位图字库（5x7 白数字 + 2x9 黄绿括号），
		// 不再走 Text(A11B, YELLOW)；LV 徽章改画 Basic.img/LevelNo 橙盒数字。
		Charset classic_levelno;			// Basic.img/LevelNo{0-9,blank} 11x13
		Charset classic_showlevel;			// Basic.img/ShowLevel/0 白数字 5x12（LevelNo 缺失时回退）
		Texture classic_showlevel_bracket;	// ShowLevel/Bracket/right 2x12
		ColorBox classic_exp_dot;			// number 无 '.' 字模：EXP 小数点用 1x1 白点
		ColorBox classic_exp_pct_dot;		// 经验值百分比位图 tint（蓝紫，独立于血量白字）
		Texture classic_key_label[7];		// key/0..6：Shift/Ins/Hm/Pup/Ctrl/Del/End 键名贴纸（key/7 本包为红箭头，非 Pdn）
		mutable OutlinedText classic_qs_keytext;	// 数字/字母/Pdn 键帽：#AABBCC 字身 + 白描边（原生位图字母缺失，描边文字统一回退）
		mutable Text classic_qs_label;		// 快捷格中文名称：Text maxwidth（T12b 度量换行路径）
		Point<int16_t> lvbadge_pos;

		static constexpr size_t CLASSIC_QS_SLOTS = 26;
		std::string classic_qs_names[CLASSIC_QS_SLOTS];	// SKILL/ITEM 绑定名（随图标刷新）
		int16_t classic_qs_cols = 13;		// 面板列数：32 格=16 / 26 格=13 / 8 格=4
		size_t classic_qs_count = CLASSIC_QS_SLOTS;

		// Classic StatusBar.img chrome
		Texture classic_backgrnd;
		Texture classic_backgrnd2;
		Texture classic_quickslot;
		Texture classic_graduation;
		Texture classic_gauge_bar;
		Texture classic_gauge_gray;			// gauge/gray 1x16 (204,204,204)：量规未填充段底色
		// 掉血/掉蓝闪光：原生是 5 帧瞬态动画（delay 130ms），不是常驻覆盖层。
		// 计数 -1 = 本包无该素材，0 = 空闲，>0 = 剩余播放帧。
		Sprite classic_hp_flash_sprite;
		Sprite classic_mp_flash_sprite;
		int8_t classic_hp_flash = -1;
		int8_t classic_mp_flash = -1;
		float classic_last_hp = -1.0f;
		float classic_last_mp = -1.0f;
		Texture classic_qs_icons[CLASSIC_QS_SLOTS];

		Point<int16_t> exp_pos;
		Point<int16_t> hpmp_pos;
		Point<int16_t> hpset_pos;
		Point<int16_t> mpset_pos;
		Point<int16_t> statset_pos;
		Point<int16_t> levelset_pos;
		Point<int16_t> namelabel_pos;
		Point<int16_t> joblabel_pos;	// 原生 LV 芯片内「职业名」一行
		Point<int16_t> quickslot_pos;
		Point<int16_t> quickslot_adj;
		Point<int16_t> quickslot_qs_adj;
		Point<int16_t> menu_pos;
		Point<int16_t> setting_pos;
		Point<int16_t> community_pos;
		Point<int16_t> character_pos;
		Point<int16_t> event_pos;
		int16_t quickslot_min;
		int16_t position_x;
		int16_t position_y;

		bool quickslot_active;
		int16_t VWIDTH;
		int16_t VHEIGHT;

		bool menu_active;
		bool setting_active;
		bool community_active;
		bool character_active;
		bool event_active;
		bool classic_mode;
		bool classic_qs_wide = false;	// true=26 格(13x2) quickSlot；false=8 格(4x2) quickSlot08

		// D9 · Angel 083 真 KEY_ANI 动画（.nx 实测 StatusBar.img/<Key>/ani 两帧齐全）。
		// 索引 0/1/2/3 对应 BT_CLASSIC_EQUIP/INV/STAT/SKILL。
		std::array<Animation, 4> blink_ani;
		// 083 SetButtonBlink：a3==1 记 timeGetTime() 于 [idx]，a3==0 / v8==-1 清除；
		// Update 里 v8 + 90000 < timeGetTime() 超时清除。此处用逻辑帧累计毫秒等价实现。
		std::array<int64_t, 4> blink_elapsed_ms = { -1, -1, -1, -1 };
		int16_t blink_last_level = 0;
		static constexpr int64_t BLINK_TIMEOUT_MS = 90000; // 083 Update 的 +90000ms
		static constexpr int16_t BLINK_LEVEL_LIMIT = 10;   // 083 OnStatChanged 的 <=0xAu
	};
}