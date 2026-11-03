//////////////////////////////////////////////////////////////////////////////////
//	PartySearch UI：BtStart→222 / BtStop·Pause→223；列表板无 S→C（匹配走组队邀请）
//////////////////////////////////////////////////////////////////////////////////
#include "UIPartySearch.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/PartySearchPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include <cstdlib>

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIPartySearch::UIPartySearch() :
		has_assets(false),
		searching(false),
		min_level(1),
		max_level(30),
		jobs_mask(0x7FFFFFFF),
		status_label(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK, "Idle — press Start to search")
	{
		position = Point<int16_t>(260, 60);
		dimension = Point<int16_t>(305, 407);
		dragarea = Point<int16_t>(305, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("PartySearch");

		if (!root)
		{
			LOG(LOG_WARN, "UIPartySearch: UIWindow.img/PartySearch missing — inactive");
			return;
		}

		has_assets = true;

		if (root["backgrnd"])
		{
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();

			if (o.x() != 0 || o.y() != 0)
				backgrnd.shift(o);

			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}

		nl::node close = UiAssetResolve::close_btn();

		if (close)
		{
			buttons[Buttons::BtClose] = std::make_unique<MapleButton>(
				close,
				Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 5)
			);
		}

		const int16_t by = static_cast<int16_t>(dimension.y() - 36);
		auto place = [&](Buttons id, nl::node n, int16_t x)
		{
			if (n)
				buttons[id] = std::make_unique<MapleButton>(n, Point<int16_t>(x, by));
		};

		place(Buttons::BtStart, root["BtStart"], 12);
		place(Buttons::BtStop, root["BtStop"], 80);
		place(Buttons::BtPause, root["BtPause"], 148);
		place(Buttons::BtReg, root["BtReg"], 216);
		place(Buttons::BtCancel, root["BtCancel"], 268);

		// 默认等级窗：以自身等级为中心，跨度 ≤30
		int32_t lv = static_cast<int32_t>(Stage::get().get_player().get_level());

		if (lv < 1)
			lv = 1;

		min_level = (lv > 15) ? (lv - 15) : 1;
		max_level = min_level + 30;

		if (max_level < lv)
			max_level = lv;

		if (lv > max_level)
			max_level = lv;

		if (max_level - min_level > 30)
			min_level = max_level - 30;

		UI::get().emplace<UISoftKey>([this](const std::string& range) {
			// SoftKey："min-max" 或单数字作 max
			if (range.empty()) return;
			int32_t a = 0, b = 0;
			size_t dash = range.find('-');
			if (dash != std::string::npos) {
				a = static_cast<int32_t>(std::strtol(range.c_str(), nullptr, 10));
				b = static_cast<int32_t>(std::strtol(range.c_str() + dash + 1, nullptr, 10));
			} else {
				b = static_cast<int32_t>(std::strtol(range.c_str(), nullptr, 10));
				a = (b > 15) ? b - 15 : 1;
			}
			if (a < 1) a = 1;
			if (b < a) b = a;
			if (b - a > 30) a = b - 30;
			min_level = a;
			max_level = b;
			status_label.change_text(Locale::format("partysearch.range", std::to_string(a) + "-" + std::to_string(b)));
			LOG(LOG_DEBUG, "UIPartySearch: SoftKey range=" << a << "-" << b);
		}, []() {}, "Lv min-max");
#endif
	}

	void UIPartySearch::set_searching(bool on)
	{
		searching = on;
		status_label.change_text(on
			? Locale::format("partysearch.searching", std::to_string(min_level) + "-" + std::to_string(max_level))
			: Locale::text("partysearch.idle"));
	}

	void UIPartySearch::start_search()
	{
		LOG(LOG_DEBUG, "UIPartySearch: START min=" << min_level << " max=" << max_level);
		// members 字段服务端忽略；jobs 全职业位掩码
		PartySearchStartPacket(min_level, max_level, 6, jobs_mask).dispatch();
		set_searching(true);
	}

	void UIPartySearch::stop_search()
	{
		if (!searching)
		{
			LOG(LOG_DEBUG, "UIPartySearch: STOP (not searching)");
			return;
		}

		LOG(LOG_DEBUG, "UIPartySearch: STOP");
		PartySearchStopPacket().dispatch();
		set_searching(false);
	}

	void UIPartySearch::toggle_active()
	{
		if (active)
		{
			stop_search();
			UIElement::deactivate();
		}
		else
		{
			UIElement::makeactive();
		}
	}

	void UIPartySearch::draw(float inter) const
	{
		if (!has_assets)
			return;

		if (backgrnd)
			backgrnd.draw(position);

		status_label.draw(position + Point<int16_t>(16, 48));
		UIElement::draw_buttons(inter);
	}

	void UIPartySearch::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape)
		{
			if (active)
				toggle_active();
			return;
		}
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's') {
			start_search();
		} else if (keycode == 'X' || keycode == 'x' || keycode == 'P' || keycode == 'p') {
			stop_search();
		} else if (keycode == 'R' || keycode == 'r') {
			UI::get().emplace<UISoftKey>([this](const std::string& range) {
				if (range.empty()) return;
				int32_t a = 0, b = 0;
				size_t dash = range.find('-');
				if (dash != std::string::npos) {
					a = static_cast<int32_t>(std::strtol(range.c_str(), nullptr, 10));
					b = static_cast<int32_t>(std::strtol(range.c_str() + dash + 1, nullptr, 10));
				} else {
					b = static_cast<int32_t>(std::strtol(range.c_str(), nullptr, 10));
					a = (b > 15) ? b - 15 : 1;
				}
				if (a < 1) a = 1;
				if (b < a) b = a;
				if (b - a > 30) a = b - 30;
				min_level = a;
				max_level = b;
				status_label.change_text(Locale::format("partysearch.range", std::to_string(a) + "-" + std::to_string(b)));
				LOG(LOG_DEBUG, "UIPartySearch: SoftKey range=" << a << "-" << b);
			}, []() {}, "Lv min-max");
		} else if (keycode == 'J' || keycode == 'j') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				char* end = nullptr;
				long v = std::strtol(tip.c_str(), &end, 0); // 支持 0x… / 十进制
				if (end == tip.c_str()) return;
				jobs_mask = static_cast<int32_t>(v);
				if (jobs_mask == 0)
					jobs_mask = 0x7FFFFFFF;
				status_label.change_text(Locale::format("partysearch.jobs_mask", std::to_string(jobs_mask)));
				LOG(LOG_DEBUG, "UIPartySearch: SoftKey jobs_mask=" << jobs_mask);
			}, []() {}, "Jobs mask");
		}
	}

	UIElement::Type UIPartySearch::get_type() const
	{
		return TYPE;
	}

	Button::State UIPartySearch::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtClose:
			case Buttons::BtCancel:
				if (active)
					toggle_active();
				break;
			case Buttons::BtStart:
				start_search();
				break;
			case Buttons::BtStop:
			case Buttons::BtPause:
				// s9 无独立 Pause；Pause ≡ Stop（UPDATE 注销）
				stop_search();
				break;
			case Buttons::BtReg:
				// PARTY_SEARCH_REGISTER(220) 在 s9 为空 stub → Start 代理（与 SoftKey S 一致）
				start_search();
				break;
			default:
				break;
		}

		return Button::State::NORMAL;
	}
}
