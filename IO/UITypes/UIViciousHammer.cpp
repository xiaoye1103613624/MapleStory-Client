//////////////////////////////////////////////////////////////////////////////////
//	ViciousHammer — drop equip → BtStart → USE_CASH_ITEM
//////////////////////////////////////////////////////////////////////////////////
#include "UIViciousHammer.h"
#include "UISoftKey.h"
#include "UIItemInventory.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Gameplay/Stage.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/HammerPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// Prefer common vicious hammer cash ids; find first owned.
		constexpr int32_t HAMMER_IDS[] = { 5570000, 5570001 };
	}

	UIViciousHammer::UIViciousHammer() : has_assets(false), hammer_used(0),
		equip_slot(0), equip_itemid(0),
		status(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("hammer.hint_drop"))
	{
		position = Point<int16_t>(320, 140);
		dimension = Point<int16_t>(178, 206);
		dragarea = Point<int16_t>(178, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("ViciousHammer");
		if (!root) {
			LOG(LOG_WARN, "UIViciousHammer: missing");
			return;
		}
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y())
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		int16_t by = static_cast<int16_t>(dimension.y() - 36);
		if (root["BtStart"])
			buttons[Buttons::BtStart] = std::make_unique<MapleButton>(root["BtStart"], Point<int16_t>(20, by));
		if (root["BtCancel"])
			buttons[Buttons::BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(100, by));
		LOG(LOG_DEBUG, "UIViciousHammer: open");
#endif
	}

	Rectangle<int16_t> UIViciousHammer::drop_area() const
	{
		return {
			position + Point<int16_t>(40, 60),
			position + Point<int16_t>(40 + 48, 60 + 48)
		};
	}

	void UIViciousHammer::set_status(const std::string& msg)
	{
		status.change_text(msg);
	}

	void UIViciousHammer::set_hammer_used(int32_t used)
	{
		hammer_used = used;
		status.change_text(Locale::format("hammer.used", std::to_string(used)));
	}

	bool UIViciousHammer::send_icon(const Icon& icon, Point<int16_t> cursor)
	{
		Rectangle<int16_t> win{ position, position + dimension };
		if (!drop_area().contains(cursor) && !win.contains(cursor))
			return true;

		if (icon.get_inventory_tab() != InventoryType::Id::EQUIP) {
			status.change_text(Locale::text("hammer.need_equip_tab"));
			return true;
		}

		equip_slot = icon.get_inventory_slot();
		equip_itemid = icon.get_item_id();
		equip_icon = ItemData::get(equip_itemid).get_icon(false);
		status.change_text(Locale::format("hammer.equip_slot", std::to_string(equip_slot)));
		LOG(LOG_DEBUG, "UIViciousHammer: equip slot=" << equip_slot << " id=" << equip_itemid);
		return true;
	}

	void UIViciousHammer::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		status.draw(position + Point<int16_t>(12, 28));
		if (equip_icon)
			equip_icon.draw(position + Point<int16_t>(50, 100));
		UIElement::draw_buttons(inter);
	}

	void UIViciousHammer::send_key(int32_t keycode, bool pressed, bool escape)
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
				status.change_text(Locale::format("hammer.equip_num", std::to_string(from_inv)));
				LOG(LOG_DEBUG, "UIViciousHammer: SoftKey equip slot=" << from_inv);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& s) {
					try {
						int16_t slot = static_cast<int16_t>(std::stoi(s));
						if (slot > 0) {
							equip_slot = slot;
							equip_itemid = Stage::get().get_player().get_inventory()
								.get_item_id(InventoryType::Id::EQUIP, slot);
							status.change_text(Locale::format("hammer.equip_num", s));
							LOG(LOG_DEBUG, "UIViciousHammer: SoftKey equip slot=" << slot);
						}
					} catch (...) {}
				}, []() {}, Locale::text("hammer.softkey_equip_slot"));
			}
		} else if (keycode == 'G' || keycode == 'g' || keycode == 'S' || keycode == 's') {
			button_pressed(Buttons::BtStart);
		} else if (keycode == 'X' || keycode == 'x') {
			deactivate();
		}
	}

	UIElement::Type UIViciousHammer::get_type() const { return TYPE; }

	Button::State UIViciousHammer::button_pressed(uint16_t buttonid)
	{
		switch (buttonid) {
			case Buttons::BtCancel:
				deactivate();
				break;
			case Buttons::BtStart: {
				if (equip_slot <= 0) {
					status.change_text(Locale::text("hammer.drop_first"));
					UseHammerPacket().dispatch();
					break;
				}
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int16_t cash_slot = 0;
				int32_t cash_id = 0;
				for (int32_t id : HAMMER_IDS) {
					int16_t s = inv.find_item(InventoryType::Id::CASH, id);
					if (s > 0) {
						cash_slot = s;
						cash_id = id;
						break;
					}
				}
				if (cash_slot <= 0) {
					status.change_text(Locale::text("hammer.no_hammer_cash"));
					break;
				}
				LOG(LOG_DEBUG, "UIViciousHammer: USE_CASH_ITEM hammer=" << cash_id
					<< " cashSlot=" << cash_slot << " eq=" << equip_slot);
				UseViciousHammerPacket(cash_slot, cash_id, equip_slot).dispatch();
				status.change_text(Locale::text("hammer.sent"));
				break;
			}
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
