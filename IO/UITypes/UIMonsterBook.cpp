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

		for (uint8_t i = 0; i < 6; ++i)
			info_line[i] = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "");

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

		// 按钮：id 沿用 Angel sub_86256D 立即数（1000/2000..2004），坐标 = 原生
		const Point<int16_t> BtClose_pos(429, 8);
		const Point<int16_t> BtSearch_pos(175, 29);
		const Point<int16_t> arrow_prev_card(48, 285);
		const Point<int16_t> arrow_next_card(185, 285);
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

		load_cards();
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
			it->second->set_state(note_checked[static_cast<size_t>(i)]
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
		const MonsterBook& mb = Stage::get().get_player().get_monsterbook();

		// 原生控件 2005 @(49,30) 120x15 = 左页标题行
		header.change_text(Locale::format("monsterbook.label_cards", std::to_string(cards.size())));

		for (int i = 0; i < 6; ++i)
			info_line[i].change_text("");

		if (selected_index < 0 || selected_index >= static_cast<int32_t>(cards.size()))
		{
			info_line[0].change_text(Locale::text("monsterbook.no_selection"));
			page_label.change_text(std::to_string(card_page + 1) + "/" + std::to_string(page_count()));
			return;
		}

		const CardEntry& c = cards[static_cast<size_t>(selected_index)];
		const int32_t mobid = mob_id_for_card(c.id);
		const std::string mname = mob_name_of(mobid);

		// 行 0：怪物名（命中 String）或 stub；行 1：卡片等级；行 2：封面
		if (!mname.empty())
			info_line[0].change_text(mname);
		else
			info_line[0].change_text(Locale::format("monsterbook.stub_mob", std::to_string(c.id)));

		info_line[1].change_text(Locale::format("monsterbook.info_level",
			std::to_string(static_cast<int>(c.level))));
		info_line[2].change_text(Locale::format("monsterbook.cover_id",
			mb.get_cover() > 0 ? std::to_string(mb.get_cover()) : std::string("-")));

#ifdef USE_NX
		nl::node mbnode = mobid > 0
			? nl::nx::String["MonsterBook.img"][std::to_string(mobid)]
			: nl::node{};

		// RightTab：0=简介 episode / 1=出没地图 / 2=掉落道具号 / 3=卡片号兜底
		if (active_infotab == 0)
		{
			std::string ep = mbnode ? mbnode["episode"].get_string() : std::string{};
			// episode 含换行或字面 "\\n"；左页只抽首行缩略，避免撑破 infoPage。
			size_t nlpos = ep.find('\n');
			if (nlpos == std::string::npos)
				nlpos = ep.find("\\n");
			if (nlpos != std::string::npos)
				ep = ep.substr(0, nlpos);
			info_line[3].change_text(truncate_utf8(ep, 48));
		}
		else if (active_infotab == 1 && mbnode)
		{
			std::string maps;
			nl::node mapn = mbnode["map"];
			int shown = 0;
			for (const nl::node& m : mapn)
			{
				if (shown >= 3)
					break;
				if (!maps.empty())
					maps += ",";
				maps += std::to_string(static_cast<int32_t>(m.get_integer()));
				++shown;
			}
			info_line[3].change_text(maps.empty()
				? Locale::text("monsterbook.tab_empty")
				: Locale::format("monsterbook.maps", maps));
		}
		else if (active_infotab == 2 && mbnode)
		{
			std::string drops;
			nl::node rew = mbnode["reward"];
			int shown = 0;
			for (const nl::node& r : rew)
			{
				if (shown >= 4)
					break;
				if (!drops.empty())
					drops += ",";
				drops += std::to_string(static_cast<int32_t>(r.get_integer()));
				++shown;
			}
			info_line[3].change_text(drops.empty()
				? Locale::text("monsterbook.tab_empty")
				: Locale::format("monsterbook.rewards", drops));
		}
		else
		{
			info_line[3].change_text(Locale::format("monsterbook.info_id", std::to_string(c.id)));
		}
#else
		info_line[3].change_text(Locale::format("monsterbook.info_id", std::to_string(c.id)));
#endif

		info_line[4].change_text(mobid > 0
			? Locale::format("monsterbook.mob_id", std::to_string(mobid))
			: Locale::format("monsterbook.info_id", std::to_string(c.id)));

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

		// 网格原点：cardSlot(261,53) 174x256 内容 5x6 格 31x42，居中留边
		const int16_t gx = static_cast<int16_t>(rel.x() - 270);
		const int16_t gy = static_cast<int16_t>(rel.y() - 55);
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
		// 指示灯仍由 sync_note_indicators 按持有分类点亮，不跟 click 翻转。
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

		// 左页：infoPage @(40,35)，文本行按怪物手册.png 墨迹行带 y=113/166/197/218/254
		infoPage.draw(DrawArgument(org + Point<int16_t>(40, 35)) + fade);
		header.draw(DrawArgument(org + Point<int16_t>(49, 30)) + fade);
		info_line[0].draw(DrawArgument(org + Point<int16_t>(50, 113)) + fade);
		info_line[1].draw(DrawArgument(org + Point<int16_t>(50, 166)) + fade);
		info_line[2].draw(DrawArgument(org + Point<int16_t>(50, 197)) + fade);
		info_line[3].draw(DrawArgument(org + Point<int16_t>(50, 218)) + fade);
		info_line[4].draw(DrawArgument(org + Point<int16_t>(50, 254)) + fade);

		// 选中卡片占位图（原生红卡位 x151..170 y55..78 帧 → rel(117,48)）
		if (!cell_icon.empty())
			cell_icon[0].draw(DrawArgument(org + Point<int16_t>(117, 48)) + fade);

		// 右页：cardSlot @(261,53) 5x6 格
		if (has_grid)
		{
			cardSlot.draw(DrawArgument(org + Point<int16_t>(261, 53)) + fade);

			int32_t first = card_page * CARDS_PER_PAGE;
			for (int32_t i = first; i < first + CARDS_PER_PAGE && i < static_cast<int32_t>(cards.size()); ++i)
			{
				int32_t k = i - first;
				const Point<int16_t> cell = org + Point<int16_t>(
					static_cast<int16_t>(270 + (k % GRID_COLS) * CELL_W),
					static_cast<int16_t>(55 + (k / GRID_COLS) * CELL_H));

				if (!cell_icon.empty())
					cell_icon[static_cast<size_t>(i % cell_icon.size())].draw(
						DrawArgument(cell + Point<int16_t>(4, 7)) + fade);

				// Texture::draw 的 top-left = pos - origin（DrawArgument.h:134），
				// select/cover origin=(2,40)（orange-wz 实测）⇒ 传 cell + origin
				// 才让覆盖件顶部落在格子左上角。
				if (i == selected_index && selectOv)
					selectOv.draw(DrawArgument(cell + selectOv.get_origin()) + fade);
				if (cards[static_cast<size_t>(i)].id == Stage::get().get_player().get_monsterbook().get_cover()
					&& coverOv)
					coverOv.draw(DrawArgument(cell + coverOv.get_origin()) + fade);
			}
		}

		page_label.draw(DrawArgument(org + Point<int16_t>(330, 288)) + fade);

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
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					if (s.empty()) return;
					for (int32_t i = 0; i < static_cast<int32_t>(cards.size()); ++i)
					{
						const CardEntry& ce = cards[static_cast<size_t>(i)];
						if (std::to_string(ce.id) == s)
						{
							select_card_by_index(i);
							status_line.change_text(Locale::format("monsterbook.info_id", s));
							LOG(LOG_DEBUG, "UIMonsterBook: search hit card=" << s);
							return;
						}

						const int32_t mid = mob_id_for_card(ce.id);
						const std::string nm = mob_name_of(mid);
						if (!nm.empty() && nm.find(s) != std::string::npos)
						{
							select_card_by_index(i);
							status_line.change_text(nm);
							LOG(LOG_DEBUG, "UIMonsterBook: search hit name=" << nm);
							return;
						}
					}
					status_line.change_text(Locale::text("monsterbook.not_found"));
				}, []() {}, "card id");
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
			// 当前筛选签额外保持 PRESSED 观感
			if (turn_on)
			{
				auto it = buttons.find(buttonid);
				if (it != buttons.end() && it->second)
					it->second->set_state(Button::State::PRESSED);
			}
			return Button::State::NORMAL;
		}

		if (buttonid >= Buttons::BtInfoTab0 && buttonid < static_cast<uint16_t>(Buttons::BtInfoTab0 + INFOTAB_COUNT))
		{
			active_infotab = static_cast<int16_t>(buttonid - Buttons::BtInfoTab0);
			refresh_info();
			return Button::State::NORMAL;
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

		return UIElement::send_cursor(clicked, cursorpos);
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
