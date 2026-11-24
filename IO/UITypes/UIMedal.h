////////////////////////////////////////////////////////////////////////////////
//	D1 / 分号键「勋章」窗（v83 MEDAL = CUIMedalQuestInfo，Angel ctor @0x840170）
//
//	原生模型（2026-11-11 turn8 取证 + W2 2026-11-20 IMG 复核）：
//	  ctor CreateWnd 260x374；CCtrlTab + 初始 SetTab(100) 「基本」= 已获得全量。
//	  Draw @0x843bf6：tab==100 步进 49；分类 0..3 步进 23。
//	  行点击 OnMouseButton @0x841678 → 再点 ToggleDetail → Detail 305x373 @0x83d255。
//	  数据源 QuestInfo.img viewMedalItem × QuestLog::is_completed。
//
//	素材（W2 / orange-wz live UIWindow.img/Title）：
//	  Title/backgrnd 260x374、Title/Tab/{enabled,disabled}/0..4、
//	  Title/backgrnd3|5 305x373 Detail 框 —— 有节点则优先真材，缺则 ColorBox 兜底。
//	  历史 SAD 否定仅针对截帧像素匹配，不否决节点存在时的源码级绑定。
//	  114xxxx 无画布 → 文本态；禁止盲补 WZ。
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

		// W2：UIWindow.img/Title 真材（有则优先生效）
		bool has_title_frame = false;
		bool has_detail_frame = false;
		Texture title_frame;			// Title/backgrnd 260x374
		Texture detail_tex;				// Title/backgrnd3 305x373
		std::vector<Texture> tab_on;	// Title/Tab/enabled/0..4
		std::vector<Texture> tab_off;	// Title/Tab/disabled/0..4

		ColorBox detail_frame;
		ColorBox detail_back;
		Text detail_title;
		Text detail_line[6];
	};
}
