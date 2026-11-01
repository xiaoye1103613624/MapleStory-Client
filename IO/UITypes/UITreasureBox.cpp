//////////////////////////////////////////////////////////////////////////////////
//	TreasureBox
//////////////////////////////////////////////////////////////////////////////////
#include "UITreasureBox.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UITreasureBox::UITreasureBox()
	{
		position = Point<int16_t>(320, 180);
		dimension = Point<int16_t>(170, 172);
		dragarea = Point<int16_t>(170, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("TreasureBox");
		if (!root) {
			LOG(LOG_WARN, "UITreasureBox: missing");
			return;
		}
		has_assets = true;
		nl::node box = root["0"] ? root["0"] : root["1"];
		if (box["backgrndClosed"])
			closed_tex = Texture(box["backgrndClosed"]);
		else if (box["backgrnd"])
			closed_tex = Texture(box["backgrnd"]);
		if (box["backgrndOpened"])
			opened_tex = Texture(box["backgrndOpened"]);
		if (closed_tex) {
			Point<int16_t> o = closed_tex.get_origin();
			if (o.x() || o.y()) closed_tex.shift(o);
			dimension = closed_tex.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		center_on_screen();
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			try {
				int32_t itemid = std::stoi(tip);
				if (itemid > 0) {
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int16_t slot = inv.find_item(InventoryType::Id::USE, itemid);
					if (slot <= 0) slot = inv.find_item(InventoryType::Id::ETC, itemid);
					if (slot > 0) {
						UseItemPacket(slot, itemid).dispatch();
						opened = true;
						LOG(LOG_DEBUG, "UITreasureBox: UseItem id=" << itemid);
						return;
					}
				}
			} catch (...) {}
			opened = true;
			NpcTalkMorePacket(tip).dispatch();
			LOG(LOG_DEBUG, "UITreasureBox: SoftKey NpcTalkMore len=" << tip.size());
		}, []() {}, "Treasure itemid");
		LOG(LOG_DEBUG, "UITreasureBox: open");
#endif
	}

	void UITreasureBox::set_opened(bool o)
	{
		opened = o;
	}

	void UITreasureBox::draw(float) const
	{
		if (!has_assets)
			return;
		if (opened && opened_tex)
			opened_tex.draw(position);
		else if (closed_tex)
			closed_tex.draw(position);
	}

	void UITreasureBox::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 13 || keycode == 'O' || keycode == 'o') {
			opened = !opened;
			LOG(LOG_DEBUG, "UITreasureBox: opened=" << opened);
		} else if (keycode == 'C' || keycode == 'c') {
			opened = false;
		} else if (keycode == 'U' || keycode == 'u') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				try {
					int32_t itemid = std::stoi(tip);
					if (itemid <= 0) return;
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int16_t slot = inv.find_item(InventoryType::Id::USE, itemid);
					if (slot <= 0) slot = inv.find_item(InventoryType::Id::ETC, itemid);
					if (slot > 0) {
						UseItemPacket(slot, itemid).dispatch();
						opened = true;
						LOG(LOG_DEBUG, "UITreasureBox: SoftKey UseItem id=" << itemid);
					}
				} catch (...) {}
			}, []() {}, "Treasure itemid");
		}
	}

	Cursor::State UITreasureBox::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement<PosNOTICE>::send_cursor(clicked, cursorpos);
		if (clicked && active)
		{
			Rectangle<int16_t> hit{ position, position + dimension };
			if (hit.contains(cursorpos))
			{
				opened = !opened;
				return Cursor::State::CANCLICK;
			}
		}
		return st;
	}

	UIElement::Type UITreasureBox::get_type() const { return TYPE; }
}
