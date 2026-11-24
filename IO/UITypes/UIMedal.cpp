//////////////////////////////////////////////////////////////////////////////////
//	D1 / 分号键「勋章」窗实现（v83 MEDAL = CUIMedalQuestInfo）。
//	原生模型与素材判定见 UIMedal.h 头注（turn8 取证：ctor 260x374、
//	Draw 步进 49/23、OnMouseButton 行选中、SP_5301/5302 状态文本）。
//////////////////////////////////////////////////////////////////////////////////
#include "UIMedal.h"

#include "../Components/MapleButton.h"
#include "../../Character/QuestLog.h"
#include "../../Data/ItemData.h"
#include "../../Data/QuestData.h"
#include "../../Gameplay/Stage.h"
#include "../../Graphics/DrawArgument.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>
#include <array>
#include <cstdlib>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	// Color(r,g,b,a) 四个 int 字面量会在 Color(float,float,float,float) 与
	// Color(uint8_t,uint8_t,uint8_t,uint8_t) 之间重载歧义（C2440）。
	// 走 std::array<uint8_t,4> 的明确构造，保持量测得到的 0..255 RGB 值。
	static Color rgb(uint8_t r, uint8_t g, uint8_t b)
	{
		return Color(std::array<uint8_t, Color::LENGTH>{ r, g, b, 255 });
	}

	UIMedal::UIMedal() :
		title(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("uimedal.23")),
		count_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::MINESHAFT, ""),
		empty_hint(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::DARKGREY, ""),
		detail_title(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("uimedal.detail_title"))
	{
		dimension = Point<int16_t>(FRAME_W, FRAME_H);
		center_on_screen();

		detail_frame = ColorBox(DETAIL_W, DETAIL_H, rgb(221, 221, 221), 1.0f);
		detail_back = ColorBox(static_cast<int16_t>(DETAIL_W - 2), static_cast<int16_t>(DETAIL_H - 2),
			rgb(238, 238, 255), 1.0f);
		for (int i = 0; i < 6; ++i)
			detail_line[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");

		// —— 框架色带（勋章框.png 逐像素量测，见 UIMedal.h 注释）——
		// 外沿：边框浅灰 + 白标题区
		frame_edge = ColorBox(FRAME_W, FRAME_H, rgb(221, 221, 221), 1.0f);
		frame_back = ColorBox(static_cast<int16_t>(FRAME_W - 2), static_cast<int16_t>(FRAME_H - 2), Color::Name::WHITE, 1.0f);
		title_line_top = ColorBox(static_cast<int16_t>(FRAME_W - 2), 1, rgb(85, 119, 153), 1.0f);
		title_line_bot = ColorBox(static_cast<int16_t>(FRAME_W - 2), 1, rgb(119, 153, 187), 1.0f);

		// 页签带底色（y26..45，量测 (170,170,170)）
		list_back = ColorBox(static_cast<int16_t>(FRAME_W - 4), static_cast<int16_t>(TAB_Y + TAB_H - (TITLE_H + 1)), rgb(170, 170, 170), 1.0f);

		// 计数条白底（y46..79）+ 列表面板 浅蓝白 (238,238,255)（y78..350）+ 底条蓝灰
		count_bar_bg = ColorBox(static_cast<int16_t>(FRAME_W - 10), COUNT_H, Color::Name::WHITE, 1.0f);
		list_panel = ColorBox(LIST_W, static_cast<int16_t>(LIST_BOT - LIST_Y), rgb(238, 238, 255), 1.0f);
		bottom_bar = ColorBox(static_cast<int16_t>(FRAME_W - 2), static_cast<int16_t>(FRAME_H - BOTBAR_Y), rgb(153, 187, 204), 1.0f);

		// 行底条：选中高亮 / 隔行（原生 FillRect 230x18；此处按列表宽度）
		row_strip = ColorBox(static_cast<int16_t>(LIST_W - 4), STRIP_H, rgb(221, 221, 221), 0.9f);
		sel_strip = ColorBox(static_cast<int16_t>(LIST_W - 4), STRIP_H, rgb(204, 204, 204), 1.0f);

		// 滚动条：槽 (238,238,238)、上下钮 (221,221,221)
		scroll_track = ColorBox(SB_W, static_cast<int16_t>(LIST_BOT - LIST_Y - 2 * SB_BTN_H), rgb(238, 238, 238), 1.0f);
		scroll_thumb = ColorBox(SB_W, 40, rgb(221, 221, 221), 1.0f);
		scroll_btn_top = ColorBox(SB_W, SB_BTN_H, rgb(221, 221, 221), 1.0f);
		scroll_btn_bot = ColorBox(SB_W, SB_BTN_H, rgb(221, 221, 221), 1.0f);

		const char* names[TAB_COUNT] = { "基本", "职业", "一般", "挑战", "活动" };
		tab_plate.reserve(TAB_COUNT);
		tab_label.reserve(TAB_COUNT);

		for (int16_t i = 0; i < TAB_COUNT; ++i)
		{
			tab_plate.emplace_back(TAB_W, TAB_H, rgb(204, 204, 204), 1.0f);
			tab_label.emplace_back(Text::Font::A11M, Text::Alignment::CENTER,
				Color::Name::BLACK, Locale::text(i == 0 ? "uimedal.25" : std::string("uimedal.tab.") + std::to_string(i - 1)));
			// 上面兜底 Locale；页签名以原生中文为准（names[]），若 Locale 缺键回显键名时修正
			if (tab_label[static_cast<size_t>(i)].get_text().find("uimedal.") == 0)
				tab_label[static_cast<size_t>(i)].change_text(names[i]);
		}

		build_catalog();
		select_tab(0);

#ifdef USE_NX
		// W2：Title 真材优先（orange-wz：backgrnd 260x374 / Tab 0..4 / backgrnd3 Detail）。
		nl::node title_root = UiAssetResolve::window("Title");
		if (title_root)
		{
			if (nl::node bg = title_root["backgrnd"])
			{
				title_frame = Texture(bg);
				has_title_frame = static_cast<bool>(title_frame);
				if (has_title_frame)
				{
					Point<int16_t> o = title_frame.get_origin();
					if (o.x() || o.y())
						title_frame.shift(o);
					Point<int16_t> d = title_frame.get_dimensions();
					if (d.x() > 0 && d.y() > 0)
						dimension = d;
				}
			}

			nl::node tab_en = title_root["Tab"]["enabled"];
			nl::node tab_dis = title_root["Tab"]["disabled"];
			tab_on.resize(TAB_COUNT);
			tab_off.resize(TAB_COUNT);
			for (int16_t i = 0; i < TAB_COUNT; ++i)
			{
				const std::string key = std::to_string(i);
				if (tab_en && tab_en[key])
					tab_on[static_cast<size_t>(i)] = Texture(tab_en[key]);
				if (tab_dis && tab_dis[key])
					tab_off[static_cast<size_t>(i)] = Texture(tab_dis[key]);
			}

			nl::node det = title_root["backgrnd3"] ? title_root["backgrnd3"] : title_root["backgrnd5"];
			if (det)
			{
				detail_tex = Texture(det);
				has_detail_frame = static_cast<bool>(detail_tex);
				if (has_detail_frame)
				{
					Point<int16_t> o = detail_tex.get_origin();
					if (o.x() || o.y())
						detail_tex.shift(o);
				}
			}

			LOG(LOG_INFO, "UIMedal: Title assets frame=" << has_title_frame
				<< " detail=" << has_detail_frame
				<< " tabs_on=" << tab_on.size());
		}
		else
		{
			UiAssetResolve::diag_once("medal.title", "UIWindow.img/Title 缺失 —— ColorBox 兜底");
		}

		nl::node close = UiAssetResolve::close_btn();

		if (close)
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				close, Point<int16_t>(static_cast<int16_t>(FRAME_W - 21), 3));
#endif
	}

	std::string UIMedal::medal_item_name(int32_t item_id)
	{
#ifdef USE_NX
		// 勋章段 114xxxx 落在 Eqp/Accessory；无名则回退道具号文本。
		nl::node n = nl::nx::String["Eqp.img"]["Eqp"]["Accessory"][std::to_string(item_id)]["name"];
		if (n)
		{
			const std::string s = n.get_string();
			if (!s.empty())
				return s;
		}
#endif
		return Locale::format("uimedal.item_id", std::to_string(item_id));
	}

	void UIMedal::open_detail(int32_t view_row)
	{
		if (view_row < 0 || view_row >= static_cast<int32_t>(view_rows.size()))
		{
			close_detail();
			return;
		}

		const MedalEntry& e = catalog[view_rows[static_cast<size_t>(view_row)]];
		const QuestData& qd = QuestData::get(e.qid);
		const QuestLog& log = Stage::get().get_player().get_quests();

		detail_title.change_text(Locale::text("uimedal.detail_title"));
		detail_line[0].change_text(qd.get_name());
		detail_line[1].change_text(Locale::format("uimedal.detail_qid", std::to_string(e.qid)));
		detail_line[2].change_text(Locale::format("uimedal.detail_item",
			medal_item_name(e.medal_item) + " (" + std::to_string(e.medal_item) + ")"));
		detail_line[3].change_text(Locale::format("uimedal.detail_cat", std::to_string(e.category)));

		std::string st;
		if (log.is_completed(e.qid))
			st = Locale::text("uimedal.obtained");
		else if (log.is_started(e.qid))
			st = Locale::text("uimedal.challenging");
		else if (obtainable(e))
			st = Locale::text("uimedal.obtainable");
		detail_line[4].change_text(st);

		std::string sum = qd.get_summary();
		if (sum.size() > 96)
			sum = sum.substr(0, 96) + "...";
		detail_line[5].change_text(sum.empty() ? Locale::text("uimedal.detail_close") : sum);

		detail_open = true;
		LOG(LOG_DEBUG, "UIMedal: detail open qid=" << e.qid << " item=" << e.medal_item);
	}

	void UIMedal::close_detail()
	{
		detail_open = false;
	}

	void UIMedal::draw_detail(float fade) const
	{
		if (!detail_open)
			return;

		// 贴在主窗右侧，尺寸对齐原生 Detail 305x373（Angel ToggleDetail @0x83d255）。
		const Point<int16_t> dp = position + Point<int16_t>(static_cast<int16_t>(dimension.x() + 4), 0);
		if (has_detail_frame && detail_tex)
			detail_tex.draw(DrawArgument(dp) + fade);
		else
		{
			detail_frame.draw(DrawArgument(dp) + fade);
			detail_back.draw(DrawArgument(dp + Point<int16_t>(1, 1)) + fade);
		}
		detail_title.draw(DrawArgument(dp + Point<int16_t>(10, 8)) + fade);

		int16_t y = 36;
		for (int i = 0; i < 6; ++i)
		{
			detail_line[i].draw(DrawArgument(dp + Point<int16_t>(12, y)) + fade);
			y = static_cast<int16_t>(y + (i == 0 || i == 5 ? 28 : 22));
		}
	}

	void UIMedal::build_catalog()
	{
		// 原生 GetQuestDisplayMedal @0x71df41：QuestInfo.img/<qid>/viewMedalItem。
		// .nx 探针实测本包 82 条（72×29xxx + 19xxx 等），按字段过滤非号段假设。
#ifdef USE_NX
		nl::node info_root = nl::nx::Quest["QuestInfo.img"];

		if (!info_root)
		{
			UiAssetResolve::diag_once("medal.catalog", "Quest/QuestInfo.img 缺失 —— 勋章目录为空");
			return;
		}

		for (const nl::node& q : info_root)
		{
			nl::node vmi = q["viewMedalItem"];

			if (!vmi)
				continue;

			int32_t item = static_cast<int32_t>(vmi.get_integer());

			if (item == 0)
				continue;

			MedalEntry e;
			const std::string& qname = q.name();	// nl::node::name()（node.hpp:116）
			e.qid = static_cast<int16_t>(qname.empty() ? 0 : std::atoi(qname.c_str()));
			e.medal_item = item;

			nl::node cat = q["medalCategory"];
			e.category = cat ? static_cast<int16_t>(cat.get_integer()) : 0;

			if (e.qid > 0)
				catalog.push_back(e);
		}

		LOG(LOG_INFO, "UIMedal: catalog entries=" << catalog.size());
#endif
	}

	int16_t UIMedal::row_h() const
	{
		// Draw @0x843bf6：tab==100（基本=已获得全量）步进 49；分类 tab 0..3 步进 23。
		return active_tab == 0 ? ROW_H_GOT : ROW_H_CAT;
	}

	bool UIMedal::obtainable(const MedalEntry& e) const
	{
		// IsStateButton 近似：未完成 + 满足接取条件 = 可获得(SP_5301)；
		// 进行中 = 挑战中(SP_5302)。原生按任务状态机，本地以 meets_requirements
		// 近似（QuestData 同款判定，UIQuestLog.cpp:546 同路）。
		// Player&（非 const）：get_quests() 只有非 const 版（Player.h:151）。
		Player& p = Stage::get().get_player();
		const QuestLog& log = p.get_quests();

		if (log.is_started(e.qid))
			return false;		// 挑战中行走 started 分支文案

		int16_t level = static_cast<int16_t>(p.get_level());
		uint16_t job = p.get_stats().get_job().get_id();

		return QuestData::get(e.qid).meets_requirements(level, job, log);
	}

	void UIMedal::refresh_view()
	{
		const QuestLog& log = Stage::get().get_player().get_quests();

		view_rows.clear();
		row_primary.clear();
		row_status.clear();
		row_icons.clear();
		obtained_total = 0;
		selected = -1;
		close_detail();

		for (size_t i = 0; i < catalog.size(); ++i)
		{
			const MedalEntry& e = catalog[i];

			const bool completed = log.is_completed(e.qid);

			if (completed)
				obtained_total++;

			// 页签 0（基本）= 已获得全量列表（原生 tab 100 语义）；
			// 页签 1..4 = 分类目录（全部条目按 category 过滤，含未获得）。
			bool match = false;

			if (active_tab == 0)
			{
				match = completed;
			}
			else
			{
				switch (active_tab)
				{
					case 1: match = e.category == 1; break;	// 职业
					case 2: match = e.category == 0; break;	// 一般（字段缺失）
					case 3: match = e.category == 2; break;	// 挑战
					case 4: match = e.category == 3; break;	// 活动
				}
			}

			if (!match)
				continue;

			view_rows.push_back(i);

			const QuestData& qd = QuestData::get(e.qid);

			// P2：114xxxx 走 ItemData→Character/Accessory/0114*.img/info/icon（有则画）。
			Texture icon;
			{
				const ItemData& idata = ItemData::get(e.medal_item);
				if (idata)
					icon = idata.get_icon(e.medal_item / 10000 == 238);
			}
			row_icons.push_back(icon);

			if (active_tab == 0)
			{
				// 获得页双行（步进 49）：任务名 + 勋章道具名
				row_primary.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
					Color::Name::BLACK, qd.get_name());
				row_status.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
					Color::Name::DARKGREY, medal_item_name(e.medal_item));
			}
			else
			{
				// 分类页单行（步进 23）：名 + 状态（已获得/可获得/挑战中）
				std::string st;

				if (completed)
					st = Locale::text("uimedal.obtained");
				else if (log.is_started(e.qid))
					st = Locale::text("uimedal.challenging");	// SP_5302 挑战中
				else if (obtainable(e))
					st = Locale::text("uimedal.obtainable");	// SP_5301 可获得

				row_primary.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
					completed ? Color::Name::BLACK : Color::Name::DARKGREY, qd.get_name());
				row_status.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
					Color::Name::MEDIUMBLUE, st);
			}
		}

		count_label.change_text(Locale::format("uimedal.count", std::to_string(obtained_total)));

		if (view_rows.empty())
			empty_hint.change_text(Locale::text("uimedal.empty"));

		int16_t visible = static_cast<int16_t>((LIST_BOT - LIST_Y) / row_h());
		int32_t maxscroll = std::max<int32_t>(0,
			static_cast<int32_t>(view_rows.size()) * row_h() - visible * row_h());
		scroll = static_cast<int16_t>(std::min<int32_t>(scroll, maxscroll));
		if (scroll < 0)
			scroll = 0;
	}

	void UIMedal::select_tab(int16_t index)
	{
		if (index < 0 || index >= TAB_COUNT)
			return;

		active_tab = index;
		scroll = 0;

		// 页签选中态在写侧重建色块（draw 是 const，不得改状态）：
		// 选中=粉红(238,102,136)（勋章框.png「基本」实测；ColorBox::set_color
		// 只收 Color::Name，命名色无此粉 → 直接赋值重建 custom Color 版）。
		for (int16_t i = 0; i < static_cast<int16_t>(tab_plate.size()); ++i)
		{
			const bool sel = i == active_tab;
			tab_plate[i] = ColorBox(TAB_W, TAB_H,
				sel ? rgb(238, 102, 136) : rgb(204, 204, 204), 1.0f);
			tab_label[i].change_color(Color::Name::BLACK);
		}

		refresh_view();
	}

	int32_t UIMedal::hit_row(Point<int16_t> cursorpos) const
	{
		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x()),
			static_cast<int16_t>(cursorpos.y() - position.y()));

		if (rel.x() < LIST_X || rel.x() >= static_cast<int16_t>(LIST_X + LIST_W))
			return -1;
		if (rel.y() < LIST_Y || rel.y() >= LIST_BOT)
			return -1;

		const int16_t rh = row_h();
		const int32_t row = (rel.y() - LIST_Y + scroll) / rh;
		const int32_t first = scroll / rh;

		// 屏内行号 = first..first+visible-1；越屏（下方留白）不算命中
		if (row < first || row >= first + (LIST_BOT - LIST_Y) / rh)
			return -1;
		if (row < 0 || row >= static_cast<int32_t>(view_rows.size()))
			return -1;

		return row;
	}

	void UIMedal::draw(float inter) const
	{
		// D3：自绘 ColorBox/Text 手动乘淡入（推进点 UIElement::draw_buttons 每帧 tick）。
		const float fade = fade_alpha();

		if (has_title_frame && title_frame)
		{
			// W2：Title/backgrnd 真材已含标题/外框；页签用 Tab enabled/disabled。
			title_frame.draw(DrawArgument(position) + fade);
		}
		else
		{
			// ColorBox 兜底（无 Title 节点时）。
			frame_edge.draw(DrawArgument(position) + fade);
			frame_back.draw(DrawArgument(position + Point<int16_t>(1, 1)) + fade);
			title_line_top.draw(DrawArgument(position + Point<int16_t>(1, 1)) + fade);
			title_line_bot.draw(DrawArgument(position + Point<int16_t>(1, TITLE_H)) + fade);
			list_back.draw(DrawArgument(position + Point<int16_t>(2, TITLE_H + 1)) + fade);
			title.draw(DrawArgument(position + Point<int16_t>(8, 5)) + fade);
			count_bar_bg.draw(DrawArgument(position + Point<int16_t>(5, COUNT_Y)) + fade);
			list_panel.draw(DrawArgument(position + Point<int16_t>(LIST_X, LIST_Y)) + fade);
			bottom_bar.draw(DrawArgument(position + Point<int16_t>(1, BOTBAR_Y)) + fade);
		}

		for (int16_t i = 0; i < TAB_COUNT; ++i)
		{
			const int16_t tx = static_cast<int16_t>(TAB_X0 + i * (TAB_W + TAB_GAP));
			const bool sel = (i == active_tab);
			const size_t idx = static_cast<size_t>(i);

			if (sel && idx < tab_on.size() && tab_on[idx])
				tab_on[idx].draw(DrawArgument(position + Point<int16_t>(tx, TAB_Y)) + fade);
			else if (!sel && idx < tab_off.size() && tab_off[idx])
				tab_off[idx].draw(DrawArgument(position + Point<int16_t>(tx, TAB_Y)) + fade);
			else if (idx < tab_plate.size())
			{
				tab_plate[idx].draw(DrawArgument(position + Point<int16_t>(tx, TAB_Y)) + fade);
				if (idx < tab_label.size())
					tab_label[idx].draw(DrawArgument(position + Point<int16_t>(tx, static_cast<int16_t>(TAB_Y + 2))) + fade);
			}
		}

		count_label.draw(DrawArgument(position + Point<int16_t>(0, COUNT_Y + 4)) + fade);

		const int16_t rh = row_h();
		const int16_t first = static_cast<int16_t>(scroll / rh);

		for (int16_t r = 0; ; ++r)
		{
			const size_t idx = static_cast<size_t>(first + r);

			if (idx >= view_rows.size())
				break;

			const int16_t y = static_cast<int16_t>(LIST_Y + r * rh - scroll);

			if (y + STRIP_H > LIST_BOT)
				break;

			// 行底条：原生选中行 FillRect 高亮
			if (static_cast<int32_t>(idx) == selected)
				sel_strip.draw(DrawArgument(position + Point<int16_t>(LIST_X + 2, y)) + fade);
			else if (r % 2 == 1)
				row_strip.draw(DrawArgument(position + Point<int16_t>(LIST_X + 2, y)) + fade);

			// 图标列 x≈7（原生文本 x=29，图标落其左）；无画布则跳过，文本仍在。
			const int16_t text_x = 29;
			if (idx < row_icons.size() && row_icons[idx])
			{
				row_icons[idx].draw(DrawArgument(
					position + Point<int16_t>(7, static_cast<int16_t>(y + 1))) + fade);
			}

			row_primary[idx].draw(DrawArgument(position + Point<int16_t>(text_x, static_cast<int16_t>(y + 2))) + fade);

			if (active_tab == 0)
				row_status[idx].draw(DrawArgument(position + Point<int16_t>(text_x, static_cast<int16_t>(y + 20))) + fade);
			else
				row_status[idx].draw(DrawArgument(position + Point<int16_t>(55, static_cast<int16_t>(y + 2))) + fade);
		}

		if (view_rows.empty())
			empty_hint.draw(DrawArgument(position + Point<int16_t>(0, 160)) + fade);

		// 滚动条：槽 x239..253，上下钮 (221,221,221) @LIST_Y / @LIST_BOT-SB_BTN_H
		scroll_btn_top.draw(DrawArgument(position + Point<int16_t>(SB_X, LIST_Y)) + fade);
		scroll_track.draw(DrawArgument(position + Point<int16_t>(SB_X, static_cast<int16_t>(LIST_Y + SB_BTN_H))) + fade);
		scroll_thumb.draw(DrawArgument(position + Point<int16_t>(SB_X, LIST_Y + SB_BTN_H)) + fade);
		scroll_btn_bot.draw(DrawArgument(position + Point<int16_t>(SB_X, static_cast<int16_t>(LIST_BOT - SB_BTN_H))) + fade);

		draw_detail(fade);
		UIElement::draw_buttons(inter);
	}

	void UIMedal::update()
	{
		UIElement::update();
	}

	void UIMedal::toggle_active()
	{
		if (!active)
		{
			// 重开刷新完成态（勋章=已完成任务集；开窗即准，不挂任务事件线）。
			refresh_view();
			close_detail();
			makeactive();
		}
		else
		{
			close_detail();
			deactivate();
		}
	}

	Button::State UIMedal::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				close_detail();
				deactivate();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}

	Cursor::State UIMedal::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			// 页签命中
			const Point<int16_t> rel(
				static_cast<int16_t>(cursorpos.x() - position.x()),
				static_cast<int16_t>(cursorpos.y() - position.y()));

			if (rel.y() >= TAB_Y && rel.y() < static_cast<int16_t>(TAB_Y + TAB_H))
			{
				for (int16_t i = 0; i < TAB_COUNT; ++i)
				{
					const int16_t tx = static_cast<int16_t>(TAB_X0 + i * (TAB_W + TAB_GAP));

					if (rel.x() >= tx && rel.x() < static_cast<int16_t>(tx + TAB_W))
					{
						select_tab(i);
						return Cursor::State::IDLE;
					}
				}
			}

			// 行命中（OnMouseButton @0x841678）：命中行→选中；再点已选中行→
			// ToggleDetail → CUIMedalQuestInfoDetail 305x373（@0x83d255）。
			// P1：无 114 图标素材，文本 Detail 浮层开/关。
			const int32_t row = hit_row(cursorpos);

			if (row >= 0)
			{
				if (selected == row)
				{
					if (detail_open)
						close_detail();
					else
						open_detail(row);
				}
				else
				{
					selected = row;
					close_detail();
					LOG(LOG_DEBUG, "UIMedal: row select=" << row);
				}

				return Cursor::State::IDLE;
			}

			if (selected >= 0 && rel.y() >= LIST_BOT)
			{
				selected = -1;
				close_detail();
			}
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIMedal::send_key(int32_t keycode, bool pressed, bool escape)
	{
		(void)keycode;

		if (pressed && escape && active)
			deactivate();
	}

	void UIMedal::send_scroll(double yoffset)
	{
		const int16_t rh = row_h();
		scroll = static_cast<int16_t>(scroll + (yoffset > 0 ? rh : -rh));
		if (scroll < 0)
			scroll = 0;

		const int16_t visible = static_cast<int16_t>((LIST_BOT - LIST_Y) / rh);
		const int32_t maxscroll = std::max<int32_t>(0,
			static_cast<int32_t>(view_rows.size()) * rh - visible * rh);

		if (scroll > maxscroll)
			scroll = static_cast<int16_t>(maxscroll);
	}

	UIElement::Type UIMedal::get_type() const
	{
		return TYPE;
	}
}
