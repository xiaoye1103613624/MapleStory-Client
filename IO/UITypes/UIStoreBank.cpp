//////////////////////////////////////////////////////////////////////////////////
//	StoreBank / Fredrick — 列表 + GetAll / Exit
//////////////////////////////////////////////////////////////////////////////////
#include "UIStoreBank.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Data/ItemData.h"
#include "../../Net/Packets/FredrickPackets.h"
#include "../../Net/Packets/MiscOutPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIStoreBank::UIStoreBank() : has_assets(false), mesos(0),
		mesolabel(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE),
		notice(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW)
	{
		position = Point<int16_t>(300, 100);
		dimension = Point<int16_t>(232, 318);
		dragarea = Point<int16_t>(232, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("StoreBank");

		if (!root)
		{
			LOG(LOG_WARN, "UIStoreBank: UIWindow.img/StoreBank missing");
			return;
		}

		has_assets = true;

		if (root["backgrnd"])
		{
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() != 0 || o.y() != 0)
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}

		if (root["en"])
			en = Texture(root["en"]);

		if (root["BtExit"])
			buttons[Buttons::BtExit] = std::make_unique<MapleButton>(root["BtExit"], Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 5));
		if (root["BtGetAll"])
			buttons[Buttons::BtGetAll] = std::make_unique<MapleButton>(root["BtGetAll"], Point<int16_t>(70, static_cast<int16_t>(dimension.y() - 40)));

		mesolabel.change_text(Locale::format("storebank.mesos", "0"));
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			std::string t = tip;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t == "all" || t == "get" || t == "getall") {
				FredrickGetAllPacket().dispatch();
				show_notice(Locale::text("storebank.getall_sent"));
				LOG(LOG_DEBUG, "UIStoreBank: SoftKey FredrickGetAll");
				return;
			}
			if (t == "exit" || t == "x" || t == "close") {
				send_exit();
				LOG(LOG_DEBUG, "UIStoreBank: SoftKey FredrickExit");
				return;
			}
			if (!tip.empty())
				show_notice(tip);
			LOG(LOG_DEBUG, "UIStoreBank: SoftKey tip len=" << tip.size());
		}, []() {}, "all|exit|tip");
		LOG(LOG_DEBUG, "UIStoreBank: open");
#endif
	}

	void UIStoreBank::load(int32_t m, std::vector<FredrickItem> list)
	{
		mesos = m;
		items = std::move(list);
		mesolabel.change_text(Locale::format("storebank.mesos", std::to_string(mesos)));
		notice.change_text("");
		LOG(LOG_DEBUG, "UIStoreBank: load mesos=" << mesos << " items=" << items.size());
	}

	void UIStoreBank::show_notice(const std::string& msg)
	{
		notice.change_text(msg);
	}

	void UIStoreBank::clear_and_close()
	{
		items.clear();
		mesos = 0;
		mesolabel.change_text(Locale::format("storebank.mesos", "0"));
		send_exit();
	}

	void UIStoreBank::send_exit()
	{
		FredrickExitPacket().dispatch();
		deactivate();
	}

	void UIStoreBank::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		if (en)
			en.draw(position + Point<int16_t>(16, 40));

		mesolabel.draw(position + Point<int16_t>(20, 48));

		constexpr int16_t CELL = 32;
		constexpr int16_t COLS = 4;
		Point<int16_t> grid = position + Point<int16_t>(20, 72);
		for (size_t i = 0; i < items.size(); i++)
		{
			int16_t col = static_cast<int16_t>(i % COLS);
			int16_t row = static_cast<int16_t>(i / COLS);
			Point<int16_t> pos = grid + Point<int16_t>(static_cast<int16_t>(col * CELL), static_cast<int16_t>(row * CELL));
			const Texture& icon = ItemData::get(items[i].itemid).get_icon(false);
			if (icon)
				icon.draw(pos);
			if (items[i].quantity > 1)
			{
				Text qty(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::WHITE);
				qty.change_text(std::to_string(items[i].quantity));
				qty.draw(pos + Point<int16_t>(CELL - 4, CELL - 14));
			}
		}

		if (!notice.get_text().empty())
			notice.draw(position + Point<int16_t>(16, static_cast<int16_t>(dimension.y() - 58)));

		UIElement::draw_buttons(inter);
	}

	void UIStoreBank::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			send_exit();
			return;
		}
		if (!pressed) return;
		if (keycode == 'G' || keycode == 'g' || keycode == 'A' || keycode == 'a') {
			FredrickGetAllPacket().dispatch();
			show_notice(Locale::text("storebank.getall_sent"));
			LOG(LOG_DEBUG, "UIStoreBank: GetAll");
		} else if (keycode == 'S' || keycode == 's') {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "all" || t == "get" || t == "getall") {
					FredrickGetAllPacket().dispatch();
					show_notice(Locale::text("storebank.getall_sent"));
					return;
				}
				if (t == "exit" || t == "x" || t == "close") {
					send_exit();
					return;
				}
				if (!tip.empty())
					show_notice(tip);
			}, []() {}, "all|exit|tip");
		} else if (keycode == 'R' || keycode == 'r') {
			RemoteStorePacket().dispatch();
			show_notice("RemoteStore");
			LOG(LOG_DEBUG, "UIStoreBank: RemoteStore");
		} else if (keycode == 'X' || keycode == 'x') {
			send_exit();
		}
	}

	UIElement::Type UIStoreBank::get_type() const { return TYPE; }

	Button::State UIStoreBank::button_pressed(uint16_t buttonid)
	{
		switch (buttonid)
		{
			case Buttons::BtExit:
				send_exit();
				break;
			case Buttons::BtGetAll:
				LOG(LOG_DEBUG, "UIStoreBank: BtGetAll");
				FredrickGetAllPacket().dispatch();
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
