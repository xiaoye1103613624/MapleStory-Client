////////////////////////////////////////////////////////////////////////////////
//	UIQuestLog
////////////////////////////////////////////////////////////////////////////////
#include "UIQuestLog.h"

#include "../UI.h"

#include "../../Gameplay/Stage.h"
#include "../../Data/QuestData.h"
#include "../UITypes/UIWorldMap.h"
#include "../UITypes/UINotice.h"
#include "../../Net/Packets/QuestPackets.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"
#include "../../Util/QuestTextFormatter.h"
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
	// 底带：r07 IMG 一手校正（orange-wz 10012 + 128B key + autoParse:true）。
	// Quest/backgrnd = 245x396 origin(0,0)；Quest/BtDetail/BtGiveup/BtOK/BtNo 帧0 = 57x17、
	// BtMarkNpc = 81x17、BtAlert = 56x16 —— 底带按钮墨迹高 16~17px。
	// backgrnd 底缘实测两条深蓝线在 y367 / y391（带内 23px）：带中线 = 367 + 23/2 = 378。
	// 按钮垂直位置按「带中线 - 自身高/2」逐颗反解，取代原来写死的 BAND_Y=370
	// （原来对所有按钮一律 370，16px 的 BtAlert 就会比 17px 的高出半像素）。
	static constexpr int16_t BAND_TOP = 367;
	static constexpr int16_t BAND_H = 23;
	static constexpr int16_t BAND_MID = BAND_TOP + BAND_H / 2; // = 378
	// 角锚内缩（conventions §3.2）：标题条 k=7（关闭钮实测左上 226、展开钮 528），底带 k=9（寻找NPC位置右缘 538）。
	static constexpr int16_t TITLE_INSET = 7;
	static constexpr int16_t BAND_INSET = 9;
	static constexpr int16_t BTN_GAP = 6;
	static constexpr int16_t TITLE_Y = 5;
	// 分类条：r07 IMG 一手校正。Quest/Tab/enabled = 50x15 / 50x15 / 27x15，
	// disabled = 50x14 / 49x14 / 27x14，全部 origin(0,0) —— 字图必须按自身尺寸水平居中，
	// 不能拿 64/65/65 的自绘格宽当字图宽（原实现已用 glyph.width() 居中，本批把墨迹顶
	// 换算改走 Text::y_for_ink_top，去掉手抄的 +4/+3）。
	static constexpr int16_t TAB_FILL_INSET = 2;
	static constexpr int16_t TAB_FILL_H = 16;
	static constexpr int16_t DETAIL_ROW_H = 19;
	// 行文字墨迹顶相对行框的居中目标：IMG 行底色框 19px 高；墨迹顶 = 行框中线 - 半个墨迹高。
	// A11 单行墨迹高约 11px（Graphics/Text 现取字形度量），故目标墨迹顶 = row_top + (19 - 11)/2 = +4。
	// 绘制点统一由 Text::y_for_ink_top 反解（Text.h:164），不再写死 row_top+4/+3。
	static constexpr int16_t ROW_INK_CENTER = 4;

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

		// 行底色无位图：按 任务栏.png / w10 coords 自绘。
		// 分组头 (9,) 宽 224→233；选中行 (27,) 宽 204→231（右缘仍 < list_area.right=226 的
		// 旧注释已过时——实测选中带到 x218..234，以截图为准）。
		row_highlight = ColorBox(204, 19, Color(0x336699FF), 1.0f);
		header_highlight = ColorBox(224, 19, Color(0x99BBCCFF), 1.0f);

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

			// r07 IMG 一手：Tab 字图 enabled=50/50/27x15、disabled=50/49/27x14，
			// origin(0,0)。水平按自身宽 w 居中在自绘格 tab_w[i] 内（不是拿格宽当字图宽），
			// 垂直落在内衬 tab_y+TAB_FILL_INSET；原点加回去才是左上角。
			Point<int16_t> tl(tab_x[i] + (tab_w[i] - w) / 2, tab_y + TAB_FILL_INSET + (TAB_FILL_H - sh) / 2);
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

		// Center 策略：1280×720 保留 PosQUEST；其它档位视口居中。
		set_layout_policy(LayoutPolicy::Center);
		if (Constants::Constants::get().get_viewwidth() != ResolutionTiers::LOGIN_W
			|| Constants::Constants::get().get_viewheight() != ResolutionTiers::LOGIN_H)
			center_on_screen();
	}

	void UIQuestLog::update_screen(int16_t new_width, int16_t new_height)
	{
		UIElement::update_screen(new_width, new_height);
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
				header_highlight.draw(position + Point<int16_t>(9, row_top));

				if (icon_header)
					icon_header->draw(position + Point<int16_t>(11, row_top + 2));

				Text header(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::WHITE, r.label);
				// 墨迹顶落在行框中线（row_top + ROW_INK_CENTER），绘制点交给唯一真源反解；
				// 原来写死 row_top+4 是「把绘制点当墨迹顶」，SimHei→思源宋体后字形 bitmap_top
				// 变了就整列偏移。见 Text.h:146-164 / r07 §4。
				header.draw(position + Point<int16_t>(29, header.y_for_ink_top(row_top + ROW_INK_CENTER)));
			}
			else
			{
				if (r.id == selected_id)
					row_highlight.draw(position + Point<int16_t>(27, row_top));

				if (icon)
					// 图标 13~15px 高（IMG 一手）：在 19px 行框里按中线居中。
					icon->draw(position + Point<int16_t>(12, row_top + (row_height - icon->height()) / 2));

				Text name(Text::Font::A11M, Text::Alignment::LEFT,
					r.id == selected_id ? Color::Name::WHITE : Color::Name::BLACK, r.label);
				// 同上：普通任务行文字用 y_for_ink_top 反解，取代手抄的 row_top+3。
				name.draw(position + Point<int16_t>(29, name.y_for_ink_top(row_top + ROW_INK_CENTER)));
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
		// 原 CUIQuestInfo 有独立 OnKey；旧实现空体使任务窗自身 ESC/快捷键无效
		// （关闭此前靠 UIStateGame 的全局 ESC 兜底）。这里给本窗显式关闭语义。
		if (pressed && escape)
		{
			deactivate();
			return;
		}
	}

	void UIQuestLog::send_scroll(double yoffset)
	{
		// 详情板在 dimension 之外，不能用基类 is_in_range；按光标落在左列表
		// 还是右详情区选对应 Slider（与 send_cursor():972/999 同源）。
		const Point<int16_t> local = UI::get().get_cursor_position() - position;

		if (!collapsed && detail_area.contains(local))
		{
			if (detail_slider.isenabled())
				detail_slider.send_scroll(yoffset);
		}
		else if (list_area.contains(local))
		{
			if (slider.isenabled())
				slider.send_scroll(yoffset);
		}
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

	// r07：可读 NPC 名。原生 QuestInfo/MedalQuestInfo 的 sName/sLevelLimit 都是可读串，
	// 当前端 Check.img 只给 npc id，故走 String/Npc.img/<id>/name（本包已证存在，
	// 参照 UINpcTalk.cpp:938 的既有取法）。取不到返回空串，绝不显示裸 id。
	std::string UIQuestLog::npc_name(int32_t npcid) const
	{
		if (npcid <= 0)
			return {};

#ifdef USE_NX
		const std::string name = nl::nx::String["Npc.img"][std::to_string(npcid)]["name"].get_string();
		return name;
#else
		return {};
#endif
	}

	// r07：可读前置任务名。优先取 QuestData 已缓存的 QuestInfo.img/<id>/name；
	// QuestData 在无 String/Quest.img 时 name 会回退为 "Quest <id>"，此时用可读占位，
	// 避免把一个 fallback 串当正式名字反复展示。
	std::string UIQuestLog::quest_name_readable(int16_t questid) const
	{
		if (questid <= 0)
			return {};

		const QuestData& qd = QuestData::get(questid);
		// r07：只有数据侧真给了名字才用；"Quest <id>" 兜底一律视为不可读。
		return qd.has_real_name() ? qd.get_name() : std::string();
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
				// r07：无真实任务名时用可读占位，不把 "Quest <id>" 裸 id 当名字展示。
				const QuestData& qd = QuestData::get(id);
				r.label = qd.has_real_name() ? qd.get_name() : Locale::text("quest.name_unknown");
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
		// r07：标题同样不展示 "Quest <id>" 裸 id 兜底。
		detail_title.change_text(qd.has_real_name() ? qd.get_name() : Locale::text("quest.name_unknown"));

		// 折行宽度 = 正文区宽度：原来不给 maxwidth，长句一路横铺，
		// 越过右板边框画到窗口外面（用户看到的"任务信息超出了界面显示"）。
		const uint16_t wrap = static_cast<uint16_t>(detail_area.width() > 0 ? detail_area.width() : 1);

		if (detail_mode)
		{
			// r07 字段级还原：详情态字段顺序对齐 Angel QuestInfo 的语义
			// （sName → sLevelLimit(可读串) → 区域 → NPC 名 → 前置任务名），
			// 全部来自 QuestData/String 已有字段，不发任何封包。
			// D4 硬约束：去掉自造的 quest.detail.id / quest.detail.npc / quest.detail.pre
			// 三个「std::to_string(裸id)」字段，改为可读名 / 可读占位。
			std::vector<std::string> lines;

			// 1) 任务名（原生 sName）：标题栏已显示，这里不再重复裸 id；
			//    若 QuestData 名字是 fallback "Quest <id>"，用可读提示代替。
			const std::string qname = quest_name_readable(qd.get_id());
			lines.push_back(Locale::format("quest.detail.name",
				qname.empty() ? Locale::text("quest.detail.name_unknown") : qname));

			// 2) 等级限制（原生 sLevelLimit 是可读串；本包 Check.img 只给 min/max 整数，
			//    用区间文案承载，仍是「等级范围」语义而非自造字段）。
			if (qd.get_max_level() > 0)
				lines.push_back(Locale::format_all("quest.detail.level_range",
					{ std::to_string(qd.get_min_level()), std::to_string(qd.get_max_level()) }));
			else if (qd.get_min_level() > 0)
				lines.push_back(Locale::format("quest.detail.min_level", std::to_string(qd.get_min_level())));
			else
				lines.push_back(Locale::text("quest.detail.level_none"));

			// 3) 所属区域（area_name 已有可读映射）。
			lines.push_back(Locale::format("quest.detail.area", area_name(qd.get_area())));

			// 4) 相关 NPC：改为 String/Npc.img 可读名；取不到时按「未标注」占位，
			//    不显示裸数字 id。
			const std::string npcn = npc_name(qd.get_npc());
			if (!npcn.empty())
				lines.push_back(Locale::format("quest.detail.npc_name", npcn));
			else
				lines.push_back(Locale::text("quest.detail.npc_unknown"));

			// 5) 前置任务：逐个用 QuestData 名字解析；无前置或名字不可读都给可读占位。
			std::vector<std::string> pre_names;
			for (int16_t p : qd.get_prequests())
			{
				const std::string pn = quest_name_readable(p);
				if (!pn.empty())
					pre_names.push_back(pn);
			}

			if (pre_names.empty())
			{
				lines.push_back(Locale::text(
					qd.get_prequests().empty() ? "quest.detail.pre_none" : "quest.detail.pre_unknown"));
			}
			else
			{
				std::string pre;
				for (size_t i = 0; i < pre_names.size(); i++)
				{
					if (i)
						pre += "、";
					pre += pre_names[i];
				}
				lines.push_back(Locale::format("quest.detail.pre", pre));
			}

			LOG(LOG_DEBUG, "UIQuestLog: detail fields quest=" << qd.get_id()
				<< " name=" << (qname.empty() ? "?" : qname)
				<< " npc_name=" << (npcn.empty() ? "?" : npcn)
				<< " pre_count=" << pre_names.size());

			for (const std::string& line : lines)
				detail_lines.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line, wrap);

			return;
		}

		// 按当前分类页取对应状态摘要（0/1/2），避免把三页拼成一坨。
		const std::string& sum = qd.get_summary_for_tab(tab);

		// 摘要空时给可读占位，而不是留一整片空白（本包无 String/Quest.img，
		// QuestInfo 分状态页可能缺失；原生 QuestInfo::asSummary 为空时也有提示）。
		if (sum.empty())
		{
			detail_lines.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::BLACK, Locale::text("quest.detail.summary_none"), wrap);
			return;
		}

		// r08：QuestInfo 摘要里带 #p/#t/#i/#c 等令牌（活数据实证：10000/0 的
		// "从#b#p9310088##k那里收到了紧急信息"、10002/1 的 "#b#t4031988##k"）。
		// GraphicsGL::LayoutBuilder 只吃 '#p'/'#t' 前缀、把中间裸数字当正文画，
		// 正是屏幕出现 9310088/4031988 裸 ID 的根因。用共享 formatter 先展开成
		// 可读名/持有数；NPC 9310088 在 live String/Npc.img 不存在（orange-wz
		// 10012 实查「节点路径不存在」），按原生「取不到即吞令牌」语义用可读占位
		// 补主语，绝不漏裸数字。
		// 换行说明：活数据里 CRLF 是字面 '\\r\\n'（4 个 ASCII 字符，非真实 0x0D/0x0A），
		// 故下面 find('\\n') 通常找不到、整段一次展开；LayoutBuilder 会把 '\\r\\n'
		// 转义解释成断行（GraphicsGL.cpp:1009-1024），保留原文字节即可正确排版。
		const std::string npc_fallback = Locale::text("quest.detail.npc_unknown_inline");
		size_t start = 0;

		while (start <= sum.size())
		{
			size_t nl = sum.find('\n', start);

			std::string line = (nl == std::string::npos)
				? sum.substr(start)
				: sum.substr(start, nl - start);

			const std::string formatted = QuestTextFormatter::format(line, 0, npc_fallback, true);
			detail_lines.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::BLACK, formatted, wrap);

			if (nl == std::string::npos)
				break;

			start = nl + 1;
		}
	}

	void UIQuestLog::change_tab(uint16_t tabid)
	{
		if (tabid > 2)
			return;

		LOG(LOG_INFO, "UIQuestLog: tab change " << tab << " -> " << tabid);
		tab = tabid;
		// round89 W/L/E：对齐 083 CUIQuestInfo::OnTabChanged @0x87fd4f 首行 *(this + 369) = -1;
		// 切换分类页签必须先清选中，再由 rebuild_list() 自动选第一条可见任务（TryShowTopQuest /
		// ToggleDetail(0) 语义）；旧实现保留上一页的 selected_id，跨页残留导致摘要页显示旧任务。
		// 纯客户端状态机：零 wire / 零协议 / 零 WZ / 零新封包，不触碰 s9 改造字段宽度。
		selected_id = -1;

		for (uint16_t i = 0; i < 3; i++)
			buttons[Buttons::TAB0 + i]->set_state(i == tabid ? Button::State::PRESSED : Button::State::NORMAL);

		// r09：底带按钮集合随页签 + 详情态变化（见 relayout_slots），
		// 必须重新 relayout_slots() 才能把可见按钮按右锚重新摆位，否则沿用上一页坐标。
		layout();
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

	// r10：NPC → 所在地图反查。Etc/NpcLocation.img/<npcid> 的每个子节点就是一张地图 id
	// （活数据实证：9000034「工作人员O」有 8 个 mapid）。对齐 MinMS UIQuestLog.cpp:45-69。
	// 本包 nx 有 Etc 卷（nlnx/nx.hpp:31），数据文件 live Data/Etc/NpcLocation.img 已存在。
	std::vector<int32_t> UIQuestLog::npc_location_maps(int32_t npcid)
	{
		std::vector<int32_t> maps;

		if (npcid <= 0)
			return maps;

#ifdef USE_NX
		nl::node loc = nl::nx::Etc["NpcLocation.img"][std::to_string(npcid)];

		for (nl::node entry : loc)
		{
			const int32_t mapid = static_cast<int32_t>(static_cast<int64_t>(entry));

			if (mapid > 0)
				maps.push_back(mapid);
		}
#endif
		return maps;
	}

	// r10：「寻找NPC位置」点击处理，对齐 MinMS UIQuestLog.cpp:678-736：
	//  反查地图 → 打开/复用 UIWorldMap 并 jump_to_map_region(首处) → 弹 UIOk 列出地图名。
	// 纯客户端本地查表，零封包（与 request_teleport 的付费传送是两回事）。
	void UIQuestLog::show_npc_locations(int32_t npcid)
	{
		if (npcid <= 0)
			return;

		const std::vector<int32_t> maps = npc_location_maps(npcid);
		std::string npcname = npc_name(npcid);

		if (npcname.empty())
			npcname = Locale::text("quest.npc_locations.unknown_npc");

		// 拼可读列表（最多列 6 处，超出给总数），文案走 Locale。
		std::string body = Locale::format("quest.npc_locations.header", npcname);

		if (maps.empty())
		{
			body += "\n" + Locale::text("quest.npc_locations.none");
		}
		else
		{
			const size_t limit = maps.size() < 6 ? maps.size() : 6;

			for (size_t i = 0; i < limit; i++)
			{
				const NxHelper::Map::MapInfo info = NxHelper::Map::get_map_info_by_id(maps[i]);
				std::string name = !info.full_name.empty() ? info.full_name
					: (!info.name.empty() ? info.name : Locale::format("quest.npc_locations.map_fallback", std::to_string(maps[i])));
				body += "\n" + name;
			}

			if (maps.size() > limit)
				body += "\n" + Locale::format("quest.npc_locations.more", std::to_string(maps.size()));
		}

		// 打开/复用世界地图，定位首个 NPC 出没地图（本端 jump_to_map_region 已含切分区+选中点位）。
		if (!maps.empty())
		{
			auto wm = UI::get().get_element<UIWorldMap>();

			if (!wm)
				wm = UI::get().emplace<UIWorldMap>();
			else if (!wm->is_active())
				wm->makeactive();

			if (wm)
			{
				// center_on_screen() 在本端 UIElement 是 protected（:371/:381），外部不可调；
				// jump_to_map_region 已含 clear_side_panels+update_world+select_map_spot，
				// 窗口定位由 UIWorldMap 自身构造/update_screen 处理，无需外部居中。
				wm->show_map_region(maps.front());
			}
		}

		UI::get().emplace<UIOk>(body, [](bool) {});
		LOG(LOG_INFO, "UIQuestLog: show_npc_locations npc=" << npcid << " maps=" << maps.size());
	}

	// 命中区必须等于绘制区。展开态原来写的是 left_width + right_width - bg_right_pos.x() = 307，
	// 那是把右板宽度往左投影的笔误；真实绘制宽度是 bg_right_pos.x() + right_width = 548。
	// 于是 x307 以右的 241px —— 正好盖着底带的「寻找NPC位置 / 放弃」和右板滚动条 ——
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

		// 底带按钮行：r09 按「详情态 / 摘要态」× 页签只摆**可见**的几颗，从右锚依次左移。
		// 原生逻辑（用户 2026-11-23 当面确认 + r07 IDA m_bDetailOpen/m_pBtMyLocation）：
		//   · 「详细」(BtDetail) 是详情页展开开关，恒显；
		//   · 「寻找NPC位置」(BtMarkNpc) **只在详情页(detail_mode)显示**；
		//   · 摘要态按页签：tab0 只有「详细」；tab1 进行中 = 放弃(红)+任务讯息(灰)+详细；
		//     tab2 已完成 = 放弃(灰)+任务讯息(灰)+详细。
		// 本端旧实现把「寻找NPC位置」常显在摘要态、且一次排开 6 颗靠 active 隐藏，
		// 导致按钮错位 —— 这正是用户指出的问题。
		// 顺序为「右→左」：向量首元素贴右缘；「详细」恒为最左一颗（与截图一致）。
		static const uint16_t band_detail[] = { Buttons::MARKNPC, Buttons::DETAIL };
		static const uint16_t band_tab0[] = { Buttons::DETAIL };
		static const uint16_t band_tab12[] = { Buttons::GIVEUP, Buttons::ALERT, Buttons::DETAIL };
		const uint16_t* band = detail_mode ? band_detail
			: ((tab == 0) ? band_tab0 : band_tab12);
		const size_t band_count = detail_mode ? 2 : ((tab == 0) ? 1 : 3);

		int16_t x = panel_right - BAND_INSET;

		for (size_t i = 0; i < band_count; i++)
		{
			const uint16_t id = band[i];
			const int16_t w = slot_dim[id].x();

			if (w <= 0)
				continue;

			x -= w;
			// 垂直居中：带中线减自身高度一半（BtMarkNpc/Giveup/Detail 17px → y=369~370；
			// BtAlert 16px → y=370）。IMG 一手尺寸来自 orange-wz get_node_tree_json 帧0。
			const int16_t top = static_cast<int16_t>(BAND_MID - slot_dim[id].y() / 2);
			slot_tl[id] = Point<int16_t>(x, top);
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

		// r09 底带显隐（用户当面确认的原生逻辑，见 relayout_slots 注释）：
		//   · 「详细」恒显，是详情页展开开关；
		//   · 「寻找NPC位置」只在详情页(detail_mode=true)显示；
		//   · 「放弃 / 任务讯息」只在摘要态的 tab1/tab2 显示（详情页里让位给寻找NPC）。
		act(DETAIL, true);
		act(MARKNPC, detail_mode);
		act(ALERT, !detail_mode && (tab == 1 || tab == 2));
		act(GIVEUP, !detail_mode && (tab == 1 || tab == 2));
		act(OK, false);       // 接受/拒绝只在接受确认弹层出现，本窗底带不显示
		act(NO, false);

		// 任务讯息在原生里恒为灰（无对应窗口可开），放弃在「已完成」页也是灰。
		// set_state(DISABLED) 仍绘 disabled 贴图（Button.cpp:55-58 is_active 只挡点击），
		// 正是原生 tab2「放弃」灰态的观感。
		auto setstate = [&](uint16_t id, Button::State s)
		{
			auto iter = buttons.find(id);
			if (iter != buttons.end() && iter->second)
				iter->second->set_state(s);
		};
		setstate(ALERT, Button::State::DISABLED);
		setstate(GIVEUP, tab == 1 ? Button::State::NORMAL : Button::State::DISABLED);
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
				LOG(LOG_INFO, "UIQuestLog: detail mode " << detail_mode << " -> " << !detail_mode);
				detail_mode = !detail_mode;
				// r09：切详情态会改变底带按钮集合（详情态显示「寻找NPC位置」，
				// 摘要态显示「放弃/任务讯息」），必须重排坐标 + 重算显隐。
				layout();
				activate_buttons();
				update_detail();
				update_sliders();
				return Button::State::NORMAL;
			case Buttons::GIVEUP:
				// 复用 UIMiscShells 里既有的放弃协议（action 3 = forfeit），不发明新包。
				if (selected_id > 0)
					QuestActionPacket(3, selected_id).dispatch();
				return Button::State::NORMAL;
			case Buttons::MARKNPC:
			{
				// r10：接上原生/MinMS 同款「寻找NPC位置」——纯本地查 Etc/NpcLocation.img，
				// 打开世界图定位 NPC 出没地图并弹列表。零封包（对齐 MinMS UIQuestLog.cpp:678-736）。
				const int32_t npcid = selected_id > 0 ? QuestData::get(selected_id).get_npc() : 0;

				if (npcid > 0)
					show_npc_locations(npcid);

				return Button::State::NORMAL;
			}
			case Buttons::OK:
			case Buttons::NO:
			case Buttons::ALERT:
				// ALERT「任务讯息」原生恒灰（本窗只显示、不弹窗），set_state(DISABLED) 已挡点击。
				// OK/NO 只在"接受/完成确认"弹层里出现，本窗口底带不显示。
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
