//////////////////////////////////////////////////////////////////////////////////
//	Boss HP bar（P1 多 oid）
//////////////////////////////////////////////////////////////////////////////////
#include "UIBossHP.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/HudLayout.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include <algorithm>
#include <string>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIBossHP::UIBossHP()
	{
		set_layout_edge(LayoutEdge::Top, Point<int16_t>(0, 20));

		int16_t w = Constants::Constants::get().get_viewwidth();
		int16_t h = Constants::Constants::get().get_viewheight();
		update_screen(w, h);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("DualMobGauge");
		if (!root)
			root = UiAssetResolve::window("MobGage");
		if (root) {
			nl::node bg = root["backgrd"] ? root["backgrd"] : root["backgrnd"];
			if (bg) {
				backgrnd = Texture(bg);
				Point<int16_t> o = backgrnd.get_origin();
				if (o.x() || o.y())
					backgrnd.shift(o);
				has_assets = true;
			}
			nl::node f = root["gauge"];
			if (f) {
				fill = Texture(f);
				Point<int16_t> fo = fill.get_origin();
				if (fo.x() || fo.y())
					fill.shift(fo);
			}
			nl::node t = root["text"];
			if (t) {
				textlay = Texture(t);
				Point<int16_t> to = textlay.get_origin();
				if (to.x() || to.y())
					textlay.shift(to);
			}
		}
		if (!has_assets)
			LOG(LOG_WARN, "UIBossHP: no DualMobGauge/MobGage backgrnd");
#endif
	}

	int16_t UIBossHP::bar_height() const
	{
		return backgrnd ? backgrnd.height() : 43;
	}

	void UIBossHP::touch_order(int32_t oid)
	{
		order.erase(std::remove(order.begin(), order.end(), oid), order.end());
		order.push_back(oid);
		while (order.size() > MAX_BARS)
		{
			int32_t drop = order.front();
			order.erase(order.begin());
			bars.erase(drop);
		}
	}

	void UIBossHP::set_hp(int32_t id, int32_t cur, int32_t max, uint8_t tc, uint8_t tbg)
	{
		if (id == 0)
			id = 1;

		if (cur <= 0)
		{
			bars.erase(id);
			order.erase(std::remove(order.begin(), order.end(), id), order.end());
			LOG(LOG_DEBUG, "UIBossHP: remove oid=" << id);
			return;
		}

		Entry& e = bars[id];
		if (e.label.empty())
			e.label = Text(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE);

		e.cur_hp = cur;
		e.max_hp = max > 0 ? max : 1;
		e.tag_color = tc;
		e.tag_bg = tbg;
		e.hide_ms = 8000;
		const int pct = static_cast<int>((static_cast<int64_t>(e.cur_hp) * 100) / e.max_hp);
		e.label.change_text(std::to_string(pct) + "%");
		touch_order(id);
		LOG(LOG_DEBUG, "UIBossHP: oid=" << id << " " << e.cur_hp << "/" << e.max_hp
			<< " bars=" << bars.size());
	}

	void UIBossHP::draw(float) const
	{
		if (order.empty())
			return;

		const int16_t bh = bar_height();
		int16_t stack = 0;

		// 自上而下叠画：先出现的在上，新 oid 追加在下（多段 Boss 同时可见）。
		for (int32_t oid : order)
		{
			auto it = bars.find(oid);
			if (it == bars.end() || it->second.hide_ms <= 0)
				continue;

			const Entry& e = it->second;
			Point<int16_t> pos = position + Point<int16_t>(0, stack);

			if (backgrnd)
				backgrnd.draw(pos);

			if (fill)
			{
				float ratio = std::clamp(
					static_cast<float>(e.cur_hp) / static_cast<float>(e.max_hp), 0.f, 1.f);
				int16_t fill_w = static_cast<int16_t>(BAR_INNER_W * ratio);
				int16_t fh = fill.height() > 0 ? fill.height() : 9;
				Point<int16_t> fillpos = pos + Point<int16_t>(12, backgrnd ? (bh - fh) / 2 : 17);
				fill.draw(DrawArgument(fillpos, Point<int16_t>(fill_w, fh)));
			}

			Point<int16_t> mid = pos + Point<int16_t>(
				backgrnd ? static_cast<int16_t>(backgrnd.width() / 2) : 340,
				backgrnd ? static_cast<int16_t>(bh / 2 - 6) : 10
			);
			e.label.draw(mid);

			stack = static_cast<int16_t>(stack + bh + STACK_GAP);
		}
	}

	void UIBossHP::update()
	{
		for (auto it = bars.begin(); it != bars.end(); )
		{
			Entry& e = it->second;
			if (e.hide_ms > 0)
			{
				e.hide_ms -= Constants::TIMESTEP;
				if (e.hide_ms <= 0)
				{
					int32_t oid = it->first;
					it = bars.erase(it);
					order.erase(std::remove(order.begin(), order.end(), oid), order.end());
					continue;
				}
			}
			++it;
		}
	}

	void UIBossHP::update_screen(int16_t new_width, int16_t)
	{
		int16_t bw = backgrnd ? backgrnd.width() : 681;
		const int16_t n = static_cast<int16_t>(std::min(order.size(), MAX_BARS));
		const int16_t bh = bar_height();
		const int16_t total_h = n > 0
			? static_cast<int16_t>(n * bh + (n - 1) * STACK_GAP)
			: bh;
		dimension = Point<int16_t>(bw, total_h);
		position = Point<int16_t>(
			static_cast<int16_t>((new_width - bw) / 2),
			HudLayout::edge_top(0, layout_margin.y()));
	}

	void UIBossHP::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { bars.clear(); order.clear(); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'H' || keycode == 'h')
		{
			
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			bars.clear();
			order.clear();
		}
	}

	UIElement::Type UIBossHP::get_type() const
	{
		return TYPE;
	}
}
