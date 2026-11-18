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
#include "../Components/NameTag.h"

#include "../../Character/Look/CharLook.h"

namespace ms
{
	// The character selection screen
	class UICharSelect : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHARSELECT;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = false;

		UICharSelect(std::vector<CharEntry> characters, int8_t characters_count, int32_t slots, int8_t require_pic);

		void draw(float inter) const override;
		void update() override;

		void doubleclick(Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;

		UIElement::Type get_type() const override;

		void add_character(CharEntry&& character);
		void post_add_character();
		void remove_character(int32_t id);

		// 参考端 CUICharSelect::IsRequestValid 的 CLogin+368 门闩：
		// 选角/删除请求发出后、服务器回包前阻止重复点击。
		void clear_pending();

		const CharEntry& get_character(int32_t id);

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	private:
		void update_buttons();
		void update_selected_character();
		void select_last_slot();
		std::string get_slot_text();
		std::string pad_number_with_leading_zero(uint8_t value) const;
		/// <summary>
		/// 取第 index 个角色槽的帧内坐标。
		/// occupied 用于区分参照的纵向锚点：IDA 中占用槽与空槽的 CFadeWnd 选项点不同
		/// （1384 / 1348，差 36px），故需由调用方告知该槽是否已有角色。
		/// </summary>
		Point<int16_t> get_character_slot_pos(size_t index, bool occupied = false) const;
		Point<int16_t> get_infolabel_pos(size_t index) const;
		std::string get_infolabel(size_t index, StatsEntry character_stats) const;
		void request_pic();

		/// 每页槽位数。
		///
		/// 依据 IDA：参考版 `CUICharDetail::CUICharDetail`(0x60292F) 的槽位坐标为
		///   x = 130 * (a4 % 3) - 220（a4 为**绝对**角色索引），y = 占用 1384 / 空 1348
		/// —— **y 与索引无关**，只有 x 按 %3 循环，说明参考版同时在屏上只显示 **3** 个槽位，
		/// 翻页时换掉这 3 个槽承载的角色（故位置按 3 循环）。
		/// 且 `CUICharSelect::OnButtonClicked`(0x601667) 只处理 1000/1001/1002 三个按钮、
		/// 无翻页按钮，进一步印证「固定 3 槽 + 分页换角色」的模型。
		static constexpr uint8_t PAGESIZE = 3;
		static constexpr int16_t CHARSLOT_Y_MAX = 24;

		enum Buttons : uint16_t
		{
			BtSelect,
			BtNew,
			BtDelete,
			BtPageL,
			BtPageR,
			BtChangePIC,
			BtResetPIC,
			BtCharacter,
			BtPreview,
			BtHome,          // 左侧栏「回到首页」：reconnect → 登录页（区别于 BtPreview 回选世界页）
			CHARACTER_SLOT0
		};

		std::vector<CharEntry> characters;
		int8_t characters_count;
		int32_t slots;
		int8_t require_pic;
		Text version;
		Point<int16_t> version_pos;
		Point<int16_t> pagepos;
		Point<int16_t> BtNewPos;
		uint8_t selected_character;
		uint8_t selected_page;
		uint8_t page_count;
		bool request_pending = false;
		/// 一路回车：进页默认未确认（即使 ctor 预载 DefaultCharacter 视觉选中）。
		/// 首次回车选第 1 角色并置 true；再次回车 BtSelect 进游戏。
		/// 鼠标点槽/双击进游戏路径同步置 true，避免「已点选仍多按一次」。
		bool selection_confirmed = false;
		Texture tab;
		uint8_t tab_index;
		bool tab_active;
		bool tab_move;
		Point<int16_t> tab_pos[3];
		int16_t tab_move_pos;
		std::map<uint8_t, uint16_t> tab_map;
		Animation burning_notice;
		Point <int16_t> burning_numPos;
		Text burning_count;
		std::vector<Sprite> world_sprites;
		Texture charinfo;
		// 左侧栏四块（选角色页 step2）：标题牌 y36-85 / 世界信息条 y99-148 /
		// 查看所有角色 y377-399 / 回到首页 y427-449；照 UICommonCreation.cpp:492-534。
		// 本页 position 是相机值，装饰以屏幕绝对坐标单独绘制，按钮局部=绝对−position。
		Sprite cs_step;
		Sprite cs_world_box;
		Text cs_world_name;
		Text cs_channel;
	/// 详情卷轴：选中角色后 scroll/0 展开（217x30→158），内容板(charInfo1)绘于卷内。
	/// 2026-11-16：帧源由 scroll/2(193高) 改为与正确端吻合的 scroll/0(158高)。
	/// scroll 帧组是循环动画（Animation::update 永不返回 true），旧实现
	/// charinfo_scroll_playing 永真 → 卷轴无限循环+内容板永不绘制（闪烁）。
	/// 改为手动帧播放：播完定格末帧（完全展开的卷轴）。
	std::vector<Frame> charinfo_scroll_frames;
	int16_t charinfo_scroll_frame = 0;
	uint16_t charinfo_scroll_elapsed = 0;
	bool charinfo_scroll_playing = false;
	bool charinfo_has_scroll = false;
	/// 上次播放展开动画的选中项（页*PAGESIZE+槽位），用于去重重播。
	int16_t last_detail_key = -1;
		Point<int16_t> avatarLT;
		Point<int16_t> avatarRB;
		Point<int16_t> avatarSpace;
		Point<int16_t> jobPos;
		Point<int16_t> levelPos;
		Point<int16_t> famePos;
		Point<int16_t> namePos;
		Point<int16_t> statDEXPos;
		Point<int16_t> statINTPos;
		Point<int16_t> statLUKPos;
		Point<int16_t> statSTRPos;
		Texture charslot;
		Point<int16_t> charslot_pos;
		Rectangle<int16_t> charslot_bounds;
		Texture pagebase;
		Charset pagenumber;
		nl::node pagenumberpos;
		Texture signpost[3];
		nl::node nametag;
		Charset levelset;
		OutlinedText namelabel;
		std::vector<CharLook> charlooks;
		std::vector<NameTag> nametags;
		Animation emptyslot_effect;
		Texture emptyslot;
		Animation selectedslot_effect[2];
		OutlinedText charslotlabel;
		int16_t timestamp;
		uint16_t charslot_y;
		bool use_timestamp;
		bool show_timestamp;
		bool burning_character;
		bool show_pic_btns;

		// opt-in 自动进游戏挂起标志：自动登录到达选角后，首帧调 button_pressed(BtSelect)，
		// 与手点「开始游戏」同一路径（require_pic==2 时直接 SelectCharPacket 进图）。
		bool auto_enter_pending = false;

		enum InfoLabel : uint8_t
		{
			JOB,
			FAME,
			STR,
			DEX,
			INT,
			LUK,
			NUM_LABELS
		};

		/// m08338：Angel get_basic_font(1) 单层细黑字，去 OutlinedText 四向描边副本。
		Text infolabels[UICharSelect::InfoLabel::NUM_LABELS];
		/// 选中角色详情排行带（有排行绘名次，否则「排行不可用」）。
		Text ranklabel;
	};
}