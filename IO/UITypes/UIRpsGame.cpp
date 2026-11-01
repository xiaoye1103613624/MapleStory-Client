//////////////////////////////////////////////////////////////////////////////////
//	RPS Game
//////////////////////////////////////////////////////////////////////////////////
#include "UIRpsGame.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/RpsPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIRpsGame::UIRpsGame()
		: status(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "Rock Paper Scissors")
	{
		position = Point<int16_t>(300, 140);
		dimension = Point<int16_t>(260, 200);
		dragarea = Point<int16_t>(260, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("RpsGame");
		if (!root) {
			LOG(LOG_WARN, "UIRpsGame: missing");
			return;
		}
		has_assets = true;
		nl::node bg = root["backgrnd"] ? root["backgrnd"] : root["backgrd"];
		if (bg) {
			backgrnd = Texture(bg);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		auto put = [&](Buttons id, const char* name, int16_t x, int16_t y) {
			if (root[name])
				buttons[id] = std::make_unique<MapleButton>(root[name], Point<int16_t>(x, y));
		};
		int16_t by = static_cast<int16_t>(dimension.y() - 40);
		put(BtRock, "BtRock", 20, by);
		put(BtPaper, "BtPaper", 80, by);
		put(BtScissor, "BtScissor", 140, by);
		put(BtStart, "BtStart", 20, 40);
		put(BtContinue, "BtContinue", 100, 40);
		put(BtRetry, "BtRetry", 180, 40);
		put(BtExit, "BtExit", static_cast<int16_t>(dimension.x() - 40), 8);
		center_on_screen();
		LOG(LOG_DEBUG, "UIRpsGame: open");
#endif
	}

	void UIRpsGame::set_status(const std::string& msg)
	{
		status.change_text(msg);
	}

	void UIRpsGame::set_round_result(uint8_t selection, uint8_t answer)
	{
		status.change_text(
			"You:" + std::to_string(selection) + " NPC:" + std::to_string(answer)
		);
	}

	void UIRpsGame::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		status.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 70));
		UIElement::draw_buttons(inter);
	}

	void UIRpsGame::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			RpsActionPacket(4).dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		// 1/R Rock · 2/P Paper · 3 Scissor · S Start · C Continue · Y Retry · X Exit
		if (keycode == '1' || keycode == 'R' || keycode == 'r') {
			RpsActionPacket(1, 0).dispatch();
			status.change_text("Rock");
		} else if (keycode == '2' || keycode == 'P' || keycode == 'p') {
			RpsActionPacket(1, 1).dispatch();
			status.change_text("Paper");
		} else if (keycode == '3') {
			RpsActionPacket(1, 2).dispatch();
			status.change_text("Scissor");
		} else if (keycode == 'S' || keycode == 's') {
			RpsActionPacket(0).dispatch();
			status.change_text("Started");
		} else if (keycode == 'C' || keycode == 'c') {
			RpsActionPacket(3).dispatch();
			status.change_text("Continue");
		} else if (keycode == 'Y' || keycode == 'y') {
			RpsActionPacket(5).dispatch();
			status.change_text("Retry");
		} else if (keycode == 'X' || keycode == 'x') {
			RpsActionPacket(4).dispatch();
			deactivate();
		} else if (keycode == 'T' || keycode == 't') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "rock" || t == "r" || t == "0") {
					RpsActionPacket(1, 0).dispatch();
					status.change_text("Rock");
				} else if (t == "paper" || t == "p" || t == "1") {
					RpsActionPacket(1, 1).dispatch();
					status.change_text("Paper");
				} else if (t == "scissor" || t == "s" || t == "2") {
					RpsActionPacket(1, 2).dispatch();
					status.change_text("Scissor");
				} else if (t == "start") {
					RpsActionPacket(0).dispatch();
					status.change_text("Started");
				} else if (t == "exit" || t == "x") {
					RpsActionPacket(4).dispatch();
					deactivate();
				}
			}, []() {}, "rock|paper|scissor");
		}
	}

	UIElement::Type UIRpsGame::get_type() const { return TYPE; }

	Button::State UIRpsGame::button_pressed(uint16_t id)
	{
		switch (id) {
			case BtStart:
				RpsActionPacket(0).dispatch();
				status.change_text("Started");
				break;
			case BtRetry:
				RpsActionPacket(5).dispatch();
				status.change_text("Retry");
				break;
			case BtRock:
				RpsActionPacket(1, 0).dispatch();
				break;
			case BtPaper:
				RpsActionPacket(1, 1).dispatch();
				break;
			case BtScissor:
				RpsActionPacket(1, 2).dispatch();
				break;
			case BtContinue:
				RpsActionPacket(3).dispatch();
				break;
			case BtExit:
				RpsActionPacket(4).dispatch();
				deactivate();
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
