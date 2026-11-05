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

#include "../Components/Charset.h"
#include "../Components/Slider.h"
#include "../Components/StatefulIcon.h"

#include "../../Character/CharStats.h"
#include "../../Character/SkillBook.h"
#include "../../Graphics/Text.h"

#include <array>

namespace ms
{
	class UISkillBook : public UIDragElement<PosSKILL>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::SKILLBOOK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UISkillBook(const CharStats& stats, const SkillBook& skillbook);

		void draw(float alpha) const override;
		bool send_icon(const Icon& icon, Point<int16_t> cursor_position) override;

		void toggle_active() override;
		void doubleclick(Point<int16_t> cursorpos) override;
		void remove_cursor() override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void update_stat(MapleStat::Id stat, int16_t value);
		void update_skills(int32_t skill_id);
		bool is_skillpoint_enabled();
		void refresh_macros();
		int32_t get_selected_skill_id() const { return selected_skill_id; }

	protected:
		Button::State button_pressed(uint16_t id) override;

	private:
		class SkillIcon : public StatefulIcon::Type
		{
		public:
			SkillIcon(int32_t skill_id);

			void drop_on_stage() const override {}
			void drop_on_equips(EquipSlot::Id) const override {}
			bool drop_on_items(InventoryType::Id, EquipSlot::Id, int16_t, bool) const override { return true; }
			void drop_on_bindings(Point<int16_t> cursorposition, bool remove) const override;
			void set_count(int16_t) override {}
			void set_state(StatefulIcon::State) override {}
			Icon::IconType get_type() override;
			int32_t get_skill_id() const override { return skill_id; }

		private:
			int32_t skill_id;
		};

		class SkillDisplayMeta
		{
		public:
			// classic：v83 经典窗用纯黑技能名（原生截图逐像素实测），
			//         现代窗沿用上游的 EMPEROR 灰。
			SkillDisplayMeta(int32_t id, int32_t level, bool classic);

			// parent = 窗口 position；cell = 本格格底（窗口相对，来自 skill_cell()）。
			// 图标本格、书名、等级都由这一格推出来 ⇒ 画在哪和点哪算是同一份数。
			void draw(Point<int16_t> parent, Point<int16_t> cell) const;

			int32_t get_id() const;
			int32_t get_level() const;
			StatefulIcon* get_icon() const;

		private:
			int32_t id;
			int32_t level;
			std::unique_ptr<StatefulIcon> icon;
			Text name_text;
			Text level_text;
		};

		void change_job(uint16_t id);
		void change_sp();
		void change_tab(uint16_t new_tab);
		void change_offset(uint16_t new_offset);

		// 当前布局下窗口内实际可见的行数（经典单列 4 行 / 现代双列 12 行）。
		int16_t visible_rows() const;
		// 可见第 row 行对应的 skills 下标（现代双列 offset 以「行对」为单位）。
		size_t skill_index(int16_t row) const;
		// 可见第 row 行的格底（窗口相对）。经典单列每行推进 ROW_HEIGHT；
		// 现代双列按 (row%2, row/2) 排布。绘制、命中、拖拽三处都只认这一把尺。
		Point<int16_t> skill_cell(int16_t row) const;

		void show_skill(int32_t skill_id);
		void clear_tooltip();

		bool can_raise(int32_t skill_id) const;
		void send_spup(uint16_t row);
		void spend_sp(int32_t skill_id);

		Job::Level joblevel_by_tab(uint16_t tab) const;
		const UISkillBook::SkillDisplayMeta* skill_by_position(Point<int16_t> cursorpos) const;

		void close();
		bool check_required(int32_t id) const;

		void set_macro(bool enabled);
		void set_skillpoint(bool enabled);

