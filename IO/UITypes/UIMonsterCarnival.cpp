//////////////////////////////////////////////////////////////////////////////////
//	MonsterCarnival HUD
//////////////////////////////////////////////////////////////////////////////////
#include "UIMonsterCarnival.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMonsterCarnival::UIMonsterCarnival()
		: cp_label(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::WHITE),
		  enemy_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::ORANGE),
		  msg_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW)
	{
		position = Point<int16_t>(20, 100);
		dimension = Point<int16_t>(162, 107);
		dragarea = Point<int16_t>(162, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("MonsterCarnival");
		if (!root) {
			LOG(LOG_WARN, "UIMonsterCarnival: missing");
			return;
		}
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		cp_label.change_text(Locale::text("carnival.cp_initial"));
		enemy_label.change_text(Locale::text("carnival.enemy_initial"));
		msg_label.change_text(Locale::text("carnival.title"));
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			size_t slash = tip.find('/');
			if (slash != std::string::npos) {
				try {
					int16_t avail = static_cast<int16_t>(std::stoi(tip.substr(0, slash)));
					int16_t total = static_cast<int16_t>(std::stoi(tip.substr(slash + 1)));
					set_cp(0, total, avail);
					LOG(LOG_DEBUG, "UIMonsterCarnival: SoftKey CP " << avail << "/" << total);
					return;
				} catch (...) {}
			}
			NpcTalkMorePacket(tip).dispatch();
			msg_label.change_text(tip);
			LOG(LOG_DEBUG, "UIMonsterCarnival: SoftKey NpcTalkMore len=" << tip.size());
		}, []() {}, "avail/total|tip");
		LOG(LOG_DEBUG, "UIMonsterCarnival: open");
#endif
	}

	void UIMonsterCarnival::set_cp(int16_t team, int16_t total, int16_t available)
	{
		if (team == 0 || team == 1)
			local_team = team;
		cp_label.change_text(
			Locale::format_all("carnival.cp", { std::to_string(available), std::to_string(total) })
		);
	}

	void UIMonsterCarnival::set_enemy_cp(int16_t total, int16_t available)
	{
		enemy_label.change_text(
			Locale::format_all("carnival.enemy", { std::to_string(available), std::to_string(total) })
		);
	}

	void UIMonsterCarnival::set_message(const std::string& msg)
	{
		msg_label.change_text(msg);
	}

	void UIMonsterCarnival::draw(float) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		cp_label.draw(position + Point<int16_t>(12, 28));
		enemy_label.draw(position + Point<int16_t>(12, 46));
		msg_label.draw(position + Point<int16_t>(12, 64));
	}

	void UIMonsterCarnival::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				size_t slash = tip.find('/');
				if (slash != std::string::npos) {
					try {
						int16_t avail = static_cast<int16_t>(std::stoi(tip.substr(0, slash)));
						int16_t total = static_cast<int16_t>(std::stoi(tip.substr(slash + 1)));
						set_cp(0, total, avail);
						return;
					} catch (...) {}
				}
				NpcTalkMorePacket(tip).dispatch();
				msg_label.change_text(tip);
			}, []() {}, "avail/total|tip");
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				size_t slash = tip.find('/');
				if (slash == std::string::npos) return;
				try {
					int16_t avail = static_cast<int16_t>(std::stoi(tip.substr(0, slash)));
					int16_t total = static_cast<int16_t>(std::stoi(tip.substr(slash + 1)));
					set_enemy_cp(total, avail);
				} catch (...) {}
			}, []() {}, "enemy avail/total");
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (!tip.empty())
					set_message(tip);
			}, []() {}, "CPQ msg");
		}
	}

	UIElement::Type UIMonsterCarnival::get_type() const { return TYPE; }
}
