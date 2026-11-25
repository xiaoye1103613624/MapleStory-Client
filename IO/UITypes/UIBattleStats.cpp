//////////////////////////////////////////////////////////////////////////////////
// UIBattleStats 实现 — 见 UIBattleStats.h 契约。
// 资源：UI/UIWindow4_battlestats.img/battleStats/{main,mini}（192 原生，GMS key）。
//////////////////////////////////////////////////////////////////////////////////
#include "UIBattleStats.h"

#include "../UI.h"
#include "../Components/AreaButton.h"
#include "../../Net/Packets/BattleStatsPackets.h"
#include "../../Util/Locale.h"
#include "../../MapleStory.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 中文文案兜底：Locale 缺键时返回给定中文（避免回显 key）。
		std::string cn(const char* key, const char* fallback)
		{
			std::string s = Locale::text(key);
			return s == key ? std::string(fallback) : s;
		}

		int64_t now_ms()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now().time_since_epoch()).count();
		}

		std::string fmt_duration(int64_t ms)
		{
			if (ms < 0) ms = 0;
			const int64_t total_sec = ms / 1000;
			const int64_t h = total_sec / 3600;
			const int64_t m = (total_sec % 3600) / 60;
			const int64_t s = total_sec % 60;
			char buf[32];
			std::snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld",
				static_cast<long long>(h), static_cast<long long>(m), static_cast<long long>(s));
			return std::string(buf);
		}
	}

	UIBattleStats::UIBattleStats()
	{
		dimension = Point<int16_t>(MAIN_W, MAIN_H);
		load_assets();

		// 命中区按原生按钮 id 注册；UIElement::send_cursor 统一驱动状态机与 button_pressed。
		// 坐标以 main 981×615 为基准（mini 模式沿用同一命中区，只缩小绘制）。
		const uint16_t ids[] = { BtClose, BtMini, BtHelp, BtStart, BtEnd, BtLeft, BtRight };
		for (uint16_t id : ids)
		{
			Point<int16_t> pos;
			Point<int16_t> dim(20, 20);
			switch (id)
			{
			case BtClose: pos = Point<int16_t>(MAIN_W - 40, 6); break;
			case BtMini:  pos = Point<int16_t>(MAIN_W - 66, 6); break;
			case BtHelp:  pos = Point<int16_t>(MAIN_W - 92, 6); break;
			case BtStart: pos = Point<int16_t>(MAIN_W - 200, MAIN_H - 60); dim = Point<int16_t>(170, 44); break;
			case BtEnd:   pos = Point<int16_t>(MAIN_W - 380, MAIN_H - 60); dim = Point<int16_t>(170, 44); break;
			case BtLeft:  pos = Point<int16_t>(20, MAIN_H - 60); break;
			case BtRight: pos = Point<int16_t>(48, MAIN_H - 60); break;
			default: break;
			}
			buttons[id] = std::make_unique<AreaButton>(pos, dim);
		}

		title_text = Text(Text::Font::A13B, Text::Alignment::LEFT,
			Color::Name::YELLOW, cn("battlestats.title", "战斗统计数据"));
		empty_text = Text(Text::Font::A11M, Text::Alignment::CENTER,
			Color::Name::DARKGREY, cn("battlestats.empty", "暂无统计数据，点击开始测定"));

		center_if_position_unset(Point<int16_t>(MAIN_W, MAIN_H));
		rebuild_rows();

		// 打开面板：请求开始/继续（bOn=1，非新建），服务端回许可 + 快照。
		send_request(true, false, false);
		local_start_ms = now_ms();
		LOG(LOG_INFO, "UIBattleStats: open, request 0x3BC on, assets=" << (has_assets ? 1 : 0));
	}

	void UIBattleStats::load_assets()
	{
		if (assets_loaded)
			return;
		assets_loaded = true;

#ifdef USE_NX
		// 独立 img（不碰 live UIWindow4.img 的 equip192 结构）。
		nl::node root = nl::nx::UI["UIWindow4_battlestats.img"]["battleStats"];
		if (!root)
			root = nl::nx::UI["UIWindow4.img"]["battleStats"]; // 兼容未来合并进主 img

		if (!root)
		{
			LOG(LOG_WARNING, "UIBattleStats: battleStats root missing, pure-color fallback");
			return;
		}

		auto tex = [](Texture& out, nl::node n) -> bool {
			if (!n || n.data_type() != nl::node::type::bitmap)
				return false;
			out = Texture(n);
			return out ? true : false;
		};
		tex(bg_main, root["main"]["backgrnd"]);
		tex(bg_main2, root["main"]["backgrnd2"]);
		tex(bg_mini, root["mini"]["backgrnd"]);
		has_assets = bg_main || bg_mini;
		LOG(LOG_INFO, "UIBattleStats assets: main=" << (bg_main ? 1 : 0)
			<< " main2=" << (bg_main2 ? 1 : 0) << " mini=" << (bg_mini ? 1 : 0));
#endif
	}

	void UIBattleStats::draw(float inter) const
	{
		const float fade = fade_alpha();
		const Point<int16_t> org = position;
		const DrawArgument base(org);

		if (mode == Mode::Main)
		{
			if (bg_main) bg_main.draw(base + fade);
			if (bg_main2) bg_main2.draw(base + fade);
		}
		else
		{
			// mini：优先真 mini 背景；缺失时把 main 按 211×182 缩放降级。
			if (bg_mini)
				bg_mini.draw(base + fade);
			else if (bg_main)
				bg_main.draw(DrawArgument(org, 211.0f / MAIN_W, 182.0f / MAIN_H) + fade);
		}

		title_text.draw(DrawArgument(org + Point<int16_t>(30, 8)) + fade);

		// 四个核心字段（原生烘焙在背景上；这里叠加动态数值保证实时可见）。
		const BattleStats& data = BattleStats::get();
		const std::vector<BattleStats::Entry> snap = data.snapshot();
		BattleStats::Entry self;
		if (!snap.empty())
			self = snap.front();

		int64_t dur = self.duration_ms;
		if (dur <= 0 && data.is_running())
			dur = now_ms() - local_start_ms; // 本地计时兜底

		const int16_t fx = mode == Mode::Main ? 40 : 16;
		const int16_t fy = mode == Mode::Main ? 60 : 40;
		const int16_t step = mode == Mode::Main ? 26 : 18;
		const Text::Font font = mode == Mode::Main ? Text::Font::A13B : Text::Font::A11B;
		const int16_t valx = mode == Mode::Main ? fx + 300 : fx + 150;
		const std::string labels[] = {
			cn("battlestats.duration", "战斗时间"),
			cn("battlestats.kills", "击杀怪物"),
			cn("battlestats.meso", "累计金币"),
			cn("battlestats.exp", "获得经验值")
		};
		const std::string values[] = {
			fmt_duration(dur),
			fmt_num(self.kills),
			fmt_num(self.meso),
			fmt_num(self.exp)
		};
		for (int i = 0; i < 4; ++i)
		{
			Text lab(font, Text::Alignment::LEFT, Color::Name::WHITE, labels[i]);
			Text val(font, Text::Alignment::RIGHT, Color::Name::YELLOW, values[i]);
			const int16_t y = fy + static_cast<int16_t>(i * step);
			lab.draw(DrawArgument(org + Point<int16_t>(fx, y)) + fade);
			val.draw(DrawArgument(org + Point<int16_t>(valx, y)) + fade);
		}

		// main 模式逐队员行（翻页 + 排序）。
		if (mode == Mode::Main)
		{
			if (rows.empty())
			{
				empty_text.draw(DrawArgument(org + Point<int16_t>(MAIN_W / 2, ROW_Y + 40)) + fade);
			}
			else
			{
				for (size_t i = 0; i < row_texts.size(); ++i)
					row_texts[i].draw(DrawArgument(org + Point<int16_t>(ROW_X, ROW_Y + static_cast<int16_t>(i * ROW_H))) + fade);

				Text pg(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::LIGHTGREY,
					std::to_string(page + 1));
				pg.draw(DrawArgument(org + Point<int16_t>(MAIN_W - 60, MAIN_H - 60)) + fade);
			}
		}

		draw_buttons(fade);
		(void)inter;
	}

	void UIBattleStats::update()
	{
		UIElement::update();
	}

	Button::State UIBattleStats::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
		case BtClose:
			close();
			break;
		case BtMini:
			if (mode == Mode::Main)
			{
				mode = Mode::Mini;
				dimension = Point<int16_t>(MINI_W, MINI_H);
			}
			else
			{
				mode = Mode::Main;
				dimension = Point<int16_t>(MAIN_W, MAIN_H);
			}
			clamp_drag_position();
			break;
		case BtStart:
			send_request(true, true, false); // 新建测定（bIsNew=1，重置数据）
			BattleStats::get().reset();
			local_start_ms = now_ms();
			rebuild_rows();
			break;
		case BtEnd:
			send_request(false, false, false); // 结束测定
			BattleStats::get().set_local_running(false);
			rebuild_rows();
			break;
		case BtLeft:
			if (page > 0) --page;
			rebuild_rows();
			break;
		case BtRight:
			if ((page + 1) * MAX_ROWS < static_cast<int32_t>(rows.size())) ++page;
			rebuild_rows();
			break;
		case BtHelp:
			UI::get().show_text(Tooltip::Parent::TEXT,
				cn("battlestats.help", "开始测定后，战斗时间/击杀/金币/经验将实时统计，最多 60 分钟。"));
			break;
		default:
			break;
		}
		return Button::State::NORMAL;
	}

	Cursor::State UIBattleStats::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		// 交给基类按钮状态机处理按钮（含 NORMAL+clicked 补触发修复）；
		// 拖拽由 UIDragElement 兜底。按钮命中时不拖动窗口。
		for (const auto& kv : buttons)
		{
			AreaButton* a = static_cast<AreaButton*>(kv.second.get());
			if (a && a->is_active() && a->bounds(position).contains(cursorpos))
				return UIElement::send_cursor(clicked, cursorpos);
		}
		return UIDragElement::send_cursor(clicked, cursorpos);
	}

	void UIBattleStats::remove_cursor()
	{
		UIDragElement::remove_cursor();
	}

	void UIBattleStats::send_key(int32_t keycode, bool pressed, bool escape)
	{
		(void)keycode;
		if (pressed && escape)
			close();
	}

	UIElement::Type UIBattleStats::get_type() const
	{
		return TYPE;
	}

	void UIBattleStats::reload()
	{
		rebuild_rows();
	}

	void UIBattleStats::rebuild_rows()
	{
		rows = BattleStats::get().snapshot();
		// 7 项排序：当前实现覆盖伤害/击杀两项主维度，其余维度保留原生语义位（UI 显示同值）。
		switch (sort_key)
		{
		case SortKey::Damage:
			std::sort(rows.begin(), rows.end(), [](const BattleStats::Entry& a, const BattleStats::Entry& b) {
				return a.damage > b.damage; });
			break;
		case SortKey::Count:
			std::sort(rows.begin(), rows.end(), [](const BattleStats::Entry& a, const BattleStats::Entry& b) {
				return a.kills > b.kills; });
			break;
		case SortKey::Order:
		case SortKey::AvgDamage:
		case SortKey::AttackNum:
		case SortKey::MaxDamage:
		case SortKey::MinDamage:
		default:
			break;
		}

		row_texts.clear();
		const size_t begin = static_cast<size_t>(page) * MAX_ROWS;
		for (size_t i = begin; i < rows.size() && i < begin + MAX_ROWS; ++i)
			row_texts.emplace_back(Text::Font::A11M, Text::Alignment::LEFT,
				Color::Name::WHITE, format_field(rows[i], sort_key));
	}

	std::string UIBattleStats::fmt_num(uint64_t v)
	{
		const std::string s = std::to_string(v);
		std::string out;
		for (size_t i = 0; i < s.size(); ++i)
		{
			if (i && (s.size() - i) % 3 == 0)
				out.push_back(',');
			out.push_back(s[i]);
		}
		return out;
	}

	std::string UIBattleStats::format_field(const BattleStats::Entry& e, SortKey key) const
	{
		switch (key)
		{
		case SortKey::Damage:    return e.name + "  伤害 " + fmt_num(e.damage);
		case SortKey::Count:     return e.name + "  击杀 " + fmt_num(e.kills);
		case SortKey::AvgDamage: return e.name + "  平均 " + fmt_num(e.damage);
		case SortKey::AttackNum: return e.name + "  攻击 " + fmt_num(e.kills);
		case SortKey::MaxDamage: return e.name + "  最高 " + fmt_num(e.damage);
		case SortKey::MinDamage: return e.name + "  最低 " + fmt_num(e.damage);
		case SortKey::Order:
		default:                 return e.name + "  " + fmt_num(e.damage);
		}
	}

	void UIBattleStats::send_request(bool on, bool isNew, bool clear) const
	{
		BattleStatsRequestPacket packet(on, isNew, clear);
		packet.dispatch();
		if (on && !isNew)
			BattleStats::get().set_local_running(true);
		LOG(LOG_DEBUG, "UIBattleStats request 0x3BC on=" << (int)on
			<< " new=" << (int)isNew << " clear=" << (int)clear);
	}

	void UIBattleStats::close()
	{
		send_request(false, false, false);
		BattleStats::get().set_local_running(false);
		deactivate();
	}
}