		enum Buttons : uint16_t
		{
			BT_CLOSE,
			BT_HYPER,
			BT_GUILDSKILL,
			BT_RIDE,
			BT_MACRO,
			BT_MACRO_OK,
			BT_CANCLE,
			BT_OKAY,
			BT_SPDOWN,
			BT_SPMAX,
			BT_SPUP,
			BT_TAB0,
			BT_TAB1,
			BT_TAB2,
			BT_TAB3,
			BT_TAB4,
			BT_SPUP0,
			BT_SPUP1,
			BT_SPUP2,
			BT_SPUP3,
			BT_SPUP4,
			BT_SPUP5,
			BT_SPUP6,
			BT_SPUP7,
			BT_SPUP8,
			BT_SPUP9,
			BT_SPUP10,
			BT_SPUP11
		};

		static constexpr int16_t ROWS = 12;
		static constexpr int16_t ROW_HEIGHT = 40;
		static constexpr int16_t ROW_WIDTH = 143;
		static constexpr Point<int16_t> SKILL_OFFSET = Point<int16_t>(11, 93);
		// 原生（BeiDou V17.7）经典窗单列技能行基准：列表区顶 y=87..257（170px），
		// 行高 40 → 可见 4 行；行左缘 x=10。UIWindow.img/Skill/backgrnd 逐行扫描实测
		// （标题 0..22 / tab 23..54 / 书封暗栏 55..86 / 列表 87..257）。
		static constexpr Point<int16_t> CLASSIC_SKILL_OFFSET = Point<int16_t>(10, 87);
		// 原生经典窗列表区 y=87..257（170px），行高 40 → 可见 4 行。
		static constexpr int16_t CLASSIC_ROWS = 4;
		// 行内 SP 升级「+」钮相对行起点的偏移：实测 + 号框左上角 (132,119)。
		static constexpr Point<int16_t> CLASSIC_SPUP_OFFSET = Point<int16_t>(130, 20);
		// ---- 坐标口径（本轮实测钉死）------------------------------------------
		// oracle = 技能展开宏指令.png；它与 UIWindow.img/Skill/backgrnd 逐像素对齐
		// 后 xoff/yoff 均为 0（真机同口径复测：烘焙红下划线 ours 帧 x3..171 ==
		// backgrnd 帧 x3..171）。backgrnd 以 Sprite 偏移 (1,0) 绘制，故
		//     position == 帧原点 - (1,0)  ⇒  绘制点 Q = 帧坐标 + (1,0)。
		// 这条换算不是推演：Q=(2,23) 的底板真机落在帧 (1,23)、Q=(9,57) 的书页落在
		// 帧 (8,57)，两处都恰好比帧目标左 1px，与上式一致。
		// ----------------------------------------------------------------------
		// 底板 = UIWindow.img/Item/New/Tab0|Tab1/<页号>（34x19，灰=未选、粉=选中）：
		// 原生黑色轮廓外框落在帧 x3..36 / y23..41，与底板位图不透明外框 (0..33,0..18)
		// 同尺寸 ⇒ 底板左上 = 帧 (3,23) ⇒ Q (4,23)。页距 34（与背包窗同款底板的
		// UIItemInventory::tab_plate_pos 同一把尺）。
		static constexpr Point<int16_t> TAB_PLATE_TOPLEFT = Point<int16_t>(4, 23);
		static constexpr int16_t TAB_PLATE_PITCH = 34;
		// 页码点图标 Skill/Tab/enabled|disabled{0-4}（10x11 … 15x12）叠在底板中央：
		// 新手页的白色圆点墨迹 = 帧 x16..23 / y30..37，位图内白心 = 位图 x1..8 / y1..8
		// ⇒ 位图左上 = 帧 (15,29) ⇒ Q (16,29)。底板中心 x = 3+17 = 20 ⇒ 槽0中心 Q=21。
		// （10x11 的圆点：21 - 10/2 = 16 ✓）
		static constexpr int16_t TAB_SLOT0_CENTER = 21;
		static constexpr int16_t TAB_CENTER_Y = 34;
		// 书页图标 = Skill/<job>.img/info/icon（26x30，origin (-4,30)）。原生黑色
		// 轮廓落在帧 x10..35 / y57..84，位图内轮廓起点 (0,0) ⇒ 位图左上 = 帧 (10,57)
		// ⇒ Q (11,57) —— 也就是书名那条暗栏的左端，不是顶部 tab 条。
		static constexpr Point<int16_t> BOOK_TOPLEFT = Point<int16_t>(11, 57);
		// 书名在暗栏内水平居中：原生墨迹 = 帧 x76..133 / y67..78 ⇒ 中心 帧 104.5。
		// GraphicsGL::LayoutBuilder::add_line 里 CENTER 是「绘制点即文字中心」
		// （line_x -= ax/2，与 maxwidth 无关），故 Q.x = 104.5 + 1 ≈ 105。
		// 旧值 28 是按「150px 盒内居中」算的，那个语义在本引擎里不存在，于是整串
		// 文字压到书页图标上（真机实测 ours 墨迹中心 帧 17）。y 由 y_for_ink_top 反解。
		static constexpr int16_t BOOK_TEXT_CENTER_X = 105;
		static constexpr int16_t BOOK_TEXT_INK_TOP = 68;
		// 宏指令钮（Basic.img/BtMacro 48x18）：实测左上角 (121,265)。
		static constexpr Point<int16_t> MACRO_BTN_TOPLEFT = Point<int16_t>(121, 265);
		// 滚动条：Basic.img/VScr（宽12）prev/next 钮实测左上角 x=156、顶 y=100、底 y=242。
		static constexpr int16_t CLASSIC_SLIDER_X = 158;
		static constexpr int16_t CLASSIC_SLIDER_TOP = 87;
		static constexpr int16_t CLASSIC_SLIDER_BOTTOM = 258;
		static constexpr Point<int16_t> SKILL_META_OFFSET = Point<int16_t>(2, 2);
		static constexpr Point<int16_t> LINE_OFFSET = Point<int16_t>(0, 37);

