//////////////////////////////////////////////////////////////////////////////////
//	Family UI + OPEN_FAMILY 出站
//////////////////////////////////////////////////////////////////////////////////
#include "UIFamily.h"
#include "UICharInfo.h"
#include "UIFamilyTree.h"
#include "UISoftKey.h"
#include "UINotice.h"

#include "../UI.h"
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
	UIFamily::UIFamily() : has_assets(false),
		name_label(Text::Font::A12B, Text::Alignment::LEFT, Color::Name::BLACK),
		msg_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK),
		rep_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK)
	{
		position = Point<int16_t>(280, 60);
		dimension = Point<int16_t>(224, 392);
		dragarea = Point<int16_t>(224, 20);

#ifdef USE_NX
		nl::node root = UiAssetResolve::window("Family");
		if (!root) { LOG(LOG_WARN, "UIFamily: missing"); return; }
		has_assets = true;
		if (root["backgrnd"]) {
			backgrnd = Texture(root["backgrnd"]);
			Point<int16_t> o = backgrnd.get_origin();
			if (o.x() || o.y()) backgrnd.shift(o);
			dimension = backgrnd.get_dimensions();
			dragarea = Point<int16_t>(dimension.x(), 20);
		}
		auto put = [&](Buttons id, const char* name, int16_t x, int16_t y) {
			if (root[name]) buttons[id] = std::make_unique<MapleButton>(root[name], Point<int16_t>(x, y));
		};
		put(BtClose, "BtClose", static_cast<int16_t>(dimension.x() - 21), 5);
		put(BtOK, "BtOK", 80, static_cast<int16_t>(dimension.y() - 36));
		put(BtTree, "BtTree", 20, static_cast<int16_t>(dimension.y() - 36));
		put(BtLeft, "BtLeft", 40, 200);
		put(BtRight, "BtRight", 160, 200);
		put(BtJunior, "BtJuniorEntry", 20, 360);
		put(BtPrecept, "BtFamilyPrecept", 100, 360);
		put(BtSpecial, "BtSpecial", 160, 360);
		name_label.change_text(Locale::text("family.none"));
		rep_label.change_text(Locale::text("family.rep_initial"));
		LOG(LOG_DEBUG, "UIFamily: open");
#endif
	}

	void UIFamily::set_info(const FamilyInfoData& data)
	{
		info = data;
		name_label.change_text(info.name.empty() ? Locale::text("family.none") : info.name);
		msg_label.change_text(info.message);
		rep_label.change_text(
			Locale::format_all("family.rep_line",
			{ std::to_string(info.cur_rep), std::to_string(info.tot_rep), std::to_string(info.today_rep),
			std::to_string(info.juniors), std::to_string(info.juniors_max) })
		);
		LOG(LOG_DEBUG, "UIFamily: info name=" << info.name);
	}

	void UIFamily::set_privileges(const std::vector<std::string>& names)
	{
		std::string tip = Locale::format("family.priv_count", std::to_string(names.size()));
		for (size_t i = 0; i < names.size() && i < 3; ++i)
		{
			if (!names[i].empty())
				tip += (i == 0 ? ": " : ", ") + names[i];
		}
		msg_label.change_text(tip);
		LOG(LOG_DEBUG, "UIFamily: privileges n=" << names.size());
	}

	void UIFamily::draw(float inter) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		name_label.draw(position + Point<int16_t>(16, 40));
		msg_label.draw(position + Point<int16_t>(16, 60));
		rep_label.draw(position + Point<int16_t>(16, 80));
		UIElement::draw_buttons(inter);
	}

	void UIFamily::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a') {
			// SoftKey inviter cid → name → y/n；CharInfo 齐全则只问 y/n
			int32_t pre_id = 0;
			std::string pre_name;
			if (auto ci = UI::get().get_element<UICharInfo>()) {
				pre_id = ci->get_target_cid();
				pre_name = ci->get_target_name();
			}
			auto ask_yn = [](int32_t id, const std::string& name) {
				UI::get().emplace<UISoftKey>([id, name](const std::string& yn) {
					bool ok = !yn.empty() && (yn[0] == 'y' || yn[0] == 'Y' || yn[0] == '1');
					FamilyAcceptPacket(id, name, ok).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Accept id=" << id << " ok=" << ok);
				}, []() {}, "y/n");
			};
			if (pre_id > 0 && !pre_name.empty()) {
				ask_yn(pre_id, pre_name);
			} else if (pre_id > 0) {
				UI::get().emplace<UISoftKey>([ask_yn, pre_id](const std::string& name) {
					if (name.empty()) return;
					ask_yn(pre_id, name);
				}, []() {}, "Inviter name");
			} else {
				UI::get().emplace<UISoftKey>([ask_yn, pre_name](const std::string& id_s) {
					int32_t id = 0;
					try { id = std::stoi(id_s); } catch (...) { return; }
					if (id <= 0) return;
					if (!pre_name.empty()) {
						ask_yn(id, pre_name);
					} else {
						UI::get().emplace<UISoftKey>([ask_yn, id](const std::string& name) {
							if (name.empty()) return;
							ask_yn(id, name);
						}, []() {}, "Inviter name");
					}
				}, []() {}, "Inviter cid");
			}
		} else if (keycode == 'J' || keycode == 'j') {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyAddPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: Add junior=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (!name.empty()) {
						FamilyAddPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Add junior=" << name);
					}
				}, []() {}, "Junior name");
			}
		} else if (keycode == 'P' || keycode == 'p') {
			UI::get().emplace<UISoftKey>([](const std::string& msg) {
				if (!msg.empty()) {
					FamilyPreceptPacket(msg).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Precept len=" << msg.size());
				}
			}, []() {}, "Precept");
		} else if (keycode == 'T' || keycode == 't') {
			OpenFamilyPedigreePacket().dispatch();
			UI::get().emplace<UIFamilyTree>();
			LOG(LOG_DEBUG, "UIFamily: Tree");
		} else if (keycode == 'O' || keycode == 'o') {
			OpenFamilyPacket().dispatch();
			LOG(LOG_DEBUG, "UIFamily: OpenFamily");
		} else if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's') {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyUsePacket(1, pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: Summon target=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& spec) {
					if (spec.empty()) return;
					int type = 0;
					std::string name;
					size_t c = spec.find(',');
					try {
						if (c == std::string::npos)
							type = std::stoi(spec);
						else {
							type = std::stoi(spec.substr(0, c));
							name = spec.substr(c + 1);
						}
					} catch (...) { return; }
					FamilyUsePacket(type, name).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Use type=" << type << " name=" << name);
				}, []() {}, "type[,name] 0=reunite 1=summon");
			}
		} else if (keycode == 'X' || keycode == 'x') {
			// FamilyTree 选中 / CharInfo cid → YesNo Separate；否则 SoftKey
			int32_t from_tree = 0;
			std::string from_name;
			if (auto tree = UI::get().get_element<UIFamilyTree>()) {
				if (const FamilyTreeEntry* e = tree->selected_entry()) {
					from_tree = e->chr_id;
					from_name = e->name;
				} else {
					from_tree = tree->get_selected_cid();
				}
			}
			if (from_tree <= 0)
				if (auto ci = UI::get().get_element<UICharInfo>())
					from_tree = ci->get_target_cid();
			if (from_tree > 0) {
				std::string label = from_name.empty()
					? ("cid " + std::to_string(from_tree)) : from_name;
				UI::get().emplace<UIYesNo>(Locale::format("family.ask_separate", label), [from_tree](bool yes) {
					if (!yes) return;
					FamilySeparateSeniorPacket(from_tree).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Separate junior cid=" << from_tree);
				});
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& id_s) {
					if (id_s.empty()) {
						UI::get().emplace<UIYesNo>(Locale::text("family.ask_leave"), [](bool yes) {
							if (!yes) return;
							FamilySeparateJuniorPacket().dispatch();
							LOG(LOG_DEBUG, "UIFamily: Separate as junior");
						});
						return;
					}
					int32_t id = 0;
					try { id = std::stoi(id_s); } catch (...) { return; }
					if (id <= 0) return;
					UI::get().emplace<UIYesNo>(Locale::format("family.ask_separate_cid", std::to_string(id)), [id](bool yes) {
						if (!yes) return;
						FamilySeparateSeniorPacket(id).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Separate junior cid=" << id);
					});
				}, []() {}, "junior cid (empty=leave)");
			}
		}
	}

	UIElement::Type UIFamily::get_type() const { return TYPE; }

	Button::State UIFamily::button_pressed(uint16_t id)
	{
		if (id == BtClose || id == BtOK)
			deactivate();
		else if (id == BtTree) {
			OpenFamilyPedigreePacket().dispatch();
			UI::get().emplace<UIFamilyTree>();
		} else if (id == BtJunior) {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyAddPacket(pre).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (!name.empty())
						FamilyAddPacket(name).dispatch();
				}, []() {}, "Junior name");
			}
		} else if (id == BtPrecept) {
			UI::get().emplace<UISoftKey>([](const std::string& msg) {
				if (!msg.empty())
					FamilyPreceptPacket(msg).dispatch();
			}, []() {}, "Precept");
		} else if (id == BtSpecial) {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyUsePacket(1, pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: BtSpecial Summon target=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& spec) {
					if (spec.empty()) return;
					int type = 0;
					std::string name;
					size_t c = spec.find(',');
					try {
						if (c == std::string::npos)
							type = std::stoi(spec);
						else {
							type = std::stoi(spec.substr(0, c));
							name = spec.substr(c + 1);
						}
					} catch (...) { return; }
					FamilyUsePacket(type, name).dispatch();
				}, []() {}, "type[,name] 0=reunite 1=summon");
			}
		} else {
			OpenFamilyPedigreePacket().dispatch();
			LOG(LOG_DEBUG, "UIFamily: btn=" << id << " → pedigree");
		}
		return Button::State::NORMAL;
	}
}
