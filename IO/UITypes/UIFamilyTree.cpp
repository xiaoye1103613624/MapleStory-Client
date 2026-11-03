//////////////////////////////////////////////////////////////////////////////////
//	Family pedigree tree
//////////////////////////////////////////////////////////////////////////////////
#include "UIFamilyTree.h"
#include "UICharInfo.h"
#include "UISoftKey.h"
#include "UINotice.h"

#include "../UI.h"
#include "../KeyAction.h"
#include "../Components/MapleButton.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UIFamilyTree::UIFamilyTree() : has_assets(false), viewed_id(0), add_enabled(0),
		header(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK)
	{
		position = Point<int16_t>(120, 80);
		dimension = Point<int16_t>(578, 386);
		dragarea = Point<int16_t>(578, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("FamilyTree");
		if (!root) {
			LOG(LOG_WARN, "UIFamilyTree: missing");
			return;
		}
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y())
				backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		auto put = [&](Buttons id, const char* n, int16_t x, int16_t y) {
			if (root[n])
				buttons[id] = std::make_unique<MapleButton>(root[n], Point<int16_t>(x, y));
		};
		put(BtClose, "BtClose", static_cast<int16_t>(dimension.x() - 21), 5);
		put(BtBye, "BtBye", 20, static_cast<int16_t>(dimension.y() - 36));
		put(BtLeft, "BtLeft", 40, 180);
		put(BtRight, "BtRight", 500, 180);
		put(BtJunior, "BtJuniorEntry", 200, static_cast<int16_t>(dimension.y() - 36));
		header.change_text(Locale::text("familytree.header"));
		LOG(LOG_DEBUG, "UIFamilyTree: shell open");
#endif
	}

	void UIFamilyTree::set_chart(int32_t viewed, std::vector<FamilyTreeEntry> ents, int16_t add)
	{
		viewed_id = viewed;
		entries = std::move(ents);
		add_enabled = add;
		name_labels.clear();
		name_labels.reserve(entries.size());
		for (const auto& e : entries) {
			Text t(Text::Font::A11M, Text::Alignment::LEFT,
				e.online ? Color::Name::GREEN : Color::Name::BLACK);
			std::string line = e.name + " " + Locale::format("familytree.level", std::to_string(e.level))
				+ " " + Locale::text(e.online ? "familytree.online" : "familytree.offline")
				+ " " + Locale::format("familytree.rep", std::to_string(e.rep));
			t.change_text(line);
			name_labels.push_back(std::move(t));
		}
		header.change_text(Locale::format("familytree.pedigree", std::to_string(entries.size()))
			+ (add_enabled ? (" " + Locale::text("familytree.can_add")) : ""));
		LOG(LOG_DEBUG, "UIFamilyTree: chart viewed=" << viewed_id << " n=" << entries.size());
	}

	void UIFamilyTree::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (backgrnd)
			backgrnd.draw(position);
		header.draw(position + Point<int16_t>(16, 28));
		int16_t y = 52;
		for (size_t i = 0; i < name_labels.size() && i < 12; i++) {
			name_labels[i].draw(position + Point<int16_t>(24, y));
			y = static_cast<int16_t>(y + 18);
		}
		UIElement::draw_buttons(inter);
	}

	Cursor::State UIFamilyTree::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		Cursor::State st = UIDragElement::send_cursor(clicked, cursorpos);
		if (!clicked || !active || !has_assets)
			return st;
		Point<int16_t> rel = cursorpos - position;
		if (rel.x() < 20 || rel.x() > 400)
			return st;
		int16_t row = static_cast<int16_t>((rel.y() - ROW_TOP) / ROW_H);
		if (row >= 0 && row < static_cast<int16_t>(entries.size()) && row < 12)
		{
			selected_cid = entries[static_cast<size_t>(row)].chr_id;
			LOG(LOG_DEBUG, "UIFamilyTree: select cid=" << selected_cid
				<< " name=" << entries[static_cast<size_t>(row)].name);
		}
		return st;
	}

	const FamilyTreeEntry* UIFamilyTree::selected_entry() const
	{
		for (const auto& e : entries)
			if (e.chr_id == selected_cid)
				return &e;
		return nullptr;
	}

	void UIFamilyTree::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if ((keycode == 'J' || keycode == 'j') && add_enabled) {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyAddPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIFamilyTree: SoftKey Add " << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (!name.empty()) {
						FamilyAddPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIFamilyTree: SoftKey Add " << name);
					}
				}, []() {}, "Junior name");
			}
		} else if (keycode == 'X' || keycode == 'x') {
			if (const FamilyTreeEntry* e = selected_entry()) {
				int32_t id = e->chr_id;
				UI::get().emplace<UIYesNo>(Locale::format("family.ask_separate", e->name), [id](bool yes) {
					if (!yes) return;
					FamilySeparateSeniorPacket(id).dispatch();
					LOG(LOG_DEBUG, "UIFamilyTree: Separate cid=" << id);
				});
			} else {
				int32_t from_ci = 0;
				if (auto ci = UI::get().get_element<UICharInfo>())
					from_ci = ci->get_target_cid();
				if (from_ci > 0) {
					UI::get().emplace<UIYesNo>(Locale::format("family.ask_separate_cid", std::to_string(from_ci)), [this, from_ci](bool yes) {
						if (!yes) return;
						selected_cid = from_ci;
						FamilySeparateSeniorPacket(from_ci).dispatch();
						LOG(LOG_DEBUG, "UIFamilyTree: Separate cid=" << from_ci);
					});
				} else {
					UI::get().emplace<UISoftKey>([this](const std::string& id_s) {
						if (id_s.empty()) {
							UI::get().emplace<UIYesNo>(Locale::text("family.ask_leave"), [](bool yes) {
								if (!yes) return;
								FamilySeparateJuniorPacket().dispatch();
							});
							return;
						}
						try {
							int32_t id = std::stoi(id_s);
							if (id > 0) {
								selected_cid = id;
								FamilySeparateSeniorPacket(id).dispatch();
								LOG(LOG_DEBUG, "UIFamilyTree: Separate cid=" << id);
							}
						} catch (...) {}
					}, []() {}, "junior cid (empty=leave)");
				}
			}
		} else if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's') {
			if (const FamilyTreeEntry* e = selected_entry()) {
				FamilyUsePacket(1, e->name).dispatch();
				LOG(LOG_DEBUG, "UIFamilyTree: Summon " << e->name);
			} else {
				std::string pre;
				if (auto ci = UI::get().get_element<UICharInfo>())
					pre = ci->get_target_name();
				if (!pre.empty()) {
					FamilyUsePacket(1, pre).dispatch();
					LOG(LOG_DEBUG, "UIFamilyTree: Summon " << pre);
				} else {
					UI::get().emplace<UISoftKey>([](const std::string& name) {
						if (name.empty()) return;
						FamilyUsePacket(1, name).dispatch();
						LOG(LOG_DEBUG, "UIFamilyTree: Summon " << name);
					}, []() {}, "Summon name");
				}
			}
		} else if (keycode == 'O' || keycode == 'o' || keycode == 'R' || keycode == 'r') {
			OpenFamilyPedigreePacket().dispatch();
			LOG(LOG_DEBUG, "UIFamilyTree: Refresh pedigree");
		} else if (keycode == 'L' || keycode == 'l' || keycode == KeyAction::Id::LEFT) {
			button_pressed(BtLeft);
		} else if (keycode == KeyAction::Id::RIGHT) {
			button_pressed(BtRight);
		}
	}

	UIElement::Type UIFamilyTree::get_type() const { return TYPE; }

	Button::State UIFamilyTree::button_pressed(uint16_t id)
	{
		if (id == BtClose || id == BtBye)
			deactivate();
		else if (id == BtJunior && add_enabled) {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyAddPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIFamilyTree: BtJunior Add " << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (!name.empty())
						FamilyAddPacket(name).dispatch();
				}, []() {}, "Junior name");
			}
		} else if (id == BtLeft || id == BtRight) {
			OpenFamilyPedigreePacket().dispatch();
			LOG(LOG_DEBUG, "UIFamilyTree: nav → pedigree");
		} else
			LOG(LOG_DEBUG, "UIFamilyTree: btn=" << id);
		return Button::State::NORMAL;
	}
}
