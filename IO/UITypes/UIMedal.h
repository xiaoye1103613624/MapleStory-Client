////////////////////////////////////////////////////////////////////////////////
//	D1 / 分号键「勋章」窗（v83 MEDAL = CUIMedalQuestInfo，Angel ctor @0x840170）
//
//	原生模型（2026-11-11 turn8 取证，evidence/d1b_ida.json + d1c_detail.json）：
//	  ctor CreateWnd 260x374；CCtrlTab 4 页签 + 初始 SetTab(100) —— 「基本」页
//	  不是页签 0，而是 tab==100 的「已获得全量列表」特殊态（Draw @0x843bf6：
//	  v6==100 分支步进 49 双行文本；v6 0..3 分支步进 23 分类目录）。
//	  中文勋章框.png 5 页签 = [基本(=100 全量) | 职业 | 一般 | 挑战 | 活动]，
//	  分类页签 0..3 ↔ medalCategory 1/缺失/2/3（探针分布 1:16/2:28/3:2/缺失:26）。
//	  数据源 = QuestInfo.img viewMedalItem≠0（82 条）× QuestLog::is_completed。
//	  行点击（OnMouseButton @0x841678）：命中行→选中；再点已选中行→ToggleDetail
//	  → CUIMedalQuestInfoDetail 305x373（Angel ctor @0x83d255）。P1：无 114xxxx
//	    图标素材时以文本 Detail 浮层（305x373 ColorBox）呈现任务名/道具号/摘要。
//	  状态文本 SP_5301「可获得」/SP_5302「挑战中」（IsStateButton 语义，
//	  本地以 meets_requirements 近似，注释标明）。
//
//	素材判定（探针否定，不盲改）：
//	  · Title 根件对 勋章框.png SAD=46~214 无一坐实（d1b_title_assets.json）→
//	    窗框保留 ColorBox 复用件，不换 Title。
//	  · Item.nx 无 114xxxx 勋章画布 → 图标列仍预留；String/Eqp 有名则文本态显示。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIElement.h"

