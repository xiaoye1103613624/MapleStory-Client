//////////////////////////////////////////////////////////////////////////////////
//	MTS / ITC list shell
//////////////////////////////////////////////////////////////////////////////////
#include "UIMTS.h"

#include "UISoftKey.h"

#include "../UI.h"
#include "../../Constants.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Net/Packets/OwlPackets.h"
#include "../../Util/Misc.h"

#include <string>
#include <utility>
#include <vector>
#include "../../Util/Locale.h"

namespace ms
{
	UIMTS::UIMTS()
		: title(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::YELLOW),
		  cash_line(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE),
		  list_line(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE),
		  items_line(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE),
		  status_line(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::ORANGE)
	{
		title.change_text(Locale::text("mts.title"));
		int16_t w = Constants::Constants::get().get_viewwidth();
		int16_t h = Constants::Constants::get().get_viewheight();
		update_screen(w, h);
	}

	void UIMTS::set_list(int32_t t, int32_t ty, int32_t p, int32_t n, int32_t s)
	{
		tab = t;
		type = ty;
		page = p;
		count = n;
		slots = s;
		visible = true;
		list_line.change_text(
			Locale::format_all("mts.list_line",
				{ std::to_string(tab), std::to_string(type), std::to_string(page),
					std::to_string(count), std::to_string(slots) })
		);
		makeactive();
		LOG(LOG_DEBUG, "UIMTS list " << list_line.get_text());
	}

	void UIMTS::set_cash(int32_t n, int32_t m)
	{
		nx = n;
		maple = m;
		visible = true;
		cash_line.change_text(Locale::format_all("mts.cash_line", { std::to_string(nx), std::to_string(maple) }));
		makeactive();
		LOG(LOG_DEBUG, "UIMTS cash " << cash_line.get_text());
	}

	void UIMTS::set_status(const std::string& tip)
	{
		visible = true;
		status_line.change_text(tip.empty() ? Locale::text("mts.status_default") : tip);
		makeactive();
	}

	void UIMTS::set_items(const std::vector<std::pair<int32_t, int32_t>>& items)
	{
		visible = true;
		std::string line;
		for (size_t i = 0; i < items.size() && i < 4; ++i)
		{
			if (!line.empty())
				line += " | ";
			line += Locale::format_all("mts.item_slot", { std::to_string(items[i].first), std::to_string(items[i].second) });
		}
		if (items.size() > 4)
			line += " " + Locale::format("mts.more", std::to_string(items.size() - 4));
		items_line.change_text(line.empty() ? Locale::text("mts.no_items") : line);
		makeactive();
		LOG(LOG_DEBUG, "UIMTS items " << items_line.get_text());
	}

	void UIMTS::draw(float) const
	{
		if (!visible || !active)
			return;
		title.draw(position);
		cash_line.draw(position + Point<int16_t>(0, 18));
		list_line.draw(position + Point<int16_t>(0, 36));
		items_line.draw(position + Point<int16_t>(0, 54));
		status_line.draw(position + Point<int16_t>(0, 72));
	}

	void UIMTS::update()
	{
	}

	void UIMTS::update_screen(int16_t new_width, int16_t new_height)
	{
		position = Point<int16_t>(
			static_cast<int16_t>(new_width / 2 - 140),
			static_cast<int16_t>(new_height / 2 - 60)
		);
		dimension = Point<int16_t>(320, 96);
		(void)new_height;
	}

	void UIMTS::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (!pressed) return;
		if (escape) { visible = false; deactivate(); return; }
		if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				set_status(tip);
				NpcTalkMorePacket(tip).dispatch();
			}, []() {}, "MTS|tip");
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			set_status("MTS refresh");
			LOG(LOG_DEBUG, "UIMTS: SoftKey refresh status");
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			visible = false;
			deactivate();
		}
		else if (keycode == 'I' || keycode == 'i')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					size_t c = s.find(',');
					int32_t id = std::stoi(c == std::string::npos ? s : s.substr(0, c));
					int32_t price = c == std::string::npos ? 0 : std::stoi(s.substr(c + 1));
					if (id <= 0) return;
					std::vector<std::pair<int32_t, int32_t>> one = { { id, price } };
					set_items(one);
					set_status("Item " + std::to_string(id) + " @" + std::to_string(price));
				} catch (...) {}
			}, []() {}, "itemid[,price]");
		}
		else if (keycode == 'E' || keycode == 'e')
		{
			EnterMtsPacket().dispatch();
			set_status("Enter MTS");
			LOG(LOG_DEBUG, "UIMTS: EnterMTS");
		}
		else if (keycode == 'O' || keycode == 'o')
		{
			UseOwlPacket().dispatch();
			set_status("Owl open");
			LOG(LOG_DEBUG, "UIMTS: UseOwl");
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					size_t c = s.find(',');
					if (c == std::string::npos) return;
					int32_t owner = std::stoi(s.substr(0, c));
					int32_t mapid = std::stoi(s.substr(c + 1));
					if (owner <= 0 || mapid <= 0) return;
					OwlWarpPacket(owner, mapid).dispatch();
					set_status("OwlWarp " + std::to_string(owner) + "@" + std::to_string(mapid));
					LOG(LOG_DEBUG, "UIMTS: OwlWarp owner=" << owner << " map=" << mapid);
				} catch (...) {}
			}, []() {}, "ownerid,mapid");
		}
	}

	UIElement::Type UIMTS::get_type() const
	{
		return TYPE;
	}
}
