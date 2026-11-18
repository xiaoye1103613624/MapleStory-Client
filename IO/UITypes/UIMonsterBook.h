////////////////////////////////////////////////////////////////////////////////
//	怪物图鉴窗口：v83 书本双页版式（D2 修复，2026-11-11）。
//
//	证据链（三份对齐，非截图猜）：
//	  · orange-wz MCP 128B gate：UIWindow.img/MonsterBook 15 直接子节点 + 尺寸
//	    backgrnd 475x349 / cardSlot 174x256 / infoPage 170x231 / select·cover
//	    31x42 / fullMark 7x11 / icon 0..7 22x27 / LeftTab 0..8 20x20 /
//	    RightTab 0..3 52x38 / BtClose 18x16 / BtSearch 34x17 / arrow* 23~24x16
//	    （evidence/orange_wz_monsterbook_facts.json）
//	  · Angel 083 IDA：sub_86256D 建钮立即数 —— BtClose id1000 (429,8)、
//	    BtSearch id2000 (175,29)、arrowLeft id2001 (48,285)、arrowRight id2002
//	    (185,285)、arrowLeft id2003 (270,285)、arrowRight id2004 (407,285)、
//	    控件 2005 (49,30,120x15)；UpdateCheckList @0x867be6 逐 i<9 设勾选态
//	    （evidence/ida_monsterbook_ctor.json）
//	  · 正确客户端截图/怪物手册.png 540x357：backgrnd 模板匹配 meanSAD 57
//	    定窗锚 (34,7)，infoPage 窗相对 (40,35) SAD 36.8、cardSlot (261,53)
//	    SAD 43.1、LeftTab y=131+20i SAD 0.0、RightTab y=30+39i
//	    （evidence/d2_canvas_anchor.json、d2_tab_anchor.json）
//
//	数据面：Character/MonsterBook 存 cardid%10000 → level（与 s9 addMonsterBookInfo
//	一致）。卡片道具 238xxxx 的 %10000 与 String/MonsterBook.img 的 mob 键无直接
//	相等关系；P1 用各条目 reward 中的 238xxxx 建 reverse map（suffix→mobId），
//	再读 Mob.img 名与 MonsterBook episode/map/reward。便签套筛选仍预留。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../../Character/MonsterBook.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Texture.h"
#include "../../Graphics/Text.h"

#include <map>
#include <vector>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 怪物图鉴：左页 = 搜索 + 5×5 卡片格（cardSlot），右页 = 选中怪名/HP/MP +
	// RightTab 四页签；左列 LeftTab = 分类筛选，BtSearch = 编号/名称检索。
	// 「基本情报」书级统计（等级/卡片数）走 infoPage 左页（无选中卡时）。
	class UIMonsterBook : public UIDragElement<PosMONSTERBOOK>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MONSTERBOOK;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMonsterBook();

		void draw(float inter) const override;
		void update() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		void send_scroll(double yoffset) override;
		UIElement::Type get_type() const override;

		// 封面/卡片数据变化后由 handler 调用刷新
		void reload();
		int16_t get_selected_card_id() const { return selected_card_id; }

	private:
		struct CardEntry
		{
			int16_t id;
			int8_t level;
		};

		void load_cards();
		void apply_note_filter();
		void sync_note_indicators();	// UpdateCheckList：有卡的便签亮
		void sync_infotab_indicators();	// 右标签常驻激活态：旧签 NORMAL + 当前 PRESSED（UISkillBook::change_tab 范式）
		void select_card_by_index(int32_t index);	// 全局序号（含跨页，相对当前 view）
		void page_cards(int8_t delta);
		int32_t page_count() const;
		int32_t cell_at(Point<int16_t> cursorpos) const;	// -> 全局序号，-1 无
		void refresh_info();
		void set_note(int16_t index, bool on);
		// card_suffix(cardid%10000) → Mob.img / MonsterBook.img 的 mobId；0=未命中
		static int32_t mob_id_for_card(int16_t card_suffix);
		static std::string mob_name_of(int32_t mob_id);
		static std::string truncate_utf8(const std::string& s, size_t max_chars);
		// P3：Angel LoadCard @0x6848a6 — category=(itemId-2380000)/1000 ≡ card_suffix/1000，钳到 0..8。
		static int16_t note_category_of(int16_t card_suffix);

		// 按钮 id：1000/2000..2004 沿用原生 id，页签/便签另起段避免撞号
		enum Buttons : uint16_t
		{
			BtClose = 1000,
			BtSearch = 2000,
			BtPrevCard = 2001,
			BtNextCard = 2002,
			BtPrevPage = 2003,
			BtNextPage = 2004,
			BtNote0 = 3000,			// 3000..3008 = LeftTab 0..8
			BtInfoTab0 = 3100		// 3100..3103 = RightTab 0..3
		};

		// 正确截图（怪物手册2/卡片页）：左页 = 搜索+5×5 卡片格，右页 = 怪名/HP/MP。
		// cardSlot 174×256 落左页；单元 31×42（= select/cover）；旧「右页格」系
		// 对基本情报截图的误匹配，已翻案。
		static constexpr int16_t GRID_X = 48;
		static constexpr int16_t GRID_Y = 55;
		static constexpr int16_t CELL_W = 31;
		static constexpr int16_t CELL_H = 42;
		static constexpr int16_t GRID_COLS = 5;
		static constexpr int16_t GRID_ROWS = 5;
		static constexpr int16_t CARDS_PER_PAGE = GRID_COLS * GRID_ROWS;
		static constexpr int16_t DETAIL_X = 270;
		static constexpr int16_t DETAIL_NAME_Y = 40;
		static constexpr int16_t DETAIL_HP_Y = 280;
		static constexpr int16_t DETAIL_MP_Y = 300;

		static constexpr int16_t NOTE_COUNT = 9;
		static constexpr int16_t INFOTAB_COUNT = 4;

		bool has_assets = false;
		bool has_grid = false;

		// 原生画布件（13 节点全绑）
		Texture backgrnd;
		Texture infoPage;
		Texture cardSlot;
		Texture selectOv;
		Texture coverOv;
		Texture fullMark;
		std::vector<Texture> note_icon;		// LeftTab 0..8（画布态由按钮件承担）
		std::vector<Texture> cell_icon;		// icon 0..7 卡片占位图

		std::vector<CardEntry> all_cards;		// 玩家持有全量
		std::vector<CardEntry> cards;			// 当前便签筛选视图
		int32_t card_page = 0;
		int32_t selected_index = -1;			// 相对 cards 的全局序号
		int16_t selected_card_id = 0;
		int16_t active_note = -1;				// 便签筛选（-1 = 全部）
		int16_t active_infotab = 0;
		std::vector<bool> note_checked;			// UpdateCheckList：该分类是否有卡

		Text header;			// 原生控件 2005 (49,30) 120x15
		Text info_line[6];
		// 基本情报页数值列：infoPage.png 已印死 5 个原生标签，数值按 Angel
		// DrawTextA nTop=5/56/89/108/146（canvas 相对，右对齐 nLeft=140-w）
		// 填在标签右侧同一行。独立 RIGHT 对齐 Text，避免与卡片页 info_line 混用。
		Text stats_value[5];
		Text page_label;
		Text status_line;
	};
}
