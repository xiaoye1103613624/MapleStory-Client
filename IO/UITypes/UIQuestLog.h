////////////////////////////////////////////////////////////////////////////////
//	UIQuestLog
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"

#include "../Components/Slider.h"
#include "../Components/TwoSpriteButton.h"
#include "../Components/MapleButton.h"

#include "../../Character/QuestLog.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Color.h"
#include "../../Graphics/Geometry.h"
#include "../../Graphics/Texture.h"
#include "../../Template/Rectangle.h"
#include "../../Data/QuestData.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace ms
{
	class UIQuestLog : public UIDragElement<PosQUEST>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::QUESTLOG;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIQuestLog(const QuestLog& questlog);

		void draw(float inter) const override;
		void update() override;
		void send_key(int32_t keycode, bool pressed, bool escape) override;
		UIElement::Type get_type() const override;
		void refresh();

		int16_t get_selected_quest_id() const
		{
			return selected_id;
		}

	protected:
		Button::State button_pressed(uint16_t buttonid) override;

	public:
		void doubleclick(Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		void remove_cursor() override;

	private:
		enum Buttons : uint16_t
		{
			TAB0,
			TAB1,
			TAB2,
			CLOSE,
			COLLAPSE,
			MARKNPC,
			DETAIL,
			GIVEUP,
			OK,
			NO,
			ALERT,
			COUNT
		};

		struct Row
		{
			bool header;
			std::string label;
			int16_t id;
			int32_t area;
		};

		void change_tab(uint16_t tabid);
		void on_list_scroll(bool upwards);
		void on_detail_scroll(bool upwards);
		void rebuild_list();
		void select_row(size_t index);
		void update_detail();
		int16_t row_index_at(Point<int16_t> local) const;
		const Texture* current_icon() const;
		void draw_gauge() const;
		std::string area_name(int32_t area) const;
		void set_collapsed(bool collapsed);
		void relayout_slots();
		void layout();
		void activate_buttons();
		int16_t visible_rows() const;
		int16_t detail_visible_rows() const;
		int16_t gauge_ratio() const;
		void update_sliders();
		// 命中区必须等于绘制区，否则右侧 241px 的点击会被窗口自己吞掉（见 cpp 注释）。
		void update_dimension();
		// 按区域分组追加行；处于收起状态的区域只保留分组标题行。
		void append_grouped(const std::vector<int16_t>& ids);
		void toggle_area(int32_t area);
		int16_t detail_content_height() const;
		// 正文总高折算成滚动格数（一格 = DETAIL_ROW_H），滚动条按格数而非段落数计。
		int16_t detail_total_notches() const;
		void load_npc_portrait(int16_t quest_id);

		const QuestLog& questlog;

		uint16_t tab;
		int16_t selected_id;
		int16_t scroll;
		int16_t detail_scroll;

		bool collapsed;
		// true = 详细 (level/area/pre-quest lines from QuestData), false = 简述 (summary).
		bool detail_mode;

		Texture bg_left;
		Texture bg_right;
		Point<int16_t> bg_right_pos;
		int16_t left_width;
		int16_t right_width;

		std::unique_ptr<Texture> icon_avail;
		std::unique_ptr<Texture> icon_progress;
		std::unique_ptr<Texture> icon_complete;
		// 分组行的"书"标记 = Quest/icon4（原生实测的蓝色书本，非箭头）。
		std::unique_ptr<Texture> icon_header;

		// 分类条：Quest/Tab{i} 只是透明字图（选中=白字、未选=粉字），粉底/灰底与黑描边
		// 在原生里自绘（backgrnd 里没有这两块，参考截图实测粉 (238,102,136)、灰 (187,187,187)）。
		ColorBox tab_border[3];
		ColorBox tab_fill_active[3];
		ColorBox tab_fill_idle[3];
		ColorBox row_highlight;       // selected quest row (dark blue)
		ColorBox header_highlight;    // group header row (light blue)

		Texture gauge_frame;
		Texture gauge_bar;
		Texture gauge_spot;
		Point<int16_t> gauge_frame_origin;

		Text detail_title;
		std::vector<Text> detail_lines;
		// 右板顶部蓝横幅里 NPC 立绘（Npc.nx/<id>/stand/0），无 npc 时为空贴图。
		Texture npc_portrait;

		std::vector<Row> rows;
		// 被收起的区域 id 集合（不在集合里 = 展开）。原生分组标题可点，点击即折叠/展开该区域。
		std::set<int32_t> collapsed_areas;

		Texture notices[3];
		bool has_notice[3] = {};

		Slider slider;
		Slider detail_slider;
		Rectangle<int16_t> list_area;
		// 右板正文区（详情/简述共用的滚动范围）。
		Rectangle<int16_t> detail_area;
		int16_t row_height;
		int16_t tab_x[3];
		int16_t tab_w[3];
		int16_t tab_y;
		int16_t tab_height;

		// 按钮贴图左上角（窗口相对）与宽高；构造期先算好，再喂给 try_maple_btn。
		Point<int16_t> slot_tl[Buttons::COUNT];
		Point<int16_t> slot_dim[Buttons::COUNT];

		Point<int16_t> detail_title_pos;
		Point<int16_t> detail_desc_pos;
		Point<int16_t> notice_pos;
		Point<int16_t> gauge_pos;
	};
}
