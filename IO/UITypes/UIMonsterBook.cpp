////////////////////////////////////////////////////////////////////////////////
//	怪物图鉴 v83 书本双页实现（D2 修复）。坐标三方对齐，见 UIMonsterBook.h 头注。
////////////////////////////////////////////////////////////////////////////////
#include "UIMonsterBook.h"
#include "UISoftKey.h"
#include "../UI.h"
#include "UIMiscShells.h"

#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MonsterBookPackets.h"
#include "../../Util/Locale.h"
#include "../../Util/UiAssetResolve.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	int32_t UIMonsterBook::mob_id_for_card(int16_t card_suffix)
	{
#ifdef USE_NX
		// 懒建：MonsterBook.img/<mobId>/reward 内 238xxxx → suffix=item%10000。
		// 342 条实测 suffix 无碰撞（_tools/_mb_card_map.py）。
		static std::map<int16_t, int32_t> suffix_to_mob;
		static bool loaded = false;

		if (!loaded)
		{
			loaded = true;
			nl::node root = nl::nx::String["MonsterBook.img"];

			if (root)
			{
				for (const nl::node& mobnode : root)
				{
					const std::string& n = mobnode.name();
					if (n.empty() || n[0] < '0' || n[0] > '9')
						continue;

					const int32_t mobid = static_cast<int32_t>(std::atoi(n.c_str()));
					if (mobid <= 0)
						continue;

					nl::node reward = mobnode["reward"];
					if (!reward)
						continue;

					for (const nl::node& r : reward)
					{
						const int32_t item = static_cast<int32_t>(r.get_integer());
						if (item / 10000 != 238)
							continue;

						suffix_to_mob[static_cast<int16_t>(item % 10000)] = mobid;
						break;
					}
				}

				LOG(LOG_INFO, "UIMonsterBook: card→mob map size=" << suffix_to_mob.size());
			}
			else
			{
				LOG(LOG_WARN, "UIMonsterBook: String/MonsterBook.img missing");
			}
		}

		const auto it = suffix_to_mob.find(card_suffix);
		return it != suffix_to_mob.end() ? it->second : 0;
#else
		(void)card_suffix;
		return 0;
#endif
	}

	std::string UIMonsterBook::mob_name_of(int32_t mob_id)
	{
#ifdef USE_NX
		if (mob_id <= 0)
			return {};

		nl::node n = nl::nx::String["Mob.img"][std::to_string(mob_id)]["name"];
		if (n)
			return n.get_string();
#endif
		(void)mob_id;
		return {};
	}

	std::string UIMonsterBook::truncate_utf8(const std::string& s, size_t max_chars)
	{
		if (s.size() <= max_chars)
			return s;

		// 按字节截断后避开半截 UTF-8 续字节（简化，不拆完整 codepoint 表）。
		size_t cut = max_chars;
		while (cut > 0 && (static_cast<unsigned char>(s[cut]) & 0xC0) == 0x80)
			--cut;

		return s.substr(0, cut) + "...";
	}

	// 布局基点：backgrnd(475x349) 在窗内的落点。原生帧里 LeftTab 探出底图
	// 左沿 7px、RightTab 探出右沿 ~27px（怪物手册.png 模板匹配 rel=(-7,131+) /
	// (450,30+)），故窗原点给底图留 (10,0) 左边距、窗宽 516。
	static constexpr int16_t ORG_X = 10;
	static constexpr int16_t ORG_Y = 0;

	UIMonsterBook::UIMonsterBook() :
		header(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, ""),
		page_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, ""),
		status_line(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::EMPEROR, "")
	{
		dimension = Point<int16_t>(516, 349);
		dragarea = Point<int16_t>(dimension.x(), 20);

		for (uint8_t i = 0; i < 6; ++i)
			info_line[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");

		// 基本情报页数值列：RIGHT 对齐，填在 infoPage 印死标签右侧。
		for (uint8_t i = 0; i < 5; ++i)
			stats_value[i] = Text(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK, "");

		for (uint8_t i = 0; i < NOTE_COUNT; ++i)
			note_checked.push_back(false);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("MonsterBook");

		if (!UiAssetResolve::usable(root))
		{
			LOG(LOG_WARN, "UIMonsterBook: UIWindow.img/MonsterBook missing — inactive");
			return;
		}

		has_assets = true;

		// ---- 13 节点全绑（orange-wz 实测清单）----
		backgrnd = Texture(root["backgrnd"]);			// 475x349 书本底
		infoPage = Texture(root["infoPage"]);			// 左页 170x231 @(40,35)
		cardSlot = Texture(root["cardSlot"]);			// 右页 174x256 @(261,53)
		has_grid = UiAssetResolve::usable(root["cardSlot"]);
		selectOv = Texture(root["select"]);				// 选中框 31x42 origin(2,40)
		coverOv = Texture(root["cover"]);				// 封面框 31x42 origin(2,40)
		fullMark = Texture(root["fullMark"]);			// 全套标记 7x11

		for (int32_t i = 0; i < 8; ++i)
		{
			if (UiAssetResolve::usable(root["icon"][i]))
				cell_icon.push_back(Texture(root["icon"][i]));	// 卡片占位 22x27
		}

		// 按钮：id 沿用 Angel sub_86256D 立即数；搜索/翻页落左页（卡片格下方）。
		const Point<int16_t> BtClose_pos(429, 8);
		const Point<int16_t> BtSearch_pos(175, 29);
		const Point<int16_t> arrow_prev_card(48, 285);
		const Point<int16_t> arrow_next_card(185, 285);
		// 右页翻页箭头（Angel 仍建；卡片页主翻页用左组）
		const Point<int16_t> arrow_prev_page(270, 285);
		const Point<int16_t> arrow_next_page(407, 285);

		if (UiAssetResolve::usable(root["BtClose"]))
			try_maple_btn(buttons, Buttons::BtClose, root["BtClose"], BtClose_pos, "mb BtClose");
		else if (nl::node close = UiAssetResolve::close_btn())
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(close, BtClose_pos);

		try_maple_btn(buttons, Buttons::BtSearch, root["BtSearch"], BtSearch_pos, "mb BtSearch");
		try_maple_btn(buttons, Buttons::BtPrevCard, root["arrowLeft"], arrow_prev_card, "mb arrowL card");
		try_maple_btn(buttons, Buttons::BtNextCard, root["arrowRight"], arrow_next_card, "mb arrowR card");
		try_maple_btn(buttons, Buttons::BtPrevPage, root["arrowLeft"], arrow_prev_page, "mb arrowL page");
		try_maple_btn(buttons, Buttons::BtNextPage, root["arrowRight"], arrow_next_page, "mb arrowR page");

		// 9 张便签（LeftTab 0..8，原生 UpdateCheckList @0x867be6 勾选态）
		for (int16_t i = 0; i < NOTE_COUNT; ++i)
		{
			const Point<int16_t> p(-7, 131 + 20 * i);	// d2_tab_anchor SAD=0.0
			if (try_maple_btn(buttons, static_cast<uint16_t>(Buttons::BtNote0 + i),
				root["LeftTab"][i], p, "mb LeftTab"))
			{
				note_icon.push_back(Texture(root["LeftTab"][i]["normal"]["0"]));
			}
		}

		// 右列 4 页签（RightTab 0..3，怪物手册.png 行带 y=30+39i）
		for (int16_t i = 0; i < INFOTAB_COUNT; ++i)
		{
			try_maple_btn(buttons, static_cast<uint16_t>(Buttons::BtInfoTab0 + i),
				root["RightTab"][i], Point<int16_t>(450, 30 + 39 * i), "mb RightTab");
		}
#endif

		// 首次打开（用户尚未拖动过）按当前视口居中；拖动后由基类 save(position)
		// 写入 Setting<PosMONSTERBOOK>，再次打开时 center_if_position_unset 不再干预。
		center_if_position_unset(dimension);

		load_cards();
		sync_infotab_indicators();	// 右标签初始激活态（默认 tab 0 = 基本情报）
		// 有卡则默认选中首张，进入图3/5 卡片页；无卡保持图4 基本情报。
		if (!cards.empty())
			select_card_by_index(0);
		else
			refresh_info();
	}

	int16_t UIMonsterBook::note_category_of(int16_t card_suffix)
	{
		// Angel CMonsterBookMan::LoadCard @0x6848a6：
		//   v11 = (itemId - 2380000) / 1000; *(card+8)=v11; 再按 v11 挂 LeftTab。
		// 本端 card 键为 itemId%10000（= suffix），故 category = suffix/1000 ∈ [0,8]。
		const int32_t suffix = card_suffix < 0 ? -card_suffix : card_suffix;
		int16_t cat = static_cast<int16_t>(suffix / 1000);
		if (cat < 0)
			cat = 0;
		if (cat >= NOTE_COUNT)
			cat = static_cast<int16_t>(NOTE_COUNT - 1);
		return cat;
	}

	void UIMonsterBook::load_cards()
	{
		all_cards.clear();

		const MonsterBook& mb = Stage::get().get_player().get_monsterbook();
		for (const auto& kv : mb.get_cards())
			all_cards.push_back({ kv.first, kv.second });

		sync_note_indicators();
		apply_note_filter();
	}

	void UIMonsterBook::sync_note_indicators()
	{
		// Angel UpdateCheckList @0x867be6：便签 i 亮 = 分类 i 计数>0。
		note_checked.assign(NOTE_COUNT, false);
		for (const auto& c : all_cards)
		{
			const int16_t cat = note_category_of(c.id);
			if (cat >= 0 && cat < NOTE_COUNT)
				note_checked[static_cast<size_t>(cat)] = true;
		}

		for (int16_t i = 0; i < NOTE_COUNT; ++i)
		{
			const uint16_t bid = static_cast<uint16_t>(Buttons::BtNote0 + i);
			auto it = buttons.find(bid);
			if (it == buttons.end() || !it->second)
				continue;
			// 三态（对齐 Angel UpdateCheckList @0x867be6 的勾选/禁用）：
			// 当前筛选签 PRESSED 常驻；其余有卡 NORMAL、无卡 DISABLED。
			// 常驻态不会被框架冲掉——UIElement 只重置 MOUSEOVER（:290/:342），不碰 PRESSED/DISABLED。
			Button::State st;
			if (active_note == i)
				st = Button::State::PRESSED;
			else if (note_checked[static_cast<size_t>(i)])
				st = Button::State::NORMAL;
			else
				st = Button::State::DISABLED;
			it->second->set_state(st);
		}
	}

	void UIMonsterBook::sync_infotab_indicators()
	{
		// 右标签常驻激活态（Angel UpdateUI @0x862527 开头 for(i=3){*(this+2836+2i)=1}
		// 明确设 3 个状态标志）。照 UISkillBook::change_tab（:998-1004）范式：
		// 旧签 NORMAL + active_infotab PRESSED，按钮视觉态直接由 IMG normal/pressed 帧表达。
		for (int16_t i = 0; i < INFOTAB_COUNT; ++i)
		{
			const uint16_t bid = static_cast<uint16_t>(Buttons::BtInfoTab0 + i);
			auto it = buttons.find(bid);
			if (it != buttons.end() && it->second)
				it->second->set_state(i == active_infotab
					? Button::State::PRESSED : Button::State::NORMAL);
		}
	}

	void UIMonsterBook::apply_note_filter()
	{
		cards.clear();
		if (active_note < 0)
		{
			cards = all_cards;
		}
		else
		{
			for (const auto& c : all_cards)
			{
				if (note_category_of(c.id) == active_note)
					cards.push_back(c);
			}
		}

		if (selected_index >= static_cast<int32_t>(cards.size()))
			selected_index = cards.empty() ? -1 : static_cast<int32_t>(cards.size()) - 1;
		// 从基本情报页（selected_index=-1）点左标签筛出卡时，自动选中首张进入
		// 卡片页，否则重开仍停在统计页，观感就是"标签点了没反应"。
		// active_note>=0 限定为用户主动筛选，避免无卡初始态被误选中。
		if (selected_index < 0 && !cards.empty() && active_note >= 0)
			selected_index = 0;
		if (selected_index >= 0)
			selected_card_id = cards[static_cast<size_t>(selected_index)].id;

		card_page = std::min(card_page, static_cast<int32_t>(page_count() - 1));
		if (card_page < 0)
			card_page = 0;

		refresh_info();
		LOG(LOG_DEBUG, "UIMonsterBook: filter note=" << active_note
			<< " view=" << cards.size() << "/" << all_cards.size());
	}

	int32_t UIMonsterBook::page_count() const
	{
		if (cards.empty())
			return 1;
		return static_cast<int32_t>((cards.size() + CARDS_PER_PAGE - 1) / CARDS_PER_PAGE);
	}

	void UIMonsterBook::refresh_info()
	{
		// 分页箭头激活/禁用态（第 4 点）：首页禁"上一页"、末页禁"下一页"，
		// 中间两态可点。page_count 固定 >=1（见 :317），单页时两侧都禁用。
		{
			const int32_t pc = page_count();
			auto set_arrow = [this](uint16_t bid, Button::State st) {
				auto it = buttons.find(bid);
				if (it != buttons.end() && it->second)
					it->second->set_state(st);
			};
			set_arrow(Buttons::BtPrevCard, card_page <= 0 ? Button::State::DISABLED : Button::State::NORMAL);
			set_arrow(Buttons::BtNextCard, card_page >= pc - 1 ? Button::State::DISABLED : Button::State::NORMAL);
			set_arrow(Buttons::BtPrevPage, card_page <= 0 ? Button::State::DISABLED : Button::State::NORMAL);
			set_arrow(Buttons::BtNextPage, card_page >= pc - 1 ? Button::State::DISABLED : Button::State::NORMAL);
		}

		const MonsterBook& mb = Stage::get().get_player().get_monsterbook();

		header.change_text(Locale::format("monsterbook.label_cards", std::to_string(cards.size())));

		for (int i = 0; i < 6; ++i)
		{
			info_line[i].change_text("");
			// 从卡片页切回基本情报页时复位配色（卡片页会把 1/2 行染成 HP/MP 色）。
			info_line[i].change_color(Color::Name::BLACK);
		}

		// 无选中卡：图4 基本情报书级统计。infoPage.png 背景已印死 5 个原生标签
		// （手册等级/总登记/特殊/一般/书签），此处只填数值，RIGHT 对齐落在标签右侧，
		// 不再重复画 label——这正是截图里标签与数值叠成"乱码"的根因。
		if (selected_index < 0 || selected_index >= static_cast<int32_t>(cards.size()))
		{
			int32_t total = static_cast<int32_t>(all_cards.size());
			int32_t special = 0;
			for (const auto& c : all_cards)
				if (c.level >= 2)
					++special;

			// 手册等级：复刻服务端 MonsterBook.calculateLevel 的 collectionExp 公式，
			// 只是本地现算（手册包不含 bookLevel；读服务端 charInfo 会动公共协议）。
			int32_t level = 0;
			int32_t exp_to_next = 1;
			do {
				++level;
				exp_to_next += level * 10;
			} while (total >= exp_to_next);

			stats_value[0].change_text(std::to_string(level));
			stats_value[1].change_text(std::to_string(total));
			stats_value[2].change_text(std::to_string(special));
			stats_value[3].change_text(std::to_string(total - special));
			stats_value[4].change_text(mb.get_cover() > 0
				? std::to_string(mb.get_cover()) : std::string("-"));
			page_label.change_text(std::to_string(card_page + 1) + "/" + std::to_string(page_count()));
			return;
		}

		const CardEntry& c = cards[static_cast<size_t>(selected_index)];
		const int32_t mobid = mob_id_for_card(c.id);
		const std::string mname = mob_name_of(mobid);

		// 右页：怪名 + HP/MP。原生卡片页（怪物手册2.png）无论是否收集都固定显示
		// HP/MP ???（HP 玫红 PINK、MP 蓝 BLUE），本端无 Mob 数值源，恒 ??? 对齐原生。
		info_line[0].change_text(!mname.empty()
			? mname
			: Locale::format("monsterbook.stub_mob", std::to_string(c.id)));
		info_line[1].change_text("HP : ???");
		info_line[1].change_color(Color::Name::PINK);
		info_line[2].change_text("MP : ???");
		info_line[2].change_color(Color::Name::BLUE);

#ifdef USE_NX
		nl::node mbnode = mobid > 0
			? nl::nx::String["MonsterBook.img"][std::to_string(mobid)]
			: nl::node{};

		// RightTab 1..3 附加情报写入 status_line，避免挤占右页 HP/MP 行
		if (active_infotab == 1 && mbnode)
		{
			std::string maps;
			int shown = 0;
			for (const nl::node& m : mbnode["map"])
			{
				if (shown >= 3) break;
				if (!maps.empty()) maps += ",";
				maps += std::to_string(static_cast<int32_t>(m.get_integer()));
				++shown;
			}
			status_line.change_text(maps.empty()
				? Locale::text("monsterbook.tab_empty")
				: Locale::format("monsterbook.maps", maps));
		}
		else if (active_infotab == 2 && mbnode)
		{
			std::string drops;
			int shown = 0;
			for (const nl::node& r : mbnode["reward"])
			{
				if (shown >= 4) break;
				if (!drops.empty()) drops += ",";
				drops += std::to_string(static_cast<int32_t>(r.get_integer()));
				++shown;
			}
			status_line.change_text(drops.empty()
				? Locale::text("monsterbook.tab_empty")
				: Locale::format("monsterbook.rewards", drops));
		}
		else if (active_infotab == 3)
		{
			status_line.change_text(Locale::format("monsterbook.info_id", std::to_string(c.id)));
		}
		else
		{
			status_line.change_text("");
		}
#endif

		page_label.change_text(std::to_string(card_page + 1) + "/" + std::to_string(page_count()));
	}

	void UIMonsterBook::select_card_by_index(int32_t index)
	{
		if (index < 0 || index >= static_cast<int32_t>(cards.size()))
			return;

		selected_index = index;
		selected_card_id = cards[static_cast<size_t>(index)].id;
		card_page = index / CARDS_PER_PAGE;
		refresh_info();
	}

	void UIMonsterBook::page_cards(int8_t delta)
	{
		int32_t np = static_cast<int32_t>(page_count()) - 1;
		card_page = std::max<int32_t>(0, std::min<int32_t>(np, card_page + delta));

		// 选中项跟随翻页落在新页首格（原生无此细节记录，取最直观行为并留日志）
		if (!cards.empty())
		{
			int32_t first = card_page * CARDS_PER_PAGE;
			if (selected_index < first || selected_index >= first + CARDS_PER_PAGE)
			{
				selected_index = first;
				selected_card_id = cards[static_cast<size_t>(first)].id;
			}
		}
		refresh_info();
	}

	int32_t UIMonsterBook::cell_at(Point<int16_t> cursorpos) const
	{
		if (!has_grid)
			return -1;

		const Point<int16_t> rel(
			static_cast<int16_t>(cursorpos.x() - position.x() - ORG_X),
			static_cast<int16_t>(cursorpos.y() - position.y() - ORG_Y));

		// 左页卡片格 GRID_X/Y（5×5）
		const int16_t gx = static_cast<int16_t>(rel.x() - GRID_X);
		const int16_t gy = static_cast<int16_t>(rel.y() - GRID_Y);
		if (gx < 0 || gy < 0)
			return -1;

		int32_t col = gx / CELL_W;
		int32_t row = gy / CELL_H;
		if (col >= GRID_COLS || row >= GRID_ROWS)
			return -1;

		int32_t idx = card_page * CARDS_PER_PAGE + row * GRID_COLS + col;
		if (idx >= static_cast<int32_t>(cards.size()))
			return -1;
		return idx;
	}

	void UIMonsterBook::set_note(int16_t index, bool on)
	{
		if (index < 0 || index >= NOTE_COUNT)
			return;
		// P2：LeftTab 作分类筛选（Angel sub_8603DF 切 tab）；再点同签取消 → 全量。
		// 视觉态由 button_pressed 调 sync_note_indicators 统一设：
		// 当前签 PRESSED、有卡签 NORMAL、无卡签 DISABLED（对齐 Angel UpdateCheckList）。
		active_note = on ? index : -1;
		card_page = 0;
		apply_note_filter();
	}

	void UIMonsterBook::draw(float inter) const
	{
		if (!has_assets)
			return;

		// D3：自绘 Texture/Text 手动乘淡入（GraphicsGL::drawtext/draw 均消费
		// args.get_color() 的 alpha；推进点在 UIElement::draw_buttons 每帧 tick）。
		const float fade = fade_alpha();
		const Point<int16_t> org = position + Point<int16_t>(ORG_X, ORG_Y);

		backgrnd.draw(DrawArgument(org) + fade);

		// 便签筛选态（active_note>=0）即便筛出 0 张也留在卡片页（空 5×5 格），
		// 不跳基本情报页——否则切到没有卡的分类就以为卡片丢了。只有"未筛选且无选中卡"
		// 才显示书级统计页。
		const bool book_stats_mode = (selected_index < 0 && active_note < 0 && active_infotab == 0);

		if (book_stats_mode)
		{
			// 图4 基本情报：左页书级统计（infoPage），右页空白。
			// infoPage 背景已印死 5 个原生标签，数值用 RIGHT 对齐填在标签右侧
			// 同一行（标签 canvas x≈8..115，数值右缘 canvas x≈158 → 窗内 x=org+198）。
			// y 沿用对齐背景标签的窗内行位，避免重复 label 造成叠字。
			infoPage.draw(DrawArgument(org + Point<int16_t>(40, 35)) + fade);
			header.draw(DrawArgument(org + Point<int16_t>(49, 30)) + fade);
			stats_value[0].draw(DrawArgument(org + Point<int16_t>(198, 113)) + fade);
			stats_value[1].draw(DrawArgument(org + Point<int16_t>(198, 166)) + fade);
			stats_value[2].draw(DrawArgument(org + Point<int16_t>(198, 197)) + fade);
			stats_value[3].draw(DrawArgument(org + Point<int16_t>(198, 218)) + fade);
			stats_value[4].draw(DrawArgument(org + Point<int16_t>(198, 254)) + fade);
		}
		else
		{
			// 图3/5 卡片页：左 5×5 格，右怪名 + HP/MP。
			if (has_grid)
			{
				cardSlot.draw(DrawArgument(org + Point<int16_t>(GRID_X - 8, GRID_Y - 2)) + fade);

				int32_t first = card_page * CARDS_PER_PAGE;
				for (int32_t i = first; i < first + CARDS_PER_PAGE && i < static_cast<int32_t>(cards.size()); ++i)
				{
					int32_t k = i - first;
					const Point<int16_t> cell = org + Point<int16_t>(
						static_cast<int16_t>(GRID_X + (k % GRID_COLS) * CELL_W),
						static_cast<int16_t>(GRID_Y + (k / GRID_COLS) * CELL_H));

					if (!cell_icon.empty())
						cell_icon[static_cast<size_t>(i % cell_icon.size())].draw(
							DrawArgument(cell + Point<int16_t>(4, 7)) + fade);

					if (i == selected_index && selectOv)
						selectOv.draw(DrawArgument(cell + selectOv.get_origin()) + fade);
					if (cards[static_cast<size_t>(i)].id == Stage::get().get_player().get_monsterbook().get_cover()
						&& coverOv)
						coverOv.draw(DrawArgument(cell + coverOv.get_origin()) + fade);
					// 收集完成（level>=5，服务端 addCard 上限 5）的卡画满级角标：
					// 复用 fullMark 7x11，落在格子右上角。原生 LoadBookIcon/LoadMobInfo
					// 对满级卡有专门标记，本端以现有 IMG fullMark 表达，不新增资源。
					if (cards[static_cast<size_t>(i)].level >= 5 && fullMark)
						fullMark.draw(DrawArgument(cell + Point<int16_t>(CELL_W - 9, 0)) + fade);
				}
			}

			// 右页详情：怪名居中，HP/MP 底栏（未收集显示 ???）
			info_line[0].draw(DrawArgument(org + Point<int16_t>(DETAIL_X + 40, DETAIL_NAME_Y)) + fade);
			info_line[1].draw(DrawArgument(org + Point<int16_t>(DETAIL_X + 40, DETAIL_HP_Y)) + fade);
			info_line[2].draw(DrawArgument(org + Point<int16_t>(DETAIL_X + 40, DETAIL_MP_Y)) + fade);
			page_label.draw(DrawArgument(org + Point<int16_t>(GRID_X + 70, 288)) + fade);
		}

		// 全套标记：右页底。d2_canvas_anchor 实测 win_rel=(236,335) SAD=0.0，
		// win_rel 即 backgrnd 左上为原点（org），故直接 org+(236,335)。
		if (fullMark)
			fullMark.draw(DrawArgument(org + Point<int16_t>(236, 335)) + fade);

		status_line.draw(DrawArgument(org + Point<int16_t>(49, 322)) + fade);

		UIElement::draw_buttons(inter);
	}

	void UIMonsterBook::update()
	{
		UIElement::update();
	}

	Button::State UIMonsterBook::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
				deactivate();
				return Button::State::NORMAL;
			case Buttons::BtSearch:
			{
				// IDA 核对（13337，input MD5 同 Angel）：CUIMonsterBook/CMonsterBookMan
				// 全部具名方法无 Search handler、无软键盘调用；find_regex 全库仅
				// NewYearsCard/BtSearch，MonsterBook 无搜索字符串。故 BtSearch 在原生
				// 无独立逻辑（或走基类未具名 handler），本端不再自创软键盘搜索。
				return Button::State::NORMAL;
			}
			case Buttons::BtPrevCard:
				select_card_by_index(selected_index - 1);
				return Button::State::NORMAL;
			case Buttons::BtNextCard:
				select_card_by_index(selected_index + 1);
				return Button::State::NORMAL;
			case Buttons::BtPrevPage:
				page_cards(-1);
				return Button::State::NORMAL;
			case Buttons::BtNextPage:
				page_cards(1);
				return Button::State::NORMAL;
			default:
				break;
		}

		if (buttonid >= Buttons::BtNote0 && buttonid < static_cast<uint16_t>(Buttons::BtNote0 + NOTE_COUNT))
		{
			int16_t i = static_cast<int16_t>(buttonid - Buttons::BtNote0);
			const bool turn_on = (active_note != i);
			set_note(i, turn_on);
			sync_note_indicators();
			// 常驻 PRESSED 由 sync_note_indicators 统一设（当前筛选签），
			// 返回 PRESSED 让框架 send_cursor 不把它打回 NORMAL（UISkillBook 范式）。
			return Button::State::PRESSED;
		}

		if (buttonid >= Buttons::BtInfoTab0 && buttonid < static_cast<uint16_t>(Buttons::BtInfoTab0 + INFOTAB_COUNT))
		{
			active_infotab = static_cast<int16_t>(buttonid - Buttons::BtInfoTab0);
			// 点"基本情报"(tab 0)回首页：清便签筛选 + 清选中卡 → 走统计页。
			if (active_infotab == 0)
			{
				active_note = -1;
				selected_index = -1;
				selected_card_id = 0;
				card_page = 0;
				apply_note_filter();
			}
			sync_infotab_indicators();
			refresh_info();
			return Button::State::PRESSED;
		}

		return Button::State::DISABLED;
	}

	Cursor::State UIMonsterBook::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (clicked)
		{
			int32_t idx = cell_at(cursorpos);
			if (idx >= 0)
			{
				select_card_by_index(idx);
				return Cursor::State::IDLE;
			}
		}

		// 交给 UIDragElement：标题条（dragarea 顶 20px）命中时进入拖动，
		// 落点保存由基类 remove_cursor/send_cursor 完成。卡片格点击在上方已截获。
		return UIDragElement<PosMONSTERBOOK>::send_cursor(clicked, cursorpos);
	}

	void UIMonsterBook::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;

		switch (keycode)
		{
			case 'C': case 'c':
				if (selected_card_id > 0)
				{
					MonsterBookSetCoverPacket(selected_card_id).dispatch();
					status_line.change_text(Locale::format("monsterbook.cover_id", std::to_string(selected_card_id)));
					LOG(LOG_DEBUG, "UIMonsterBook: cover selected=" << selected_card_id);
					refresh_info();
				}
				break;
			case 'R': case 'r':
				reload();
				LOG(LOG_DEBUG, "UIMonsterBook: reload");
				break;
			case 'S': case 's':
				button_pressed(Buttons::BtSearch);
				break;
			default:
				break;
		}
	}

	void UIMonsterBook::send_scroll(double yoffset)
	{
		page_cards(yoffset > 0 ? 1 : -1);
	}

	void UIMonsterBook::reload()
	{
		load_cards();
	}

	UIElement::Type UIMonsterBook::get_type() const
	{
		return TYPE;
	}
}
