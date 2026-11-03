//////////////////////////////////////////////////////////////////////////////////
//	Boss HP bar
//////////////////////////////////////////////////////////////////////////////////
#include "UIBossHP.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
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
		: label(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE)
	{
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
			// 填充条（原生 DualMobGauge 的 gauge 分件，1×9 横向拉伸）：
			// REA 反编译 sub_56C9B7 依次加载 backgrd/text/gauge 三分件
			// （docs/features/beidou-v17-parity/notes/evidence/r17/rea_decompile_DualMobGauge_56C9B7.txt）。
			// 修前 fill_w 算了却被 (void) 丢弃，血条只有背景+百分比文字、无填充。
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

	void UIBossHP::set_hp(int32_t id, int32_t cur, int32_t max, uint8_t tc, uint8_t tbg)
	{
		oid = id;
		cur_hp = cur;
		max_hp = max > 0 ? max : 1;
		tag_color = tc;
		tag_bg = tbg;
		visible = cur_hp > 0;
		hide_ms = visible ? 8000 : 0;
		int pct = static_cast<int>((static_cast<int64_t>(cur_hp) * 100) / max_hp);
		label.change_text(std::to_string(pct) + "%");
		LOG(LOG_DEBUG, "UIBossHP: oid=" << oid << " " << cur_hp << "/" << max_hp);
	}

	void UIBossHP::draw(float) const
	{
		if (!visible || hide_ms <= 0)
			return;

		if (backgrnd)
			backgrnd.draw(position);

		// 填充条：gauge 分件从 1px 横向拉伸到 fill_w（对齐原生填充比例）。
		if (fill)
		{
			float ratio = std::clamp(
				static_cast<float>(cur_hp) / static_cast<float>(max_hp), 0.f, 1.f);
			int16_t fill_w = static_cast<int16_t>(BAR_INNER_W * ratio);
			int16_t fh = fill.height() > 0 ? fill.height() : 9;
			// 填充条锚点：背景内左起，纵向对齐到条带。
			Point<int16_t> fillpos = position + Point<int16_t>(12, backgrnd ? (backgrnd.height() - fh) / 2 : 17);
			fill.draw(DrawArgument(fillpos, Point<int16_t>(fill_w, fh)));
		}

		Point<int16_t> mid = position + Point<int16_t>(
			backgrnd ? static_cast<int16_t>(backgrnd.width() / 2) : 340,
			backgrnd ? static_cast<int16_t>(backgrnd.height() / 2 - 6) : 10
		);
		label.draw(mid);
	}

	void UIBossHP::update()
	{
		if (hide_ms > 0) {
			hide_ms -= Constants::TIMESTEP;
			if (hide_ms <= 0) {
				hide_ms = 0;
				visible = false;
			}
		}
	}

	void UIBossHP::update_screen(int16_t new_width, int16_t)
	{
		int16_t bw = backgrnd ? backgrnd.width() : 681;
		position = Point<int16_t>(
			static_cast<int16_t>((new_width - bw) / 2),
			20
		);
		dimension = Point<int16_t>(bw, backgrnd ? backgrnd.height() : 43);
	}

	void UIBossHP::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { visible = false; hide_ms = 0; return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'H' || keycode == 'h')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					size_t c = tip.find(',');
					if (c != std::string::npos) {
						int32_t cur = std::stoi(tip.substr(0, c));
						int32_t maxv = std::stoi(tip.substr(c + 1));
						set_hp(oid > 0 ? oid : 1, cur, maxv, 0, 0);
						LOG(LOG_DEBUG, "UIBossHP: SoftKey hp=" << cur << "/" << maxv);
						return;
					}
					int pct = std::stoi(tip);
					if (pct < 0) pct = 0;
					if (pct > 100) pct = 100;
					set_hp(oid > 0 ? oid : 1, pct, 100, 0, 0);
					LOG(LOG_DEBUG, "UIBossHP: SoftKey pct=" << pct);
				} catch (...) {
					NpcTalkMorePacket(tip).dispatch();
					label.change_text(tip);
					LOG(LOG_DEBUG, "UIBossHP: SoftKey NpcTalkMore len=" << tip.size());
				}
			}, []() {}, "cur,max|pct|tip");
		}
		else if (keycode == 'V' || keycode == 'v')
		{
			visible = !visible;
			LOG(LOG_DEBUG, "UIBossHP: visible=" << visible);
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			hide_ms = 0;
			visible = false;
			label.change_text("");
		}
	}

	UIElement::Type UIBossHP::get_type() const
	{
		return TYPE;
	}
}