#include "../../Graphics/Color.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class UIMedal : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::MEDAL;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIMedal();

		void draw(float inter) const override;
		void update() override;
		void toggle_active() override;
		Button::State button_pressed(uint16_t buttonid) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		void send_scroll(double yoffset) override;
		UIElement::Type get_type() const override;

	private:
		enum Buttons : uint16_t
		{
			BtClose
		};

		// ctor @0x840170 CreateWnd nWidth:260 nHeight:374（截帧 255x372 为裁切）
		static constexpr int16_t FRAME_W = 260;
		static constexpr int16_t FRAME_H = 374;

		// 原生框架色带（勋章框.png 逐像素量测，evidence/medal_frame_metrics.json
		// + turn10 定点采样）：
		//   y0 外沿蓝(85,170,221) / y1 深蓝线(85,119,153) / y2..~20 白标题区
		//   （「勋章」黑字左对齐 + 右上蓝 X 34,170,238）/ 标题下 1px 蓝线
		//   (119,153,187) / 页签带底 (170,170,170) / 内容区浅蓝白(238,238,255)
		//   / 底条蓝灰(153,187,204) y351.. / 边框浅灰(221,221,221)。
		// 页签：y28..45（高 18），选中=粉红底(238,102,136)+黑字，未选=灰阶
		// (187..221)；x 分界实测约 5 格均分于 3..240。
		static constexpr int16_t TITLE_H = 22;			// 白标题区高（y1..22）
		static constexpr int16_t TAB_Y = 28;
		static constexpr int16_t TAB_H = 18;
		static constexpr int16_t TAB_X0 = 6;
		static constexpr int16_t TAB_W = 46;
		static constexpr int16_t TAB_GAP = 2;
		static constexpr int16_t TAB_COUNT = 5;

		// 计数条：白底横条 y46..79（量测 count_bar y0=46 y1=79），文字深灰(51,51,51)
		static constexpr int16_t COUNT_Y = 46;
		static constexpr int16_t COUNT_H = 20;
		// 列表区：浅蓝白 (238,238,255)，y78..350（底条起 y351）
		static constexpr int16_t LIST_Y = 78;
		static constexpr int16_t LIST_BOT = 351;
		static constexpr int16_t LIST_X = 5;
		static constexpr int16_t LIST_W = 232;
		// 滚动条槽：x239..253（量测 239-250 列），上下按钮 (221,221,221)
		static constexpr int16_t SB_X = 239;
		static constexpr int16_t SB_W = 14;
		static constexpr int16_t SB_BTN_H = 14;
		// 底条：蓝灰 (153,187,204) y351..371
		static constexpr int16_t BOTBAR_Y = 351;

		// Draw @0x843bf6 立即数：SetClipRect(7,48,230,291) 行内容裁剪矩形。
		// 注意：这是原生 CWnd 客户区坐标，与截帧量测色带（LIST_*，255x372
		// 裁切帧坐标系）不同坐标系，y 相差约 30px（标题/页签/计数带）。
		// 此处作为原生证据独立编码，行绘制裁剪以 LIST_* 可视带为准。
		static constexpr int16_t CLIP_L = 7;
		static constexpr int16_t CLIP_T = 48;
		static constexpr int16_t CLIP_R = 230;
		static constexpr int16_t CLIP_B = 291;

		// Draw @0x843bf6 立即数：行步进 分类页 23 / 获得页 49；行底条
		// FillRect 高 18；文本 x=29，状态 x=55。
		static constexpr int16_t ROW_H_CAT = 23;
		static constexpr int16_t ROW_H_GOT = 49;
		static constexpr int16_t STRIP_H = 18;

		struct MedalEntry
		{
			int16_t qid;			// 勋章任务 id（QuestInfo.img/<qid>）
			int32_t medal_item;		// viewMedalItem（勋章道具 id，114xxxx 段）
			int16_t category;		// medalCategory；0 = 字段缺失（归「一般」）
		};

		void build_catalog();
		void refresh_view();
		void select_tab(int16_t index);
		int16_t row_h() const;		// 基本页 49 / 分类页 23
		int32_t hit_row(Point<int16_t> cursorpos) const;	// -> view 下标，-1 无
		bool obtainable(const MedalEntry& e) const;		// IsStateButton 近似
		void open_detail(int32_t view_row);
		void close_detail();
		void draw_detail(float fade) const;
		static std::string medal_item_name(int32_t item_id);

		// CUIMedalQuestInfoDetail CreateWnd 305x373（Angel @0x83d255）
		static constexpr int16_t DETAIL_W = 305;
		static constexpr int16_t DETAIL_H = 373;

		int16_t active_tab = 0;		// UI 页签 0..4（0=基本=原生 100）
		int16_t scroll = 0;
		int32_t selected = -1;		// view_rows 下标（原生 *(this+370)）
		int32_t obtained_total = 0;
		bool detail_open = false;

		std::vector<MedalEntry> catalog;
		std::vector<size_t> view_rows;
		std::vector<Text> row_primary;	// 任务名（含勋章名）
		std::vector<Text> row_status;	// 已获得 / 可获得 / 挑战中
		std::vector<Texture> row_icons;	// 114xxxx Character/Accessory icon（无则空）

		ColorBox frame_back;
		ColorBox frame_edge;
		ColorBox title_line_top;	// 顶 1px 深蓝线 (85,119,153)
		ColorBox title_line_bot;	// 标题下 1px 蓝线 (119,153,187)
		ColorBox list_back;			// 页签带灰底 (170,170,170)
		ColorBox list_panel;		// 列表面板 浅蓝白 (238,238,255)
		ColorBox count_bar_bg;		// 计数条白底 y46..79
		ColorBox bottom_bar;		// 底条 蓝灰 (153,187,204) y351..
		ColorBox row_strip;
		ColorBox sel_strip;
		ColorBox scroll_track;
		ColorBox scroll_thumb;
		ColorBox scroll_btn_top;
		ColorBox scroll_btn_bot;
		std::vector<ColorBox> tab_plate;
		std::vector<Text> tab_label;
		Text title;
		Text count_label;
		Text empty_hint;

		ColorBox detail_frame;
		ColorBox detail_back;
		Text detail_title;
		Text detail_line[6];
	};
}
