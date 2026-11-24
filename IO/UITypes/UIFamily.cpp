////////////////////////////////////////////////////////////////////////////////
//	学院（Family）UI + OPEN_FAMILY 出站 —— 2026-11-07 原生「学院.png」重排
//	W2（2026-11-20）：复核 Family 节点 / 特权翻页 / FamilyUse；禁 SoftKey 弹窗。
////////////////////////////////////////////////////////////////////////////////
#include "UIFamily.h"
#include "UICharInfo.h"
#include "UIChatBar.h"
#include "UIFamilyTree.h"
#include "UINotice.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/FamilyModel.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#include <cstdio>
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		void chat_hint(const char* key)
		{
			auto chatbar = UI::get().get_element<UIChatBar>();
			if (!chatbar)
				chatbar = UI::get().emplace<UIChatBar>();
			if (chatbar)
			{
				chatbar->toggle_input(true);
				chatbar->show_message(Locale::text(key).c_str(),
					UIChatBar::MessageType::YELLOW);
			}
		}

		// s9 desc 里的 #c 色标与 \\n 转义：loadFamily 下发的是原文（含 "#c1.5倍#"
		// 高亮标记与换行）。窗内先按行拆分并剥掉成对 #c...# 标记体（保留内容），
		// 与原生观感一致（原生用色字，本端先保文字正确）。
		std::vector<std::string> split_priv_desc(const std::string& raw)
		{
			std::vector<std::string> lines;
			std::string cur;
			for (size_t i = 0; i < raw.size(); i++)
			{
				char c = raw[i];
				if (c == '\\' && i + 1 < raw.size() && raw[i + 1] == 'n')
				{
					lines.push_back(cur);
					cur.clear();
					++i;
					continue;
				}
				if (c == '#' && i + 1 < raw.size() && raw[i + 1] == 'c')
				{
					i += 1;
					continue;
				}
				if (c == '#')
					continue;
				cur += c;
			}
			lines.push_back(cur);
			return lines;
		}
	}

	UIFamily::UIFamily() : has_assets(false),
		banner_label(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE),
		precept_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK),
		junior_label(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK),
		rep_label(Text::Font::A12M, Text::Alignment::RIGHT, Color::Name::BLACK),
		today_label(Text::Font::A12M, Text::Alignment::RIGHT, Color::Name::BLACK),
		priv_title_label(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::BLACK),
		priv_cost_label(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK),
		priv_limit_label(Text::Font::A11M, Text::Alignment::RIGHT, Color::Name::BLACK),
		priv_desc_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "", 196)
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
		// 落位 = academy_top/priv_3x 实测行带；IMG Family 子树橙色-wz 复核 2026-11-20。
		put(BtClose, "BtClose", static_cast<int16_t>(dimension.x() - 21), 5);
		put(BtOK, "BtOK", 92, static_cast<int16_t>(dimension.y() - 26));
		put(BtTree, "BtTree", 166, 76);
		put(BtLeft, "BtLeft", 12, 152);
		put(BtRight, "BtRight", 196, 152);
		put(BtJunior, "BtJuniorEntry", 96, 76);
		put(BtPrecept, "BtFamilyPrecept", 160, 76);
		put(BtSpecial, "BtSpecial", 150, 322);

		refresh_from_model();
		LOG(LOG_DEBUG, "UIFamily: open");
