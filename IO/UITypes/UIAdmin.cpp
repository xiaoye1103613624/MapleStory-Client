//////////////////////////////////////////////////////////////////////////////////
//	Admin — SoftKey announce
//////////////////////////////////////////////////////////////////////////////////
#include "UIAdmin.h"
#include "UIMiscShells.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIAdmin::UIAdmin()
		: msg_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("admin.title"))
	{
		position = Point<int16_t>(300, 200);
		dimension = Point<int16_t>(209, 156);
		dragarea = Point<int16_t>(209, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Admin");
		if (!root) {
			LOG(LOG_WARN, "UIAdmin: missing");
			return;
		}
		has_assets = true;
		nl::node block = root["Block"];
		if (block["backgrnd"]) {
			backgrnd = Texture(block["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		center_on_screen();
		UI::get().emplace<UISoftKey>([this](const std::string& msg) {
			if (msg.empty()) return;
			std::string t = msg;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t == "users" || t == "show") {
				AdminEnterMapPacket(AdminEnterMapPacket::Operation::SHOW_USERS).dispatch();
				msg_label.change_text(Locale::text("admin.show_users"));
				LOG(LOG_DEBUG, "UIAdmin: AdminEnterMap SHOW_USERS");
				return;
			}
			if (t == "alert") {
				AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
				msg_label.change_text(Locale::text("admin.alert_admins"));
				LOG(LOG_DEBUG, "UIAdmin: AdminEnterMap ALERT_ADMINS");
				return;
			}
			msg_label.change_text(msg);
			GeneralChatPacket(msg, true).dispatch();
			LOG(LOG_DEBUG, "UIAdmin: SoftKey GeneralChat len=" << msg.size());
		}, []() {}, "users|alert|msg");
		LOG(LOG_DEBUG, "UIAdmin: open");
#endif
	}

	void UIAdmin::set_message(const std::string& msg)
	{
		msg_label.change_text(msg);
	}

	void UIAdmin::draw(float) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		msg_label.draw(position + Point<int16_t>(16, 40));
	}

	void UIAdmin::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's') {
			UI::get().emplace<UISoftKey>([this](const std::string& msg) {
				if (msg.empty()) return;
				std::string t = msg;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				if (t == "users" || t == "show") {
					AdminEnterMapPacket(AdminEnterMapPacket::Operation::SHOW_USERS).dispatch();
					msg_label.change_text(Locale::text("admin.show_users"));
					return;
				}
				if (t == "alert") {
					AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
					msg_label.change_text(Locale::text("admin.alert_admins"));
					return;
				}
				msg_label.change_text(msg);
				GeneralChatPacket(msg, true).dispatch();
			}, []() {}, "users|alert|msg");
		}
		else if (keycode == 'U' || keycode == 'u')
		{
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::SHOW_USERS).dispatch();
			msg_label.change_text(Locale::text("admin.show_users"));
		}
		else if (keycode == 'L' || keycode == 'l')
		{
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
			msg_label.change_text(Locale::text("admin.alert_admins"));
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			auto ui = UI::get().get_element<UIGMBoard>();
			if (!ui) ui = UI::get().emplace<UIGMBoard>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UIAdmin::get_type() const { return TYPE; }
}