		const CharStats& stats;
		const SkillBook& skillbook;

		Slider slider;
		Texture skille;
		Texture skilld;
		Texture skillb;
		Texture line;
		// 经典窗顶部 tab 底板由 TwoSpriteButton 自带（Tab1=粉=选中、Tab0=灰=未选），
		// 页码点图标 Skill/Tab/enabled|disabled{0-4} 只是叠在底板上的贴图，故这里
		// 存成两套 Texture，在 draw_buttons 之后按当前页选 en/dis 绘制。
		// 现代窗不画底板（无此件），页码点仍是可点按钮。
		std::array<Texture, 5> tab_glyph_en;
		std::array<Texture, 5> tab_glyph_dis;
		bool classic_tab_plate = false;
		Texture bookicon;
		Text booktext;
		Text splabel;

		Job job;
		int16_t sp;
		int16_t beginner_sp;

		uint16_t tab;
		uint16_t skillcount;
		uint16_t offset;

		std::vector<SkillDisplayMeta> skills;
		bool grabbing;
		int32_t selected_skill_id = 0;

		Point<int16_t> bg_dimensions;

		// v83 经典布局（Skill 无 main 子节点，backgrnd 175x289）：
		// 单列技能行 + tab 名绘于 tab 条 + 技能点行内显示 SP，
		// 坐标依据见 docs/features/beidou-v17-parity/notes/r05-ui-layout.md §3。
		bool classic = false;

		bool macro_enabled;
		Texture macro_backgrnd;
		Texture macro_backgrnd2;
		Texture macro_backgrnd3;
		std::array<Text, 5> macro_labels;

		bool sp_enabled;
		Texture sp_backgrnd;
		Texture sp_backgrnd2;
		Texture sp_backgrnd3;
		Charset sp_before;
		Charset sp_after;
		std::string sp_before_text;
		std::string sp_after_text;
		Text sp_used;
		Text sp_remaining;
		Text sp_name;
		Texture sp_skill;
		int32_t sp_id;
		int32_t sp_masterlevel;
	};
}