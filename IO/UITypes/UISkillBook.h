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

			void draw(const DrawArgument& args) const;

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
		// v83 经典窗单列技能行基准：行底左上角实测 (7,99)（打开了S-E-I-K页面.png，
		// 行框 y=99/139/179/219，pitch 40；行右缘 x=148 → 左缘 148-141=7）。
		static constexpr Point<int16_t> CLASSIC_SKILL_OFFSET = Point<int16_t>(7, 99);
		// v83 经典窗列表区实测 y=99..253（154px），行高 40 → 可见 4 行。
		static constexpr int16_t CLASSIC_ROWS = 4;
		// 行内 SP 升级「+」钮相对行起点的偏移：实测 + 号框左上角 (132,119)。
		static constexpr Point<int16_t> CLASSIC_SPUP_OFFSET = Point<int16_t>(125, 20);
		// 顶部职业 tab 条：5 个 33px 槽位，槽0中心 x=21、条竖直中心 y=34
		// （新手圆点图标实测左上角 (16,29)，10x11 → 中心 (21,34)）。
		static constexpr int16_t TAB_SLOT0_CENTER = 21;
		static constexpr int16_t TAB_SLOT_PITCH = 33;
		static constexpr int16_t TAB_CENTER_Y = 34;
		// 选中页的桃色横幅 = UIWindow.img/Skill/recommend/0（105x35，origin (16,0)）。
		// 实测（r15，参考截图 打开了S-E-I-K页面.png，窗口原点标定见 evidence/w9-skill/coords-r15.md）：
		// 橙色描边左缘 x4、右缘 x108（宽 105 ✓），上描边 y22、下描边 y56（高 35 ✓）。
		static constexpr Point<int16_t> RECOMMEND_TOPLEFT = Point<int16_t>(4, 22);
		// 书页图标 = Skill/<job>.img/info/icon（26x30，origin (-4,-1)），原生落在横幅左段：
		// 黑色轮廓左列 x323-309=14、上列 y27-2=25，轮廓在位图内缩 1px → 位图左上 (13,25)。
		static constexpr Point<int16_t> BOOK_TOPLEFT = Point<int16_t>(13, 25);
		// 书名（String/Skill.img/<job>/bookName）左对齐贴在书封暗条左缘：
		// 截图-贴图差分得墨迹 x6..59、y76..85；A11M 字形顶 = 绘制点 + 6 → 绘制点 (5,70)。
		static constexpr Point<int16_t> BOOK_TEXT_TOPLEFT = Point<int16_t>(5, 70);
		// 宏指令钮（Basic.img/BtMacro 48x18）：实测左上角 (121,265)。
		static constexpr Point<int16_t> MACRO_BTN_TOPLEFT = Point<int16_t>(121, 265);
		// 滚动条：Basic.img/VScr（宽12）prev/next 钮实测左上角 x=156、顶 y=100、底 y=242。
		static constexpr int16_t CLASSIC_SLIDER_X = 156;
		static constexpr int16_t CLASSIC_SLIDER_TOP = 100;
		static constexpr int16_t CLASSIC_SLIDER_BOTTOM = 242;
		static constexpr Point<int16_t> SKILL_META_OFFSET = Point<int16_t>(2, 2);
		static constexpr Point<int16_t> LINE_OFFSET = Point<int16_t>(0, 37);

		const CharStats& stats;
		const SkillBook& skillbook;

		Slider slider;
		Texture skille;
		Texture skilld;
		Texture skillb;
		Texture line;
		// 经典窗选中页横幅（Skill/recommend/0）；现代窗无此件，保持无效贴图即不画。
		Texture recommend;
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