////////////////////////////////////////////////////////////////////////////////
//	学院（Family）UI + OPEN_FAMILY 出站 —— 2026-11-07 原生「学院.png」重排
////////////////////////////////////////////////////////////////////////////////
#include "UIFamily.h"
#include "UICharInfo.h"
#include "UIChatBar.h"
#include "UIFamilyTree.h"
#include "UISoftKey.h"
#include "UINotice.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/FamilyModel.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"

#include "../../Util/Locale.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
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
					// "#c" 标记：跳过 2 字符；闭合的独立 "#" 也跳过
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
		// 落位 = academy_top/priv_3x 实测行带（evidence/）；BtClose 12x12 右上。
		put(BtClose, "BtClose", static_cast<int16_t>(dimension.x() - 21), 5);
		// 确认（BtOK 40x22）：原生底部居中（academy_bottom_3x y≈368..383 实测 x≈95..131）
		put(BtOK, "BtOK", 92, static_cast<int16_t>(dimension.y() - 26));
		// 校谱（BtTree 55x18）：同学行右侧，紧挨「登录同学」（实测 y≈77..92）
		put(BtTree, "BtTree", 166, 76);
		// 翻页 13x13：[01/11] 计数框两侧（实测 y≈152..166）
		put(BtLeft, "BtLeft", 12, 152);
		put(BtRight, "BtRight", 196, 152);
		// 同学行：登录同学（BtJuniorEntry 69x18）+ 箴言（BtFamilyPrecept 11x20）
		put(BtJunior, "BtJuniorEntry", 96, 76);
		put(BtPrecept, "BtFamilyPrecept", 160, 76);
		// 使用特权（BtSpecial 57x18）：面板右下（实测 y≈322..337 x≈154..207）
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
			priv_title_label.change_text(d.name);
			// 两个数值位分离（原生「消耗名声 300」「限制次数 0/1」各有烘焙标签）：
			// 值各自右对齐到对应标签的空白槽（academy_priv_3x 实测 x≈140 / x≈214）。
			priv_cost_label.change_text(std::to_string(d.rep_cost));
			priv_limit_label.change_text(priv_used_text());
			// 描述多行：Text 带 maxwidth 自动换行 + 手动 \n 已折进原文，
			// 这里把 s9 的 \\n 转义换成真空行（Text 支持 \n）。
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
			// SUMMON_FAMILY 需要目标名：CharInfo 预名，否则 SoftKey。
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				FamilyUsePacket(1, pre).dispatch();
				LOG(LOG_DEBUG, "UIFamily: Use summon target=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& name) {
					if (name.empty()) return;
					FamilyUsePacket(1, name).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Use summon target=" << name);
				}, []() {}, Locale::text("family.softkey_member"));
			}
			return;
		}
		// FAMILY_REUINION(0) 无目标；其余 ordinal 原生也走「使用」→ s9 FamilyUseHandler
		// 只实现 0/1 两支，其它 ordinal 服务端回 FAMILY_RESULT 错误码，照发不误。
		FamilyUsePacket(d.ordinal).dispatch();
		LOG(LOG_DEBUG, "UIFamily: Use ordinal=" << d.ordinal);
	}

	void UIFamily::draw(float inter) const
	{
		if (!has_assets) return;
		if (backgrnd) backgrnd.draw(position);
		// 动态件落位（相对窗左上；行带 = academy_top/priv_3x 实测）
		banner_label.draw(position + Point<int16_t>(112, 30));		// 蓝横幅内居中
		precept_label.draw(position + Point<int16_t>(8, 58));			// 白箴言行
		junior_label.draw(position + Point<int16_t>(92, 78));			// 同学 0/2（右对齐到 x92）
		rep_label.draw(position + Point<int16_t>(212, 108));			// 名声度值（右对齐）
		today_label.draw(position + Point<int16_t>(212, 126));		// 今日名声（右对齐）
		priv_title_label.draw(position + Point<int16_t>(112, 156));	// 特权名（计数框行居中）
		priv_cost_label.draw(position + Point<int16_t>(140, 180));	// 消耗名声 值（右对齐）
		priv_limit_label.draw(position + Point<int16_t>(214, 180));	// 限制次数 值（右对齐）
		priv_desc_label.draw(position + Point<int16_t>(10, 200));		// 描述多行
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
				}, []() {}, Locale::text("family.softkey_inviter_name"));
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
						}, []() {}, Locale::text("family.softkey_inviter_name"));
					}
				}, []() {}, Locale::text("family.softkey_inviter_cid"));
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
				}, []() {}, Locale::text("family.softkey_junior"));
			}
		} else if (keycode == 'P' || keycode == 'p') {
			UI::get().emplace<UISoftKey>([](const std::string& msg) {
				if (!msg.empty()) {
					FamilyPreceptPacket(msg).dispatch();
					LOG(LOG_DEBUG, "UIFamily: Precept len=" << msg.size());
				}
			}, []() {}, Locale::text("family.softkey_precept"));
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
				}, []() {}, Locale::text("family.softkey_junior_cid"));
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
				UI::get().emplace<UISoftKey>([](const std::string& name) {
					if (!name.empty())
						FamilyAddPacket(name).dispatch();
				}, []() {}, Locale::text("family.softkey_junior"));
			}
		} else if (id == BtPrecept) {
			UI::get().emplace<UISoftKey>([](const std::string& msg) {
				if (!msg.empty())
					FamilyPreceptPacket(msg).dispatch();
			}, []() {}, Locale::text("family.softkey_precept"));
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
