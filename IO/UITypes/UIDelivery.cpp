//////////////////////////////////////////////////////////////////////////////////
//	Delivery / Duey UI
//////////////////////////////////////////////////////////////////////////////////
#include "UIDelivery.h"
#include "UISoftKey.h"
#include "UICharInfo.h"
#include "UIItemInventory.h"
#include "UINotice.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../Components/TwoSpriteButton.h"
#include "../../Net/Packets/DueyPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIDelivery::UIDelivery() :
		has_assets(false), tab(RECV), selected(-1),
		notice(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "")
	{
		position = Point<int16_t>(280, 80);
		dimension = Point<int16_t>(290, 348);
		dragarea = Point<int16_t>(290, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Delivery");
		if (!root) { LOG(LOG_WARN, "UIDelivery: missing"); return; }
		has_assets = true;

		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		if (root["sendTB"]) send_tb = Texture(root["sendTB"]);
		if (root["receiveTB"]) recv_tb = Texture(root["receiveTB"]);

		nl::node close = UiAssetResolve::close_btn();
		if (close)
			buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 5));

		nl::node tabdis = root["Tab"]["disabled"];
		nl::node taben = root["Tab"]["enabled"];
		if (tabdis["0"] && taben["0"])
			buttons[TabSend] = std::make_unique<TwoSpriteButton>(tabdis["0"], taben["0"], Point<int16_t>(10, 24));
		if (tabdis["1"] && taben["1"])
			buttons[TabRecv] = std::make_unique<TwoSpriteButton>(tabdis["1"], taben["1"], Point<int16_t>(72, 24));
		if (tabdis["2"] && taben["2"])
			buttons[TabTax] = std::make_unique<TwoSpriteButton>(tabdis["2"], taben["2"], Point<int16_t>(110, 24));

		DueyOpenRecvPacket().dispatch();
		LOG(LOG_DEBUG, "UIDelivery: open + refresh recv list");
