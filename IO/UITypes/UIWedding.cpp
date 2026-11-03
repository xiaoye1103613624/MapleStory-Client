//////////////////////////////////////////////////////////////////////////////////
//	Wedding UI
//////////////////////////////////////////////////////////////////////////////////
#include "UIWedding.h"
#include "UICharInfo.h"
#include "UIMiscShells.h"
#include "UINewYearsCard.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#include "../../Util/Locale.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIWedding::UIWedding()
		: tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK),
		  progress_label(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE)
	{
		position = Point<int16_t>(280, 120);
		dimension = Point<int16_t>(234, 250);
		dragarea = Point<int16_t>(234, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Wedding");
		if (!root) {
			LOG(LOG_WARN, "UIWedding: missing");
			return;
		}
		has_assets = true;
		nl::node inv = root["Invitation"];
		nl::node bg = inv["Cathedral"] ? inv["Cathedral"] : inv["Vegas"];
		if (bg) {
			invite = Texture(bg);
			Point<int16_t> o = invite.get_origin();
			if (o.x() || o.y()) invite.shift(o);
			dimension = invite.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		if (inv["BtOK"])
			buttons[BtOK] = std::make_unique<MapleButton>(inv["BtOK"], Point<int16_t>(80, static_cast<int16_t>(dimension.y() - 40)));
		tip_label.change_text(Locale::text("wedding.hint_softkey"));
		progress_label.change_text("");
		center_on_screen();
		{
			std::string guest_pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				guest_pre = ci->get_target_name();
			if (!guest_pre.empty()) {
				tip_label.change_text(Locale::format("wedding.guest", guest_pre));
				UI::get().emplace<UISoftKey>([guest_pre](const std::string& msg) {
					std::string body = msg.empty() ? "Wedding invite" : msg;
					WhisperPacket(guest_pre, body).dispatch();
					LOG(LOG_DEBUG, "UIWedding: Whisper guest=" << guest_pre);
				}, []() {}, Locale::text("wedding.softkey_invite_msg"));
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& guest) {
					if (guest.empty()) return;
					tip_label.change_text(Locale::format("wedding.guest", guest));
					UI::get().emplace<UISoftKey>([guest](const std::string& msg) {
						std::string body = msg.empty() ? "Wedding invite" : msg;
						WhisperPacket(guest, body).dispatch();
						LOG(LOG_DEBUG, "UIWedding: Whisper guest=" << guest);
					}, []() {}, Locale::text("wedding.softkey_invite_msg"));
				}, []() {}, Locale::text("wedding.softkey_guest"));
			}
		}
		LOG(LOG_DEBUG, "UIWedding: open");
#endif
	}

	void UIWedding::set_invitation(bool cathedral, const std::string& tip)
	{
#ifdef USE_NX
		nl::node inv = UiAssetResolve::window("Wedding")["Invitation"];
		nl::node bg = cathedral ? inv["Cathedral"] : inv["Vegas"];
		if (bg) {
			invite = Texture(bg);
			Point<int16_t> o = invite.get_origin();
			if (o.x() || o.y()) invite.shift(o);
			dimension = invite.get_dimensions();
		}
#endif
		tip_label.change_text(tip);
	}

	void UIWedding::set_progress(uint8_t step, int32_t groom, int32_t bride)
	{
		progress_label.change_text(Locale::format_all("wedding.progress",
			{ std::to_string(step), std::to_string(groom), std::to_string(bride) }));
	}

	void UIWedding::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (invite)
			invite.draw(position);
		tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 40));
		progress_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 60));
		UIElement::draw_buttons(inter);
	}

	void UIWedding::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'W' || keycode == 'w' || keycode == 'I' || keycode == 'i') {
			std::string guest_pre;
			if (auto ci = UI::get().get_element<UICharInfo>()) {
				// 角色信息窗打开时优先用其目标名
				guest_pre = ci->get_target_name();
			}
			if (!guest_pre.empty()) {
				std::string guest = guest_pre;
				tip_label.change_text(Locale::format("wedding.guest", guest));
				UI::get().emplace<UISoftKey>([this, guest](const std::string& msg) {
					std::string body = msg.empty() ? "Wedding invite" : msg;
					WhisperPacket(guest, body).dispatch();
					tip_label.change_text(Locale::format("wedding.invited", guest));
					LOG(LOG_DEBUG, "UIWedding: Whisper guest=" << guest);
				}, []() {}, Locale::text("wedding.softkey_invite_msg"));
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& guest) {
					if (guest.empty()) return;
					tip_label.change_text(Locale::format("wedding.guest", guest));
					UI::get().emplace<UISoftKey>([this, guest](const std::string& msg) {
						std::string body = msg.empty() ? "Wedding invite" : msg;
						WhisperPacket(guest, body).dispatch();
						tip_label.change_text(Locale::format("wedding.invited", guest));
						LOG(LOG_DEBUG, "UIWedding: Whisper guest=" << guest);
					}, []() {}, Locale::text("wedding.softkey_invite_msg"));
				}, []() {}, Locale::text("wedding.softkey_guest"));
			}
		}
		else if (keycode == 'M' || keycode == 'm')
		{
			auto ui = UI::get().get_element<UIMateMessage>();
			if (!ui) ui = UI::get().emplace<UIMateMessage>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'N' || keycode == 'n')
		{
			auto ui = UI::get().get_element<UINewYearsCard>();
			if (!ui) ui = UI::get().emplace<UINewYearsCard>();
			else if (!ui->is_active()) ui->makeactive();
		}
		else if (keycode == 'G' || keycode == 'g')
		{
			auto ui = UI::get().get_element<UIClaim>();
			if (!ui) ui = UI::get().emplace<UIClaim>();
			else if (!ui->is_active()) ui->makeactive();
		}
	}

	UIElement::Type UIWedding::get_type() const { return TYPE; }

	Button::State UIWedding::button_pressed(uint16_t id)
	{
		if (id == BtOK) {
			std::string guest_pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				guest_pre = ci->get_target_name();
			auto ask_msg = [this](const std::string& guest) {
				tip_label.change_text(Locale::format("wedding.guest", guest));
				UI::get().emplace<UISoftKey>([this, guest](const std::string& msg) {
					std::string body = msg.empty() ? "Wedding invite" : msg;
					WhisperPacket(guest, body).dispatch();
					tip_label.change_text(Locale::format("wedding.invited", guest));
					LOG(LOG_DEBUG, "UIWedding: BtOK Whisper guest=" << guest);
					deactivate();
				}, [this]() { deactivate(); }, Locale::text("wedding.softkey_invite_msg"));
			};
			if (!guest_pre.empty())
				ask_msg(guest_pre);
			else
				UI::get().emplace<UISoftKey>([this, ask_msg](const std::string& guest) {
					if (guest.empty()) { deactivate(); return; }
					ask_msg(guest);
				}, [this]() { deactivate(); }, Locale::text("wedding.softkey_guest"));
		}
		return Button::State::NORMAL;
	}
}
