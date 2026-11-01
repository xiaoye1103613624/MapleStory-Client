#include "UIMapleTV.h"
#include "UICharInfo.h"
#include "UISoftKey.h"
#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif
namespace ms {
UIMapleTV::UIMapleTV() : tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "MapleTV") {
	position = Point<int16_t>(280, 160); dimension = Point<int16_t>(280, 180); dragarea = Point<int16_t>(280, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("MapleTV");
	if (!root) { LOG(LOG_WARN, "UIMapleTV: missing"); return; }
	has_assets = true;
	nl::node bg = root["backgrnd"] ? root["backgrnd"] : root["backgrd"];
	if (bg) {
		backgrnd = Texture(bg);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
	}
	int16_t by = static_cast<int16_t>(dimension.y() - 36);
	if (root["BtOk"]) buttons[BtOk] = std::make_unique<MapleButton>(root["BtOk"], Point<int16_t>(60, by));
	if (root["BtCancel"]) buttons[BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(140, by));
	center_on_screen();
#endif
}
void UIMapleTV::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIMapleTV::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x()/2), 40));
	UIElement::draw_buttons(inter);
}
void UIMapleTV::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 13)
		button_pressed(BtOk);
	else if (keycode == 'C' || keycode == 'c')
	{
		auto ui = UI::get().get_element<UIClassMatch>();
		if (!ui) ui = UI::get().emplace<UIClassMatch>();
		else if (!ui->is_active()) ui->makeactive();
	}
}
UIElement::Type UIMapleTV::get_type() const { return TYPE; }
Button::State UIMapleTV::button_pressed(uint16_t id)
{
	if (id == BtOk) {
		UI::get().emplace<UISoftKey>([this](const std::string& msg) {
			if (msg.empty()) return;
			tip_label.change_text(msg);
			const Inventory& inv = Stage::get().get_player().get_inventory();
			int16_t cash_slot = 0;
			int32_t cash_id = 0;
			int16_t max = inv.get_slotmax(InventoryType::Id::CASH);
			for (int16_t s = 1; s <= max; ++s) {
				int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
				if (id / 10000 == 512) { cash_slot = s; cash_id = id; break; }
			}
			if (cash_slot > 0) {
				UseMapEffectPacket(cash_slot, cash_id, msg).dispatch();
				LOG(LOG_DEBUG, "UIMapleTV: UseMapEffect id=" << cash_id);
			} else {
				tip_label.change_text("No 512* map effect");
			}
		}, []() {}, "MapleTV msg");
		return Button::State::NORMAL;
	}
	deactivate();
	return Button::State::NORMAL;
}

UIAdminClaim::UIAdminClaim() {
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(220, 160); dragarea = Point<int16_t>(220, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("AdminClaim");
	if (!root) { LOG(LOG_WARN, "UIAdminClaim: missing"); return; }
	has_assets = true;
	nl::node bg = root["backgrnd"] ? root["backgrnd"] : root["backgrd"];
	if (bg) {
		backgrnd = Texture(bg);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
	}
	int16_t by = static_cast<int16_t>(dimension.y() - 36);
	if (root["BtClaim"]) buttons[BtClaim] = std::make_unique<MapleButton>(root["BtClaim"], Point<int16_t>(40, by));
	if (root["BtCancel"]) buttons[BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(120, by));
	center_on_screen();
#endif
}
void UIAdminClaim::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	UIElement::draw_buttons(inter);
}
void UIAdminClaim::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's')
		button_pressed(BtClaim);
}
UIElement::Type UIAdminClaim::get_type() const { return TYPE; }
Button::State UIAdminClaim::button_pressed(uint16_t id)
{
	if (id == BtClaim) {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto ask_desc = [](const std::string& victim) {
			UI::get().emplace<UISoftKey>([victim](const std::string& desc) {
				if (desc.empty()) return;
				ReportPlayerPacket(victim, desc).dispatch();
				LOG(LOG_DEBUG, "UIAdminClaim: Report " << victim);
			}, []() {}, "Claim desc");
		};
		if (!pre.empty()) {
			ask_desc(pre);
		} else {
			UI::get().emplace<UISoftKey>([ask_desc](const std::string& victim) {
				if (victim.empty()) return;
				ask_desc(victim);
			}, []() {}, "Claim victim");
		}
		return Button::State::NORMAL;
	}
	deactivate();
	return Button::State::NORMAL;
}

UIClassMatch::UIClassMatch() {
	position = Point<int16_t>(240, 60); dimension = Point<int16_t>(312, 389); dragarea = Point<int16_t>(312, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("classMatch");
	if (!root) { LOG(LOG_WARN, "UIClassMatch: missing"); return; }
	has_assets = true;
	if (root["backgrnd"]) {
		backgrnd = Texture(root["backgrnd"]);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
	}
	if (root["BtOK"])
		buttons[BtOK] = std::make_unique<MapleButton>(root["BtOK"], Point<int16_t>(120, static_cast<int16_t>(dimension.y()-40)));
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& tip) {
		if (tip.empty()) return;
		NpcTalkMorePacket(tip).dispatch();
		LOG(LOG_DEBUG, "UIClassMatch: SoftKey NpcTalkMore len=" << tip.size());
	}, []() {}, "ClassMatch");
#endif
}
void UIClassMatch::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	UIElement::draw_buttons(inter);
}
void UIClassMatch::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& tip) {
			if (tip.empty()) return;
			NpcTalkMorePacket(tip).dispatch();
		}, []() {}, "ClassMatch");
	}
}
UIElement::Type UIClassMatch::get_type() const { return TYPE; }
Button::State UIClassMatch::button_pressed(uint16_t) { deactivate(); return Button::State::NORMAL; }
}
