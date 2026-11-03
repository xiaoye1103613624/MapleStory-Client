//////////////////////////////////////////////////////////////////////////////////
//	PartyRace HUD
//////////////////////////////////////////////////////////////////////////////////
#include "UIPartyRace.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIPartyRace::UIPartyRace()
		: stage_label(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE),
		  score_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW)
	{
		position = Point<int16_t>(20, 80);
		dimension = Point<int16_t>(153, 65);
		dragarea = Point<int16_t>(153, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("PartyRace");
		if (!root) {
			LOG(LOG_WARN, "UIPartyRace: missing");
			return;
		}
		has_assets = true;
		if (root["Stage"]["backgrd"]) {
			stage_bg = Texture(root["Stage"]["backgrd"]);
			Point<int16_t> o = stage_bg.get_origin();
			if (o.x() || o.y()) stage_bg.shift(o);
			dimension = stage_bg.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		if (root["Result"]["backgrd"])
			result_bg = Texture(root["Result"]["backgrd"]);
		if (root["Result"]["win"])
			win_tex = Texture(root["Result"]["win"]);
		if (root["Result"]["lose"])
			lose_tex = Texture(root["Result"]["lose"]);

		stage_label.change_text(Locale::text("partyrace.stage_initial"));
		score_label.change_text(Locale::text("partyrace.score_initial"));
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			std::string t = tip;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t == "win") { show_result(true); return; }
			if (t == "lose") { show_result(false); return; }
			size_t colon = tip.find(':');
			if (colon != std::string::npos) {
				try {
					set_score(std::stoi(tip.substr(0, colon)), std::stoi(tip.substr(colon + 1)));
					LOG(LOG_DEBUG, "UIPartyRace: SoftKey score " << tip);
					return;
				} catch (...) {}
			}
			try {
				set_stage(std::stoi(tip));
				return;
			} catch (...) {}
			NpcTalkMorePacket(tip).dispatch();
			score_label.change_text(tip);
			LOG(LOG_DEBUG, "UIPartyRace: SoftKey NpcTalkMore len=" << tip.size());
		}, []() {}, "r:b|win|stage|tip");
		LOG(LOG_DEBUG, "UIPartyRace: shell open");
#endif
	}

	void UIPartyRace::set_stage(int32_t s)
	{
		stage = s;
		stage_label.change_text(Locale::format("partyrace.stage", std::to_string(stage)));
	}

	void UIPartyRace::set_score(int32_t r, int32_t b)
	{
		red = r;
		blue = b;
		score_label.change_text(Locale::format_all("partyrace.score", { std::to_string(red), std::to_string(blue) }));
	}

	void UIPartyRace::show_result(bool win)
	{
		show_result_panel = true;
		last_win = win;
	}

	void UIPartyRace::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (stage_bg)
			stage_bg.draw(position);
		stage_label.draw(position + Point<int16_t>(12, 18));
		score_label.draw(position + Point<int16_t>(12, 38));
		if (show_result_panel && result_bg) {
			Point<int16_t> rp = position + Point<int16_t>(0, static_cast<int16_t>(dimension.y() + 8));
			result_bg.draw(rp);
			if (last_win && win_tex)
				win_tex.draw(rp + Point<int16_t>(40, 80));
			else if (!last_win && lose_tex)
				lose_tex.draw(rp + Point<int16_t>(40, 80));
		}
	}

	void UIPartyRace::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "win") { show_result(true); return; }
				if (t == "lose") { show_result(false); return; }
				size_t colon = tip.find(':');
				if (colon != std::string::npos) {
					try {
						set_score(std::stoi(tip.substr(0, colon)), std::stoi(tip.substr(colon + 1)));
						return;
					} catch (...) {}
				}
				try {
					set_stage(std::stoi(tip));
				} catch (...) {
					NpcTalkMorePacket(tip).dispatch();
					score_label.change_text(tip);
				}
			}, []() {}, "r:b|win|stage|tip");
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			show_result(true);
		}
		else if (keycode == 'L' || keycode == 'l')
		{
			show_result(false);
		}
	}

	UIElement::Type UIPartyRace::get_type() const { return TYPE; }

	Button::State UIPartyRace::button_pressed(uint16_t)
	{
		return Button::State::NORMAL;
	}
}
