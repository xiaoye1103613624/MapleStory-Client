#include "UICashGachapon.h"
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
bool find_gacha_ticket(int16_t& slot, int32_t& itemid)
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 == 522) { slot = i; itemid = id; return true; }
	}
	return false;
}
}
UICashGachapon::UICashGachapon() : has_assets(false),
	status(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE)
{
	position = Point<int16_t>(340, 160); dimension = Point<int16_t>(170, 172); dragarea = Point<int16_t>(170, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("CashGachapon");
	if (!root) { LOG(LOG_WARN, "UICashGachapon: missing"); return; }
	has_assets = true;
	if (root["backgrnd"]) {
		backgrnd = Texture(root["backgrnd"]);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions(); dragarea = Point<int16_t>(dimension.x(), 20);
	}
	if (root["BtClose"]) buttons[BtClose] = std::make_unique<MapleButton>(root["BtClose"], Point<int16_t>(static_cast<int16_t>(dimension.x()-21), 5));
	if (root["BtOpen"]) buttons[BtOpen] = std::make_unique<MapleButton>(root["BtOpen"], Point<int16_t>(50, static_cast<int16_t>(dimension.y()-36)));
	LOG(LOG_DEBUG, "UICashGachapon: shell open");
#endif
}
void UICashGachapon::set_result(int32_t reward_id, int8_t qty, int32_t remaining, bool jackpot)
{
	status.change_text(Locale::format_all(jackpot ? "gachapon.jackpot" : "gachapon.reward",
		{ std::to_string(reward_id), std::to_string(qty), std::to_string(remaining) }));
}
void UICashGachapon::set_failed()
{
	status.change_text(Locale::text("gachapon.failed"));
}
void UICashGachapon::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	UIElement::draw_buttons(inter);
	if (status.empty()) return;
	status.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2),
		static_cast<int16_t>(dimension.y() - 52)));
}
void UICashGachapon::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'O' || keycode == 'o' || keycode == 13) {
		button_pressed(BtOpen);
	} else if (keycode == 'X' || keycode == 'x' || keycode == 'C' || keycode == 'c') {
		deactivate();
	}
}
UIElement::Type UICashGachapon::get_type() const { return TYPE; }
Button::State UICashGachapon::button_pressed(uint16_t id) {
	if (id == BtClose) deactivate();
	else if (id == BtOpen) {
		UI::get().emplace<UISoftKey>([](const std::string& tip) {
			(void)tip;
			int16_t slot = 0; int32_t itemid = 0;
			if (!find_gacha_ticket(slot, itemid)) {
				LOG(LOG_DEBUG, "UICashGachapon: no 522* ticket");
				return;
			}
			UseCashItemBasicPacket(slot, itemid).dispatch();
			LOG(LOG_DEBUG, "UICashGachapon: USE_CASH basic id=" << itemid);
		}, []() {}, "Gacha OK");
	}
	return Button::State::NORMAL;
}
}
