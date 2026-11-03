#include "UIMegaphone.h"
#include "UISoftKey.h"
#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif
namespace ms {
namespace {
bool find_cash_prefix(int32_t want_prefix, int16_t& slot, int32_t& itemid)
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 == want_prefix) { slot = i; itemid = id; return true; }
	}
	return false;
}
bool find_art_mega(int16_t& slot, int32_t& itemid)
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 != 507) continue;
		int32_t sub = (id / 1000) % 10;
		if (sub == 3 || sub == 7) { slot = i; itemid = id; return true; }
	}
	return find_cash_prefix(507, slot, itemid);
}
}
UIArtMegaphone::UIArtMegaphone() {
	position = Point<int16_t>(200, 160); dimension = Point<int16_t>(435, 182); dragarea = Point<int16_t>(435, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("ArtMegaphone");
	if (!root) { LOG(LOG_WARN, "UIArtMegaphone: missing"); return; }
	has_assets = true;
	if (root["backgrnd"]) {
		backgrnd = Texture(root["backgrnd"]);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
	}
	if (root["BtEx"]) buttons[BtEx] = std::make_unique<MapleButton>(root["BtEx"], Point<int16_t>(static_cast<int16_t>(dimension.x()-30), 8));
	if (root["BtPreView"]) buttons[BtPreView] = std::make_unique<MapleButton>(root["BtPreView"], Point<int16_t>(180, static_cast<int16_t>(dimension.y()-36)));
	center_on_screen();
#endif
}
void UIArtMegaphone::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	UIElement::draw_buttons(inter);
}
void UIArtMegaphone::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's')
		button_pressed(BtPreView);
}
UIElement::Type UIArtMegaphone::get_type() const { return TYPE; }
Button::State UIArtMegaphone::button_pressed(uint16_t id) {
	if (id == BtEx) deactivate();
	if (id == BtPreView) {
		UI::get().emplace<UISoftKey>([](const std::string& msg) {
			if (msg.empty()) return;
			int16_t slot = 0; int32_t itemid = 0;
			if (!find_art_mega(slot, itemid)) {
				LOG(LOG_DEBUG, "UIArtMegaphone: no 5073/5077");
				return;
			}
			UseMegaphonePacket(slot, itemid, msg, false).dispatch();
			LOG(LOG_DEBUG, "UIArtMegaphone: USE_CASH id=" << itemid);
		}, []() {}, "Art Megaphone");
	}
	return Button::State::NORMAL;
}

UIAvatarMegaphone::UIAvatarMegaphone()
	: tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "Avatar — SoftKey / Ok") {
	position = Point<int16_t>(300, 140); dimension = Point<int16_t>(196, 225); dragarea = Point<int16_t>(196, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("AvatarMegaphone");
	if (!root) { LOG(LOG_WARN, "UIAvatarMegaphone: missing"); return; }
	has_assets = true;
	if (root["backgrnd"]) {
		backgrnd = Texture(root["backgrnd"]);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
	}
	int16_t by = static_cast<int16_t>(dimension.y() - 36);
	if (root["BtOk"]) buttons[BtOk] = std::make_unique<MapleButton>(root["BtOk"], Point<int16_t>(30, by));
	if (root["BtCancel"]) buttons[BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(100, by));
	center_on_screen();
#endif
}
void UIAvatarMegaphone::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIAvatarMegaphone::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x()/2), 40));
	UIElement::draw_buttons(inter);
}
void UIAvatarMegaphone::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's' || keycode == 13)
		button_pressed(BtOk);
}
UIElement::Type UIAvatarMegaphone::get_type() const { return TYPE; }
Button::State UIAvatarMegaphone::button_pressed(uint16_t id) {
	if (id == BtCancel) { deactivate(); return Button::State::NORMAL; }
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) { tip_label.change_text(Locale::text("megaphone.empty")); return; }
		int16_t slot = 0; int32_t itemid = 0;
		if (!find_cash_prefix(539, slot, itemid)) {
			tip_label.change_text(Locale::text("megaphone.no_539"));
			return;
		}
		UseAvatarMegaphonePacket(slot, itemid, msg, false).dispatch();
		tip_label.change_text(Locale::text("megaphone.sent"));
		LOG(LOG_DEBUG, "UIAvatarMegaphone: USE_CASH id=" << itemid);
	}, []() {}, "Avatar Mega");
	return Button::State::NORMAL;
}
}
