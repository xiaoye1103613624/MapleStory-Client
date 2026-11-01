//////////////////////////////////////////////////////////////////////////////////
//	AriantMatch
//////////////////////////////////////////////////////////////////////////////////
#include "UIAriantMatch.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAriantMatch::UIAriantMatch()
		: score_label(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE)
	{
		position = Point<int16_t>(250, 80);
		dimension = Point<int16_t>(320, 100);
		dragarea = Point<int16_t>(320, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("AriantMatch");
		if (!root) {
			LOG(LOG_WARN, "UIAriantMatch: missing");
			return;
		}
		has_assets = true;
		show_result_frame(0);
		score_label.change_text("Ariant");
		center_on_screen();
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			try {
				int idx = std::stoi(tip);
				show_result_frame(idx);
				score_label.change_text("Result " + tip);
				LOG(LOG_DEBUG, "UIAriantMatch: SoftKey result=" << idx);
				return;
			} catch (...) {}
			NpcTalkMorePacket(tip).dispatch();
			score_label.change_text(tip);
			LOG(LOG_DEBUG, "UIAriantMatch: SoftKey NpcTalkMore len=" << tip.size());
		}, []() {}, "result idx|tip");
		LOG(LOG_DEBUG, "UIAriantMatch: open");
#endif
	}

	void UIAriantMatch::show_result_frame(int index)
	{
#ifdef USE_NX
		nl::node res = UiAssetResolve::window("AriantMatch")["Result"];
		if (!res) return;
		nl::node frame = res[std::to_string(index)];
		if (!frame) frame = res["0"];
		if (frame) {
			result_tex = Texture(frame);
			Point<int16_t> o = result_tex.get_origin();
			if (o.x() || o.y()) result_tex.shift(o);
			dimension = result_tex.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
#else
		(void)index;
#endif
	}

	void UIAriantMatch::set_score(const std::string& line)
	{
		score_label.change_text(line);
	}

	void UIAriantMatch::draw(float) const
	{
		if (!has_assets)
			return;
		if (result_tex)
			result_tex.draw(position);
		score_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 20));
	}

	void UIAriantMatch::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int idx = std::stoi(tip);
					show_result_frame(idx);
					score_label.change_text("Result " + tip);
					return;
				} catch (...) {}
				NpcTalkMorePacket(tip).dispatch();
				score_label.change_text(tip);
			}, []() {}, "result idx|tip");
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			show_result_frame(0);
			score_label.change_text("Result 0");
		}
		else if (keycode == 'N' || keycode == 'n')
		{
			show_result_frame(1);
			score_label.change_text("Result 1");
		}
	}

	UIElement::Type UIAriantMatch::get_type() const { return TYPE; }
}
