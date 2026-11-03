//////////////////////////////////////////////////////////////////////////////////
//	Minigame lobby shell
//////////////////////////////////////////////////////////////////////////////////
#include "UIMinigame.h"

#include "UIMapleTV.h"
#include "UIMiscShells.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMinigame::UIMinigame()
		: tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "Minigame")
	{
		position = Point<int16_t>(280, 140);
		dimension = Point<int16_t>(246, 200);
		dragarea = Point<int16_t>(246, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Minigame");
		if (!root) {
			LOG(LOG_WARN, "UIMinigame: missing");
			return;
		}
		has_assets = true;
		nl::node common = root["Common"];
		if (common["draw"]) {
			info = Texture(common["draw"]);
			Point<int16_t> o = info.get_origin();
			if (o.x() || o.y()) info.shift(o);
			dimension = info.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		int16_t by = static_cast<int16_t>(dimension.y() - 36);
		if (common["btReady"])
			buttons[BtReady] = std::make_unique<MapleButton>(common["btReady"], Point<int16_t>(20, by));
		if (common["btStart"])
			buttons[BtStart] = std::make_unique<MapleButton>(common["btStart"], Point<int16_t>(90, by));
		if (common["btExit"])
			buttons[BtExit] = std::make_unique<MapleButton>(common["btExit"], Point<int16_t>(160, by));
		center_on_screen();
		LOG(LOG_DEBUG, "UIMinigame: shell open");
#endif
	}

	void UIMinigame::set_tip(const std::string& tip)
	{
		tip_label.change_text(tip);
	}

	void UIMinigame::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (info)
			info.draw(position);
		tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 24));
		UIElement::draw_buttons(inter);
	}

	void UIMinigame::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			MinigameExitPacket().dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'R' || keycode == 'r') {
			MinigameReadyPacket().dispatch();
			tip_label.change_text(Locale::text("minigame.ready"));
		} else if (keycode == 'S' || keycode == 's') {
			MinigameStartPacket().dispatch();
			tip_label.change_text(Locale::text("minigame.start"));
		} else if (keycode == 'X' || keycode == 'x') {
			MinigameExitPacket().dispatch();
			deactivate();
		} else if (keycode == 'T' || keycode == 't' || keycode == 'C' || keycode == 'c') {
			UI::get().emplace<UISoftKey>([this](const std::string& msg) {
				if (msg.empty()) return;
				TradeChatPacket(msg).dispatch();
				tip_label.change_text(msg);
				LOG(LOG_DEBUG, "UIMinigame: SoftKey TradeChat len=" << msg.size());
			}, []() {}, "Minigame chat");
		} else if (keycode == 'O' || keycode == 'o') {
			// SoftKey x,y[,piece] → Omok place
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				int vals[3] = { 0, 0, 1 };
				size_t start = 0; int vi = 0;
				while (vi < 3 && start <= s.size()) {
					size_t c = s.find(',', start);
					std::string part = (c == std::string::npos) ? s.substr(start) : s.substr(start, c - start);
					try { vals[vi] = std::stoi(part); } catch (...) {}
					++vi;
					if (c == std::string::npos) break;
					start = c + 1;
				}
				MinigameOmokPlacePacket(
					static_cast<uint8_t>(vals[0]),
					static_cast<uint8_t>(vals[1]),
					static_cast<uint8_t>(vals[2])
				).dispatch();
				tip_label.change_text(Locale::format("minigame.omok", s));
				LOG(LOG_DEBUG, "UIMinigame: OmokPlace " << s);
			}, []() {}, "x,y[,piece]");
		} else if (keycode == 'M' || keycode == 'm') {
			// SoftKey turn,slot → MatchCard select
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				size_t c = s.find(',');
				if (c == std::string::npos) return;
				try {
					uint8_t turn = static_cast<uint8_t>(std::stoi(s.substr(0, c)));
					uint8_t slot = static_cast<uint8_t>(std::stoi(s.substr(c + 1)));
					MinigameMatchCardSelectPacket(turn, slot).dispatch();
					tip_label.change_text(Locale::format("minigame.card", s));
					LOG(LOG_DEBUG, "UIMinigame: MatchCard " << s);
				} catch (...) {}
			}, []() {}, "turn,slot");
		} else if (keycode == 'L' || keycode == 'l') {
			auto ui = UI::get().get_element<UIMuruengRaid>();
			if (!ui) ui = UI::get().emplace<UIMuruengRaid>();
			else if (!ui->is_active()) ui->makeactive();
		} else if (keycode == 'P' || keycode == 'p') {
			auto ui = UI::get().get_element<UIClassMatch>();
			if (!ui) ui = UI::get().emplace<UIClassMatch>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UIMinigame::get_type() const { return TYPE; }

	Button::State UIMinigame::button_pressed(uint16_t id)
	{
		switch (id) {
			case BtReady:
				MinigameReadyPacket().dispatch();
				tip_label.change_text(Locale::text("minigame.ready"));
				break;
			case BtStart:
				MinigameStartPacket().dispatch();
				tip_label.change_text(Locale::text("minigame.start"));
				break;
			case BtExit:
				MinigameExitPacket().dispatch();
				deactivate();
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
