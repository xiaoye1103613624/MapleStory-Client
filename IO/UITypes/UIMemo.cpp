//////////////////////////////////////////////////////////////////////////////////
//	Memo UI — MEMO_RESULT mode3 列表 / Delete → NOTE_ACTION
//////////////////////////////////////////////////////////////////////////////////
#include "UIMemo.h"
#include "UICharInfo.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/NotePackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIMemo::UIMemo() : has_assets(false), selected(-1),
		tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW)
	{
		position = Point<int16_t>(280, 100);
		dimension = Point<int16_t>(295, 240);
		dragarea = Point<int16_t>(295, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Memo");
		if (!root) { LOG(LOG_WARN, "UIMemo: missing"); return; }
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		if (root["backgrnd2"]) backgrnd2 = Texture(root["backgrnd2"]);
		nl::node close = UiAssetResolve::close_btn();
		if (close) buttons[BtClose] = std::make_unique<MapleButton>(close, Point<int16_t>(static_cast<int16_t>(dimension.x() - 21), 5));
		int16_t by = static_cast<int16_t>(dimension.y() - 36);
		if (root["BtOpen"]) buttons[BtOpen] = std::make_unique<MapleButton>(root["BtOpen"], Point<int16_t>(20, by));
		if (root["BtSave"]) buttons[BtSave] = std::make_unique<MapleButton>(root["BtSave"], Point<int16_t>(100, by));
		if (root["BtDelete"]) buttons[BtDelete] = std::make_unique<MapleButton>(root["BtDelete"], Point<int16_t>(180, by));
		LOG(LOG_DEBUG, "UIMemo: open");
#endif
	}

	void UIMemo::set_notes(std::vector<MemoEntry> list)
	{
		notes = std::move(list);
		selected = notes.empty() ? -1 : 0;
		tip_label.change_text(std::to_string(notes.size()) + " note(s)");
		LOG(LOG_DEBUG, "UIMemo: notes=" << notes.size());
	}

	void UIMemo::show_tip(const std::string& tip)
	{
		tip_label.change_text(tip);
	}

	void UIMemo::draw(float inter) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		if (backgrnd2) backgrnd2.draw(position);

		int16_t y = 36;
		size_t maxshow = 6;
		for (size_t i = 0; i < notes.size() && i < maxshow; i++) {
			Text row(Text::Font::A11M, Text::Alignment::LEFT,
				(static_cast<int>(i) == selected) ? Color::Name::YELLOW : Color::Name::BLACK);
			std::string line = notes[i].from + ": " + notes[i].message;
			if (line.size() > 36) line = line.substr(0, 33) + "...";
			row.change_text(line);
			row.draw(position + Point<int16_t>(16, y));
			y = static_cast<int16_t>(y + 18);
		}

		if (!tip_label.get_text().empty())
			tip_label.draw(position + Point<int16_t>(16, static_cast<int16_t>(dimension.y() - 56)));

		UIElement::draw_buttons(inter);
	}

	Cursor::State UIMemo::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active || !has_assets)
			return st;
		Point<int16_t> rel = cursorpos - position;
		if (rel.x() < 12 || rel.x() > 280)
			return st;
		int16_t row = static_cast<int16_t>((rel.y() - 36) / 18);
		if (row >= 0 && row < static_cast<int16_t>(notes.size()) && row < 6)
		{
			selected = row;
			tip_label.change_text("Sel " + notes[static_cast<size_t>(row)].from);
			LOG(LOG_DEBUG, "UIMemo: select id=" << notes[static_cast<size_t>(row)].id);
		}
		return st;
	}

	void UIMemo::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'S' || keycode == 's') {
			button_pressed(BtSave);
		} else if (keycode == 'D' || keycode == 'd') {
			button_pressed(BtDelete);
		} else if (keycode == 'O' || keycode == 'o') {
			button_pressed(BtOpen);
		} else if (keycode == 'N' || keycode == 'n') {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				std::string to = pre;
				UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
					if (msg.empty()) return;
					NoteSendPacket(to, msg).dispatch();
					tip_label.change_text("Sent→" + to);
				}, []() {}, "Memo message");
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& to) {
					if (to.empty()) return;
					UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
						if (msg.empty()) return;
						NoteSendPacket(to, msg).dispatch();
						tip_label.change_text("Sent→" + to);
					}, []() {}, "Memo message");
				}, []() {}, "Memo to");
			}
		} else if (keycode == 'X' || keycode == 'x') {
			if (selected >= 0 && selected < static_cast<int>(notes.size())) {
				std::vector<int32_t> ids{ notes[static_cast<size_t>(selected)].id };
				NoteDiscardPacket(ids).dispatch();
				tip_label.change_text("Discard id=" + std::to_string(ids[0]));
				LOG(LOG_DEBUG, "UIMemo: SoftKey Discard id=" << ids[0]);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& id_s) {
					try {
						int32_t id = std::stoi(id_s);
						if (id > 0) {
							std::vector<int32_t> ids{ id };
							NoteDiscardPacket(ids).dispatch();
							tip_label.change_text("Discard id=" + id_s);
							LOG(LOG_DEBUG, "UIMemo: SoftKey Discard id=" << id);
						}
					} catch (...) {}
				}, []() {}, "Note id");
			}
		} else if (keycode == 'R' || keycode == 'r') {
			tip_label.change_text("Notes: " + std::to_string(notes.size()));
			LOG(LOG_DEBUG, "UIMemo: refresh count=" << notes.size());
		} else if (keycode == 'L' || keycode == 'l') {
			show_tip("List " + std::to_string(notes.size()));
		}
	}

	UIElement::Type UIMemo::get_type() const { return TYPE; }

	Button::State UIMemo::button_pressed(uint16_t id)
	{
		switch (id) {
			case BtClose:
				deactivate();
				break;
			case BtDelete:
				if (selected >= 0 && selected < static_cast<int>(notes.size())) {
					std::vector<int32_t> ids{ notes[static_cast<size_t>(selected)].id };
					LOG(LOG_DEBUG, "UIMemo: discard id=" << ids[0]);
					NoteDiscardPacket(ids).dispatch();
				} else {
					tip_label.change_text("No note selected");
				}
				break;
			case BtOpen:
				tip_label.change_text(selected >= 0 && selected < static_cast<int>(notes.size())
					? notes[static_cast<size_t>(selected)].message
					: "No note");
				break;
			case BtSave:
			{
				// SoftKey: CharInfo 名则只问正文；否则 收件人名 → 正文
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					std::string to = pre;
					UI::get().emplace<UISoftKey>([to](const std::string& msg) {
						if (msg.empty()) return;
						NoteSendPacket(to, msg).dispatch();
						LOG(LOG_DEBUG, "UIMemo: NoteSend to=" << to << " len=" << msg.size());
					}, []() {}, "Memo message");
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& to) {
						if (to.empty()) return;
						UI::get().emplace<UISoftKey>([to](const std::string& msg) {
							if (msg.empty()) return;
							NoteSendPacket(to, msg).dispatch();
							LOG(LOG_DEBUG, "UIMemo: NoteSend to=" << to << " len=" << msg.size());
						}, []() {}, "Memo message");
					}, []() {}, "Memo to");
				}
				break;
			}
			default:
				break;
		}
		return Button::State::NORMAL;
	}
}
