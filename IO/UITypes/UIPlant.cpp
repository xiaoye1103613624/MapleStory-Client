//////////////////////////////////////////////////////////////////////////////////
//	plant / raise
//////////////////////////////////////////////////////////////////////////////////
#include "UIPlant.h"
#include "UIItemInventory.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	static bool try_use_selected_inv(Text& tip_label, const char* tag)
	{
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			int16_t slot = uii->get_selected_slot();
			auto tab = uii->get_tab();
			if (slot > 0 && (tab == InventoryType::Id::USE || tab == InventoryType::Id::ETC)) {
				int32_t itemid = Stage::get().get_player().get_inventory().get_item_id(tab, slot);
				if (itemid > 0) {
					UseItemPacket(slot, itemid).dispatch();
					tip_label.change_text(Locale::format("plant.use_slot", std::to_string(slot)));
					LOG(LOG_DEBUG, tag << ": UseItem from inv id=" << itemid << " slot=" << slot);
					return true;
				}
			}
		}
		return false;
	}

	static bool load_bg(const char* window, Texture& out, Point<int16_t>& dim)
	{
#ifdef USE_NX
		nl::node root = UiAssetResolve::window(window);
		if (!root)
			return false;
		nl::node bg = root["backgrnd"] ? root["backgrnd"] : root["backgrd"];
		if (!bg)
			bg = root["backgrndtop"];
		if (!bg && root["0"])
			bg = root["0"]["0"] ? root["0"]["0"] : root["0"]["backgrnd"];
		if (!bg && root["default"] && root["default"]["7"])
			bg = root["default"]["7"];
		if (!bg)
			return false;
		out = Texture(bg);
		Point<int16_t> o = out.get_origin();
		if (o.x() || o.y())
			out.shift(o);
		dim = out.get_dimensions();
		return true;
#else
		(void)window; (void)out; (void)dim;
		return false;
#endif
	}

	UIPlant::UIPlant()
		: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("plant.title"))
	{
		position = Point<int16_t>(300, 180);
		dimension = Point<int16_t>(200, 150);
		dragarea = Point<int16_t>(200, 20);
		has_assets = load_bg("plant", backgrnd, dimension);
		if (has_assets)
			dragarea = Point<int16_t>(dimension.x(), 20);
		else
			LOG(LOG_WARN, "UIPlant: missing");
		center_on_screen();
		if (!try_use_selected_inv(tip_label, "UIPlant")) {
			
		}
	}

	void UIPlant::set_tip(const std::string& tip) { tip_label.change_text(tip); }

	void UIPlant::draw(float) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		tip_label.draw(position + Point<int16_t>(12, 24));
	}

	void UIPlant::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'U' || keycode == 'u') {
			if (!try_use_selected_inv(tip_label, "UIPlant")) {
				
			}
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			auto ui = UI::get().get_element<UIRaise>();
			if (!ui) ui = UI::get().emplace<UIRaise>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'O' || keycode == 'o')
		{
			
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			WaterOfLifePacket().dispatch();
			tip_label.change_text(Locale::text("plant.water_of_life"));
			LOG(LOG_DEBUG, "UIPlant: WaterOfLife");
		}
		else if (keycode == 'X' || keycode == 'x')
		{
			deactivate();
		}
	}

	UIElement::Type UIPlant::get_type() const { return TYPE; }

	UIRaise::UIRaise()
		: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("raise.title"))
	{
		position = Point<int16_t>(300, 180);
		dimension = Point<int16_t>(200, 150);
		dragarea = Point<int16_t>(200, 20);
		has_assets = load_bg("raise", backgrnd, dimension);
		if (has_assets)
			dragarea = Point<int16_t>(dimension.x(), 20);
		else
			LOG(LOG_WARN, "UIRaise: missing");
		center_on_screen();
		if (!try_use_selected_inv(tip_label, "UIRaise")) {
			
		}
	}

	void UIRaise::set_tip(const std::string& tip) { tip_label.change_text(tip); }

	void UIRaise::draw(float) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		tip_label.draw(position + Point<int16_t>(12, 24));
	}

	void UIRaise::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'U' || keycode == 'u') {
			if (!try_use_selected_inv(tip_label, "UIRaise")) {
				
			}
		}
		else if (keycode == 'P' || keycode == 'p')
		{
			auto ui = UI::get().get_element<UIPlant>();
			if (!ui) ui = UI::get().emplace<UIPlant>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'O' || keycode == 'o')
		{
			
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			WaterOfLifePacket().dispatch();
			tip_label.change_text(Locale::text("plant.water_of_life"));
			LOG(LOG_DEBUG, "UIRaise: WaterOfLife");
		}
		else if (keycode == 'X' || keycode == 'x')
		{
			deactivate();
		}
	}

	UIElement::Type UIRaise::get_type() const { return TYPE; }
}
