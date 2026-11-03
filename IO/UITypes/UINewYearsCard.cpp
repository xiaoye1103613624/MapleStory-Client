//////////////////////////////////////////////////////////////////////////////////
//	NewYearsCard — SoftKey 收件人/留言 → NEW_YEAR_CARD_REQUEST
//////////////////////////////////////////////////////////////////////////////////
#include "UINewYearsCard.h"
#include "UICharInfo.h"
#include "UIMemo.h"
#include "UISoftKey.h"
#include "UIWedding.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/NotePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UINewYearsCard::UINewYearsCard()
		: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "New Year — SoftKey to/msg")
	{
		position = Point<int16_t>(220, 160);
		dimension = Point<int16_t>(357, 188);
		dragarea = Point<int16_t>(357, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("NewYearsCard");
		if (!root) {
			LOG(LOG_WARN, "UINewYearsCard: missing");
			return;
		}
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		int16_t by = static_cast<int16_t>(dimension.y() - 36);
		if (root["BtOk"])
			buttons[BtOk] = std::make_unique<MapleButton>(root["BtOk"], Point<int16_t>(80, by));
		if (root["BtCancel"])
			buttons[BtCancel] = std::make_unique<MapleButton>(root["BtCancel"], Point<int16_t>(160, by));
		if (root["BtClose"])
			buttons[BtClose] = std::make_unique<MapleButton>(root["BtClose"], Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 5));
		center_on_screen();
		begin_compose();
		LOG(LOG_DEBUG, "UINewYearsCard: open");
#endif
	}

	bool UINewYearsCard::find_card(int16_t& slot, int32_t& itemid) const
	{
		const Inventory& inv = Stage::get().get_player().get_inventory();
		for (int32_t id : { 2160101, 2160100, 2160000 }) {
			int16_t s = inv.find_item(InventoryType::Id::USE, id);
			if (s <= 0) s = inv.find_item(InventoryType::Id::ETC, id);
			if (s <= 0) s = inv.find_item(InventoryType::Id::CASH, id);
			if (s > 0) { slot = s; itemid = id; return true; }
		}
		return false;
	}

	void UINewYearsCard::begin_compose()
	{
		auto ask_msg = [this](const std::string& to) {
			if (to.empty()) { tip_label.change_text(Locale::text("nycard.need_receiver")); return; }
			pending_receiver = to;
			tip_label.change_text(Locale::format("nycard.to", to));
			UI::get().emplace<UISoftKey>([this](const std::string& msg) {
				int16_t slot = 0; int32_t itemid = 0;
				if (!find_card(slot, itemid)) {
					tip_label.change_text(Locale::text("nycard.no_card"));
					return;
				}
				NewYearCardSendPacket(slot, itemid, pending_receiver, msg).dispatch();
				tip_label.change_text(Locale::format("nycard.sent_to", pending_receiver));
			}, []() {}, "NY message");
		};
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty())
			ask_msg(pre);
		else
			UI::get().emplace<UISoftKey>(ask_msg, []() {}, "NY receiver");
	}

	void UINewYearsCard::set_tip(const std::string& tip)
	{
		tip_label.change_text(tip);
	}

	void UINewYearsCard::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		tip_label.draw(position + Point<int16_t>(16, 40));
		UIElement::draw_buttons(inter);
	}

	void UINewYearsCard::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's' || keycode == 13 || keycode == 'N' || keycode == 'n')
			begin_compose();
		else if (keycode == 'M' || keycode == 'm')
		{
			auto ui = UI::get().get_element<UIMemo>();
			if (!ui) ui = UI::get().emplace<UIMemo>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'W' || keycode == 'w')
		{
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui) ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UINewYearsCard::get_type() const { return TYPE; }

	Button::State UINewYearsCard::button_pressed(uint16_t id)
	{
		if (id == BtOk)
			begin_compose();
		else if (id == BtCancel || id == BtClose)
			deactivate();
		return Button::State::NORMAL;
	}
}
