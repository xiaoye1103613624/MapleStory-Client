////////////////////////////////////////////////////////////////////////////////
//	UIQuestLog
////////////////////////////////////////////////////////////////////////////////
#include "UIQuestLog.h"

#include "../../Gameplay/Stage.h"
#include "../../Data/QuestData.h"
#include "../../Net/Packets/QuestPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"
#include "../../Util/ResolutionTiers.h"
#include "../../Util/UiAssetResolve.h"

#include <cctype>
#include <cstdlib>
#include <functional>
#include <map>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// 底带：backgrnd 实测两条深蓝线在 y367 / y391（带内 23px），17 高按钮居中 → 左上角 y=370。
	static constexpr int16_t BAND_TOP = 367;
	static constexpr int16_t BAND_Y = 370;
	// 角锚内缩（conventions §3.2）：标题条 k=7（关闭钮实测左上 226、展开钮 528），底带 k=9（寻找NPC位置右缘 538）。
	static constexpr int16_t TITLE_INSET = 7;
	static constexpr int16_t BAND_INSET = 9;
	static constexpr int16_t BTN_GAP = 6;
	static constexpr int16_t TITLE_Y = 5;
	// 分类条：黑描边 x2/66/131/196、y22..40，内衬粉底 x4..64 y24..39（参考截图 1:1 实测）。
	static constexpr int16_t TAB_FILL_INSET = 2;
	static constexpr int16_t TAB_FILL_H = 16;
	static constexpr int16_t DETAIL_ROW_H = 19;

	UIQuestLog::UIQuestLog(const QuestLog& questlog) :
		UIDragElement<PosQUEST>(Point<int16_t>(245, 45)),
		questlog(questlog),
		tab(0),
		selected_id(-1),
		scroll(0),
		detail_scroll(0),
		collapsed(false),
		detail_mode(false),
		left_width(245),
		right_width(305),
		row_height(23),
		tab_x{ 2, 66, 131 },
		tab_w{ 64, 65, 65 },
		tab_y(22),
		tab_height(18)
	{
		nl::node quest = nl::nx::UI["UIWindow.img"]["Quest"];
		nl::node root = quest;

		if (quest["list"])
			root = quest["list"];

		if (UiAssetResolve::usable(root["backgrnd"]))
			bg_left = Texture(root["backgrnd"]);

		if (UiAssetResolve::usable(root["backgrnd2"]))
			bg_right = Texture(root["backgrnd2"]);

		left_width = bg_left.width();
		right_width = bg_right.width();

		// 右板左缘压在左板右边框上：参考截图接缝实测（左板右边框 x=241，右板左边框 x=243）。
		bg_right_pos = Point<int16_t>(left_width > 2 ? left_width - 2 : left_width, 0);
		update_dimension();

		auto load_tex = [](nl::node src) -> std::unique_ptr<Texture>
		{
			return UiAssetResolve::usable(src) ? std::make_unique<Texture>(src) : nullptr;
		};

		// 行首项目符（原生动画序列取首帧）：可接=金"!"、进行中=蓝箭头、完成=灰"!"。
		icon_avail = load_tex(quest["icon0"]);
		icon_progress = load_tex(quest["icon2"]["0"]);
		icon_complete = load_tex(quest["icon1"]);
		// 分组行标记 = Quest/icon4（参考截图里那本蓝书，不是箭头）。
		icon_header = load_tex(quest["icon4"]);

		// 行底色没有对应位图（backgrnd 里只有空白列表区），按参考截图实测自绘。
		// 右缘必须收在 list_area.right()=226 之内，否则会从滚动条底下透出来：
		// 选中行 x25 起、宽 199（到 224）；分组行占满列表宽 x7 起、宽 219（到 226）。
		row_highlight = ColorBox(199, 19, Color(0x336699FF), 1.0f);
		header_highlight = ColorBox(219, 19, Color(0x99BBCCFF), 1.0f);

		for (uint16_t i = 0; i < 3; i++)
		{
			tab_border[i] = ColorBox(tab_w[i] + 1, tab_height + 1, Color(0x000000FF), 1.0f);
			tab_fill_active[i] = ColorBox(tab_w[i] - 3, TAB_FILL_H, Color(0xEE6688FF), 1.0f);
			tab_fill_idle[i] = ColorBox(tab_w[i] - 3, TAB_FILL_H, Color(0xBBBBBBFF), 1.0f);
		}

		if (UiAssetResolve::usable(quest["Gauge"]["frame"]))
		{
			gauge_frame = Texture(quest["Gauge"]["frame"]);
			gauge_frame_origin = gauge_frame.get_origin();
		}
		if (UiAssetResolve::usable(quest["Gauge"]["gauge"]))
			gauge_bar = Texture(quest["Gauge"]["gauge"]);
		if (UiAssetResolve::usable(quest["Gauge"]["spot"]))
			gauge_spot = Texture(quest["Gauge"]["spot"]);

		detail_title = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE);

		for (uint16_t i = 0; i < 3; i++)
		{
			nl::node tabdis = quest["Tab"]["disabled"][i];
			nl::node taben = quest["Tab"]["enabled"][i];

			if (!UiAssetResolve::usable(taben))
				taben = tabdis;
			if (!UiAssetResolve::usable(tabdis))
				tabdis = taben;

			Texture glyph(taben);
			int16_t w = glyph.width();
			int16_t sh = glyph.height();

			if (w <= 0)
			{
				Texture alt(tabdis);
				w = alt.width();
				sh = alt.height();
				glyph = alt;
			}

			// 字图水平居中在分类格里，垂直落在内衬里（实测字图 y24 起）；原点加回去才是左上角。
			Point<int16_t> tl(tab_x[i] + (tab_w[i] - w) / 2, tab_y + TAB_FILL_INSET);
			tl = tl + glyph.get_origin();

			// 原生"选中=白字(disabled)、未选=粉字(enabled)"，TwoSpriteButton 的选中态取第二参。
			buttons[Buttons::TAB0 + i] = std::make_unique<TwoSpriteButton>(taben, tabdis, tl, tl);
		}

		buttons[Buttons::TAB0]->set_state(Button::State::PRESSED);

		nl::node uiroot = nl::nx::UI;

		// 关闭钮走 UiAssetResolve::close_btn()：本包 Basic.img/BtClose 是**实心**方块，
		// 原生标题栏那颗描边 X 其实是 BtClose2；BtUIClose(32x15 "X CLOSE") 只作最后兜底。
		nl::node close = UiAssetResolve::close_btn();

		if (!UiAssetResolve::usable(close))
			close = uiroot["UIWindow.img"]["BtUIClose"];

		// 参考截图右板右上角那颗绿色左箭头钮 = Basic.img/BtHide，用作展开/收缩开关。
		nl::node hide = uiroot["Basic.img"]["BtHide"];

		// RC12 根因 1：坐标必须在构造之前算好，故先量宽高再 relayout_slots()。
		auto measure = [&](uint16_t id, nl::node src)
		{
			nl::node n0 = src["normal"]["0"];

			if (UiAssetResolve::usable(n0))
				slot_dim[id] = Texture(n0).get_dimensions();
			else
				slot_dim[id] = Point<int16_t>(0, 0);
		};

		measure(Buttons::CLOSE, close);
		measure(Buttons::COLLAPSE, hide);
		measure(Buttons::MARKNPC, quest["BtMarkNpc"]);
		measure(Buttons::DETAIL, quest["BtDetail"]);
		measure(Buttons::GIVEUP, quest["BtGiveup"]);
		measure(Buttons::OK, quest["BtOK"]);
		measure(Buttons::NO, quest["BtNo"]);
		measure(Buttons::ALERT, quest["BtAlert"]);

		relayout_slots();

		try_maple_btn(buttons, Buttons::CLOSE, close, slot_tl[Buttons::CLOSE], "UIQuestLog");
		try_maple_btn(buttons, Buttons::COLLAPSE, hide, slot_tl[Buttons::COLLAPSE], "UIQuestLog");
		try_maple_btn(buttons, Buttons::MARKNPC, quest["BtMarkNpc"], slot_tl[Buttons::MARKNPC], "UIQuestLog");
		try_maple_btn(buttons, Buttons::DETAIL, quest["BtDetail"], slot_tl[Buttons::DETAIL], "UIQuestLog");
		try_maple_btn(buttons, Buttons::GIVEUP, quest["BtGiveup"], slot_tl[Buttons::GIVEUP], "UIQuestLog");
		// 接受/拒绝/任务讯息：底带实测只证到 寻找NPC位置，其余三颗无位可放且不发包 → 预留（见 coords.md）。
		try_maple_btn(buttons, Buttons::OK, quest["BtOK"], slot_tl[Buttons::OK], "UIQuestLog");
		try_maple_btn(buttons, Buttons::NO, quest["BtNo"], slot_tl[Buttons::NO], "UIQuestLog");
		try_maple_btn(buttons, Buttons::ALERT, quest["BtAlert"], slot_tl[Buttons::ALERT], "UIQuestLog");

		for (uint16_t i = 0; i < 3; i++)
		{
			nl::node n = quest["notice" + std::to_string(i)];

			if (UiAssetResolve::usable(n))
			{
				notices[i] = Texture(n);
				has_notice[i] = true;
			}
		}

		// Rectangle(left, right, top, bottom)；列表区实测 x9..226 y48..366（13 行 x23）。
		list_area = Rectangle<int16_t>(9, 226, 48, BAND_TOP - 1);
		// 正文区：蓝横幅（烤在 backgrnd2 里，实测到 y117）之下、进度条之上。
		detail_area = Rectangle<int16_t>(bg_right_pos.x() + 18, bg_right_pos.x() + right_width - 24, 124, 340);
		detail_title_pos = Point<int16_t>(bg_right_pos.x() + 34, 40);
		detail_desc_pos = detail_area.get_left_top();
		notice_pos = Point<int16_t>(list_area.left(), list_area.top() + 48);

		slider = Slider(Slider::Type::DEFAULT_SILVER,
			list_area.get_vertical(),
			list_area.right(),
			visible_rows(),
			visible_rows(),
			std::bind(&UIQuestLog::on_list_scroll, this, std::placeholders::_1));

		detail_slider = Slider(Slider::Type::DEFAULT_SILVER,
			detail_area.get_vertical(),
			detail_area.right() + 6,
			detail_visible_rows(),
			detail_visible_rows(),
			std::bind(&UIQuestLog::on_detail_scroll, this, std::placeholders::_1));

		layout();
		activate_buttons();
		rebuild_list();

		// 多分辨率：PosQUEST 是 1280×720 基准下的绝对读数；其它档位按新视口居中。
		// 1280×720 保持原生绝对位置，不覆盖用户拖拽后保存的 Setting<PosQUEST>。
		if (Constants::Constants::get().get_viewwidth() != ResolutionTiers::LOGIN_W
			|| Constants::Constants::get().get_viewheight() != ResolutionTiers::LOGIN_H)
			center_on_screen();
	}

	void UIQuestLog::update_screen(int16_t, int16_t)
	{
		// 分辨率实时切换：重锚到视口中心，窗内控件相对偏移与 1x 位图尺寸保持不变。
		center_on_screen();
	}

	void UIQuestLog::draw(float inter) const
	{
		bg_left.draw(position);

		if (!collapsed)
			bg_right.draw(position + bg_right_pos);

		for (uint16_t i = 0; i < 3; i++)
		{
			tab_border[i].draw(position + Point<int16_t>(tab_x[i], tab_y));
			(i == tab ? tab_fill_active[i] : tab_fill_idle[i])
				.draw(position + Point<int16_t>(tab_x[i] + TAB_FILL_INSET, tab_y + TAB_FILL_INSET));
		}

		const Texture* icon = current_icon();
		int16_t n = static_cast<int16_t>(rows.size());
		int16_t visible = visible_rows();

		for (int16_t i = 0; i < visible && (scroll + i) < n; i++)
		{
			int16_t di = scroll + i;
			int16_t row_top = list_area.top() + i * row_height;
			const Row& r = rows[di];

			if (r.header)
			{
				header_highlight.draw(position + Point<int16_t>(7, row_top));

				if (icon_header)
					icon_header->draw(position + Point<int16_t>(9, row_top + 2));

				Text header(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, r.label);
				header.draw(position + Point<int16_t>(29, row_top + 4));
			}
			else
			{
				if (r.id == selected_id)
					row_highlight.draw(position + Point<int16_t>(25, row_top));

				if (icon)
					icon->draw(position + Point<int16_t>(12, row_top + 3));

				Text name(Text::Font::A11M, Text::Alignment::LEFT,
					r.id == selected_id ? Color::Name::WHITE : Color::Name::BLACK, r.label);
				name.draw(position + Point<int16_t>(29, row_top + 3));
			}
		}

		if (n == 0 && has_notice[tab])
		{
			// 空列表提示图（"没有可以开始的任务"…）在列表区水平居中。
			int16_t x = list_area.left() + (list_area.width() - notices[tab].width()) / 2;
			notices[tab].draw(position + Point<int16_t>(x, notice_pos.y()));
		}

		if (!collapsed && npc_portrait.width() > 0)
		{
			// 蓝横幅右半格放任务 NPC 立绘：backgrnd2 实测横幅 y26..118、内部分隔线在本地 x190，
			// 右格即 bg2 x190..297（窗口 x433..540）。stand 帧原点是脚底中心，按格底落位。
			const int16_t slot_left = bg_right_pos.x() + 190;
			const int16_t slot_right = bg_right_pos.x() + right_width - 8;

			npc_portrait.draw(position + Point<int16_t>((slot_left + slot_right) / 2, 112));
		}

		if (!collapsed && selected_id >= 0)
		{
			// 标题前的小圆点已烤在 backgrnd2 的蓝条里，不再自绘项目符。
			detail_title.draw(position + detail_title_pos);

			// 每段 detail_lines[i] 内部已按正文区宽度自动折行，行高不再相等，
			// 所以只能逐段累加高度定位；原来固定 i * DETAIL_ROW_H 会让长文本整段互相压字。
			const Range<int16_t> clip(position.y() + detail_area.top(), position.y() + detail_area.bottom());
			int16_t yoff = static_cast<int16_t>(-detail_scroll * DETAIL_ROW_H);

			for (const Text& line : detail_lines)
			{
				// clip 是屏幕坐标：超出正文区上下缘的字形会被裁掉，不再铺到窗口外。
				line.draw(position + detail_desc_pos + Point<int16_t>(0, yoff), clip);
				yoff = static_cast<int16_t>(yoff + line.height());
			}

			draw_gauge();
		}

		if (bg_left.width() > 0 && bg_left.height() > 0)
		{
			slider.draw(position);

			if (!collapsed)
				detail_slider.draw(position);
		}

		UIElement::draw_buttons(inter);
	}

	bool UIQuestLog::has_chain_pages() const
	{
		if (selected_id < 0)
			return false;

		return QuestData::get(selected_id).is_chain_quest();
	}

	// 连锁页码条（C8）：parent 链 total>1 时，按 order 填 Gauge，并在分界处戳 spot。
	// 证据：IDA sub_83EA05（total>1 才画刻度）+ sub_83FEC6 格式串 "%s ( %d / %d )" + pool 4497 "Chain Quest"。
	void UIQuestLog::draw_chain_page_gauge() const
	{
		if (gauge_frame.width() <= 0 || selected_id < 0)
			return;

		const QuestData& qd = QuestData::get(selected_id);
		const int16_t order = qd.get_chain_order();
		const int16_t total = qd.get_chain_total();

		if (total <= 1 || order <= 0)
			return;

		const int16_t frame_w = gauge_frame.width();
		const int16_t frame_h = gauge_frame.height();
		const Point<int16_t> frame_tl = position + gauge_pos + gauge_frame_origin;
		gauge_frame.draw(frame_tl);

		const int16_t bar_x = position.x() + gauge_pos.x() + 2;
		const int16_t bar_y = position.y() + gauge_pos.y() + (frame_h - gauge_bar.height()) / 2;
		const int16_t inner_max = frame_w > 4 ? static_cast<int16_t>(frame_w - 4) : frame_w;
		// order/total：第 1/2 步填一半，第 2/2 填满（与原生分段 spot 同序）。
		int16_t inner = static_cast<int16_t>(inner_max * order / total);

		if (inner < 0)
			inner = 0;
		if (inner > inner_max)
			inner = inner_max;

		if (gauge_bar.width() > 0 && inner > 0)
			gauge_bar.draw(DrawArgument(Point<int16_t>(bar_x, bar_y), Point<int16_t>(inner, gauge_bar.height())));

		// 页界 spot：k=1..total-1 处分段戳点（对应原生 for 循环写边界）。
		if (gauge_spot.width() > 0 && total > 1)
		{
			const int16_t spot_dy = (gauge_bar.height() - gauge_spot.height()) / 2;

			for (int16_t k = 1; k < total; k++)
			{
				int16_t x = static_cast<int16_t>(inner_max * k / total);
				gauge_spot.draw(DrawArgument(Point<int16_t>(bar_x + x, bar_y + spot_dy)));
			}
		}

		// 宋体 A11M 黑字；格式对齐原生 "%s ( %d / %d )"。
		Text page_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
			Locale::format_all("quest.chain_page",
				{ std::to_string(order), std::to_string(total) }));
		page_label.draw(position + Point<int16_t>(gauge_pos.x() + frame_w + 6, gauge_pos.y() + 1));
	}

	// 进度条：Gauge/frame(163x14，原点 (81,0)) 作底，1x11 的 gauge 横向拉伸成进度，spot 收尾。
	// 连锁任务优先画页码条；否则走需求进度（进行中 tab）。
	void UIQuestLog::draw_gauge() const
	{
		if (gauge_frame.width() <= 0)
			return;

		if (has_chain_pages())
		{
			draw_chain_page_gauge();
			return;
		}

		int16_t ratio = gauge_ratio();

		if (ratio < 0)
			return;

		const int16_t frame_w = gauge_frame.width();
		const int16_t frame_h = gauge_frame.height();
		// gauge_pos 是底板左上角；贴图按 anchor 绘制，故加回自身 origin。
		const Point<int16_t> frame_tl = position + gauge_pos + gauge_frame_origin;

		gauge_frame.draw(frame_tl);

		const int16_t bar_x = position.x() + gauge_pos.x() + 2;
		const int16_t bar_y = position.y() + gauge_pos.y() + (frame_h - gauge_bar.height()) / 2;
		int16_t inner = static_cast<int16_t>((frame_w - 4) * ratio / 100);

		if (gauge_bar.width() > 0 && inner > 0)
		{
			// DrawArgument 的 stretch 是绝对宽高（0 才回落到原尺寸）。
			gauge_bar.draw(DrawArgument(Point<int16_t>(bar_x, bar_y), Point<int16_t>(inner, gauge_bar.height())));
		}

		if (gauge_spot.width() > 0 && inner < frame_w - 4)
		{
			gauge_spot.draw(DrawArgument(Point<int16_t>(bar_x + inner, bar_y)
				+ Point<int16_t>(0, (gauge_bar.height() - gauge_spot.height()) / 2)));
		}

		Text progress(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
			questlog.get_progress(selected_id));
		progress.draw(position + Point<int16_t>(gauge_pos.x() + frame_w + 6, gauge_pos.y() + 1));
	}

	void UIQuestLog::update()
	{
	}

	void UIQuestLog::send_key(int32_t keycode, bool pressed, bool escape)
	{
	}

	UIElement::Type UIQuestLog::get_type() const
	{
		return TYPE;
	}

	void UIQuestLog::refresh()
	{
		rebuild_list();
	}

	const Texture* UIQuestLog::current_icon() const
	{
		switch (tab)
		{
			case 0: return icon_avail.get();
			case 1: return icon_progress.get();
			case 2: return icon_complete.get();
			default: return nullptr;
		}
	}

	std::string UIQuestLog::area_name(int32_t area) const
	{
		// 键名用区域号而不是英文地名：这套区域号来自任务数据，本包没有原生英文地名依据，
		// 编出 "Ellin Forest" 这种名字会把没核对过的语义写进键名。
		// default 仍指 30 —— 改动前「除 20 外一律显示金银岛」是既有行为，本批只搬字面量不改语义。
		switch (area)
		{
			case 20: return Locale::text("quest.area.20");
			case 30: return Locale::text("quest.area.30");
			default: return Locale::text("quest.area.30");
		}
	}

	// 三个分类页统一按区域分组：分组标题行可点，点一次收起该区域、再点展开。
	// 原来只有"可开始"分组，且 select_row 直接吞掉标题行点击 → 用户看到的"不能展开收缩"。
	void UIQuestLog::append_grouped(const std::vector<int16_t>& ids)
	{
		std::vector<int32_t> order;
		std::map<int32_t, std::vector<int16_t>> grouped;

		for (int16_t id : ids)
		{
			const int32_t a = QuestData::get(id).get_area();

			if (grouped.find(a) == grouped.end())
				order.push_back(a);

			grouped[a].push_back(id);
		}

		for (int32_t a : order)
		{
			const std::vector<int16_t>& group = grouped[a];

			Row hdr;
			hdr.header = true;
			hdr.label = area_name(a) + " (" + std::to_string(group.size()) + ")";
			hdr.id = -1;
			hdr.area = a;
			rows.push_back(hdr);

			// 收起的区域只保留标题行（标题上已带数量，收起后仍能看见有几条）。
			if (collapsed_areas.count(a))
				continue;

			for (int16_t id : group)
			{
				Row r;
				r.header = false;
				r.label = QuestData::get(id).get_name();
				r.id = id;
				r.area = a;
				rows.push_back(r);
			}
		}
	}

	void UIQuestLog::toggle_area(int32_t area)
	{
		if (!collapsed_areas.erase(area))
			collapsed_areas.insert(area);

		// 收起可能把当前选中行整组抹掉，重算选中并同步滚动位置。
		rebuild_list();
	}

	void UIQuestLog::rebuild_list()
	{
		rows.clear();
		detail_lines.clear();

		switch (tab)
		{
			case 0:
			{
				int16_t level = Stage::get().get_player().get_level();
				uint16_t job = Stage::get().get_player().get_stats().get_job().get_id();

				append_grouped(QuestData::list_available(level, job, questlog));
				break;
			}
			case 1:
			{
				std::vector<int16_t> ids;
				for (const auto& kv : questlog.get_started())
					ids.push_back(kv.first);

				append_grouped(ids);
				break;
			}
			case 2:
			{
				std::vector<int16_t> ids;
				for (const auto& kv : questlog.get_completed())
					ids.push_back(kv.first);

				append_grouped(ids);
				break;
			}
		}

		scroll = 0;
		detail_scroll = 0;

		// 重算选中：旧选中行如果还在列表里就保持，否则退回第一条可见任务。
		bool keep = false;

		for (const Row& r : rows)
		{
			if (!r.header && r.id == selected_id)
			{
				keep = true;
				break;
			}
		}

		if (!keep)
		{
			selected_id = -1;

			for (const Row& r : rows)
			{
				if (!r.header)
				{
					selected_id = r.id;
					break;
				}
			}
		}

		update_detail();
		update_sliders();
	}

	void UIQuestLog::select_row(size_t index)
	{
		if (index >= rows.size())
			return;

		if (rows[index].header)
		{
			toggle_area(rows[index].area);
			return;
		}

		selected_id = rows[index].id;

		update_detail();
		detail_scroll = 0;
		update_sliders();
	}

	void UIQuestLog::update_detail()
	{
		detail_lines.clear();
		load_npc_portrait(selected_id);

		if (selected_id < 0)
		{
			detail_title.change_text("");
			return;
		}

		const QuestData& qd = QuestData::get(selected_id);
		detail_title.change_text(qd.get_name());

		// 折行宽度 = 正文区宽度：原来不给 maxwidth，长句一路横铺，
		// 越过右板边框画到窗口外面（用户看到的"任务信息超出了界面显示"）。
		const uint16_t wrap = static_cast<uint16_t>(detail_area.width() > 0 ? detail_area.width() : 1);

		if (detail_mode)
		{
			// 详细态：只读 QuestData 已缓存的字段，不涉及任何封包。
			std::vector<std::string> lines;
			lines.push_back(Locale::format("quest.detail.id", std::to_string(qd.get_id())));

			if (qd.get_max_level() > 0)
				lines.push_back(Locale::format_all("quest.detail.level_range",
					{ std::to_string(qd.get_min_level()), std::to_string(qd.get_max_level()) }));
			else if (qd.get_min_level() > 0)
				lines.push_back(Locale::format("quest.detail.min_level", std::to_string(qd.get_min_level())));

			lines.push_back(Locale::format("quest.detail.area", area_name(qd.get_area())));

			if (qd.get_npc() > 0)
				lines.push_back(Locale::format("quest.detail.npc", std::to_string(qd.get_npc())));

			if (qd.get_prequests().empty())
				lines.push_back(Locale::text("quest.detail.pre_none"));
			else
			{
				std::string pre;
				for (int16_t p : qd.get_prequests())
					pre += std::to_string(p) + " ";
				// 值 `前置任务: {}` 的尾空格会被加载器 trim 掉，故把 id 串当实参填进占位符，
				// 而不是在代码里拼 `Locale::text(...) + pre`。
				lines.push_back(Locale::format("quest.detail.pre", pre));
			}

			for (const std::string& line : lines)
				detail_lines.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line, wrap);

			return;
		}

		// 按当前分类页取对应状态摘要（0/1/2），避免把三页拼成一坨。
		const std::string& sum = qd.get_summary_for_tab(tab);
		size_t start = 0;

		while (start <= sum.size())
		{
			size_t nl = sum.find('\n', start);

			std::string line = (nl == std::string::npos)
				? sum.substr(start)
				: sum.substr(start, nl - start);

			detail_lines.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line, wrap);

			if (nl == std::string::npos)
				break;

			start = nl + 1;
		}
	}

	void UIQuestLog::change_tab(uint16_t tabid)
	{
		if (tabid > 2)
			return;

		tab = tabid;

		for (uint16_t i = 0; i < 3; i++)
			buttons[Buttons::TAB0 + i]->set_state(i == tabid ? Button::State::PRESSED : Button::State::NORMAL);

		activate_buttons();
		rebuild_list();
		// 分类页切换后摘要页（QuestInfo 0/1/2）要跟着换。
		update_detail();
		update_sliders();
	}

	void UIQuestLog::on_list_scroll(bool upwards)
	{
		int16_t n = static_cast<int16_t>(rows.size());
		int16_t maxscroll = std::max<int16_t>(0, n - visible_rows());

		scroll = std::max<int16_t>(0, std::min<int16_t>(maxscroll, scroll + (upwards ? -1 : 1)));
	}

	void UIQuestLog::on_detail_scroll(bool upwards)
	{
		const int16_t total = detail_total_notches();
		const int16_t maxscroll = std::max<int16_t>(0, total - detail_visible_rows());

		detail_scroll = std::max<int16_t>(0, std::min<int16_t>(maxscroll, detail_scroll + (upwards ? -1 : 1)));
	}

	int16_t UIQuestLog::visible_rows() const
	{
		return row_height > 0 ? list_area.height() / row_height : 0;
	}

	int16_t UIQuestLog::gauge_ratio() const
	{
		if (tab != 1 || selected_id < 0)
			return -1;

		const std::string progress = questlog.get_progress(selected_id);
		size_t i = 0;

		while (i < progress.size() && !std::isdigit(static_cast<unsigned char>(progress[i])))
			i++;

		size_t head = i;

		while (i < progress.size() && std::isdigit(static_cast<unsigned char>(progress[i])))
			i++;

		if (i == head || i >= progress.size() || progress[i] != '/')
			return -1;

		i++;

		size_t tail = i;

		while (i < progress.size() && std::isdigit(static_cast<unsigned char>(progress[i])))
			i++;

		if (i == tail)
			return -1;

		int32_t current = atoi(progress.substr(head, i - tail).c_str());
		int32_t total = atoi(progress.substr(tail).c_str());

		if (total <= 0)
			return -1;

		return static_cast<int16_t>(std::max(0, std::min(100, current * 100 / total)));
	}

	int16_t UIQuestLog::detail_visible_rows() const
	{
		return DETAIL_ROW_H > 0 ? detail_area.height() / DETAIL_ROW_H : 0;
	}

	int16_t UIQuestLog::detail_content_height() const
	{
		int16_t h = 0;

		for (const Text& line : detail_lines)
			h = static_cast<int16_t>(h + line.height());

		return h;
	}

	int16_t UIQuestLog::detail_total_notches() const
	{
		if (DETAIL_ROW_H <= 0)
			return 0;

		return static_cast<int16_t>((detail_content_height() + DETAIL_ROW_H - 1) / DETAIL_ROW_H);
	}

	// 任务 NPC 立绘：QuestData 只存 npc id，图按 UINpcTalk 同一取法从 Npc.nx 拿 stand 首帧。
	void UIQuestLog::load_npc_portrait(int16_t quest_id)
	{
		npc_portrait = Texture();

		if (quest_id < 0)
			return;

		const int32_t npcid = QuestData::get(quest_id).get_npc();

		if (npcid <= 0)
			return;

		nl::node src = nl::nx::Npc[string_format::extend_id(npcid, 7) + ".img"]["stand"]["0"];

		if (UiAssetResolve::usable(src))
			npc_portrait = Texture(src);
	}

	// 命中区必须等于绘制区。展开态原来写的是 left_width + right_width - bg_right_pos.x() = 307，
	// 那是把右板宽度往左投影的笔误；真实绘制宽度是 bg_right_pos.x() + right_width = 548。
	// 于是 x307 以右的 241px —— 正好盖着底带的「详细」「寻找NPC位置」和右板滚动条 ——
	// 被 UIElement::is_in_range 判在窗口外，点击根本传不进来，表现为"按了没反应"。
	void UIQuestLog::update_dimension()
	{
		dimension = Point<int16_t>(collapsed ? left_width : bg_right_pos.x() + right_width,
			bg_left.height());
	}

	// 收缩态面板 245 宽、展开态右板内缘 548：所有角锚都按当前宽度反算，不写死绝对值。
	void UIQuestLog::relayout_slots()
	{
		const int16_t panel_x = collapsed ? 0 : bg_right_pos.x();
		const int16_t panel_right = panel_x + (collapsed ? left_width : right_width);

		slot_tl[Buttons::CLOSE] = Point<int16_t>(
			left_width - slot_dim[Buttons::CLOSE].x() - TITLE_INSET, TITLE_Y);

		// 展开态收缩钮挂在右板右上角（实测 528,5）；收缩态挪到关闭钮左侧。
		if (collapsed)
			slot_tl[Buttons::COLLAPSE] = Point<int16_t>(
				slot_tl[Buttons::CLOSE].x() - BTN_GAP - slot_dim[Buttons::COLLAPSE].x(), TITLE_Y);
		else
			slot_tl[Buttons::COLLAPSE] = Point<int16_t>(
				panel_right - slot_dim[Buttons::COLLAPSE].x() - TITLE_INSET, TITLE_Y);

		// 底带按钮行：寻找NPC位置贴实测右锚，其余依次左移（内容区下方的同一行）。
		int16_t x = panel_right - BAND_INSET;

		for (uint16_t id : { Buttons::MARKNPC, Buttons::DETAIL, Buttons::GIVEUP, Buttons::OK, Buttons::NO, Buttons::ALERT })
		{
			const int16_t w = slot_dim[id].x();

			if (w <= 0)
				continue;

			x -= w;
			slot_tl[id] = Point<int16_t>(x, BAND_Y);
			x -= BTN_GAP;
		}

		// 进度条：右板水平居中，贴在底带之上。
		gauge_pos = Point<int16_t>(panel_x + (right_width - gauge_frame.width()) / 2,
			BAND_TOP - gauge_frame.height() - 8);
	}

	void UIQuestLog::layout()
	{
		relayout_slots();

		auto place = [&](uint16_t id)
		{
			auto iter = buttons.find(id);

			if (iter == buttons.end() || !iter->second)
				return;

			// Button 存的是 anchor，故左上角要加回 origin。
			iter->second->set_position(slot_tl[id] + iter->second->origin());
		};

		place(CLOSE);
		place(COLLAPSE);
		place(MARKNPC);
		place(DETAIL);
		place(GIVEUP);
		place(OK);
		place(NO);
		place(ALERT);

		slider.setvertical(list_area.get_vertical());
		detail_slider.setvertical(detail_area.get_vertical());
	}

	void UIQuestLog::activate_buttons()
	{
		auto act = [&](uint16_t id, bool on)
		{
			auto iter = buttons.find(id);

			if (iter != buttons.end() && iter->second)
				iter->second->set_active(on);
		};

		act(CLOSE, true);
		act(COLLAPSE, true);
		act(MARKNPC, true);
		act(DETAIL, true);
		// 放弃只对"正在进行"有意义（发已有的 QuestActionPacket(3)）。
		act(GIVEUP, tab == 1);
		// 接受/拒绝/任务讯息：参考截图底带里没有这三颗，且无对应窗口可开 → 预留不显示。
		act(OK, false);
		act(NO, false);
		act(ALERT, false);
	}

	void UIQuestLog::set_collapsed(bool is_collapsed)
	{
		collapsed = is_collapsed;
		update_dimension();
		layout();
		activate_buttons();
		update_sliders();
	}

	void UIQuestLog::update_sliders()
	{
		slider.setenabled(true);
		slider.setrows(visible_rows(), static_cast<int16_t>(rows.size()));
		detail_slider.setenabled(!collapsed);
		detail_slider.setrows(detail_visible_rows(), detail_total_notches());
	}

	Button::State UIQuestLog::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::TAB0:
			case Buttons::TAB1:
			case Buttons::TAB2:
				change_tab(buttonid - Buttons::TAB0);
				return Button::State::NORMAL;
			case Buttons::CLOSE:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::COLLAPSE:
				set_collapsed(!collapsed);
				return Button::State::NORMAL;
			case Buttons::DETAIL:
				detail_mode = !detail_mode;
				update_detail();
				update_sliders();
				return Button::State::NORMAL;
			case Buttons::GIVEUP:
				// 复用 UIMiscShells 里既有的放弃协议（action 3 = forfeit），不发明新包。
				if (selected_id > 0)
					QuestActionPacket(3, selected_id).dispatch();
				return Button::State::NORMAL;
			case Buttons::MARKNPC:
			case Buttons::OK:
			case Buttons::NO:
			case Buttons::ALERT:
				// 显式预留（不是漏实现）：
				//  MARKNPC「寻找NPC位置」原生是"把该任务 NPC 所在地图在世界地图上高亮点出"，
				//    需要 npc→map 反查表（服务端数据）+ UIWorldMap 的标记入口，本包两者都没有，
				//    且没有任何可对照的封包证据 —— 不发明协议，见 notes/open-issues.md。
				//  OK/NO/ALERT 只在"接受/完成确认"弹层里出现，本窗口底带不显示（activate_buttons 已置 false）。
				return Button::State::NORMAL;
			default:
				return Button::State::NORMAL;
		}
	}

	int16_t UIQuestLog::row_index_at(Point<int16_t> local) const
	{
		if (!list_area.contains(local))
			return -1;

		int16_t rel = local.y() - list_area.top();

		if (rel < 0)
			return -1;

		int16_t di = scroll + rel / row_height;

		if (di < 0 || di >= static_cast<int16_t>(rows.size()))
			return -1;

		return di;
	}

	Cursor::State UIQuestLog::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State dstate = UIDragElement::send_cursor(clicked, cursorpos);

		if (dragged)
			return dstate;

		Point<int16_t> cursor_relative = cursorpos - position;

		if (!collapsed && detail_slider.isenabled())
		{
			if (Cursor::State sstate = detail_slider.send_cursor(cursor_relative, clicked))
				return sstate;
		}

		if (slider.isenabled())
		{
			if (Cursor::State sstate = slider.send_cursor(cursor_relative, clicked))
				return sstate;
		}

		// 单击即选中并刷新详情（原生行为）；双击语义保留在 doubleclick()。
		if (clicked)
		{
			int16_t index = row_index_at(cursor_relative);

			if (index >= 0)
				select_row(static_cast<size_t>(index));
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIQuestLog::remove_cursor()
	{
		UIDragElement::remove_cursor();
		slider.remove_cursor();
		detail_slider.remove_cursor();
	}

	void UIQuestLog::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t index = row_index_at(cursorpos - position);

		if (index >= 0)
			select_row(static_cast<size_t>(index));
	}
}