#endif
	}

	void UIDelivery::set_tab(uint8_t t) { tab = t; }

	void UIDelivery::set_packages(std::vector<DueyPackage> pkgs)
	{
		packages = std::move(pkgs);
		selected = packages.empty() ? -1 : 0;
		row_labels.clear();
		for (const auto& p : packages) {
			std::string line = Locale::format_all("delivery.row_meso",
				{ p.sender, std::to_string(p.mesos) });
			if (p.itemid)
				line += "  " + Locale::format("delivery.row_item", std::to_string(p.itemid));
			row_labels.emplace_back(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, line);
		}
		tab = RECV;
		notice.change_text(packages.empty() ? Locale::text("delivery.no_packages")
			: Locale::format("delivery.count", std::to_string(packages.size())));
	}

	void UIDelivery::remove_package(int32_t package_id)
	{
		for (size_t i = 0; i < packages.size(); i++) {
			if (packages[i].id == package_id) {
				packages.erase(packages.begin() + static_cast<std::ptrdiff_t>(i));
				if (i < row_labels.size())
					row_labels.erase(row_labels.begin() + static_cast<std::ptrdiff_t>(i));
				break;
			}
		}
		if (selected >= static_cast<int16_t>(packages.size()))
			selected = static_cast<int16_t>(packages.size()) - 1;
		notice.change_text(Locale::text("delivery.updated"));
	}

	void UIDelivery::show_notice(const std::string& msg)
	{
		notice.change_text(msg);
		LOG(LOG_DEBUG, "UIDelivery: " << msg);
	}

	int16_t UIDelivery::row_at(Point<int16_t> cursorpos) const
	{
		Point<int16_t> rel = cursorpos - position;
		int16_t y0 = 60;
		int16_t row = (rel.y() - y0) / 18;
		if (rel.x() < 10 || rel.x() > 280 || row < 0 || row >= static_cast<int16_t>(packages.size()))
			return -1;
		return row;
	}

	void UIDelivery::claim_selected()
	{
		if (selected < 0 || selected >= static_cast<int16_t>(packages.size()))
			return;
		DueyClaimPacket(packages[static_cast<size_t>(selected)].id).dispatch();
		show_notice("Claiming…");
	}

	void UIDelivery::discard_selected()
	{
		if (selected < 0 || selected >= static_cast<int16_t>(packages.size()))
			return;
		DueyDiscardPacket(packages[static_cast<size_t>(selected)].id).dispatch();
		show_notice("Discarding…");
	}

	void UIDelivery::prompt_send()
	{
		int8_t invent = 0;
		int16_t slot = 0;
		int16_t qty = 1;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			slot = inv->get_selected_slot();
			if (slot > 0)
				invent = static_cast<int8_t>(inv->get_tab());
		}
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto ask_meso = [this, invent, slot, qty](const std::string& name) {
			if (name.empty()) return;
			UI::get().emplace<UISoftKey>([this, name, invent, slot, qty](const std::string& meso_s) {
				int32_t mesos = 0;
				try { mesos = std::stoi(meso_s); } catch (...) { mesos = 0; }
				int16_t amount = (invent > 0 && slot > 0) ? qty : 0;
				DueySendPacket(invent, slot, amount, mesos, name, false, "").dispatch();
				show_notice(Locale::format("delivery.sending_to", name));
				LOG(LOG_DEBUG, "UIDelivery: Send name=" << name << " inv=" << (int)invent
					<< " slot=" << slot << " meso=" << mesos);
			}, []() {}, "Mesos");
		};
		if (!pre.empty())
			ask_meso(pre);
		else
			UI::get().emplace<UISoftKey>(ask_meso, []() {}, "Recipient");
	}

	void UIDelivery::draw(float inter) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		if (tab == SEND && send_tb) send_tb.draw(position + Point<int16_t>(8, 48));
		if (tab == RECV && recv_tb) recv_tb.draw(position + Point<int16_t>(0, 48));

		if (tab == RECV) {
			for (size_t i = 0; i < row_labels.size() && i < 10; i++)
				row_labels[i].draw(position + Point<int16_t>(16, static_cast<int16_t>(60 + i * 18)));
		}

		notice.draw(position + Point<int16_t>(16, static_cast<int16_t>(dimension.y() - 50)));
		UIElement::draw_buttons(inter);
	}

	void UIDelivery::doubleclick(Point<int16_t> cursorpos)
	{
		int16_t row = row_at(cursorpos);
		if (row >= 0) {
			selected = row;
			claim_selected();
		}
	}

	Cursor::State UIDelivery::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active || tab != RECV)
			return st;
		int16_t row = row_at(cursorpos);
		if (row >= 0)
		{
			selected = row;
			LOG(LOG_DEBUG, "UIDelivery: select pkg id="
				<< packages[static_cast<size_t>(row)].id);
		}
		return st;
	}

	void UIDelivery::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) {
			DueyClosePacket().dispatch();
			deactivate();
			return;
		}
		if (!pressed) return;
		if (keycode == 'C' || keycode == 'c')
			claim_selected();
		else if (keycode == 'D' || keycode == 'd')
			discard_selected();
		else if (keycode == 'S' || keycode == 's')
			prompt_send();
		else if (keycode == 'I' || keycode == 'i') {
			int8_t invent = 0;
			int16_t slot = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				slot = inv->get_selected_slot();
				if (slot > 0)
					invent = static_cast<int8_t>(inv->get_tab());
			}
			if (invent > 0 && slot > 0) {
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				auto send_spec = [this, invent, slot](const std::string& name, const std::string& rest) {
					if (name.empty()) return;
					int32_t mesos = 0;
					int16_t qty = 1;
					size_t p2 = rest.find(',');
					try {
						if (rest.empty()) {
							// meso default 0
						} else if (p2 == std::string::npos) {
							mesos = std::stoi(rest);
						} else {
							mesos = std::stoi(rest.substr(0, p2));
							qty = static_cast<int16_t>(std::stoi(rest.substr(p2 + 1)));
						}
					} catch (...) {}
					DueySendPacket(invent, slot, qty, mesos, name, false, "").dispatch();
					show_notice(Locale::format_all("delivery.sent", { name, std::to_string(mesos) }));
					LOG(LOG_DEBUG, "UIDelivery: SoftKey DueySend from inv name=" << name
						<< " meso=" << mesos << " inv=" << (int)invent << " slot=" << slot);
				};
				if (!pre.empty()) {
					UI::get().emplace<UISoftKey>([send_spec, pre](const std::string& rest) {
						send_spec(pre, rest);
					}, []() {}, "meso[,qty]");
				} else {
					UI::get().emplace<UISoftKey>([send_spec](const std::string& spec) {
						if (spec.empty()) return;
						size_t p1 = spec.find(',');
						if (p1 == std::string::npos)
							send_spec(spec, "");
						else
							send_spec(spec.substr(0, p1), spec.substr(p1 + 1));
					}, []() {}, "name,meso[,qty]");
				}
			} else {
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				auto parse_rest = [this](const std::string& name, const std::string& rest) {
					if (name.empty()) return;
					int32_t mesos = 0;
					int8_t invent = 0;
					int16_t slot = 0, qty = 0;
					size_t p2 = rest.find(',');
					try {
						if (rest.empty()) {
						} else if (p2 == std::string::npos) {
							mesos = std::stoi(rest);
						} else {
							mesos = std::stoi(rest.substr(0, p2));
							size_t p3 = rest.find(',', p2 + 1);
							if (p3 == std::string::npos)
								invent = static_cast<int8_t>(std::stoi(rest.substr(p2 + 1)));
							else {
								invent = static_cast<int8_t>(std::stoi(rest.substr(p2 + 1, p3 - p2 - 1)));
								size_t p4 = rest.find(',', p3 + 1);
								if (p4 == std::string::npos)
									slot = static_cast<int16_t>(std::stoi(rest.substr(p3 + 1)));
								else {
									slot = static_cast<int16_t>(std::stoi(rest.substr(p3 + 1, p4 - p3 - 1)));
									qty = static_cast<int16_t>(std::stoi(rest.substr(p4 + 1)));
								}
							}
						}
					} catch (...) {}
					DueySendPacket(invent, slot, qty, mesos, name, false, "").dispatch();
					show_notice(Locale::format_all("delivery.sent", { name, std::to_string(mesos) }));
					LOG(LOG_DEBUG, "UIDelivery: SoftKey DueySend name=" << name
						<< " meso=" << mesos << " inv=" << (int)invent << " slot=" << slot);
				};
				if (!pre.empty()) {
					UI::get().emplace<UISoftKey>([parse_rest, pre](const std::string& rest) {
						parse_rest(pre, rest);
					}, []() {}, "meso[,iv,slot,qty]");
				} else {
					UI::get().emplace<UISoftKey>([parse_rest](const std::string& spec) {
						if (spec.empty()) return;
						size_t p1 = spec.find(',');
						if (p1 == std::string::npos)
							parse_rest(spec, "");
						else
							parse_rest(spec.substr(0, p1), spec.substr(p1 + 1));
					}, []() {}, "name,meso[,iv,slot,qty]");
				}
			}
		}
		else if (keycode == 'X' || keycode == 'x') {
			if (selected >= 0 && selected < static_cast<int16_t>(packages.size()))
				discard_selected();
			else {
				UI::get().emplace<UISoftKey>([this](const std::string& id_s) {
					try {
						int32_t id = std::stoi(id_s);
						if (id > 0) {
							DueyDiscardPacket(id).dispatch();
							show_notice(Locale::format("delivery.discard_id", id_s));
							LOG(LOG_DEBUG, "UIDelivery: SoftKey Discard id=" << id);
						}
					} catch (...) {}
				}, []() {}, "Package id");
			}
		} else if (keycode == 'K' || keycode == 'k') {
			if (selected >= 0 && selected < static_cast<int16_t>(packages.size()))
				claim_selected();
			else {
				UI::get().emplace<UISoftKey>([this](const std::string& id_s) {
					try {
						int32_t id = std::stoi(id_s);
						if (id > 0) {
							DueyClaimPacket(id).dispatch();
							show_notice(Locale::format("delivery.claim_id", id_s));
							LOG(LOG_DEBUG, "UIDelivery: SoftKey Claim id=" << id);
						}
					} catch (...) {}
				}, []() {}, "Claim pkg id");
			}
		} else if (keycode == 'F' || keycode == 'f') {
			DueyOpenRecvPacket().dispatch();
			show_notice("Refresh…");
			LOG(LOG_DEBUG, "UIDelivery: DueyOpenRecv refresh");
		}
	}

	UIElement::Type UIDelivery::get_type() const { return TYPE; }

	Button::State UIDelivery::button_pressed(uint16_t buttonid)
	{
		switch (buttonid) {
			case BtClose:
				DueyClosePacket().dispatch();
				deactivate();
				break;
			case TabSend:
				tab = SEND;
				prompt_send();
				break;
			case TabRecv:
				tab = RECV;
				DueyOpenRecvPacket().dispatch();
				break;
			case TabTax:
				tab = TAX;
				UI::get().emplace<UISoftKey>([this](const std::string& tip) {
					if (tip.empty()) {
						show_notice(Locale::text("delivery.fee_help"));
						return;
					}
					try {
						int64_t m = std::stoll(tip);
						if (m < 0) m = 0;
						int64_t fee = m + 5000;
						show_notice(Locale::format_all("delivery.fee_calc", { tip, std::to_string(fee) }));
						LOG(LOG_DEBUG, "UIDelivery: TabTax SoftKey fee=" << fee);
					} catch (...) {
						show_notice(Locale::text("delivery.fee_help"));
					}
				}, []() {}, "mesos|empty=help");
				break;
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
