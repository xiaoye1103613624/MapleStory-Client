#include "UIRemoteGachapon.h"
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
bool find_remote_gacha(int16_t& slot, int32_t& itemid)
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		// 545* 远程 / 522* 扭蛋票均可触发浅出站
		if (id / 10000 == 545 || id / 10000 == 522) { slot = i; itemid = id; return true; }
	}
	return false;
}
}
UIRemoteGachapon::UIRemoteGachapon() : has_assets(false),
	status(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE)
{
	position = Point<int16_t>(300, 120); dimension = Point<int16_t>(246, 235); dragarea = Point<int16_t>(246, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("RemoteGachapon");
	if (!root) { LOG(LOG_WARN, "UIRemoteGachapon: missing"); return; }
	has_assets = true;
	if (root["backgrnd"]) {
		backgrnd = Texture(root["backgrnd"]);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions(); dragarea = Point<int16_t>(dimension.x(), 20);
	}
	nl::node close = UiAssetResolve::close_btn();
	if (close) buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(static_cast<int16_t>(dimension.x()-21), 5));
	for (int i = 0; i < 10; i++) {
		nl::node b = root["Bt" + std::to_string(i)];
		if (b)
			buttons[static_cast<uint16_t>(Bt0 + i)] = std::make_unique<MapleButton>(b, Point<int16_t>(static_cast<int16_t>(16 + (i % 5) * 44), static_cast<int16_t>(50 + (i / 5) * 44)));
	}
	LOG(LOG_DEBUG, "UIRemoteGachapon: shell open");
#endif
}
void UIRemoteGachapon::set_result(int8_t mode)
{
	status.change_text(Locale::format("remotegacha.box_mode", std::to_string(mode)));
}
void UIRemoteGachapon::draw(float inter) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	UIElement::draw_buttons(inter);
	if (status.empty()) return;
	status.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2),
		static_cast<int16_t>(dimension.y() - 28)));
}
void UIRemoteGachapon::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	// SoftKey town 0-9 → UseCashItemBasic（545/522）
	if (keycode >= '0' && keycode <= '9') {
		uint16_t town = static_cast<uint16_t>(keycode - '0');
		button_pressed(static_cast<uint16_t>(Bt0 + town));
	} else if (keycode == 'G' || keycode == 'g') {
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			int town = 0;
			try { town = std::stoi(s); } catch (...) { return; }
			if (town < 0 || town > 9) return;
			int16_t slot = 0; int32_t itemid = 0;
			if (!find_remote_gacha(slot, itemid)) return;
			UseCashItemBasicPacket(slot, itemid).dispatch();
			LOG(LOG_DEBUG, "UIRemoteGachapon: SoftKey town=" << town << " id=" << itemid);
		}, []() {}, "Town 0-9");
	} else if (keycode == 'X' || keycode == 'x' || keycode == 'C' || keycode == 'c') {
		deactivate();
	}
}
UIElement::Type UIRemoteGachapon::get_type() const { return TYPE; }
Button::State UIRemoteGachapon::button_pressed(uint16_t id) {
	if (id == BtClose) { deactivate(); return Button::State::NORMAL; }
	uint16_t town = (id >= Bt0) ? static_cast<uint16_t>(id - Bt0) : 0;
	int16_t slot = 0; int32_t itemid = 0;
	if (!find_remote_gacha(slot, itemid)) {
		LOG(LOG_DEBUG, "UIRemoteGachapon: no 545/522 cash");
		return Button::State::NORMAL;
	}
	UseCashItemBasicPacket(slot, itemid).dispatch();
	LOG(LOG_DEBUG, "UIRemoteGachapon: town=" << town << " USE_CASH id=" << itemid);
	return Button::State::NORMAL;
}
}
