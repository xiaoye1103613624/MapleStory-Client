//////////////////////////////////////////////////////////////////////////////////
//	VegaSpell — drop equip (EQUIP) + scroll (USE) + cash spell
//////////////////////////////////////////////////////////////////////////////////
#include "UIVegaSpell.h"
#include "UISoftKey.h"
#include "UIItemInventory.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		constexpr int32_t VEGA_IDS[] = { 5610000, 5610001 };
	}

	UIVegaSpell::UIVegaSpell() : has_assets(false),
		equip_slot(0), equip_itemid(0), scroll_slot(0), scroll_itemid(0),
		cash_slot(0), cash_itemid(0),
		status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "Drop equip + scroll")
	{
		position = Point<int16_t>(320, 120);
		dimension = Point<int16_t>(178, 235);
		dragarea = Point<int16_t>(178, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("VegaSpell");
		if (!root) {
			LOG(LOG_WARN, "UIVegaSpell: missing");
			return;
		}
		has_assets = true;
		nl::node bg = root["backgrnd10"] ? root["backgrnd10"] : root["backgrnd60"];
		if (bg) {
			backgrnd = Texture(bg);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y())
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		int16_t by = static_cast<int16_t>(dimension.y() - 36);
		if (root["BtStart"])
			buttons[Buttons::BtStart] = std::make_unique<MapleButton>(root["BtStart"], Point<int16_t>(12, by));
		if (root["BtOK"])
			buttons[Buttons::BtOK] = std::make_unique<MapleButton>(root["BtOK"], Point<int16_t>(70, by));
		if (root["BtCancel"])
			buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(120, by));
		LOG(LOG_DEBUG, "UIVegaSpell: open");
#endif
	}

	void UIVegaSpell::set_result(uint8_t op)
	{
		switch (op) {
			case 0x40: status.change_text("Scrolling…"); break;
			case 0x41: status.change_text("Success!"); break;
			case 0x43: status.change_text("Failed."); break;
			default: status.change_text("Result op=0x" + std::to_string(op)); break;
		}
	}

	bool UIVegaSpell::send_icon(const Icon& icon, Point<int16_t>)
	{
		auto tab = icon.get_inventory_tab();
		if (tab == InventoryType::Id::EQUIP) {
			equip_slot = icon.get_inventory_slot();
			equip_itemid = icon.get_item_id();
			equip_icon = ItemData::get(equip_itemid).get_icon(false);
			status.change_text("Equip @" + std::to_string(equip_slot));
		} else if (tab == InventoryType::Id::USE) {
			scroll_slot = icon.get_inventory_slot();
			scroll_itemid = icon.get_item_id();
			scroll_icon = ItemData::get(scroll_itemid).get_icon(false);
			status.change_text("Scroll @" + std::to_string(scroll_slot));
		} else if (tab == InventoryType::Id::CASH) {
			cash_slot = icon.get_inventory_slot();
			cash_itemid = icon.get_item_id();
			status.change_text("Vega @" + std::to_string(cash_slot));
		} else {
			status.change_text("Drop equip / scroll / vega");
		}
		return true;
	}

	void UIVegaSpell::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		status.draw(position + Point<int16_t>(12, 28));
		if (equip_icon)
			equip_icon.draw(position + Point<int16_t>(30, 90));
		if (scroll_icon)
			scroll_icon.draw(position + Point<int16_t>(90, 90));
		UIElement::draw_buttons(inter);
	}

	void UIVegaSpell::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 13) { button_pressed(Buttons::BtStart); return; }
		if (keycode == 'E' || keycode == 'e') {
			int16_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::EQUIP)
					from_inv = inv->get_selected_slot();
			}
			if (from_inv > 0) {
				equip_slot = from_inv;
				equip_itemid = Stage::get().get_player().get_inventory()
					.get_item_id(InventoryType::Id::EQUIP, from_inv);
				equip_icon = ItemData::get(equip_itemid).get_icon(false);
				status.change_text("Equip #" + std::to_string(from_inv));
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int16_t slot = static_cast<int16_t>(std::stoi(s));
						if (slot > 0) {
							equip_slot = slot;
							equip_itemid = Stage::get().get_player().get_inventory()
								.get_item_id(InventoryType::Id::EQUIP, slot);
							equip_icon = ItemData::get(equip_itemid).get_icon(false);
							status.change_text("Equip #" + s);
						}
					} catch (...) {}
				}, []() {}, "Equip slot");
			}
		} else if (keycode == 'S' || keycode == 's') {
			int16_t from_inv = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::USE)
					from_inv = inv->get_selected_slot();
			}
			if (from_inv > 0) {
				scroll_slot = from_inv;
				scroll_itemid = Stage::get().get_player().get_inventory()
					.get_item_id(InventoryType::Id::USE, from_inv);
				scroll_icon = ItemData::get(scroll_itemid).get_icon(false);
				status.change_text("Scroll #" + std::to_string(from_inv));
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int16_t slot = static_cast<int16_t>(std::stoi(s));
						if (slot > 0) {
							scroll_slot = slot;
							scroll_itemid = Stage::get().get_player().get_inventory()
								.get_item_id(InventoryType::Id::USE, slot);
							scroll_icon = ItemData::get(scroll_itemid).get_icon(false);
							status.change_text("Scroll #" + s);
						}
					} catch (...) {}
				}, []() {}, "Scroll slot");
			}
		} else if (keycode == 'G' || keycode == 'g') {
			button_pressed(Buttons::BtStart);
		} else if (keycode == 'X' || keycode == 'x') {
			deactivate();
		}
	}

	UIElement::Type UIVegaSpell::get_type() const { return TYPE; }

	Button::State UIVegaSpell::button_pressed(uint16_t buttonid)
	{
		switch (buttonid) {
			case Buttons::BtCancel:
			case Buttons::BtOK:
				deactivate();
				break;
			case Buttons::BtStart: {
				if (equip_slot <= 0 || scroll_slot <= 0) {
					status.change_text("Need equip + scroll");
					break;
				}
				if (cash_slot <= 0) {
					const Inventory& inv = Stage::get().get_player().get_inventory();
					for (int32_t id : VEGA_IDS) {
						int16_t s = inv.find_item(InventoryType::Id::CASH, id);
						if (s > 0) {
							cash_slot = s;
							cash_itemid = id;
							break;
						}
					}
				}
				if (cash_slot <= 0) {
					status.change_text("No Vega Spell in Cash");
					break;
				}
				LOG(LOG_DEBUG, "UIVegaSpell: USE_CASH_ITEM eq=" << equip_slot
					<< " use=" << scroll_slot << " cash=" << cash_itemid);
				UseVegaSpellPacket(cash_slot, cash_itemid, equip_slot, scroll_slot).dispatch();
				status.change_text("Vega sent…");
				break;
			}
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
