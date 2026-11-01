//////////////////////////////////////////////////////////////////////////////////
//	EnergyBar HUD
//////////////////////////////////////////////////////////////////////////////////
#include "UIEnergyBar.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Character/Buff.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/AttackAndSkillPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Constants.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIEnergyBar::UIEnergyBar()
	{
		int16_t w = Constants::Constants::get().get_viewwidth();
		int16_t h = Constants::Constants::get().get_viewheight();
		update_screen(w, h);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("EnergyBar");
		if (!root) {
			LOG(LOG_WARN, "UIEnergyBar: missing EnergyBar assets");
			return;
		}
		has_assets = true;
		if (root["w"]) bar_w = Texture(root["w"]);
		if (root["c"]) bar_c = Texture(root["c"]);
		if (root["e"]) bar_e = Texture(root["e"]);
		if (root["Gage"]["1"])
			gage = Texture(root["Gage"]["1"]);
		else if (root["Gage"]["0"])
			gage = Texture(root["Gage"]["0"]);
#endif
	}

	void UIEnergyBar::draw(float) const
	{
		if (!has_assets || !visible || energy <= 0)
			return;

		Point<int16_t> pos = position;
		if (bar_w)
			bar_w.draw(pos);
		int16_t fill = static_cast<int16_t>(
			(static_cast<int32_t>(energy) * BAR_INNER_W) / MAX_ENERGY);
		if (fill < 0) fill = 0;
		if (fill > BAR_INNER_W) fill = BAR_INNER_W;

		Point<int16_t> mid = pos + Point<int16_t>(bar_w ? bar_w.width() : 6, 0);
		if (bar_c && fill > 0) {
			for (int16_t i = 0; i < fill; i++)
				bar_c.draw(mid + Point<int16_t>(i, 0));
		}
		if (bar_e)
			bar_e.draw(mid + Point<int16_t>(fill, 0));
		if (gage)
			gage.draw(pos);
	}

	void UIEnergyBar::update()
	{
		const auto& player = Stage::get().get_player();
		visible = player.has_buff(Buffstat::Id::ENERGY_CHARGE);
		energy = visible ? player.get_buff_value(Buffstat::Id::ENERGY_CHARGE) : 0;
		if (energy > MAX_ENERGY)
			energy = MAX_ENERGY;
	}

	void UIEnergyBar::update_screen(int16_t new_width, int16_t new_height)
	{
		// Above classic status bar HP/MP area (bottom-center-left)
		position = Point<int16_t>(
			static_cast<int16_t>(new_width / 2 - 120),
			static_cast<int16_t>(new_height - 90)
		);
		dimension = Point<int16_t>(90, 14);
	}

	void UIEnergyBar::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { visible = false; return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
			|| keycode == 'U' || keycode == 'u')
		{
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int32_t id = std::stoi(tip);
					if (id <= 0) return;
					int32_t lv = 1;
					try {
						lv = Stage::get().get_player().get_skills().get_level(id);
						if (lv <= 0) lv = 1;
					} catch (...) {}
					UseSkillPacket(id, lv).dispatch();
					LOG(LOG_DEBUG, "UIEnergyBar: SoftKey UseSkill id=" << id);
				} catch (...) {}
			}, []() {}, "Energy skillid");
		}
		else if (keycode == 'C' || keycode == 'c' || keycode == 'X' || keycode == 'x')
		{
			deactivate();
		}
	}

	UIElement::Type UIEnergyBar::get_type() const
	{
		return TYPE;
	}
}
