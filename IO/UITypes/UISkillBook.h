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

#include <chrono>

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
		// Wave3 2026-11-13：以 evidence/w9-skill/coords-r15.md 为准（skill1 模板匹配）。
		// 行底 Skill/skill1 @ 窗口 (7,99)，行距 40；SP「+」框绝对 (132,119)
		// ⇒ CLASSIC_SKILL+(125,20)=(132,119)。旧值 (10,87)+(130,20)=(140,107) 与
		// 注释里的 (132,119) 自相矛盾，会把四行列表整体上移 12px、右移 3px。
		static constexpr Point<int16_t> CLASSIC_SKILL_OFFSET = Point<int16_t>(7, 99);
		static constexpr int16_t CLASSIC_ROWS = 4;
		static constexpr Point<int16_t> CLASSIC_SPUP_OFFSET = Point<int16_t>(125, 20);
		// ---- 坐标口径（Wave3：backgrnd 无 Sprite 偏移，Q == 帧坐标）--------------
		// oracle = 技能展开宏指令.png；Skill/backgrnd 模板匹配 best=(0,0)。
		// ----------------------------------------------------------------------
		// 底板 = Item/New/Tab0|Tab1（34x19）：帧外框 x3..36 / y23..41 ⇒ 左上 (3,23)。
		static constexpr Point<int16_t> TAB_PLATE_TOPLEFT = Point<int16_t>(3, 23);
		static constexpr int16_t TAB_PLATE_PITCH = 34;
		// 页码点居中：底板中心 x = 3+17 = 20。
		static constexpr int16_t TAB_SLOT0_CENTER = 20;
		static constexpr int16_t TAB_CENTER_Y = 34;
		// 书页图标：帧轮廓 x10..35 / y57..84 ⇒ 左上 (10,57)。
		static constexpr Point<int16_t> BOOK_TOPLEFT = Point<int16_t>(10, 57);
		// 书名墨迹中心 ≈ 帧 104.5；CENTER 绘制点即中心。
		static constexpr int16_t BOOK_TEXT_CENTER_X = 104;
		static constexpr int16_t BOOK_TEXT_INK_TOP = 68;
		// 宏指令钮：原 Q(121,265) 含 +1 假偏移 ⇒ 帧 (120,265)。
		static constexpr Point<int16_t> MACRO_BTN_TOPLEFT = Point<int16_t>(120, 265);
		// 滚动条：轨道贴列表区 y=99..258（与 CLASSIC_SKILL_OFFSET / 4×40 行同源）。
		static constexpr int16_t CLASSIC_SLIDER_X = 157;
		static constexpr int16_t CLASSIC_SLIDER_TOP = 99;
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

		// L2：083 OnSkillLevelUpButton 前 CanSendExclRequest(500,0) 等价本地节流（不动 wire）。
		std::chrono::steady_clock::time_point last_sp_send{};
	};
}