#endif
	}

	void UIFamily::refresh_from_model()
	{
		const FamilyModel& model = FamilyModel::get();
		info = model.info();

		banner_label.change_text(info.has_family() ? info.name : Locale::text("family.none"));
		precept_label.change_text(info.message);
		junior_label.change_text(
			Locale::format_all("family.junior_count",
			{ std::to_string(info.juniors), std::to_string(info.juniors_max) }));
		rep_label.change_text(
			Locale::format_all("family.rep_value",
			{ std::to_string(info.cur_rep), std::to_string(info.tot_rep) }));
		today_label.change_text(Locale::format("family.today_value", std::to_string(info.today_rep)));

		const auto& defs = model.privileges();
		if (priv_index >= (int32_t)defs.size())
			priv_index = defs.empty() ? 0 : (int32_t)defs.size() - 1;
		if (!defs.empty())
		{
			const FamilyPrivilegeDef& d = defs[priv_index];
			// 特权名旁附页码 [01/11]（底图计数框烘焙位；无独立 canvas 时文本叠绘）。
			const int32_t page_n = static_cast<int32_t>(defs.size());
			char pagebuf[24];
			std::snprintf(pagebuf, sizeof(pagebuf), " [%02d/%02d]",
				priv_index + 1, page_n);
			priv_title_label.change_text(d.name + pagebuf);
			priv_cost_label.change_text(std::to_string(d.rep_cost));
			priv_limit_label.change_text(priv_used_text());
			std::string desc;
			for (const std::string& line : split_priv_desc(d.desc))
			{
				if (!desc.empty()) desc += "\n";
				desc += line;
			}
			priv_desc_label.change_text(desc);
		}
		else
		{
			priv_title_label.change_text(Locale::text("family.priv_none"));
			priv_cost_label.change_text("");
			priv_limit_label.change_text("");
			priv_desc_label.change_text("");
		}
	}

	std::string UIFamily::priv_used_text() const
	{
		const auto& defs = FamilyModel::get().privileges();
		if (priv_index < 0 || priv_index >= (int32_t)defs.size())
			return "0/0";
		const FamilyPrivilegeDef& d = defs[priv_index];
		int used = FamilyModel::get().is_used(d.ordinal) ? 1 : 0;
		return std::to_string(used) + "/" + std::to_string(d.usage_limit);
	}

	void UIFamily::page_privilege(int8_t delta)
	{
		const auto& defs = FamilyModel::get().privileges();
		if (defs.empty())
			return;
		int32_t n = static_cast<int32_t>(defs.size());
		priv_index = (priv_index + delta) % n;
		if (priv_index < 0) priv_index += n;
		refresh_from_model();
		LOG(LOG_DEBUG, "UIFamily: priv page=" << priv_index);
	}

	void UIFamily::use_selected()
	{
		const auto& defs = FamilyModel::get().privileges();
		if (defs.empty() || priv_index < 0 || priv_index >= (int32_t)defs.size())
			return;
		const FamilyPrivilegeDef& d = defs[priv_index];
		if (FamilyModel::get().is_used(d.ordinal))
		{
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(Locale::text("family.priv_used").c_str(),
					UIChatBar::MessageType::YELLOW);
			return;
		}
		if (d.ordinal == 1)
		{
			// SUMMON_FAMILY 需要目标名：CharInfo 预名，否则物理键盘 / ChatBar 提示。
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyUsePacket(1, pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: Use summon target=" << pre);
			} else {
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						FamilyUsePacket(1, name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Use summon target=" << name);
					}, 12);
			}
			return;
		}
		// FAMILY_REUINION(0) 无目标；其余 ordinal 照发 FamilyUse（s9 只实现 0/1）。
		FamilyUsePacket(d.ordinal).dispatch();
		LOG(LOG_DEBUG, "UIFamily: Use ordinal=" << d.ordinal);
	}

	void UIFamily::draw(float inter) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		banner_label.draw(position + Point<int16_t>(112, 30));
		precept_label.draw(position + Point<int16_t>(8, 58));
		junior_label.draw(position + Point<int16_t>(92, 78));
		rep_label.draw(position + Point<int16_t>(212, 108));
		today_label.draw(position + Point<int16_t>(212, 126));
		priv_title_label.draw(position + Point<int16_t>(112, 156));
		priv_cost_label.draw(position + Point<int16_t>(140, 180));
		priv_limit_label.draw(position + Point<int16_t>(214, 180));
		priv_desc_label.draw(position + Point<int16_t>(10, 200));
		UIElement::draw_buttons(inter);
	}

	void UIFamily::send_key(int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && escape) { deactivate(); return; }
		if (!pressed) return;
		if (keycode == 'A' || keycode == 'a') {
			int32_t pre_id = 0;
			std::string pre_name;
			if (auto ci = UI::get().get_element<UICharInfo>()) {
				pre_id = ci->get_target_cid();
				pre_name = ci->get_target_name();
			}
			if (pre_id > 0 && !pre_name.empty()) {
				UI::get().emplace<UIYesNo>(
					Locale::format("family.ask_add", pre_name),
					[pre_id, pre_name](bool yes) {
						if (!yes) return;
						FamilyAddPacket(pre_name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Add senior/junior id=" << pre_id);
					});
			} else {
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						FamilyAddPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Add name=" << name);
					}, 12);
			}
		} else if (keycode == 'J' || keycode == 'j') {
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyAddPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: Add junior=" << pre);
			} else {
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						FamilyAddPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Add junior=" << name);
					}, 12);
			}
		} else if (keycode == 'P' || keycode == 'p') {
			// 箴言：Angel CUtilDlgEx → UIEnterString（禁 SoftKeyboard）。
			UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_precept"),
				[](std::string msg) {
					FamilyPreceptPacket(msg).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Precept len=" << msg.size());
				}, 100, true);
		} else if (keycode == 'T' || keycode == 't') {
			OpenFamilyPedigreePacket().dispatch();
			UI::get().emplace<UIFamilyTree>();
			LOG(LOG_DEBUG, "UIFamily: Tree");
		} else if (keycode == 'O' || keycode == 'o') {
			OpenFamilyPacket().dispatch();
			LOG(LOG_DEBUG, "UIFamily: OpenFamily");
		} else if (keycode == ',') {
			page_privilege(-1);
		} else if (keycode == '.') {
			page_privilege(1);
		} else if (keycode == 'U' || keycode == 'u') {
			use_selected();
		} else if (keycode == 'X' || keycode == 'x') {
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
				chat_hint("guild.need_select");
			}
		}
	}

	UIElement::Type UIFamily::get_type() const { return TYPE; }

	Button::State UIFamily::button_pressed(uint16_t id)
	{
		if (id == BtClose)
			deactivate();
		else if (id == BtOK)
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
				UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_name"),
					[](std::string name) {
						FamilyAddPacket(name).dispatch();
						LOG(LOG_DEBUG, "UIFamily: Add junior=" << name);
					}, 12);
			}
		} else if (id == BtPrecept) {
			UI::get().emplace<UIEnterString>(Locale::text("userlist.enter_precept"),
				[](std::string msg) {
					FamilyPreceptPacket(msg).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Precept len=" << msg.size());
				}, 100, true);
		} else if (id == BtSpecial) {
			use_selected();
		} else if (id == BtLeft) {
			page_privilege(-1);
		} else if (id == BtRight) {
			page_privilege(1);
		} else {
			OpenFamilyPedigreePacket().dispatch();
			LOG(LOG_DEBUG, "UIFamily: btn=" << id << " → pedigree");
		}
		return Button::State::NORMAL;
	}
}
