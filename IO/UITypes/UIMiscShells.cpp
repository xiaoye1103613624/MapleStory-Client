#include "UIMiscShells.h"
#include "../UI.h"
#include "../Components/MapleButton.h"
#include "UIAdmin.h"
#include "UICharInfo.h"
#include "UIChatBar.h"
#include "UIDelivery.h"
#include "UIEquipInventory.h"
#include "UIAlliance.h"
#include "UIChannel.h"
#include "UICygnusCreation.h"
#include "UIFamily.h"
#include "UIGuild.h"
#include "UIMaker.h"
#include "UIMemo.h"
#include "UIMinigame.h"
#include "UIMonsterBook.h"
#include "UIPartySearch.h"
#include "UIRemoteGachapon.h"
#include "UIStoreBank.h"
#include "UIWedding.h"
#include "UIWorldMap.h"
#include "UIEvent.h"
#include "UIItemInventory.h"
#include "UIMonsterCarnival.h"
#include "UIAriantMatch.h"
#include "UIPartyRace.h"
#include "UIPlant.h"
#include "UITreasureBox.h"
#include "UIBossHP.h"
#include "UIStatusMessenger.h"
#include "UIMapleTV.h"
#include "UIFamilyTree.h"
#include "UINewYearsCard.h"
#include "UIBuffList.h"
#include "UIDailyGift.h"
#include "UIPartySearch.h"
#include "UIQuestLog.h"
#include "UISkillBook.h"
#include "UISoftKey.h"
#include "UIStatsInfo.h"
#include "UIUserList.h"
#include "UIBuddyList.h"
#include "UIParty.h"
#include "../Components/Textfield.h"
#include "../../Audio/Audio.h"
#include "../../Util/Locale.h"
#include "../../Constants.h"
#include "../../Character/Inventory/InventoryType.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Net/Packets/TradePackets.h"
#include "../../Net/Packets/BuddyPackets.h"
#include "../../Net/Packets/CashItemPackets.h"
#include "../../Net/Packets/NotePackets.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Net/Packets/SkillMacroPackets.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/GameplayPackets.h"
#include "../../Net/Packets/AttackAndSkillPackets.h"
#include "../../Net/Packets/MonsterBookPackets.h"
#include "../../Net/Packets/QuestPackets.h"
#include "../../Net/Packets/InventoryPackets.h"
#include "../../Net/Packets/GuildPackets.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Net/Packets/PartyPackets.h"
#include "../../Net/Packets/MessengerPackets.h"
#include "UIRpsGame.h"
#include "../../Character/SkillMacros.h"
#include "../../Data/SkillData.h"
#include "UIJoypad.h"
#include "UIKeyConfig.h"
#include "UIMessenger.h"
#include "UIOptionMenu.h"
#include "UIQuit.h"
#include "../../Template/Rectangle.h"
// MinigameOmokPlacePacket / MatchCardSelectPacket
#include "../../Util/UiAssetResolve.h"
#include "../../Util/Misc.h"
#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif
#include <algorithm>
namespace ms {
namespace {
int32_t nearest_map_oid(MapObjects* objs)
{
	if (!objs || objs->size() == 0)
		return 0;
	Point<int16_t> p = Stage::get().get_player().get_position();
	int32_t best = 0;
	int64_t bestd = -1;
	for (auto it = objs->begin(); it != objs->end(); ++it) {
		if (!it->second)
			continue;
		Point<int16_t> np = it->second->get_position();
		int64_t dx = static_cast<int64_t>(np.x()) - p.x();
		int64_t dy = static_cast<int64_t>(np.y()) - p.y();
		int64_t d = dx * dx + dy * dy;
		if (bestd < 0 || d < bestd) {
			bestd = d;
			best = it->second->get_oid();
		}
	}
	return best;
}
int32_t nearest_npc_oid()
{
	return nearest_map_oid(Stage::get().get_npcs().get_npcs());
}
int32_t nearest_reactor_oid()
{
	return nearest_map_oid(Stage::get().get_reactors().get_reactors());
}
// 店铺上架：有背包选中则直接 SetItem(qty=1,slot=0)，否则 SoftKey iv,pos,qty,slot
template <typename TipFn>
void softkey_or_inv_set_item(TipFn tip_fn)
{
	if (auto inv = UI::get().get_element<UIItemInventory>()) {
		int16_t pos = inv->get_selected_slot();
		if (pos > 0) {
			int8_t iv = static_cast<int8_t>(inv->get_tab());
			HiredMerchantSetItemPacket(iv, pos, 1, 0).dispatch();
			tip_fn("SetItem #" + std::to_string(pos));
			LOG(LOG_DEBUG, "Shop: SetItem from inv tab=" << (int)iv << " pos=" << pos);
			return;
		}
	}
	UI::get().emplace<UISoftKey>([tip_fn](const std::string& spec) {
		int vals[4] = { 2, 1, 1, 0 };
		size_t start = 0; int vi = 0;
		while (vi < 4 && start <= spec.size()) {
			size_t c = spec.find(',', start);
			std::string part = (c == std::string::npos) ? spec.substr(start) : spec.substr(start, c - start);
			try { vals[vi] = std::stoi(part); } catch (...) {}
			++vi;
			if (c == std::string::npos) break;
			start = c + 1;
		}
		HiredMerchantSetItemPacket(
			static_cast<int8_t>(vals[0]),
			static_cast<int16_t>(vals[1]),
			static_cast<int16_t>(vals[2]),
			static_cast<int8_t>(vals[3])
		).dispatch();
		tip_fn("SetItem " + spec);
		LOG(LOG_DEBUG, "Shop: SoftKey SetItem " << spec);
	}, []() {}, "iv,pos,qty,slot");
}
bool load_window_bg(const char* name, Texture& out, Point<int16_t>& dim) {
#ifdef USE_NX
	nl::node root = UiAssetResolve::window(name);
	if (!root) return false;
	// 多数窗用 backgrnd；UtilDlgEx* / QuestIcon 等用 t 顶片或 Cover。
	nl::node bg = root["backgrnd"] ? root["backgrnd"] : root["backgrd"];
	if (!bg) bg = root["t"];
	if (!bg) bg = root["Cover"];
	if (!bg && root["0"]) bg = root["0"]["Cover"] ? root["0"]["Cover"] : root["0"]["backgrnd"];
	if (!bg) return false;
	out = Texture(bg);
	Point<int16_t> o = out.get_origin();
	if (o.x() || o.y()) out.shift(o);
	dim = out.get_dimensions();
	return true;
#else
	(void)name; (void)out; (void)dim; return false;
#endif
}

// prefer_player_shop：优先 514* 个人店，否则 503* 雇佣商人（s9 ItemConstants）
bool find_shop_permit(int16_t& slot, int32_t& itemid, bool prefer_player_shop)
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	int16_t max = inv.get_slotmax(InventoryType::Id::CASH);
	auto scan = [&](int prefix) -> bool {
		for (int16_t s = 1; s <= max; ++s) {
			int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
			if (id / 10000 == prefix) { slot = s; itemid = id; return true; }
		}
		return false;
	};
	if (prefer_player_shop) {
		if (scan(514)) return true;
		return scan(503);
	}
	if (scan(503)) return true;
	return scan(514);
}

void dispatch_shop_create_or_open(const std::string& title, bool prefer_player_shop, Text& tip)
{
	auto sm_tip = [](Color::Name color, const std::string& msg) {
		auto sm = UI::get().get_element<UIStatusMessenger>();
		if (!sm) sm = UI::get().emplace<UIStatusMessenger>();
		if (sm) sm->show_status(color, msg);
	};
	if (title.empty()) {
		HiredMerchantOpenPacket().dispatch();
		tip.change_text(Locale::text("common.open_store"));
		sm_tip(Color::Name::WHITE, Locale::text("common.open_store"));
		return;
	}
	int16_t slot = 0; int32_t itemid = 0;
	if (!find_shop_permit(slot, itemid, prefer_player_shop)) {
		tip.change_text(Locale::text("shop.no_514_503_permit"));
		sm_tip(Color::Name::RED, Locale::text("shop.no_514_503_permit"));
		return;
	}
	// s9：createType 4|5 均为 shop；514* 用 4，503* 用 5
	int8_t ctype = (itemid / 10000 == 503) ? 5 : 4;
	HiredMerchantCreatePacket(title, itemid, ctype).dispatch();
	tip.change_text(Locale::format("shop.create", title));
	sm_tip(Color::Name::YELLOW, Locale::format("shop.create", title));
	LOG(LOG_DEBUG, "Shop CREATE titleLen=" << title.size() << " item=" << itemid << " type=" << (int)ctype);
	(void)slot;
}

}

UIReport::UIReport()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("report.report_softkey_name"))
{
	position = Point<int16_t>(320, 180); dimension = Point<int16_t>(154, 166); dragarea = Point<int16_t>(154, 20);
	has_assets = load_window_bg("Report", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIReport: missing");
	center_on_screen();
	{
		std::string pre = preset_target;
		if (pre.empty()) {
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
		}
		if (!pre.empty()) {
			ReportPlayerPacket(pre, "client report").dispatch();
			tip_label.change_text(Locale::format("report.reported", pre));
			LOG(LOG_DEBUG, "UIReport: REPORT " << pre);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& victim) {
				std::string name = victim.empty() ? preset_target : victim;
				if (name.empty()) { tip_label.change_text(Locale::text("common.empty_name")); return; }
				ReportPlayerPacket(name, "client report").dispatch();
				tip_label.change_text(Locale::format("report.reported", name));
				LOG(LOG_DEBUG, "UIReport: REPORT " << name);
			}, []() {}, "Report target");
		}
	}
}
void UIReport::set_target(const std::string& name)
{
	preset_target = name;
	if (!name.empty())
		tip_label.change_text(Locale::format("report.target_softkey_enter", name));
}
void UIReport::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(8, 36));
}
void UIReport::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13 && !preset_target.empty()) {
		ReportPlayerPacket(preset_target, "client report").dispatch();
		tip_label.change_text(Locale::format("report.reported", preset_target));
		return;
	}
	if (keycode == 'S' || keycode == 's' || keycode == 'R' || keycode == 'r') {
		std::string pre = preset_target;
		if (pre.empty()) {
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
		}
		if (!pre.empty()) {
			ReportPlayerPacket(pre, "client report").dispatch();
			tip_label.change_text(Locale::format("report.reported", pre));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& victim) {
				std::string name = victim.empty() ? preset_target : victim;
				if (name.empty()) { tip_label.change_text(Locale::text("common.empty_name")); return; }
				ReportPlayerPacket(name, "client report").dispatch();
				tip_label.change_text(Locale::format("report.reported", name));
			}, []() {}, "Report target");
		}
	} else if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIReport::get_type() const { return TYPE; }

UICoupon::UICoupon() : tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("coupon.coupon_enter_for_code")) {
	position = Point<int16_t>(280, 200); dimension = Point<int16_t>(266, 125); dragarea = Point<int16_t>(266, 20);
	has_assets = load_window_bg("Coupon", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICoupon: missing");
	center_on_screen();
	// SoftKey → s9 CouponCodeHandler (RecvOpcode.COUPON_CODE=0xE6)
	UI::get().emplace<UISoftKey>([this](const std::string& code) {
		if (code.empty()) {
			tip_label.change_text(Locale::text("coupon.empty_code"));
			return;
		}
		CouponCodePacket(code).dispatch();
		tip_label.change_text(Locale::text("common.sent"));
		LOG(LOG_DEBUG, "UICoupon: COUPON_CODE len=" << code.size());
	}, []() {}, "Coupon code");
}
void UICoupon::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UICoupon::draw(float) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(16, 40));
}
void UICoupon::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 13) {
		UI::get().emplace<UISoftKey>([this](const std::string& code) {
			if (code.empty()) return;
			CouponCodePacket(code).dispatch();
			tip_label.change_text(Locale::text("common.sent"));
		}, []() {}, "Coupon code");
	} else if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UICoupon::get_type() const { return TYPE; }

UIClaim::UIClaim() : tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("claim.claim_softkey_to_msg")) {
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(220, 160); dragarea = Point<int16_t>(220, 20);
	has_assets = load_window_bg("Claim", backgrnd, dimension);
	if (!has_assets) has_assets = load_window_bg("AdminClaim", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIClaim: missing");
	center_on_screen();
	{
		auto ask_msg = [this](const std::string& to) {
			if (to.empty()) { tip_label.change_text(Locale::text("claim.need_name")); return; }
			UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
				NoteSendPacket(to, msg).dispatch();
				tip_label.change_text(Locale::format("common.note", to));
			}, []() {}, "Claim msg");
		};
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty())
			ask_msg(pre);
		else
			UI::get().emplace<UISoftKey>(ask_msg, []() {}, "Claim to");
	}
}
void UIClaim::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIClaim::draw(float) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(16, 40));
}
void UIClaim::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'N' || keycode == 'n') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			std::string to = pre;
			UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
				NoteSendPacket(to, msg).dispatch();
				tip_label.change_text(Locale::format("common.note", to));
			}, []() {}, "Claim msg");
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& to) {
				if (to.empty()) return;
				UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
					NoteSendPacket(to, msg).dispatch();
					tip_label.change_text(Locale::format("common.note", to));
				}, []() {}, "Claim msg");
			}, []() {}, "Claim to");
		}
	} else if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIClaim::get_type() const { return TYPE; }

UIIncubator::UIIncubator() : tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("incubator.drop_egg_softkey_slot")) {
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(200, 160); dragarea = Point<int16_t>(200, 20);
	has_assets = load_window_bg("Incubator", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIIncubator: missing");
	center_on_screen();
	{
		int16_t from_inv = 0;
		InventoryType::Id tab = InventoryType::Id::USE;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			from_inv = inv->get_selected_slot();
			if (from_inv > 0)
				tab = inv->get_tab();
		}
		if (from_inv > 0) {
			egg_inv = static_cast<int8_t>(tab);
			egg_slot = from_inv;
			tip_label.change_text(Locale::format("incubator.egg_enter", std::to_string(from_inv)));
			LOG(LOG_DEBUG, "UIIncubator: SoftKey egg slot=" << from_inv);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						egg_inv = static_cast<int8_t>(InventoryType::Id::USE);
						egg_slot = slot;
						tip_label.change_text(Locale::format("incubator.egg_enter", s));
						LOG(LOG_DEBUG, "UIIncubator: SoftKey egg slot=" << slot);
					}
				} catch (...) {}
			}, []() {}, "Egg USE slot");
		}
	}
}
void UIIncubator::set_tip(const std::string& tip) { tip_label.change_text(tip); }
bool UIIncubator::find_incubator(int16_t& slot, int32_t& itemid) const
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int32_t id : { 5060002 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) { slot = s; itemid = id; return true; }
	}
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 == 506) { slot = i; itemid = id; return true; }
	}
	return false;
}
bool UIIncubator::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	egg_inv = static_cast<int8_t>(icon.get_inventory_tab());
	egg_slot = icon.get_inventory_slot();
	tip_label.change_text(Locale::format("incubator.egg_enter", std::to_string(egg_slot)));
	return true;
}
void UIIncubator::draw(float) const {
	if (!has_assets) return;
	if (backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(16, 40));
}
void UIIncubator::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13) { button_pressed(BtOk); return; }
	if (keycode == 'S' || keycode == 's' || keycode == 'E' || keycode == 'e') {
		int16_t from_inv = 0;
		InventoryType::Id tab = InventoryType::Id::USE;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			from_inv = inv->get_selected_slot();
			if (from_inv > 0)
				tab = inv->get_tab();
		}
		if (from_inv > 0) {
			egg_inv = static_cast<int8_t>(tab);
			egg_slot = from_inv;
			tip_label.change_text(Locale::format("incubator.egg_enter", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						egg_inv = static_cast<int8_t>(InventoryType::Id::USE);
						egg_slot = slot;
						tip_label.change_text(Locale::format("incubator.egg_enter", s));
					}
				} catch (...) {}
			}, []() {}, "Egg USE slot");
		}
	} else if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIIncubator::get_type() const { return TYPE; }
Button::State UIIncubator::button_pressed(uint16_t)
{
	if (egg_slot <= 0) {
		tip_label.change_text(Locale::text("incubator.drop_an_egg_first"));
		return Button::State::NORMAL;
	}
	int16_t slot = 0; int32_t itemid = 0;
	if (!find_incubator(slot, itemid)) {
		tip_label.change_text(Locale::text("incubator.no_incubator"));
		return Button::State::NORMAL;
	}
	UseIncubatorPacket(slot, itemid, egg_inv, egg_slot).dispatch();
	tip_label.change_text(Locale::text("common.sent"));
	return Button::State::NORMAL;
}

UIRadio::UIRadio() {
	update_screen(Constants::Constants::get().get_viewwidth(), Constants::Constants::get().get_viewheight());
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("Radio");
	if (!root) { LOG(LOG_WARN, "UIRadio: missing"); return; }
	has_assets = true;
	if (root["On"]["0"]) tex_on = Texture(root["On"]["0"]);
	if (root["Off"]["0"]) tex_off = Texture(root["Off"]["0"]);
#endif
}
void UIRadio::set_on(bool v) { on = v; }
void UIRadio::draw(float) const {
	if (!has_assets) return;
	if (on && tex_on) tex_on.draw(position);
	else if (tex_off) tex_off.draw(position);
}
void UIRadio::update_screen(int16_t new_width, int16_t) {
	position = Point<int16_t>(static_cast<int16_t>(new_width - 70), 120);
	dimension = Point<int16_t>(54, 32);
}
Cursor::State UIRadio::send_cursor(bool clicked, Point<int16_t> cursorpos)
{
	Rectangle<int16_t> hit{ position, position + dimension };
	if (hit.contains(cursorpos)) {
		if (clicked) {
			on = !on;
			LOG(LOG_DEBUG, "UIRadio: toggle on=" << on);
			if (on) {
				UI::get().emplace<UISoftKey>([](const std::string& ch) {
					if (ch.empty()) return;
					try {
						int n = std::stoi(ch);
						if (n >= 1 && n <= 20) {
							ChangeChannelPacket(static_cast<uint8_t>(n - 1)).dispatch();
							LOG(LOG_DEBUG, "UIRadio: ChangeChannel ch=" << n);
							return;
						}
					} catch (...) {}
					auto ui = UI::get().get_element<UIChannel>();
					if (!ui) ui = UI::get().emplace<UIChannel>();
					else if (!ui->is_active()) ui->makeactive();
				}, []() {}, "Channel 1-N");
			}
		}
		return Cursor::State::CANCLICK;
	}
	return Cursor::State::IDLE;
}
void UIRadio::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'T' || keycode == 't' || keycode == ' ') {
		on = !on;
		LOG(LOG_DEBUG, "UIRadio: key toggle on=" << on);
		if (on) {
			UI::get().emplace<UISoftKey>([](const std::string& ch) {
				if (ch.empty()) return;
				try {
					int n = std::stoi(ch);
					if (n >= 1 && n <= 20) {
						ChangeChannelPacket(static_cast<uint8_t>(n - 1)).dispatch();
						return;
					}
				} catch (...) {}
				auto ui = UI::get().get_element<UIChannel>();
				if (!ui) ui = UI::get().emplace<UIChannel>();
				else if (!ui->is_active()) ui->makeactive();
			}, []() {}, "Channel 1-N");
		}
	} else if (keycode == 'S' || keycode == 's' || keycode == 'C' || keycode == 'c') {
		UI::get().emplace<UISoftKey>([](const std::string& ch) {
			if (ch.empty()) return;
			try {
				int n = std::stoi(ch);
				if (n >= 1 && n <= 20) {
					ChangeChannelPacket(static_cast<uint8_t>(n - 1)).dispatch();
					return;
				}
			} catch (...) {}
			auto ui = UI::get().get_element<UIChannel>();
			if (!ui) ui = UI::get().emplace<UIChannel>();
			else if (!ui->is_active()) ui->makeactive();
		}, []() {}, "Channel 1-N");
	} else if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIRadio::get_type() const { return TYPE; }

UIKarmaScissors::UIKarmaScissors()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("common.drop_equip_softkey_slot"))
{
	position = Point<int16_t>(320, 180); dimension = Point<int16_t>(178, 179); dragarea = Point<int16_t>(178, 20);
	has_assets = load_window_bg("KarmaScissors", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIKarmaScissors: missing");
	center_on_screen();
	{
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::EQUIP)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			equip_slot = from_inv;
			tip_label.change_text(Locale::format("common.equip_enter", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						equip_slot = slot;
						tip_label.change_text(Locale::format("common.equip_enter", s));
					}
				} catch (...) {}
			}, []() {}, "Equip slot");
		}
	}
}
bool UIKarmaScissors::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	if (icon.get_inventory_tab() != InventoryType::Id::EQUIP) {
		tip_label.change_text(Locale::text("common.need_equip"));
		return true;
	}
	equip_slot = icon.get_inventory_slot();
	tip_label.change_text(Locale::format("common.equip_enter_to_use", std::to_string(equip_slot)));
	return true;
}
void UIKarmaScissors::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIKarmaScissors::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13) { button_pressed(BtOk); return; }
	if (keycode == 'S' || keycode == 's' || keycode == 'E' || keycode == 'e') {
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::EQUIP)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			equip_slot = from_inv;
			tip_label.change_text(Locale::format("common.equip_enter", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						equip_slot = slot;
						tip_label.change_text(Locale::format("common.equip_enter", s));
					}
				} catch (...) {}
			}, []() {}, "Equip slot");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIKarmaScissors::get_type() const { return TYPE; }
Button::State UIKarmaScissors::button_pressed(uint16_t)
{
	if (equip_slot <= 0) {
		tip_label.change_text(Locale::text("common.drop_an_equip_first"));
		return Button::State::NORMAL;
	}
	const Inventory& inv = Stage::get().get_player().get_inventory();
	int16_t cash_slot = 0;
	int32_t cash_id = 0;
	for (int32_t id : { 5520000, 5520001 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) { cash_slot = s; cash_id = id; break; }
	}
	if (cash_slot <= 0) {
		tip_label.change_text(Locale::text("karmascissors.no_karma_scissors"));
		return Button::State::NORMAL;
	}
	UseKarmaScissorsPacket(cash_slot, cash_id, equip_slot).dispatch();
	tip_label.change_text(Locale::text("common.sent"));
	return Button::State::NORMAL;
}

UIEnchantSkill::UIEnchantSkill()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Locale::text("enchantskill.e_equip_s_scroll_enter"))
{
	position = Point<int16_t>(300, 200); dimension = Point<int16_t>(172, 86); dragarea = Point<int16_t>(172, 20);
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("EnchantSkill");
	if (!root || !root["Effect"]["0"]) { LOG(LOG_WARN, "UIEnchantSkill: missing"); return; }
	has_assets = true;
	effect = Texture(root["Effect"]["0"]);
	Point<int16_t> o = effect.get_origin();
	if (o.x() || o.y()) effect.shift(o);
	dimension = effect.get_dimensions();
	dragarea = Point<int16_t>(dimension.x(), 20);
	center_on_screen();
#endif
}
bool UIEnchantSkill::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	if (icon.get_inventory_tab() != InventoryType::Id::EQUIP) {
		tip_label.change_text(Locale::text("common.need_equip"));
		return true;
	}
	equip_slot = icon.get_inventory_slot();
	tip_label.change_text(Locale::format("enchantskill.enchant_slot", std::to_string(equip_slot)));
	return true;
}
void UIEnchantSkill::draw(float) const
{
	if (has_assets && effect) effect.draw(position);
	tip_label.draw(position + Point<int16_t>(8, 8));
}
void UIEnchantSkill::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13) {
		if (scroll_slot > 0 && equip_slot > 0) {
			ScrollEquipPacket(scroll_slot, static_cast<EquipSlot::Id>(equip_slot)).dispatch();
			tip_label.change_text(Locale::format_all("enchantskill.scroll", {std::to_string(scroll_slot), std::to_string(equip_slot)}));
			LOG(LOG_DEBUG, "UIEnchantSkill: ScrollEquip src=" << scroll_slot << " dst=" << equip_slot);
		} else {
			tip_label.change_text(Locale::text("enchantskill.need_scroll_equip"));
		}
		return;
	}
	if (keycode == 'E' || keycode == 'e') {
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::EQUIP)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			equip_slot = from_inv;
			tip_label.change_text(Locale::format("enchantskill.equip", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						equip_slot = slot;
						tip_label.change_text(Locale::format("enchantskill.equip", s));
					}
				} catch (...) {}
			}, []() {}, "Equip slot id");
		}
	} else if (keycode == 'S' || keycode == 's') {
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::USE)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			scroll_slot = from_inv;
			tip_label.change_text(Locale::format("enchantskill.scroll_2", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						scroll_slot = slot;
						tip_label.change_text(Locale::format("enchantskill.scroll_2", s));
					}
				} catch (...) {}
			}, []() {}, "Scroll USE slot");
		}
	} else if (keycode == 'R' || keycode == 'r') {
		if (scroll_slot > 0 && equip_slot > 0) {
			ScrollEquipPacket(scroll_slot, static_cast<EquipSlot::Id>(equip_slot)).dispatch();
			tip_label.change_text(Locale::format_all("enchantskill.scroll", {std::to_string(scroll_slot), std::to_string(equip_slot)}));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& spec) {
				size_t c = spec.find(',');
				if (c == std::string::npos) return;
				try {
					int16_t src = static_cast<int16_t>(std::stoi(spec.substr(0, c)));
					int16_t dst = static_cast<int16_t>(std::stoi(spec.substr(c + 1)));
					if (src > 0 && dst > 0) {
						scroll_slot = src;
						equip_slot = dst;
						ScrollEquipPacket(src, static_cast<EquipSlot::Id>(dst)).dispatch();
						tip_label.change_text(Locale::format("enchantskill.scroll_3", spec));
					}
				} catch (...) {}
			}, []() {}, "scroll,equip");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIEnchantSkill::get_type() const { return TYPE; }

UIItemProtector::UIItemProtector()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("common.drop_equip_softkey_slot"))
{
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(200, 160); dragarea = Point<int16_t>(200, 20);
	has_assets = load_window_bg("ItemProtector", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIItemProtector: missing");
	center_on_screen();
	{
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::EQUIP)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			equip_slot = from_inv;
			tip_label.change_text(Locale::format("common.equip_enter", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						equip_slot = slot;
						tip_label.change_text(Locale::format("common.equip_enter", s));
					}
				} catch (...) {}
			}, []() {}, "Equip slot");
		}
	}
}
bool UIItemProtector::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	if (icon.get_inventory_tab() != InventoryType::Id::EQUIP) {
		tip_label.change_text(Locale::text("common.need_equip"));
		return true;
	}
	equip_slot = icon.get_inventory_slot();
	tip_label.change_text(Locale::format("common.equip_enter_to_use", std::to_string(equip_slot)));
	return true;
}
void UIItemProtector::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIItemProtector::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13) { button_pressed(BtOk); return; }
	if (keycode == 'S' || keycode == 's' || keycode == 'E' || keycode == 'e') {
		int16_t from_inv = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::EQUIP)
				from_inv = inv->get_selected_slot();
		}
		if (from_inv > 0) {
			equip_slot = from_inv;
			tip_label.change_text(Locale::format("common.equip_enter", std::to_string(from_inv)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot > 0) {
						equip_slot = slot;
						tip_label.change_text(Locale::format("common.equip_enter", s));
					}
				} catch (...) {}
			}, []() {}, "Equip slot");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIItemProtector::get_type() const { return TYPE; }
Button::State UIItemProtector::button_pressed(uint16_t)
{
	if (equip_slot <= 0) {
		tip_label.change_text(Locale::text("common.drop_an_equip_first"));
		return Button::State::NORMAL;
	}
	const Inventory& inv = Stage::get().get_player().get_inventory();
	int16_t cash_slot = 0;
	int32_t cash_id = 0;
	for (int32_t id : { 5590000, 5590001 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) { cash_slot = s; cash_id = id; break; }
	}
	if (cash_slot <= 0) {
		tip_label.change_text(Locale::text("itemprotector.no_item_protector"));
		return Button::State::NORMAL;
	}
	UseItemProtectorPacket(cash_slot, cash_id, equip_slot).dispatch();
	tip_label.change_text(Locale::text("common.sent"));
	return Button::State::NORMAL;
}

UITeleport::UITeleport()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("teleport.teleport_rock"))
{
	position = Point<int16_t>(280, 160); dimension = Point<int16_t>(260, 200); dragarea = Point<int16_t>(260, 20);
	has_assets = load_window_bg("Teleport", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UITeleport: missing");
	center_on_screen();
	refresh_tip();
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto do_warp = [this](const std::string& input) {
			int16_t slot = 0; int32_t itemid = 0; bool vip = false;
			if (!find_rock(slot, itemid, vip)) {
				tip_label.change_text(Locale::text("teleport.no_teleport_rock"));
				return;
			}
			if (input.empty()) {
				tip_label.change_text(Locale::text("teleport.need_map_name"));
				return;
			}
			bool all_digit = true;
			for (char c : input) {
				if (c < '0' || c > '9') { all_digit = false; break; }
			}
			if (all_digit) {
				UseTeleportRockPacket(slot, itemid, std::stoi(input)).dispatch();
				tip_label.change_text(Locale::text("teleport.warp_map"));
			} else if (vip || itemid / 1000 >= 5041) {
				UseTeleportRockToPlayerPacket(slot, itemid, input).dispatch();
				tip_label.change_text(Locale::format("teleport.warp_to", input));
			} else {
				tip_label.change_text(Locale::text("teleport.need_vip_rock_for_name"));
			}
		};
		if (!pre.empty())
			do_warp(pre);
		else
			UI::get().emplace<UISoftKey>(do_warp, []() {}, "Map ID / player");
	}
}
void UITeleport::refresh_tip()
{
	const TeleportRock& rock = Stage::get().get_player().get_teleportrock();
	int32_t map = rock.first_valid(false);
	int32_t vipmap = rock.first_valid(true);
	// R27-B1c-6：整句建键（原分四段拼装，段间空格紧贴变量；ini 加载器只 trim 两端，
	// 拆段搬会把「 #」「 VIP#」的前导空格吃掉 ⇒ 按三个完整分支各建一条整句，值逐字=拼接产物）。
	if (map)
		tip_label.change_text(Locale::format("teleport.tip_first_map", std::to_string(map)));
	else if (vipmap)
		tip_label.change_text(Locale::format("teleport.tip_vip_map", std::to_string(vipmap)));
	else
		tip_label.change_text(Locale::text("teleport.tip_empty"));
}
bool UITeleport::find_rock(int16_t& slot, int32_t& itemid, bool& vip) const
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int32_t id : { 5041000, 5040000, 5040001, 5040002, 5040003, 5040004 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) {
			slot = s; itemid = id; vip = (id / 1000 >= 5041); return true;
		}
	}
	// 泛搜 504*：找任意现金栏缩地石
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 == 504) {
			slot = i; itemid = id; vip = (id / 1000 >= 5041); return true;
		}
	}
	return false;
}
void UITeleport::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UITeleport::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(16, 40));
}
void UITeleport::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13) { button_pressed(BtOk); return; }
	if (keycode == 'S' || keycode == 's') { button_pressed(BtSave); return; }
	if (keycode == 'T' || keycode == 't' || keycode == 'W' || keycode == 'w') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto do_warp = [this](const std::string& input) {
			int16_t slot = 0; int32_t itemid = 0; bool vip = false;
			if (!find_rock(slot, itemid, vip)) {
				tip_label.change_text(Locale::text("teleport.no_teleport_rock"));
				return;
			}
			if (input.empty()) {
				if (auto wm = UI::get().get_element<UIWorldMap>()) {
					int32_t mid = wm->get_selected_map_id();
					if (mid > 0) {
						UseTeleportRockPacket(slot, itemid, mid).dispatch();
						tip_label.change_text(Locale::text("teleport.warp_map"));
						return;
					}
				}
				tip_label.change_text(Locale::text("teleport.need_map_name"));
				return;
			}
			bool all_digit = true;
			for (char c : input) {
				if (c < '0' || c > '9') { all_digit = false; break; }
			}
			if (all_digit) {
				UseTeleportRockPacket(slot, itemid, std::stoi(input)).dispatch();
				tip_label.change_text(Locale::text("teleport.warp_map"));
			} else if (vip || itemid / 1000 >= 5041) {
				UseTeleportRockToPlayerPacket(slot, itemid, input).dispatch();
				tip_label.change_text(Locale::format("teleport.warp_to", input));
			} else {
				tip_label.change_text(Locale::text("teleport.need_vip_rock_for_name"));
			}
		};
		if (!pre.empty())
			do_warp(pre);
		else
			UI::get().emplace<UISoftKey>(do_warp, []() {}, "Map ID / player|empty=WMap");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UITeleport::get_type() const { return TYPE; }
Button::State UITeleport::button_pressed(uint16_t id)
{
	int16_t slot = 0; int32_t itemid = 0; bool vip = false;
	if (!find_rock(slot, itemid, vip)) {
		tip_label.change_text(Locale::text("teleport.no_teleport_rock"));
		return Button::State::NORMAL;
	}
	if (id == BtSave) {
		TrockAddMapPacket(vip).dispatch();
		tip_label.change_text(Locale::text("teleport.saved_map"));
		return Button::State::NORMAL;
	}
	if (auto wm = UI::get().get_element<UIWorldMap>()) {
		int32_t mid = wm->get_selected_map_id();
		if (mid > 0) {
			UseTeleportRockPacket(slot, itemid, mid).dispatch();
			tip_label.change_text(Locale::text("teleport.warp_map"));
			return Button::State::NORMAL;
		}
	}
	if (vip || itemid / 1000 >= 5041) {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			UseTeleportRockToPlayerPacket(slot, itemid, pre).dispatch();
			tip_label.change_text(Locale::format("teleport.warp_to", pre));
			return Button::State::NORMAL;
		}
	}
	const TeleportRock& rock = Stage::get().get_player().get_teleportrock();
	int32_t map = rock.first_valid(vip);
	if (!map) map = rock.first_valid(!vip);
	if (!map) {
		tip_label.change_text(Locale::text("teleport.no_saved_map_softkey"));
		return Button::State::NORMAL;
	}
	UseTeleportRockPacket(slot, itemid, map).dispatch();
	tip_label.change_text(Locale::format("teleport.warp", std::to_string(map)));
	return Button::State::NORMAL;
}

UISpeedQuiz::UISpeedQuiz()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("speedquiz.quiz_softkey_answer"))
{
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(300, 200); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("SpeedQuiz", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UISpeedQuiz: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& ans) {
		if (ans.empty()) { tip_label.change_text(Locale::text("speedquiz.empty_answer")); return; }
		tip_label.change_text(Locale::format("speedquiz.ans", ans));
		NpcTalkMorePacket(ans).dispatch();
		LOG(LOG_DEBUG, "UISpeedQuiz: SoftKey NpcTalkMore len=" << ans.size());
	}, []() {}, "Quiz answer");
}
void UISpeedQuiz::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UISpeedQuiz::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(16, 40));
}
void UISpeedQuiz::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([this](const std::string& ans) {
			if (ans.empty()) return;
			tip_label.change_text(Locale::format("speedquiz.ans", ans));
			NpcTalkMorePacket(ans).dispatch();
		}, []() {}, "Quiz answer");
	} else if (keycode == 'I' || keycode == 'i') {
		auto ui = UI::get().get_element<UIInitialQuiz>();
		if (!ui) ui = UI::get().emplace<UIInitialQuiz>();
		else if (!ui->is_active()) ui->makeactive();
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UISpeedQuiz::get_type() const { return TYPE; }

UIInitialQuiz::UIInitialQuiz() {
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(300, 200); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("InitialQuiz", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIInitialQuiz: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& ans) {
		if (ans.empty()) return;
		NpcTalkMorePacket(ans).dispatch();
		LOG(LOG_DEBUG, "UIInitialQuiz: SoftKey NpcTalkMore len=" << ans.size());
	}, []() {}, "Initial quiz");
}
void UIInitialQuiz::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIInitialQuiz::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& ans) {
			if (ans.empty()) return;
			NpcTalkMorePacket(ans).dispatch();
		}, []() {}, "Quiz answer");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIInitialQuiz::get_type() const { return TYPE; }

UIRanking::UIRanking()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("ranking.ranking_softkey_name"))
{
	position = Point<int16_t>(250, 80); dimension = Point<int16_t>(295, 335); dragarea = Point<int16_t>(295, 20);
	has_assets = load_window_bg("Ranking", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIRanking: missing");
	center_on_screen();
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			FindPlayerPacket(pre).dispatch();
			tip_label.change_text(Locale::format("ranking.find", pre));
			LOG(LOG_DEBUG, "UIRanking: FindPlayer name=" << pre);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& name) {
				if (name.empty()) { tip_label.change_text(Locale::text("common.empty_name")); return; }
				FindPlayerPacket(name).dispatch();
				tip_label.change_text(Locale::format("ranking.find", name));
				LOG(LOG_DEBUG, "UIRanking: FindPlayer name=" << name);
			}, []() {}, "Rank name");
		}
	}
}
void UIRanking::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIRanking::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'F' || keycode == 'f' || keycode == 'S' || keycode == 's') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			FindPlayerPacket(pre).dispatch();
			tip_label.change_text(Locale::format("ranking.find", pre));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& name) {
				if (name.empty()) return;
				FindPlayerPacket(name).dispatch();
				tip_label.change_text(Locale::format("ranking.find", name));
			}, []() {}, "Rank name");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIRanking::get_type() const { return TYPE; }

UIItemMegaphone::UIItemMegaphone()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("itemmegaphone.drop_item_softkey_msg"))
{
	position = Point<int16_t>(280, 150); dimension = Point<int16_t>(240, 218); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("ItemMegaphone", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIItemMegaphone: missing");
	center_on_screen();
	if (auto inv = UI::get().get_element<UIItemInventory>()) {
		int16_t sel = inv->get_selected_slot();
		if (sel > 0) {
			attach_inv = static_cast<int8_t>(inv->get_tab());
			attach_slot = sel;
			has_attach = true;
			tip_label.change_text(Locale::format_all("itemmegaphone.item_tab", {std::to_string(attach_inv), std::to_string(attach_slot)}));
		}
	}
	open_softkey();
}
bool UIItemMegaphone::find_item_mega(int16_t& slot, int32_t& itemid) const
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int32_t id : { 5076000, 5076001 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) { slot = s; itemid = id; return true; }
	}
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 1000 == 5076) { slot = i; itemid = id; return true; }
	}
	return false;
}
bool UIItemMegaphone::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	attach_inv = static_cast<int8_t>(icon.get_inventory_tab());
	attach_slot = icon.get_inventory_slot();
	has_attach = attach_slot > 0;
	if (has_attach)
		tip_label.change_text(Locale::format_all("itemmegaphone.item_tab", {std::to_string(attach_inv), std::to_string(attach_slot)}));
	else
		tip_label.change_text(Locale::text("itemmegaphone.no_item"));
	return true;
}
void UIItemMegaphone::open_softkey()
{
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) { tip_label.change_text(Locale::text("common.empty_msg")); return; }
		int16_t slot = 0; int32_t itemid = 0;
		if (!find_item_mega(slot, itemid)) {
			tip_label.change_text(Locale::text("itemmegaphone.no_item_megaphone"));
			return;
		}
		UseItemMegaphonePacket(slot, itemid, msg, false, has_attach, attach_inv, attach_slot).dispatch();
		tip_label.change_text(Locale::text("common.sent"));
	}, []() {}, "Item Megaphone");
}
void UIItemMegaphone::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIItemMegaphone::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's')
		open_softkey();
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIItemMegaphone::get_type() const { return TYPE; }

UICounsel::UICounsel()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("counsel.counsel_softkey_to_msg"))
{
	position = Point<int16_t>(280, 150); dimension = Point<int16_t>(260, 200); dragarea = Point<int16_t>(260, 20);
	has_assets = load_window_bg("counsel", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICounsel: missing");
	center_on_screen();
	// SoftKey：收件人 → 正文 → Whisper（咨询无独立 opcode 证据）；CharInfo 有名则跳过 to
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto ask_msg = [this](const std::string& to) {
			UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
				if (msg.empty()) return;
				WhisperPacket(to, msg).dispatch();
				tip_label.change_text(Locale::format("counsel.whisper", to));
				LOG(LOG_DEBUG, "UICounsel: Whisper to=" << to << " len=" << msg.size());
			}, []() {}, "Counsel msg");
		};
		if (!pre.empty()) {
			ask_msg(pre);
		} else {
			UI::get().emplace<UISoftKey>([ask_msg](const std::string& to) {
				if (to.empty()) return;
				ask_msg(to);
			}, []() {}, "Counsel to");
		}
	}
}
void UICounsel::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UICounsel::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) deactivate();
	if (!pressed) return;
	if (keycode == 'W' || keycode == 'w' || keycode == 'C' || keycode == 'c') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto ask_msg = [this](const std::string& to) {
			UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
				if (msg.empty()) return;
				WhisperPacket(to, msg).dispatch();
				tip_label.change_text(Locale::format("counsel.whisper", to));
			}, []() {}, "Counsel msg");
		};
		if (!pre.empty()) {
			ask_msg(pre);
		} else {
			UI::get().emplace<UISoftKey>([ask_msg](const std::string& to) {
				if (to.empty()) return;
				ask_msg(to);
			}, []() {}, "Counsel to");
		}
	} else if (keycode == 'N' || keycode == 'n') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		auto ask_msg = [this](const std::string& to) {
			UI::get().emplace<UISoftKey>([this, to](const std::string& msg) {
				if (msg.empty()) return;
				NoteSendPacket(to, msg).dispatch();
				tip_label.change_text(Locale::format("common.note", to));
				LOG(LOG_DEBUG, "UICounsel: NoteSend to=" << to);
			}, []() {}, "Note msg");
		};
		if (!pre.empty()) {
			ask_msg(pre);
		} else {
			UI::get().emplace<UISoftKey>([ask_msg](const std::string& to) {
				if (to.empty()) return;
				ask_msg(to);
			}, []() {}, "Note to");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UICounsel::get_type() const { return TYPE; }

UIMuruengRaid::UIMuruengRaid()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("muruengraid.raid_softkey_note"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(300, 220); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("muruengRaid", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMuruengRaid: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& note) {
		if (note.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		tip_label.change_text(Locale::format("muruengraid.note", note));
		NpcTalkMorePacket(note).dispatch();
		LOG(LOG_DEBUG, "UIMuruengRaid: SoftKey NpcTalkMore len=" << note.size());
	}, []() {}, "Raid note");
}
void UIMuruengRaid::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMuruengRaid::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([this](const std::string& note) {
			if (note.empty()) return;
			tip_label.change_text(Locale::format("muruengraid.note", note));
			NpcTalkMorePacket(note).dispatch();
		}, []() {}, "Raid note");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMuruengRaid::get_type() const { return TYPE; }

UICreateCygnus::UICreateCygnus()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("createcygnus.cygnus_softkey_name"))
{
	position = Point<int16_t>(280, 120); dimension = Point<int16_t>(300, 250); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("createCygnus", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICreateCygnus: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& name) {
		if (name.empty()) { tip_label.change_text(Locale::text("common.empty_name")); return; }
		tip_label.change_text(Locale::format("createcygnus.name", name));
		auto creation = UI::get().get_element<UICygnusCreation>();
		if (!creation)
			creation = UI::get().emplace<UICygnusCreation>();
		else if (!creation->is_active())
			creation->makeactive();
		LOG(LOG_DEBUG, "UICreateCygnus: SoftKey name=" << name << " → UICygnusCreation");
	}, []() {}, "Cygnus name");
}
void UICreateCygnus::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UICreateCygnus::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([this](const std::string& name) {
			if (name.empty()) return;
			tip_label.change_text(Locale::format("createcygnus.name", name));
			auto creation = UI::get().get_element<UICygnusCreation>();
			if (!creation)
				creation = UI::get().emplace<UICygnusCreation>();
			else if (!creation->is_active())
				creation->makeactive();
		}, []() {}, "Cygnus name");
	} else if (keycode == 'C' || keycode == 'c') {
		auto creation = UI::get().get_element<UICygnusCreation>();
		if (!creation)
			creation = UI::get().emplace<UICygnusCreation>();
		else if (!creation->is_active())
			creation->makeactive();
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UICreateCygnus::get_type() const { return TYPE; }

UIMegaphoneBasic::UIMegaphoneBasic()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("megaphonebasic.megaphone_softkey_msg"))
{
	position = Point<int16_t>(300, 160); dimension = Point<int16_t>(240, 180); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("Megaphone", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMegaphoneBasic: missing");
	center_on_screen();
	open_softkey();
}
bool UIMegaphoneBasic::find_megaphone(int16_t& slot, int32_t& itemid) const
{
	const Inventory& inv = Stage::get().get_player().get_inventory();
	for (int32_t id : { 5071000, 5072000, 5076000 }) {
		int16_t s = inv.find_item(InventoryType::Id::CASH, id);
		if (s > 0) { slot = s; itemid = id; return true; }
	}
	for (int16_t i = 1; i <= 96; ++i) {
		int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
		if (id / 10000 == 507) {
			slot = i; itemid = id; return true;
		}
	}
	return false;
}
void UIMegaphoneBasic::open_softkey()
{
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) {
			tip_label.change_text(Locale::text("common.empty_msg"));
			return;
		}
		int16_t slot = 0; int32_t itemid = 0;
		if (!find_megaphone(slot, itemid)) {
			tip_label.change_text(Locale::text("megaphonebasic.no_megaphone"));
			return;
		}
		UseMegaphonePacket(slot, itemid, msg, false).dispatch();
		tip_label.change_text(Locale::text("common.sent"));
		LOG(LOG_DEBUG, "UIMegaphoneBasic: USE_CASH id=" << itemid);
	}, []() {}, "Megaphone");
}
void UIMegaphoneBasic::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMegaphoneBasic::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'M' || keycode == 'm' || keycode == 'S' || keycode == 's')
		open_softkey();
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMegaphoneBasic::get_type() const { return TYPE; }

UIPersonalShop::UIPersonalShop()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("personalshop.shop_softkey_c_title_o_a_b_x"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(320, 280); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("PersonalShop", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIPersonalShop: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& title) {
		dispatch_shop_create_or_open(title, true, tip_label);
		LOG(LOG_DEBUG, "UIPersonalShop: SoftKey titleLen=" << title.size());
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	}, []() {}, "Shop title");
}
void UIPersonalShop::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIPersonalShop::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		MinigameExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') {
		MinigameExitPacket().dispatch();
		tip_label.change_text(Locale::text("common.exit"));
		LOG(LOG_DEBUG, "UIPersonalShop: Exit");
		deactivate();
		return;
	}
	if (keycode == 'C' || keycode == 'c') {
		// 再开店：SoftKey 店名 → CREATE（514 优先）
		UI::get().emplace<UISoftKey>([this](const std::string& title) {
			dispatch_shop_create_or_open(title, true, tip_label);
			LOG(LOG_DEBUG, "UIPersonalShop: SoftKey CREATE titleLen=" << title.size());
		}, []() {}, "Shop title");
	} else if (keycode == 'O' || keycode == 'o') {
		HiredMerchantOpenPacket().dispatch();
		tip_label.change_text(Locale::text("common.open_store"));
		LOG(LOG_DEBUG, "UIPersonalShop: Open");
	} else if (keycode == 'A' || keycode == 'a') {
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	} else if (keycode == 'B' || keycode == 'b') {
		UI::get().emplace<UISoftKey>([this](const std::string& spec) {
			int slot = 1, qty = 1;
			size_t c = spec.find(',');
			try {
				if (c == std::string::npos) slot = std::stoi(spec);
				else {
					slot = std::stoi(spec.substr(0, c));
					qty = std::stoi(spec.substr(c + 1));
				}
			} catch (...) { return; }
			if (slot > 0 && qty > 0) {
				HiredMerchantBuyPacket(static_cast<int8_t>(slot), static_cast<int16_t>(qty)).dispatch();
				tip_label.change_text(Locale::format("common.buy", spec));
				LOG(LOG_DEBUG, "UIPersonalShop: Buy slot=" << slot << " qty=" << qty);
			}
			}, []() {}, "slot,qty");
	} else if (keycode == 'M' || keycode == 'm') {
		auto ui = UI::get().get_element<UIMemberShop>();
		if (!ui) ui = UI::get().emplace<UIMemberShop>();
		else if (!ui->is_active()) ui->makeactive();
	}
}
UIElement::Type UIPersonalShop::get_type() const { return TYPE; }

UIMemberShop::UIMemberShop()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("membershop.membershop_softkey_title"))
{
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(280, 220); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("MemberShop", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMemberShop: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& title) {
		dispatch_shop_create_or_open(title, true, tip_label);
		LOG(LOG_DEBUG, "UIMemberShop: SoftKey titleLen=" << title.size());
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	}, []() {}, "Member title");
}
void UIMemberShop::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMemberShop::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		MinigameExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') {
		MinigameExitPacket().dispatch();
		tip_label.change_text(Locale::text("common.exit"));
		LOG(LOG_DEBUG, "UIMemberShop: Exit");
		deactivate();
		return;
	}
	if (keycode == 'C' || keycode == 'c') {
		UI::get().emplace<UISoftKey>([this](const std::string& title) {
			dispatch_shop_create_or_open(title, true, tip_label);
			LOG(LOG_DEBUG, "UIMemberShop: SoftKey CREATE titleLen=" << title.size());
		}, []() {}, "Member title");
	} else if (keycode == 'O' || keycode == 'o') {
		HiredMerchantOpenPacket().dispatch();
		tip_label.change_text(Locale::text("common.open_store"));
	} else if (keycode == 'A' || keycode == 'a') {
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	} else if (keycode == 'B' || keycode == 'b') {
		UI::get().emplace<UISoftKey>([this](const std::string& spec) {
			int slot = 1, qty = 1;
			size_t c = spec.find(',');
			try {
				if (c == std::string::npos) slot = std::stoi(spec);
				else {
					slot = std::stoi(spec.substr(0, c));
					qty = std::stoi(spec.substr(c + 1));
				}
			} catch (...) { return; }
			if (slot > 0 && qty > 0) {
				HiredMerchantBuyPacket(static_cast<int8_t>(slot), static_cast<int16_t>(qty)).dispatch();
				tip_label.change_text(Locale::format("common.buy", spec));
			}
		}, []() {}, "slot,qty");
	}
}
UIElement::Type UIMemberShop::get_type() const { return TYPE; }

UITradingRoom::UITradingRoom()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("tradingroom.trade_softkey_meso_chat_c_x"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(320, 280); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("TradingRoom", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UITradingRoom: missing");
	center_on_screen();
	// SoftKey：纯数字 → TradeSetMeso；否则 TradeChat
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		bool all_digit = !msg.empty();
		for (char c : msg) { if (c < '0' || c > '9') { all_digit = false; break; } }
		if (all_digit) {
			try {
				int32_t meso = std::stoi(msg);
				if (meso >= 0) {
					TradeSetMesoPacket(meso).dispatch();
					tip_label.change_text(Locale::format("common.meso", msg));
					LOG(LOG_DEBUG, "UITradingRoom: TradeSetMeso=" << meso);
					return;
				}
			} catch (...) {}
		}
		TradeChatPacket(msg).dispatch();
		tip_label.change_text(Locale::text("common.chat_sent"));
		LOG(LOG_DEBUG, "UITradingRoom: TradeChat len=" << msg.size());
	}, []() {}, "Trade meso/chat");
}
void UITradingRoom::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UITradingRoom::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		TradeExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c') {
		TradeConfirmPacket().dispatch();
		tip_label.change_text(Locale::text("common.confirm"));
		LOG(LOG_DEBUG, "UITradingRoom: TradeConfirm");
	} else if (keycode == 'T' || keycode == 't') {
		TradeTransactionPacket().dispatch();
		tip_label.change_text(Locale::text("common.transaction"));
		LOG(LOG_DEBUG, "UITradingRoom: TradeTransaction");
	} else if (keycode == 'V' || keycode == 'v') {
		TradeVisitPacket().dispatch();
		tip_label.change_text(Locale::text("common.visit"));
		LOG(LOG_DEBUG, "UITradingRoom: TradeVisit");
	} else if (keycode == 'A' || keycode == 'a') {
		int16_t inv_slot = 0;
		uint8_t inv_type = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			inv_slot = inv->get_selected_slot();
			if (inv_slot > 0)
				inv_type = static_cast<uint8_t>(inv->get_tab());
		}
		if (inv_slot > 0 && inv_type > 0) {
			uint8_t iv = inv_type;
			int16_t pos = inv_slot;
			UI::get().emplace<UISoftKey>([this, iv, pos](const std::string& s) {
				int16_t qty = 1;
				uint8_t trade_slot = 0;
				if (!s.empty()) {
					size_t c = s.find(',');
					try {
						qty = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
						if (c != std::string::npos)
							trade_slot = static_cast<uint8_t>(std::stoi(s.substr(c + 1)));
					} catch (...) { return; }
				}
				if (qty < 1) return;
				TradeAddItemPacket(iv, pos, qty, trade_slot).dispatch();
				tip_label.change_text(Locale::format("common.additem", std::to_string(pos)));
				LOG(LOG_DEBUG, "UITradingRoom: TradeAddItem from inv");
			}, []() {}, "qty[,tradeSlot]");
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& spec) {
				int vals[4] = { 2, 1, 1, 0 };
				size_t start = 0; int vi = 0;
				while (vi < 4 && start <= spec.size()) {
					size_t c = spec.find(',', start);
					std::string part = (c == std::string::npos) ? spec.substr(start) : spec.substr(start, c - start);
					try { vals[vi] = std::stoi(part); } catch (...) {}
					++vi;
					if (c == std::string::npos) break;
					start = c + 1;
				}
				TradeAddItemPacket(
					static_cast<uint8_t>(vals[0]), static_cast<int16_t>(vals[1]),
					static_cast<int16_t>(vals[2]), static_cast<uint8_t>(vals[3])
				).dispatch();
				tip_label.change_text(Locale::format("cashtradingroom.additem", spec));
				LOG(LOG_DEBUG, "UITradingRoom: TradeAddItem " << spec);
			}, []() {}, "iv,pos,qty,slot");
		}
	} else if (keycode == 'S' || keycode == 's' || keycode == 'M' || keycode == 'm') {
		UI::get().emplace<UISoftKey>([this](const std::string& msg) {
			if (msg.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
			bool all_digit = true;
			for (char c : msg) { if (c < '0' || c > '9') { all_digit = false; break; } }
			if (all_digit) {
				try {
					int32_t meso = std::stoi(msg);
					if (meso >= 0) {
						TradeSetMesoPacket(meso).dispatch();
						tip_label.change_text(Locale::format("common.meso", msg));
						return;
					}
				} catch (...) {}
			}
			TradeChatPacket(msg).dispatch();
			tip_label.change_text(Locale::text("common.chat_sent"));
		}, []() {}, "Trade meso/chat");
	} else if (keycode == 'X' || keycode == 'x') {
		TradeExitPacket().dispatch();
		tip_label.change_text(Locale::text("common.exit"));
		LOG(LOG_DEBUG, "UITradingRoom: TradeExit");
		deactivate();
	} else if (keycode == 'R' || keycode == 'r') {
		auto ui = UI::get().get_element<UICashTradingRoom>();
		if (!ui) ui = UI::get().emplace<UICashTradingRoom>();
		else if (!ui->is_active()) ui->makeactive();
	}
}
UIElement::Type UITradingRoom::get_type() const { return TYPE; }

UIEntrustedShop::UIEntrustedShop()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("entrustedshop.entrusted_softkey_o_open"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(320, 280); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("EntrustedShop", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIEntrustedShop: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& title) {
		dispatch_shop_create_or_open(title, false, tip_label);
		LOG(LOG_DEBUG, "UIEntrustedShop: SoftKey titleLen=" << title.size());
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	}, []() {}, "Entrusted");
}
void UIEntrustedShop::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIEntrustedShop::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		MinigameExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') {
		MinigameExitPacket().dispatch();
		tip_label.change_text(Locale::text("common.exit"));
		LOG(LOG_DEBUG, "UIEntrustedShop: Exit");
		deactivate();
		return;
	}
	if (keycode == 'O' || keycode == 'o') {
		HiredMerchantOpenPacket().dispatch();
		tip_label.change_text(Locale::text("common.open_store"));
	} else if (keycode == 'A' || keycode == 'a') {
		softkey_or_inv_set_item([this](const std::string& t) { tip_label.change_text(t); });
	} else if (keycode == 'B' || keycode == 'b') {
		UI::get().emplace<UISoftKey>([this](const std::string& spec) {
			int slot = 1, qty = 1;
			size_t c = spec.find(',');
			try {
				if (c == std::string::npos) slot = std::stoi(spec);
				else {
					slot = std::stoi(spec.substr(0, c));
					qty = std::stoi(spec.substr(c + 1));
				}
			} catch (...) { return; }
			if (slot > 0 && qty > 0) {
				HiredMerchantBuyPacket(static_cast<int8_t>(slot), static_cast<int16_t>(qty)).dispatch();
				tip_label.change_text(Locale::format("common.buy", spec));
				LOG(LOG_DEBUG, "UIEntrustedShop: Buy slot=" << slot << " qty=" << qty);
			}
		}, []() {}, "slot,qty");
	} else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't') {
		UI::get().emplace<UISoftKey>([this](const std::string& title) {
			dispatch_shop_create_or_open(title, false, tip_label);
		}, []() {}, "Entrusted title");
	}
}
UIElement::Type UIEntrustedShop::get_type() const { return TYPE; }

UICashTradingRoom::UICashTradingRoom()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("cashtradingroom.cashtrade_softkey_meso_chat_c_x"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(320, 280); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("CashTradingRoom", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICashTradingRoom: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		bool all_digit = true;
		for (char c : msg) { if (c < '0' || c > '9') { all_digit = false; break; } }
		if (all_digit) {
			try {
				int32_t meso = std::stoi(msg);
				if (meso >= 0) {
					TradeSetMesoPacket(meso).dispatch();
					tip_label.change_text(Locale::format("common.meso", msg));
					LOG(LOG_DEBUG, "UICashTradingRoom: TradeSetMeso=" << meso);
					return;
				}
			} catch (...) {}
		}
		TradeChatPacket(msg).dispatch();
		tip_label.change_text(Locale::text("common.chat_sent"));
		LOG(LOG_DEBUG, "UICashTradingRoom: TradeChat len=" << msg.size());
	}, []() {}, "CashTrade meso/chat");
}
void UICashTradingRoom::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UICashTradingRoom::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		TradeExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c') {
		TradeConfirmPacket().dispatch();
		tip_label.change_text(Locale::text("common.confirm"));
		LOG(LOG_DEBUG, "UICashTradingRoom: TradeConfirm");
	} else if (keycode == 'T' || keycode == 't') {
		TradeTransactionPacket().dispatch();
		tip_label.change_text(Locale::text("common.transaction"));
		LOG(LOG_DEBUG, "UICashTradingRoom: TradeTransaction");
	} else if (keycode == 'V' || keycode == 'v') {
		TradeVisitPacket().dispatch();
		tip_label.change_text(Locale::text("common.visit"));
		LOG(LOG_DEBUG, "UICashTradingRoom: TradeVisit");
	} else if (keycode == 'A' || keycode == 'a') {
		int16_t inv_slot = 0;
		uint8_t inv_type = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			inv_slot = inv->get_selected_slot();
			if (inv_slot > 0)
				inv_type = static_cast<uint8_t>(inv->get_tab());
		}
		if (inv_slot > 0 && inv_type > 0) {
			uint8_t iv = inv_type;
			int16_t pos = inv_slot;
			UI::get().emplace<UISoftKey>([this, iv, pos](const std::string& s) {
				int16_t qty = 1;
				uint8_t trade_slot = 0;
				if (!s.empty()) {
					size_t c = s.find(',');
					try {
						qty = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
						if (c != std::string::npos)
							trade_slot = static_cast<uint8_t>(std::stoi(s.substr(c + 1)));
					} catch (...) { return; }
				}
				if (qty < 1) return;
				TradeAddItemPacket(iv, pos, qty, trade_slot).dispatch();
				tip_label.change_text(Locale::format("common.additem", std::to_string(pos)));
			}, []() {}, "qty[,tradeSlot]");
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& spec) {
				int vals[4] = { 2, 1, 1, 0 };
				size_t start = 0; int vi = 0;
				while (vi < 4 && start <= spec.size()) {
					size_t c = spec.find(',', start);
					std::string part = (c == std::string::npos) ? spec.substr(start) : spec.substr(start, c - start);
					try { vals[vi] = std::stoi(part); } catch (...) {}
					++vi;
					if (c == std::string::npos) break;
					start = c + 1;
				}
				TradeAddItemPacket(
					static_cast<uint8_t>(vals[0]), static_cast<int16_t>(vals[1]),
					static_cast<int16_t>(vals[2]), static_cast<uint8_t>(vals[3])
				).dispatch();
				tip_label.change_text(Locale::format("cashtradingroom.additem", spec));
				LOG(LOG_DEBUG, "UICashTradingRoom: TradeAddItem " << spec);
			}, []() {}, "iv,pos,qty,slot");
		}
	} else if (keycode == 'S' || keycode == 's' || keycode == 'M' || keycode == 'm') {
		UI::get().emplace<UISoftKey>([this](const std::string& msg) {
			if (msg.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
			bool all_digit = true;
			for (char c : msg) { if (c < '0' || c > '9') { all_digit = false; break; } }
			if (all_digit) {
				try {
					int32_t meso = std::stoi(msg);
					if (meso >= 0) {
						TradeSetMesoPacket(meso).dispatch();
						tip_label.change_text(Locale::format("common.meso", msg));
						return;
					}
				} catch (...) {}
			}
			TradeChatPacket(msg).dispatch();
			tip_label.change_text(Locale::text("common.chat_sent"));
		}, []() {}, "CashTrade meso/chat");
	} else if (keycode == 'X' || keycode == 'x') {
		TradeExitPacket().dispatch();
		tip_label.change_text(Locale::text("common.exit"));
		LOG(LOG_DEBUG, "UICashTradingRoom: TradeExit");
		deactivate();
	}
}
UIElement::Type UICashTradingRoom::get_type() const { return TYPE; }

UIItemSearch::UIItemSearch()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("itemsearch.search_softkey_query"))
{
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(280, 200); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("itemSearch", backgrnd, dimension);
	if (!has_assets) has_assets = load_window_bg("ExceptionItemSearch", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIItemSearch: missing");
	center_on_screen();
	auto ex = UI::get().get_element<UIExceptionItemSearch>();
	if (!ex) UI::get().emplace<UIExceptionItemSearch>();
	else if (!ex->is_active()) ex->makeactive();
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			tip_label.change_text(Locale::format("common.q", pre));
			FindPlayerPacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIItemSearch: FindPlayer q=" << pre);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& q) {
				if (q.empty()) { tip_label.change_text(Locale::text("common.empty_query")); return; }
				tip_label.change_text(Locale::format("common.q", q));
				FindPlayerPacket(q).dispatch();
				LOG(LOG_DEBUG, "UIItemSearch: SoftKey FindPlayer q=" << q);
			}, []() {}, "Find name");
		}
	}
}
void UIItemSearch::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIItemSearch::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'F' || keycode == 'f') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			tip_label.change_text(Locale::format("common.q", pre));
			FindPlayerPacket(pre).dispatch();
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& q) {
				if (q.empty()) return;
				tip_label.change_text(Locale::format("common.q", q));
				FindPlayerPacket(q).dispatch();
			}, []() {}, "Find name");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIItemSearch::get_type() const { return TYPE; }

UIAranSkillGuide::UIAranSkillGuide()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("aranskillguide.aran_guide_softkey_tip"))
{
	position = Point<int16_t>(280, 120); dimension = Point<int16_t>(280, 240); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("AranSkillGuide", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIAranSkillGuide: missing");
	center_on_screen();
	{
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		auto use_or_tip = [this](int32_t id, const std::string& tip) {
			if (id > 0) {
				int32_t lv = 1;
				try {
					lv = Stage::get().get_player().get_skills().get_level(id);
					if (lv <= 0) lv = 1;
				} catch (...) {}
				UseSkillPacket(id, lv).dispatch();
				tip_label.change_text(Locale::format("aranskillguide.skill", std::to_string(id)));
				LOG(LOG_DEBUG, "UIAranSkillGuide: UseSkill id=" << id << " lv=" << lv);
				return;
			}
			if (tip.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
			tip_label.change_text(tip);
			NpcTalkMorePacket(tip).dispatch();
		};
		if (from_sb > 0) {
			use_or_tip(from_sb, "");
		} else {
			UI::get().emplace<UISoftKey>([use_or_tip](const std::string& tip) {
				int32_t id = 0;
				try { id = std::stoi(tip); } catch (...) {}
				use_or_tip(id, tip);
			}, []() {}, "Aran skillid");
		}
	}
}
void UIAranSkillGuide::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIAranSkillGuide::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's') {
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			int32_t lv = 1;
			try {
				lv = Stage::get().get_player().get_skills().get_level(from_sb);
				if (lv <= 0) lv = 1;
			} catch (...) {}
			UseSkillPacket(from_sb, lv).dispatch();
			tip_label.change_text(Locale::format("aranskillguide.skill", std::to_string(from_sb)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int32_t id = std::stoi(tip);
					if (id > 0) {
						int32_t lv = 1;
						try {
							lv = Stage::get().get_player().get_skills().get_level(id);
							if (lv <= 0) lv = 1;
						} catch (...) {}
						UseSkillPacket(id, lv).dispatch();
						tip_label.change_text(Locale::format("aranskillguide.skill", tip));
						return;
					}
				} catch (...) {}
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
			}, []() {}, "Aran skillid|tip");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIAranSkillGuide::get_type() const { return TYPE; }

UITransform::UITransform() {
	position = Point<int16_t>(300, 160); dimension = Point<int16_t>(240, 180); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("Transform", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UITransform: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& tip) {
		if (tip.empty()) return;
		try {
			int32_t id = std::stoi(tip);
			if (id > 0) {
				int32_t lv = 1;
				try {
					lv = Stage::get().get_player().get_skills().get_level(id);
					if (lv <= 0) lv = 1;
				} catch (...) {}
				UseSkillPacket(id, lv).dispatch();
				LOG(LOG_DEBUG, "UITransform: UseSkill id=" << id << " lv=" << lv);
				return;
			}
		} catch (...) {}
		NpcTalkMorePacket(tip).dispatch();
		LOG(LOG_DEBUG, "UITransform: SoftKey NpcTalkMore len=" << tip.size());
	}, []() {}, "Transform skillid|tip");
}
void UITransform::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UITransform::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& tip) {
			if (tip.empty()) return;
			try {
				int32_t id = std::stoi(tip);
				if (id > 0) {
					int32_t lv = 1;
					try {
						lv = Stage::get().get_player().get_skills().get_level(id);
						if (lv <= 0) lv = 1;
					} catch (...) {}
					UseSkillPacket(id, lv).dispatch();
					return;
				}
			} catch (...) {}
			NpcTalkMorePacket(tip).dispatch();
		}, []() {}, "Transform skillid|tip");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UITransform::get_type() const { return TYPE; }

UIMonsterKilling::UIMonsterKilling()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("monsterkilling.mobkill_softkey_tip"))
{
	position = Point<int16_t>(280, 120); dimension = Point<int16_t>(280, 200); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("MonsterKilling", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMonsterKilling: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& tip) {
		if (tip.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		tip_label.change_text(tip);
		NpcTalkMorePacket(tip).dispatch();
		LOG(LOG_DEBUG, "UIMonsterKilling: SoftKey NpcTalkMore len=" << tip.size());
	}, []() {}, "MobKill chat");
}
void UIMonsterKilling::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMonsterKilling::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			tip_label.change_text(tip);
			NpcTalkMorePacket(tip).dispatch();
		}, []() {}, "MobKill chat");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMonsterKilling::get_type() const { return TYPE; }

UIMapleEvent::UIMapleEvent() {
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(300, 240); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("MapleEvent", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMapleEvent: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& tip) {
		if (tip.empty()) return;
		NpcTalkMorePacket(tip).dispatch();
		LOG(LOG_DEBUG, "UIMapleEvent: SoftKey NpcTalkMore len=" << tip.size());
	}, []() {}, "MapleEvent");
}
void UIMapleEvent::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIMapleEvent::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& tip) {
			if (tip.empty()) return;
			NpcTalkMorePacket(tip).dispatch();
		}, []() {}, "Event chat");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMapleEvent::get_type() const { return TYPE; }

UISkillEx::UISkillEx()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("skillex.skillex_softkey_skillid"))
{
	position = Point<int16_t>(280, 100); dimension = Point<int16_t>(200, 300); dragarea = Point<int16_t>(200, 20);
	has_assets = load_window_bg("SkillEx", backgrnd, dimension);
	if (!has_assets) has_assets = load_window_bg("SkillUp", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UISkillEx: missing");
	center_on_screen();
	{
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		auto use_id = [this](int32_t id) {
			if (id <= 0) { tip_label.change_text(Locale::text("skillex.bad_id")); return; }
			int32_t lv = 1;
			try {
				lv = Stage::get().get_player().get_skills().get_level(id);
				if (lv <= 0) lv = 1;
			} catch (...) {}
			UseSkillPacket(id, lv).dispatch();
			tip_label.change_text(Locale::format_all("skillex.use_lv", {std::to_string(id), std::to_string(lv)}));
			LOG(LOG_DEBUG, "UISkillEx: UseSkill id=" << id << " lv=" << lv);
		};
		if (from_sb > 0) {
			use_id(from_sb);
		}
	}
}
void UISkillEx::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UISkillEx::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'U' || keycode == 'u' || keycode == 'S' || keycode == 's') {
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			int32_t lv = 1;
			try {
				lv = Stage::get().get_player().get_skills().get_level(from_sb);
				if (lv <= 0) lv = 1;
			} catch (...) {}
			UseSkillPacket(from_sb, lv).dispatch();
			tip_label.change_text(Locale::format("skillex.use", std::to_string(from_sb)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id <= 0) return;
					int32_t lv = 1;
					try {
						lv = Stage::get().get_player().get_skills().get_level(id);
						if (lv <= 0) lv = 1;
					} catch (...) {}
					UseSkillPacket(id, lv).dispatch();
					tip_label.change_text(Locale::format("skillex.use", s));
				} catch (...) {}
			}, []() {}, "SkillEx id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UISkillEx::get_type() const { return TYPE; }

UIBook::UIBook()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("book.book_softkey_page"))
{
	position = Point<int16_t>(260, 80); dimension = Point<int16_t>(320, 300); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("Book", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIBook: missing");
	center_on_screen();
	{
		int32_t cover = 0;
		if (auto mb = UI::get().get_element<UIMonsterBook>())
			cover = mb->get_selected_card_id();
		if (cover <= 0) {
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t sel = inv->get_selected_slot();
				if (sel > 0) {
					cover = Stage::get().get_player().get_inventory().get_item_id(inv->get_tab(), sel);
				}
			}
		}
		if (cover > 0) {
			MonsterBookSetCoverPacket(cover).dispatch();
			tip_label.change_text(Locale::format("book.cover", std::to_string(cover)));
			LOG(LOG_DEBUG, "UIBook: MonsterBookCover id=" << cover);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& page) {
				if (page.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
				try {
					int32_t id = std::stoi(page);
					if (id > 0) {
						MonsterBookSetCoverPacket(id).dispatch();
						tip_label.change_text(Locale::format("book.cover", page));
						LOG(LOG_DEBUG, "UIBook: MonsterBookCover id=" << id);
						return;
					}
				} catch (...) {}
				tip_label.change_text(Locale::format("book.page", page));
				NpcTalkMorePacket(page).dispatch();
				LOG(LOG_DEBUG, "UIBook: SoftKey NpcTalkMore page tip=" << page);
			}, []() {}, "Book cover/page");
		}
	}
}
void UIBook::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIBook::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) deactivate();
	if (!pressed) return;
	if (keycode == 'C' || keycode == 'c') {
		int32_t cover = 0;
		if (auto mb = UI::get().get_element<UIMonsterBook>())
			cover = mb->get_selected_card_id();
		if (cover <= 0) {
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t sel = inv->get_selected_slot();
				if (sel > 0)
					cover = Stage::get().get_player().get_inventory().get_item_id(inv->get_tab(), sel);
			}
		}
		if (cover > 0) {
			MonsterBookSetCoverPacket(cover).dispatch();
			tip_label.change_text(Locale::format("book.cover", std::to_string(cover)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& page) {
				try {
					int32_t id = std::stoi(page);
					if (id > 0) {
						MonsterBookSetCoverPacket(id).dispatch();
						tip_label.change_text(Locale::format("book.cover", page));
						LOG(LOG_DEBUG, "UIBook: SoftKey cover=" << id);
					}
				} catch (...) {}
			}, []() {}, "Book cover id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIBook::get_type() const { return TYPE; }

UIQuestAlarm::UIQuestAlarm()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "") {
	update_screen(Constants::Constants::get().get_viewwidth(), Constants::Constants::get().get_viewheight());
	has_assets = load_window_bg("QuestAlarm", backgrnd, dimension);
	if (has_assets) dimension = backgrnd.get_dimensions();
}
void UIQuestAlarm::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIQuestAlarm::draw(float) const {
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(8, 8));
}
void UIQuestAlarm::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (!pressed) return;
	if (escape) { tip_label.change_text(""); return; }
	if (keycode == 'S' || keycode == 's' || keycode == 'Q' || keycode == 'q'
		|| keycode == 'T' || keycode == 't')
	{
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(1, from_ql).dispatch();
			tip_label.change_text(Locale::format("questalarm.start", std::to_string(from_ql)));
			LOG(LOG_DEBUG, "UIQuestAlarm: start qid=" << from_ql);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int16_t qid = static_cast<int16_t>(std::stoi(tip));
					if (qid > 0) {
						QuestActionPacket(1, qid).dispatch();
						tip_label.change_text(Locale::format("questalarm.start", tip));
						LOG(LOG_DEBUG, "UIQuestAlarm: SoftKey start qid=" << qid);
						return;
					}
				} catch (...) {}
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
				LOG(LOG_DEBUG, "UIQuestAlarm: SoftKey NpcTalkMore len=" << tip.size());
			}, []() {}, "Quest id|tip");
		}
	} else if (keycode == 'X' || keycode == 'x' || keycode == 'C' || keycode == 'c') {
		tip_label.change_text("");
	}
}
void UIQuestAlarm::update_screen(int16_t new_width, int16_t) {
	position = Point<int16_t>(static_cast<int16_t>(new_width - 200), 200);
	dimension = Point<int16_t>(180, 40);
}
UIElement::Type UIQuestAlarm::get_type() const { return TYPE; }

UIMateMessage::UIMateMessage() {
	position = Point<int16_t>(280, 160); dimension = Point<int16_t>(280, 180); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("MateMessage", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMateMessage: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& msg) {
		if (msg.empty()) return;
		SpouseChatPacket(msg).dispatch();
		LOG(LOG_DEBUG, "UIMateMessage: SPOUSE_CHAT len=" << msg.size());
	}, []() {}, "Mate message");
}
void UIMateMessage::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIMateMessage::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'M' || keycode == 'm') {
		UI::get().emplace<UISoftKey>([](const std::string& msg) {
			if (msg.empty()) return;
			SpouseChatPacket(msg).dispatch();
		}, []() {}, "Mate message");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMateMessage::get_type() const { return TYPE; }

UIBetaEdition::UIBetaEdition() {
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(300, 220); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("BetaEdition", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIBetaEdition: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& code) {
		if (!code.empty())
			CouponCodePacket(code).dispatch();
		LOG(LOG_DEBUG, "UIBetaEdition: code len=" << code.size());
	}, []() {}, "Beta code");
}
void UIBetaEdition::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIBetaEdition::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'C' || keycode == 'c') {
		UI::get().emplace<UISoftKey>([](const std::string& code) {
			if (!code.empty())
				CouponCodePacket(code).dispatch();
		}, []() {}, "Beta code");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIBetaEdition::get_type() const { return TYPE; }

UIGMBoard::UIGMBoard()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("gmboard.gmboard_softkey_announce"))
{
	position = Point<int16_t>(260, 100); dimension = Point<int16_t>(320, 280); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("GMBoard", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIGMBoard: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& msg) {
		if (msg.empty()) return;
		std::string t = msg;
		for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
		if (t == "users" || t == "show") {
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::SHOW_USERS).dispatch();
			tip_label.change_text(Locale::text("gmboard.showusers"));
			return;
		}
		if (t == "alert") {
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
			tip_label.change_text(Locale::text("gmboard.alertadmins"));
			return;
		}
		GeneralChatPacket(msg, true).dispatch();
		tip_label.change_text(msg);
		LOG(LOG_DEBUG, "UIGMBoard: SoftKey GeneralChat len=" << msg.size());
	}, []() {}, "users|alert|msg");
}
void UIGMBoard::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIGMBoard::send_key(int32_t keycode, bool pressed, bool escape)
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
				tip_label.change_text(Locale::text("gmboard.showusers"));
				return;
			}
			if (t == "alert") {
				AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
				tip_label.change_text(Locale::text("gmboard.alertadmins"));
				return;
			}
			GeneralChatPacket(msg, true).dispatch();
			tip_label.change_text(msg);
		}, []() {}, "users|alert|msg");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIGMBoard::get_type() const { return TYPE; }

UIShortCut::UIShortCut()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, Locale::text("shortcut.1_0_a_z_symbols_gamemenu"))
{
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("ShortCut");
	nl::node bg = root["backgrnd"];

	if (UiAssetResolve::usable(bg))
	{
		backgrnd = Texture(bg);
		Point<int16_t> o = backgrnd.get_origin();
		if (o.x() || o.y()) backgrnd.shift(o);
		dimension = backgrnd.get_dimensions();
		has_assets = true;
	}
	else
	{
		UiAssetResolve::diag_once("shortcut bg", "UIWindow.img/ShortCut/backgrnd missing");
	}

	// 参考端「界面」气球实测（正确客户端截图/”界面“按钮点击后的列表.png，逐件 SAD
	// 配准，7 件 SAD=0.00 精确命中，见 evidence/w2-statusbar/shortcut-balloon.md）：
	// 底板 93x245 只放 **8** 颗 81x25 钮，x=8、首颗顶 y=22、间距 26（不是均分）。
	// 顺序 = 道具/装备窗/能力值/技能/社区/任务/怪物手册/聊天窗；包里有
	// BtGuild/BtParty/BtRanking 三张贴图，但原生这层气球不摆它们。
	if (has_assets)
	{
		static constexpr Buttons ids[] = {
			Buttons::BT_ITEM, Buttons::BT_EQUIP, Buttons::BT_STAT, Buttons::BT_SKILL,
			Buttons::BT_COMM, Buttons::BT_QUEST, Buttons::BT_MOBBOOK, Buttons::BT_MESSENGER
		};
		static constexpr const char* names[] = {
			"BtItem", "BtEquip", "BtStat", "BtSkill",
			"BtComm", "BtQuest", "BtMobbook", "BtMessenger"
		};

		for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i)
			try_maple_btn(buttons, ids[i], root[names[i]],
				Point<int16_t>(8, static_cast<int16_t>(22 + i * 26)), "shortcut btn");
	}
#endif

	update_screen(Constants::Constants::get().get_viewwidth(), Constants::Constants::get().get_viewheight());
}
void UIShortCut::draw(float inter) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	UIElement::draw(inter);
	// 有素材时 8 颗钮自带中英双语标签（烘焙在贴图里），黄色提示行会盖住气球，只在回退壳里保留。
	if (!has_assets) tip_label.draw(position + Point<int16_t>(4, 4));
}
Button::State UIShortCut::button_pressed(uint16_t id)
{
	auto& player = Stage::get().get_player();

	switch (id)
	{
		case Buttons::BT_STAT:
			UI::get().emplace<UIStatsInfo>(player.get_stats());
			break;
		case Buttons::BT_EQUIP:
			UI::get().emplace<UIEquipInventory>(player.get_inventory());
			break;
		case Buttons::BT_ITEM:
			UI::get().emplace<UIItemInventory>(player.get_inventory());
			break;
		case Buttons::BT_SKILL:
			UI::get().emplace<UISkillBook>(player.get_stats(), player.get_skills());
			break;
		case Buttons::BT_QUEST:
			UI::get().emplace<UIQuestLog>(player.get_quests());
			break;
		case Buttons::BT_MOBBOOK:
			UI::get().emplace<UIMonsterBook>();
			break;
		case Buttons::BT_COMM:
			UI::get().emplace<UIUserList>(UIUserList::Tab::FRIEND);
			break;
		case Buttons::BT_MESSENGER:
			UI::get().emplace<UIMessenger>();
			break;
		default:
			break;
	}

	return Button::State::NORMAL;
}
void UIShortCut::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') { deactivate(); return; }
	auto& player = Stage::get().get_player();
	switch (keycode) {
		case '1':
			UI::get().emplace<UIItemInventory>(player.get_inventory());
			break;
		case '2':
			UI::get().emplace<UIEquipInventory>(player.get_inventory());
			break;
		case '3':
			UI::get().emplace<UIStatsInfo>(player.get_stats());
			break;
		case '4':
			UI::get().emplace<UISkillBook>(player.get_stats(), player.get_skills());
			break;
		case '5':
			UI::get().emplace<UIQuestLog>(player.get_quests());
			break;
		case '6':
			UI::get().emplace<UIUserList>(0);
			break;
		case '7':
			UI::get().emplace<UIGuild>();
			break;
		case '8':
			UI::get().emplace<UIChannel>();
			break;
		case '9':
			UI::get().emplace<UIFamily>();
			break;
		case '0':
			UI::get().emplace<UIMaker>();
			break;
		case 'F':
		case 'f':
		{
			int32_t from_sb = 0;
			if (auto sb = UI::get().get_element<UISkillBook>())
				from_sb = sb->get_selected_skill_id();
			if (from_sb > 0) {
				int32_t lv = 1;
				try {
					lv = Stage::get().get_player().get_skills().get_level(from_sb);
					if (lv <= 0) lv = 1;
				} catch (...) {}
				UseSkillPacket(from_sb, lv).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: UseSkill id=" << from_sb);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t id = std::stoi(s);
						if (id <= 0) return;
						int32_t lv = 1;
						try {
							lv = Stage::get().get_player().get_skills().get_level(id);
							if (lv <= 0) lv = 1;
						} catch (...) {}
						UseSkillPacket(id, lv).dispatch();
						LOG(LOG_DEBUG, "UIShortCut: UseSkill id=" << id);
					} catch (...) {}
				}, []() {}, "Skill id");
			}
			break;
		}
		case 'M':
		case 'm':
		{
			int32_t mid = 0;
			if (auto wm = UI::get().get_element<UIWorldMap>())
				mid = wm->get_selected_map_id();
			if (mid > 0) {
				ChangeMapPacket(false, mid, "", false).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: ChangeMap id=" << mid);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t mapid = std::stoi(s);
						if (mapid > 0) {
							ChangeMapPacket(false, mapid, "", false).dispatch();
							LOG(LOG_DEBUG, "UIShortCut: ChangeMap id=" << mapid);
						}
					} catch (...) {}
				}, []() {}, "Map id");
			}
			break;
		}
		case 'T':
		case 't':
		{
			auto ui = UI::get().get_element<UIMapleTV>();
			if (!ui) ui = UI::get().emplace<UIMapleTV>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'R':
		case 'r':
		{
			auto ui = UI::get().get_element<UIAdminClaim>();
			if (!ui) ui = UI::get().emplace<UIAdminClaim>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case '$':
			EnterCashShopPacket().dispatch();
			LOG(LOG_DEBUG, "UIShortCut: EnterCashShop");
			break;
		case 'B':
		case 'b':
		{
			auto ui = UI::get().get_element<UIBetaEdition>();
			if (!ui) ui = UI::get().emplace<UIBetaEdition>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'N':
		case 'n':
		{
			auto ui = UI::get().get_element<UINewYearsCard>();
			if (!ui) ui = UI::get().emplace<UINewYearsCard>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'A':
		case 'a':
		{
			auto ui = UI::get().get_element<UIAdmin>();
			if (!ui) ui = UI::get().emplace<UIAdmin>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'Y':
		case 'y':
		{
			auto ui = UI::get().get_element<UIFamilyTree>();
			if (!ui) ui = UI::get().emplace<UIFamilyTree>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'G':
		case 'g':
		{
			auto ui = UI::get().get_element<UIDailyGift>();
			if (!ui) ui = UI::get().emplace<UIDailyGift>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'U':
		case 'u':
		{
			auto ui = UI::get().get_element<UIBuffList>();
			if (!ui) ui = UI::get().emplace<UIBuffList>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'P':
		case 'p':
		{
			auto ui = UI::get().get_element<UIPartySearch>();
			if (!ui) ui = UI::get().emplace<UIPartySearch>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'V':
		case 'v':
		{
			auto ui = UI::get().get_element<UIEvent>();
			if (!ui) ui = UI::get().emplace<UIEvent>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'W':
		case 'w':
		{
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui) ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'D':
		case 'd':
		{
			auto ui = UI::get().get_element<UIDelivery>();
			if (!ui) ui = UI::get().emplace<UIDelivery>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'I':
		case 'i':
		{
			auto ui = UI::get().get_element<UIMinigame>();
			if (!ui) ui = UI::get().emplace<UIMinigame>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'J':
		case 'j':
		{
			auto ui = UI::get().get_element<UIMinigameTable>();
			if (!ui) ui = UI::get().emplace<UIMinigameTable>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'L':
		case 'l':
		{
			auto ui = UI::get().get_element<UIFloatNotice>();
			if (!ui) ui = UI::get().emplace<UIFloatNotice>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'Z':
		case 'z':
		{
			auto ui = UI::get().get_element<UIMobGage>();
			if (!ui) ui = UI::get().emplace<UIMobGage>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'C':
		case 'c':
		{
			auto ui = UI::get().get_element<UIChannel>();
			if (!ui) ui = UI::get().emplace<UIChannel>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'E':
		case 'e':
		{
			auto ui = UI::get().get_element<UIEntrustedShop>();
			if (!ui) ui = UI::get().emplace<UIEntrustedShop>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'K':
		case 'k':
		{
			auto ui = UI::get().get_element<UIKeyConfig>();
			if (!ui)
				ui = UI::get().emplace<UIKeyConfig>(player.get_inventory(), player.get_skills());
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'O':
		case 'o':
		{
			// 原生 O = 游戏设置页。本包无 StatusBar3.img/OptionMenu，
			// UIOptionMenu 只剩空壳，所以直接落到 UIGameOpt（见 R27-B7-4）。
			auto ui = UI::get().get_element<UIGameOpt>();
			if (!ui) ui = UI::get().emplace<UIGameOpt>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'Q':
		case 'q':
		{
			auto ui = UI::get().get_element<UIQuestAlarm>();
			if (!ui) ui = UI::get().emplace<UIQuestAlarm>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'S':
		case 's':
		{
			auto ui = UI::get().get_element<UIStoreBank>();
			if (!ui) ui = UI::get().emplace<UIStoreBank>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case 'X':
		case 'x':
		{
			auto ui = UI::get().get_element<UIMessenger>();
			if (!ui) ui = UI::get().emplace<UIMessenger>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case '#':
		{
			int32_t from_ci = 0;
			if (auto ci = UI::get().get_element<UICharInfo>())
				from_ci = ci->get_target_cid();
			if (from_ci > 0) {
				TradeInvitePacket(from_ci).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: TradeInvite cid=" << from_ci);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					std::string t = s;
					for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
					if (t.empty() || t == "v" || t == "visit") {
						TradeVisitPacket().dispatch();
						LOG(LOG_DEBUG, "UIShortCut: TradeVisit");
						return;
					}
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) {
							TradeInvitePacket(cid).dispatch();
							LOG(LOG_DEBUG, "UIShortCut: TradeInvite cid=" << cid);
						}
					} catch (...) {}
				}, []() {}, "Trade cid|v");
			}
			break;
		}
		case '!':
		{
			int32_t near = nearest_npc_oid();
			if (near > 0) {
				TalkToNPCPacket(near).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: TalkToNPC nearest oid=" << near);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t oid = std::stoi(s);
						if (oid > 0) {
							TalkToNPCPacket(oid).dispatch();
							LOG(LOG_DEBUG, "UIShortCut: TalkToNPC oid=" << oid);
						}
					} catch (...) {}
				}, []() {}, "NPC oid");
			}
			break;
		}
		case '@':
			PlayerMapTransferPacket().dispatch();
			LOG(LOG_DEBUG, "UIShortCut: PlayerMapTransfer");
			break;
		case ';':
		case ':':
		{
			auto ui = UI::get().get_element<UIRpsGame>();
			if (!ui) ui = UI::get().emplace<UIRpsGame>();
			else if (!ui->is_active()) ui->makeactive();
			break;
		}
		case '^':
		{
			if (auto uii = UI::get().get_element<UIItemInventory>()) {
				if (uii->get_tab() == InventoryType::Id::CASH) {
					int16_t slot = uii->get_selected_slot();
					if (slot > 0) {
						const Inventory& inv = Stage::get().get_player().get_inventory();
						int32_t itemid = inv.get_item_id(InventoryType::Id::CASH, slot);
						if (itemid > 0) {
							UseCashItemBasicPacket(slot, itemid).dispatch();
							LOG(LOG_DEBUG, "UIShortCut: UseCashItem slot=" << slot << " id=" << itemid);
							break;
						}
					}
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t itemid = std::stoi(s);
					if (itemid <= 0) return;
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int16_t slot = inv.find_item(InventoryType::Id::CASH, itemid);
					if (slot > 0) {
						UseCashItemBasicPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIShortCut: UseCashItem id=" << itemid << " slot=" << slot);
					}
				} catch (...) {}
			}, []() {}, "Cash itemid");
			break;
		}
		case '%':
		{
			if (auto uii = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = uii->get_selected_slot();
				auto tab = uii->get_tab();
				if (slot > 0) {
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int32_t itemid = inv.get_item_id(tab, slot);
					if (itemid > 0) {
						NpcShopActionPacket(slot, itemid, 1, false).dispatch();
						LOG(LOG_DEBUG, "UIShortCut: NpcShop Sell slot=" << slot << " id=" << itemid);
						break;
					}
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				bool sell = false;
				std::string body = s;
				if (!body.empty() && (body[0] == 's' || body[0] == 'S')) {
					sell = true;
					body = body.substr(1);
					if (!body.empty() && body[0] == ',') body = body.substr(1);
				}
				int vals[3] = { 0, 0, 1 };
				size_t start = 0; int vi = 0;
				while (vi < 3 && start <= body.size()) {
					size_t c = body.find(',', start);
					std::string part = (c == std::string::npos) ? body.substr(start) : body.substr(start, c - start);
					try { vals[vi] = std::stoi(part); } catch (...) {}
					++vi;
					if (c == std::string::npos) break;
					start = c + 1;
				}
				if (vals[0] >= 0 && vals[1] > 0) {
					NpcShopActionPacket(
						static_cast<int16_t>(vals[0]), vals[1],
						static_cast<int16_t>(vals[2]), !sell
					).dispatch();
					LOG(LOG_DEBUG, "UIShortCut: NpcShop " << (sell ? "Sell " : "Buy ") << body);
				}
			}, []() {}, "slot,id,qty|s,…");
			break;
		}
		case '&':
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				if (s.empty()) return;
				bool all_digit = true;
				for (char c : s) if (c < '0' || c > '9') { all_digit = false; break; }
				if (all_digit) {
					try { NpcTalkMorePacket(std::stoi(s)).dispatch(); } catch (...) {}
				} else {
					NpcTalkMorePacket(s).dispatch();
				}
			}, []() {}, "NPC reply|sel");
			break;
		case '*':
		{
			int32_t from_ci = 0;
			if (auto ci = UI::get().get_element<UICharInfo>())
				from_ci = ci->get_target_cid();
			if (from_ci > 0) {
				CharInfoRequestPacket(from_ci).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) CharInfoRequestPacket(cid).dispatch();
					} catch (...) {}
				}, []() {}, "Char cid");
			}
			break;
		}
		case '(':
		{
			if (auto uii = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = uii->get_selected_slot();
				if (slot > 0) {
					NpcShopActionPacket(slot).dispatch();
					LOG(LOG_DEBUG, "UIShortCut: NpcShopRecharge slot=" << slot);
					break;
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int16_t slot = static_cast<int16_t>(std::stoi(s));
					if (slot >= 0) NpcShopActionPacket(slot).dispatch();
				} catch (...) {}
			}, []() {}, "Recharge slot");
			break;
		}
		case ')':
			NpcShopActionPacket().dispatch();
			LOG(LOG_DEBUG, "UIShortCut: NpcShopLeave");
			break;
		case '{':
		{
			int32_t from_sb = 0;
			if (auto sb = UI::get().get_element<UISkillBook>())
				from_sb = sb->get_selected_skill_id();
			if (from_sb > 0) {
				SpendSpPacket(from_sb).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: SpendSp id=" << from_sb);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t sid = std::stoi(s);
						if (sid > 0) SpendSpPacket(sid).dispatch();
					} catch (...) {}
				}, []() {}, "Skill id SP");
			}
			break;
		}
		case '~':
		{
			int32_t from_ci = 0;
			if (auto ci = UI::get().get_element<UICharInfo>())
				from_ci = ci->get_target_cid();
			if (from_ci > 0) {
				CharInfoRequestPacket(from_ci).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t cid = std::stoi(s);
						if (cid > 0) CharInfoRequestPacket(cid).dispatch();
					} catch (...) {}
				}, []() {}, "CharInfo cid");
			}
			break;
		}
		case '/':
		case '?':
		{
			int32_t mid = 0;
			if (auto wm = UI::get().get_element<UIWorldMap>())
				mid = wm->get_selected_map_id();
			if (mid > 0) {
				ChangeMapPacket(false, mid, "", false).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: ChangeMap id=" << mid);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t mapid = std::stoi(s);
						if (mapid > 0) ChangeMapPacket(false, mapid, "", false).dispatch();
					} catch (...) {}
				}, []() {}, "Map id");
			}
			break;
		}
		case '\'':
		case '"':
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			auto loot = Stage::get().get_drops().find_loot_at(pos);
			if (loot.first > 0) {
				PickupItemPacket(loot.first, loot.second).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t oid = std::stoi(s);
						if (oid > 0) {
							Point<int16_t> p = Stage::get().get_player().get_position();
							PickupItemPacket(oid, p).dispatch();
						}
					} catch (...) {}
				}, []() {}, "Pickup oid");
			}
			break;
		}
		case '}':
		{
			int32_t near = nearest_reactor_oid();
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (near > 0) {
				DamageReactorPacket(near, pos, 0, 0).dispatch();
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int32_t oid = std::stoi(s);
						if (oid > 0) {
							Point<int16_t> p = Stage::get().get_player().get_position();
							DamageReactorPacket(oid, p, 0, 0).dispatch();
						}
					} catch (...) {}
				}, []() {}, "Reactor oid");
			}
			break;
		}
		case '=':
		case '+':
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				MapleStat::Id st = MapleStat::Id::STR;
				bool ok = false;
				if (t == "str") { st = MapleStat::Id::STR; ok = true; }
				else if (t == "dex") { st = MapleStat::Id::DEX; ok = true; }
				else if (t == "int") { st = MapleStat::Id::INT; ok = true; }
				else if (t == "luk") { st = MapleStat::Id::LUK; ok = true; }
				if (ok) SpendApPacket(st).dispatch();
			}, []() {}, "AP str|dex|int|luk");
			break;
		case '[':
		{
			int tab = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>())
				tab = static_cast<int>(inv->get_tab());
			if (tab >= 1 && tab <= 5) {
				GatherItemsPacket(static_cast<InventoryType::Id>(tab)).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: Gather tab=" << tab);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int t = 2;
					try { t = std::stoi(s); } catch (...) {}
					if (t < 1 || t > 5) t = 2;
					GatherItemsPacket(static_cast<InventoryType::Id>(t)).dispatch();
				}, []() {}, "Gather tab 1-5");
			}
			break;
		}
		case ']':
		{
			int tab = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>())
				tab = static_cast<int>(inv->get_tab());
			if (tab >= 1 && tab <= 5) {
				SortItemsPacket(static_cast<InventoryType::Id>(tab)).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: Sort tab=" << tab);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int t = 2;
					try { t = std::stoi(s); } catch (...) {}
					if (t < 1 || t > 5) t = 2;
					SortItemsPacket(static_cast<InventoryType::Id>(t)).dispatch();
				}, []() {}, "Sort tab 1-5");
			}
			break;
		}
		case '\\':
		case '|':
		{
			std::string pre;
			if (auto ci = UI::get().get_element<UICharInfo>())
				pre = ci->get_target_name();
			if (!pre.empty()) {
				DenyGuildRequestPacket(pre).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: DenyGuild from=" << pre);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& from) {
					if (from.empty()) return;
					DenyGuildRequestPacket(from).dispatch();
				}, []() {}, "Deny guild from");
			}
			break;
		}
		case '-':
		case '_':
		{
			int16_t from_ql = 0;
			if (auto ql = UI::get().get_element<UIQuestLog>())
				from_ql = ql->get_selected_quest_id();
			if (from_ql > 0) {
				QuestActionPacket(3, from_ql).dispatch();
				LOG(LOG_DEBUG, "UIShortCut: Forfeit qid=" << from_ql);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					try {
						int16_t qid = static_cast<int16_t>(std::stoi(s));
						if (qid > 0) QuestActionPacket(3, qid).dispatch();
					} catch (...) {}
				}, []() {}, "Forfeit quest id");
			}
			break;
		}
		case ',':
		case '<':
		{
			int16_t scroll = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				if (inv->get_tab() == InventoryType::Id::USE)
					scroll = inv->get_selected_slot();
			}
			auto ask_equip = [](int16_t src) {
				if (auto eq = UI::get().get_element<UIEquipInventory>()) {
					EquipSlot::Id dst = eq->get_selected_eqslot();
					if (dst != EquipSlot::Id::NONE) {
						ScrollEquipPacket(src, dst).dispatch();
						return;
					}
				}
				UI::get().emplace<UISoftKey>([src](const std::string& dst_s) {
					try {
						int16_t dst = static_cast<int16_t>(std::stoi(dst_s));
						if (dst > 0)
							ScrollEquipPacket(src, static_cast<EquipSlot::Id>(dst)).dispatch();
					} catch (...) {}
				}, []() {}, "Equip slot id");
			};
			if (scroll > 0) {
				ask_equip(scroll);
			} else {
				UI::get().emplace<UISoftKey>([ask_equip](const std::string& src_s) {
					int16_t src = 0;
					try { src = static_cast<int16_t>(std::stoi(src_s)); } catch (...) { return; }
					if (src <= 0) return;
					ask_equip(src);
				}, []() {}, "Scroll slot");
			}
			break;
		}
		case '.':
		case '>':
		{
			if (auto uii = UI::get().get_element<UIItemInventory>()) {
				int16_t slot = uii->get_selected_slot();
				auto tab = uii->get_tab();
				if (slot > 0 && (tab == InventoryType::Id::USE || tab == InventoryType::Id::ETC
					|| tab == InventoryType::Id::CASH)) {
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int32_t itemid = inv.get_item_id(tab, slot);
					if (itemid > 0) {
						if (tab == InventoryType::Id::CASH)
							UseCashItemBasicPacket(slot, itemid).dispatch();
						else
							UseItemPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIShortCut: UseItem tab=" << (int)tab << " slot=" << slot << " id=" << itemid);
						break;
					}
				}
			}
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t itemid = std::stoi(s);
					if (itemid <= 0) return;
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int16_t slot = inv.find_item(InventoryType::Id::USE, itemid);
					if (slot <= 0) slot = inv.find_item(InventoryType::Id::ETC, itemid);
					if (slot > 0) {
						UseItemPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIShortCut: UseItem id=" << itemid << " slot=" << slot);
					}
				} catch (...) {}
			}, []() {}, "Use itemid");
			break;
		}
		default:
			break;
	}
}
namespace
{
	// 底栏（classic HUD）气球锚定的公共几何。group_x 与 UIStatusBar 的按钮簇同源
	// （quickslot 面板左缘 - group_gap，UIStatusBar.cpp:1993）。
	// button_top = 触发钮上沿的绝对 y：底条顶 = 屏高 - 71（CLASSIC_BAR_H），4 颗功能钮
	// 画在底条 rel y=36（IDA CreateCtrl y=543，底条顶 507）⇒ 屏高 - 35。
	struct ClassicBarAnchor
	{
		int16_t group_x;
		int16_t button_top;
	};

	ClassicBarAnchor classic_bar(int16_t new_width, int16_t new_height)
	{
		constexpr int16_t BAR_H = 71;
		constexpr int16_t BTN_Y = 36;
		const bool wide = new_width >= 1024;
		const int16_t qs_w = wide ? 466 : 151;		// StatusBar.img/base/quickSlot26 / 08
		const int16_t group_gap = wide ? 241 : 75;

		return ClassicBarAnchor{
			static_cast<int16_t>(new_width - qs_w - group_gap),
			static_cast<int16_t>(new_height - BAR_H + BTN_Y)
		};
	}
}

void UIShortCut::update_screen(int16_t new_width, int16_t new_height) {
	if (!has_assets)
	{
		position = Point<int16_t>(8, static_cast<int16_t>(new_height - 120));
		dimension = Point<int16_t>(100, 80);
		return;
	}

	// 气球底板自带向下的尖角（尖角中心 x=46）→ 挂到底栏「界面」钮正上方，尖角对准钮中心。
	// y 用「尖角末行贴钮上沿」而不是贴底条上沿，证据见 UIGameMenu::update_screen 注释。
	constexpr int16_t TAIL_CX = 46;
	const ClassicBarAnchor bar = classic_bar(new_width, new_height);
	const int16_t bt_short_cx = static_cast<int16_t>(bar.group_x + 168 + 27);	// BtShort 54x34

	position = Point<int16_t>(
		static_cast<int16_t>(bt_short_cx - TAIL_CX),
		static_cast<int16_t>(bar.button_top - dimension.y()));
}
UIElement::Type UIShortCut::get_type() const { return TYPE; }

namespace
{
	// === UIWindow.img/SkillMacro 组合栏几何 ===
	// 全部由原生帧「技能展开宏指令.png」(379x291) 与位图模板匹配解出，窗口原点 = 截图 (172,0)：
	//   backgrnd 207x289 -> SAD 69（白底被内容压住，属正常）
	//   macroslot 124x38 -> SAD 0  @截图(190,53)  => 窗口(18,53)
	//   Macroicon/0..2/icon 32x32 -> SAD 0 @截图(319,56/101/146) => 行距 45、选择钮窗口(147,56)
	//   macroname 164x57 -> SAD 14 @截图(190,195) => 窗口(18,195)
	//   line02 183x1     -> SAD 0  @截图(190,188) => 窗口(18,188)
	// 滚动条：截图 x358..372 整列非白 => 窗口 x186；上箭头 y54..66、轨道 67..165、下箭头 166..178。
	// macroslot 内三格：列亮度在 x34/72/108 处塌陷成边框 => 格中心 x 18/55/92，垂直中心 y19。
	constexpr int16_t SM_ROW_X = 18;
	constexpr int16_t SM_ROW_Y0 = 53;
	constexpr int16_t SM_ROW_PITCH = 45;
	constexpr int16_t SM_CELL_CX[3] = { 18, 55, 92 };
	constexpr int16_t SM_CELL_CY = 19;
	constexpr int16_t SM_SEL_X = 147;
	constexpr int16_t SM_SEL_Y0 = 56;
	constexpr int16_t SM_SEL_SIZE = 32;
	constexpr int16_t SM_LINE02_X = 18;
	constexpr int16_t SM_LINE02_Y = 188;
	constexpr int16_t SM_MACRONAME_X = 18;
	constexpr int16_t SM_MACRONAME_Y = 195;
	// 宏名落在 macroname 灰板的下半（原生那块白输入框是运行期描的，资源里没有位图）。
	constexpr int16_t SM_NAME_X = 68;
	constexpr int16_t SM_NAME_Y = 211;
	constexpr int16_t SM_SLIDER_X = 186;
	constexpr int16_t SM_SLIDER_TOP = 67;
	constexpr int16_t SM_SLIDER_BOTTOM = 165;

	// 组合栏一次只编辑「选中的那一行」，所以技能写进该行的第一个空格；
	// 旧实现走全局 SkillMacros::assign_skill，会把技能塞进别的宏。
	bool assign_skill_to(size_t idx, int32_t skill_id)
	{
		if (skill_id <= 0 || idx >= SkillMacros::MAX_SLOTS)
			return false;

		auto& macros = SkillMacros::get();
		SkillMacroSlot slot = macros.get_slot(idx);

		if (slot.skill1 == 0)
			slot.skill1 = skill_id;
		else if (slot.skill2 == 0)
			slot.skill2 = skill_id;
		else if (slot.skill3 == 0)
			slot.skill3 = skill_id;
		else
			return false;

		macros.set_slot(idx, std::move(slot));
		return true;
	}
}

UISkillMacroEx::UISkillMacroEx()
	: name_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, "")
{
#ifdef USE_NX
	nl::node shell = nl::nx::UI["UIWindow.img"]["SkillMacro"];

	shell_ok = static_cast<bool>(shell["backgrnd"]);

	if (shell_ok)
	{
		backgrnd = Texture(shell["backgrnd"]);
		macroslot = Texture(shell["macroslot"]);
		macroname = Texture(shell["macroname"]);
		line02 = Texture(shell["line02"]);

		for (size_t i = 0; i < MACRO_COUNT; i++)
		{
			nl::node mi = shell["Macroicon"][static_cast<int32_t>(i)];
			macro_icon[i] = Texture(mi["icon"]);
			// TwoSpriteButton 契约：icon = 未选中，iconMouseOver = 选中/悬浮。
			macro_icon_sel[i] = Texture(mi["iconMouseOver"]);
		}

		dimension = backgrnd.get_dimensions();
		dragarea = Point<int16_t>(dimension.x(), 20);
		has_assets = true;
	}
	else
	{
		// 本包一定有 SkillMacro；缺失说明换了资源代次，宁可不画也不退回旧的
		// SkillMacroEx 整块底衬（那是另一套版式，混用会画出不存在的行）。
		UiAssetResolve::diag_once("UISkillMacroEx", "UIWindow.img/SkillMacro missing");
		LOG(LOG_WARN, "UISkillMacroEx: UIWindow.img/SkillMacro missing");
	}

	slider = Slider(
		Slider::Type::DEFAULT_SILVER,
		Range<int16_t>(SM_SLIDER_TOP, SM_SLIDER_BOTTOM), SM_SLIDER_X, VISIBLE_ROWS, 1,
		[this](bool upwards)
		{
			const int16_t total = static_cast<int16_t>(SkillMacros::get().size());
			const int16_t shift = upwards ? -1 : 1;

			if (offset + shift >= 0 && offset + VISIBLE_ROWS + shift <= total)
				offset = static_cast<int16_t>(offset + shift);
		});

	// 构造时不再自动弹安全键盘（UISoftKey，148x278 的二级密码底板）：旧代码在这里
	// emplace，导致「宏指令」一按就多出一张密码键盘。改名改由 S/R 分支按需唤起。
	center_on_screen();
#endif
}

void UISkillMacroEx::draw(float) const
{
	if (!has_assets)
		return;

	backgrnd.draw(position);

	if (!shell_ok)
		return;

	line02.draw(position + Point<int16_t>(SM_LINE02_X, SM_LINE02_Y));
	macroname.draw(position + Point<int16_t>(SM_MACRONAME_X, SM_MACRONAME_Y));

	const auto& macros = SkillMacros::get();
	const int16_t total = static_cast<int16_t>(macros.size());

	slider.setrows(VISIBLE_ROWS, total);

	// 宏条数为 0（新角色 / 服务端没存过宏）时也必须画出**一行空槽**：这一行既是
	// 「正在编辑哪一条」的唯一可视指示，也是唯一能点到选择钮的地方。整段跳过会让
	// 面板变成一块白板，用户既看不到版式也没法创建第一条宏（就是「组合栏内部 UI 缺失」）。
	const int16_t rows = total > 0 ? total : 1;

	for (int16_t r = 0; r < VISIBLE_ROWS; r++)
	{
		const int16_t idx = static_cast<int16_t>(offset + r);

		if (idx >= rows)
			break;

		const int16_t row_y = static_cast<int16_t>(SM_ROW_Y0 + r * SM_ROW_PITCH);

		macroslot.draw(position + Point<int16_t>(SM_ROW_X, row_y));

		const SkillMacroSlot& slot = macros.get_slot(idx);
		const int32_t ids[3] = { slot.skill1, slot.skill2, slot.skill3 };

		for (int c = 0; c < 3; c++)
		{
			if (ids[c] <= 0)
				continue;

			const Texture& icon = SkillData::get(ids[c]).get_icon(SkillData::Icon::NORMAL);
			if (!icon)
				continue;

			// Texture 的绘制点是「原点锚」：可见左上角 = 绘制点 - origin，故加回 origin。
			const Point<int16_t> dim = icon.get_dimensions();
			const Point<int16_t> org = icon.get_origin();
			const Point<int16_t> center(SM_ROW_X + SM_CELL_CX[c], row_y + SM_CELL_CY);

			icon.draw(position + Point<int16_t>(
				static_cast<int16_t>(center.x() - dim.x() / 2 + org.x()),
				static_cast<int16_t>(center.y() - dim.y() / 2 + org.y())));
		}

		// 行尾的宏选择钮（Macroicon 的位图 origin 是 (0,32)，所以绘制点要再加 origin）。
		const Texture& sel = idx == selected ? macro_icon_sel[idx] : macro_icon[idx];
		const Point<int16_t> sel_org = sel.get_origin();

		sel.draw(position + Point<int16_t>(
			static_cast<int16_t>(SM_SEL_X + sel_org.x()),
			static_cast<int16_t>(SM_SEL_Y0 + r * SM_ROW_PITCH + sel_org.y())));
	}

	const std::string& macro_name = macros.get_slot(selected).name;

	if (macro_name != shown_name)
	{
		shown_name = macro_name;
		name_label.change_text(macro_name);
	}

	name_label.draw(position + Point<int16_t>(SM_NAME_X, SM_NAME_Y));

	slider.draw(position);
}

Cursor::State UISkillMacroEx::send_cursor(bool clicked, Point<int16_t> cursor_position)
{
	const Cursor::State dstate = UIDragElement::send_cursor(clicked, cursor_position);

	if (!shell_ok || dragged)
		return dstate;

	const Point<int16_t> relative = cursor_position - position;

	if (slider.isenabled())
	{
		if (Cursor::State new_state = slider.send_cursor(relative, clicked))
			return new_state;
	}

	if (!clicked)
		return Cursor::State::IDLE;

	// 命中范围与 draw() 一致：只有真正画出来的那一行才能被选中，否则 total=1 时
	// 点第 3 行会把 selected 抬到不存在的宏上（assign 会跳过前两条直接建第 3 条）。
	const int16_t total = static_cast<int16_t>(SkillMacros::get().size());
	const int16_t rows = total > 0 ? total : 1;

	for (int16_t r = 0; r < VISIBLE_ROWS; r++)
	{
		const int16_t idx = static_cast<int16_t>(offset + r);

		if (idx >= rows || idx >= static_cast<int16_t>(MACRO_COUNT))
			break;

		const int16_t top = static_cast<int16_t>(SM_SEL_Y0 + r * SM_ROW_PITCH);
		const Rectangle<int16_t> hit(SM_SEL_X, static_cast<int16_t>(SM_SEL_X + SM_SEL_SIZE),
			top, static_cast<int16_t>(top + SM_SEL_SIZE));

		if (hit.contains(relative))
		{
			selected = idx;
			return Cursor::State::CANCLICK;
		}
	}

	return Cursor::State::IDLE;
}

void UISkillMacroEx::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;

	const size_t sel = static_cast<size_t>(selected);

	if (keycode == 'S' || keycode == 's' || keycode == 'R' || keycode == 'r') {
		UI::get().emplace<UISoftKey>([this, sel](const std::string& name) {
			if (name.empty()) return;
			auto& macros = SkillMacros::get();
			SkillMacroSlot slot = macros.get_slot(sel);
			slot.name = name;
			macros.set_slot(sel, std::move(slot));
			ChangeSkillMacroPacket().dispatch();
			LOG(LOG_INFO, "UISkillMacroEx: macro " << (sel + 1) << " renamed -> " << name);
		}, []() {}, "MacroEx");
	} else if (keycode == 'A' || keycode == 'a') {
		int32_t sid = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			sid = sb->get_selected_skill_id();
		if (sid > 0) {
			if (assign_skill_to(sel, sid)) {
				ChangeSkillMacroPacket().dispatch();
				LOG(LOG_INFO, "UISkillMacroEx: macro " << (sel + 1) << " assigned skill " << sid);
			} else {
				LOG(LOG_WARN, "UISkillMacroEx: macro " << (sel + 1) << " full, skill " << sid << " dropped");
			}
		} else {
			UI::get().emplace<UISoftKey>([this, sel](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id <= 0) return;
					if (assign_skill_to(sel, id)) {
						ChangeSkillMacroPacket().dispatch();
						LOG(LOG_INFO, "UISkillMacroEx: macro " << (sel + 1) << " assigned skill " << id);
					} else {
						LOG(LOG_WARN, "UISkillMacroEx: macro " << (sel + 1) << " full, skill " << id << " dropped");
					}
				} catch (...) {}
			}, []() {}, "Skill id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UISkillMacroEx::get_type() const { return TYPE; }

UICharacterClone::UICharacterClone() {
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(280, 220); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("CharacterClone", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICharacterClone: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& note) {
		if (note.empty()) return;
		NpcTalkMorePacket(note).dispatch();
		LOG(LOG_DEBUG, "UICharacterClone: SoftKey NpcTalkMore len=" << note.size());
	}, []() {}, "Clone note");
}
void UICharacterClone::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UICharacterClone::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& note) {
			if (note.empty()) return;
			NpcTalkMorePacket(note).dispatch();
			LOG(LOG_DEBUG, "UICharacterClone: SoftKey NpcTalkMore len=" << note.size());
		}, []() {}, "Clone note");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UICharacterClone::get_type() const { return TYPE; }

UICancelRequests::UICancelRequests()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("cancelrequests.enter_cancel_esc_close"))
{
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(240, 160); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("CancelRequests", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UICancelRequests: missing");
	center_on_screen();
}
void UICancelRequests::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UICancelRequests::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 13 || keycode == 'Y' || keycode == 'y') {
		NpcTalkMorePacket(1, 0).dispatch(); // 取消确认
		tip_label.change_text(Locale::text("cancelrequests.cancelled"));
		deactivate();
	} else if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
		|| keycode == 'Y' || keycode == 'y' || keycode == 'N' || keycode == 'n') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			std::string t = tip;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t == "y" || t == "yes" || t == "ok" || t == "1" || t == "cancel") {
				NpcTalkMorePacket(1, 0).dispatch();
				tip_label.change_text(Locale::text("cancelrequests.cancelled"));
				deactivate();
			} else if (t == "n" || t == "no" || t == "0") {
				deactivate();
			} else {
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
			}
		}, []() {}, "y|n|tip");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UICancelRequests::get_type() const { return TYPE; }

UIReset::UIReset() {
	position = Point<int16_t>(300, 180); dimension = Point<int16_t>(240, 160); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("Reset", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIReset: missing");
	center_on_screen();
	// SoftKey: str/dex/int/luk → SPEND_AP；纯数字 → SPEND_SP skillid；技能书选中优先
	{
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			SpendSpPacket(from_sb).dispatch();
			LOG(LOG_DEBUG, "UIReset: SpendSp id=" << from_sb);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				MapleStat::Id st = MapleStat::Id::STR;
				bool ap = false;
				if (t == "str") { st = MapleStat::Id::STR; ap = true; }
				else if (t == "dex") { st = MapleStat::Id::DEX; ap = true; }
				else if (t == "int") { st = MapleStat::Id::INT; ap = true; }
				else if (t == "luk") { st = MapleStat::Id::LUK; ap = true; }
				if (ap) {
					SpendApPacket(st).dispatch();
					LOG(LOG_DEBUG, "UIReset: SpendAp " << tip);
					return;
				}
				try {
					int32_t sid = std::stoi(tip);
					if (sid > 0) {
						SpendSpPacket(sid).dispatch();
						LOG(LOG_DEBUG, "UIReset: SpendSp id=" << sid);
					}
				} catch (...) {
					LOG(LOG_DEBUG, "UIReset: SoftKey ignored tip=" << tip);
				}
			}, []() {}, "AP str|SP id");
		}
	}
}
void UIReset::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIReset::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'A' || keycode == 'a' || keycode == 'S' || keycode == 's') {
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0 && (keycode == 'S' || keycode == 's')) {
			SpendSpPacket(from_sb).dispatch();
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& tip) {
				if (tip.empty()) return;
				std::string t = tip;
				for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
				MapleStat::Id st = MapleStat::Id::STR;
				bool ap = false;
				if (t == "str") { st = MapleStat::Id::STR; ap = true; }
				else if (t == "dex") { st = MapleStat::Id::DEX; ap = true; }
				else if (t == "int") { st = MapleStat::Id::INT; ap = true; }
				else if (t == "luk") { st = MapleStat::Id::LUK; ap = true; }
				if (ap) { SpendApPacket(st).dispatch(); return; }
				try {
					int32_t sid = std::stoi(tip);
					if (sid > 0) SpendSpPacket(sid).dispatch();
				} catch (...) {}
			}, []() {}, "AP str|SP id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIReset::get_type() const { return TYPE; }

UIItemMove::UIItemMove()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("itemmove.drop_item_softkey_dst"))
{
	position = Point<int16_t>(300, 160); dimension = Point<int16_t>(240, 180); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("ItemMove", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIItemMove: missing");
	center_on_screen();
}
bool UIItemMove::send_icon(const Icon& icon, Point<int16_t> cursor)
{
	Rectangle<int16_t> win{ position, position + dimension };
	if (!win.contains(cursor)) return true;
	src_inv = static_cast<int8_t>(icon.get_inventory_tab());
	src_slot = icon.get_inventory_slot();
	tip_label.change_text(Locale::format_all("itemmove.marked_tab_softkey_dst", {std::to_string(src_inv), std::to_string(src_slot)}));
	UI::get().emplace<UISoftKey>([this](const std::string& dst_s) {
		try {
			int16_t dst = static_cast<int16_t>(std::stoi(dst_s));
			if (dst == 0 || src_slot <= 0) return;
			MoveItemPacket(static_cast<InventoryType::Id>(src_inv), src_slot, dst, 1).dispatch();
			tip_label.change_text(Locale::format_all("itemmove.moved", {std::to_string(src_slot), dst_s}));
			LOG(LOG_DEBUG, "UIItemMove: MoveItem tab=" << (int)src_inv << " " << src_slot << "→" << dst);
		} catch (...) {}
	}, []() {}, "Dest slot");
	return true;
}
void UIItemMove::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIItemMove::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'M' || keycode == 'm') {
		if (src_slot <= 0) {
			if (auto inv = UI::get().get_element<UIItemInventory>()) {
				int16_t sel = inv->get_selected_slot();
				if (sel > 0) {
					src_inv = static_cast<int8_t>(inv->get_tab());
					src_slot = sel;
					tip_label.change_text(Locale::format("itemmove.src_softkey_dst", std::to_string(sel)));
				}
			}
		}
		UI::get().emplace<UISoftKey>([this](const std::string& dst_s) {
			try {
				int16_t dst = static_cast<int16_t>(std::stoi(dst_s));
				if (dst == 0 || src_slot <= 0) return;
				MoveItemPacket(static_cast<InventoryType::Id>(src_inv), src_slot, dst, 1).dispatch();
				tip_label.change_text(Locale::format("itemmove.moved_2", dst_s));
			} catch (...) {}
		}, []() {}, "Dest slot");
	} else if (keycode == 'S' || keycode == 's') {
		// SoftKey inv,src,dst[,qty] → MoveItem
		UI::get().emplace<UISoftKey>([this](const std::string& spec) {
			int vals[4] = { 2, 1, 2, 1 };
			size_t start = 0; int vi = 0;
			while (vi < 4 && start <= spec.size()) {
				size_t c = spec.find(',', start);
				std::string part = (c == std::string::npos) ? spec.substr(start) : spec.substr(start, c - start);
				try { vals[vi] = std::stoi(part); } catch (...) {}
				++vi;
				if (c == std::string::npos) break;
				start = c + 1;
			}
			if (vals[1] <= 0 || vals[2] == 0) return;
			src_inv = static_cast<int8_t>(vals[0]);
			src_slot = static_cast<int16_t>(vals[1]);
			MoveItemPacket(
				static_cast<InventoryType::Id>(vals[0]),
				static_cast<int16_t>(vals[1]),
				static_cast<int16_t>(vals[2]),
				static_cast<int16_t>(vals[3])
			).dispatch();
			tip_label.change_text(Locale::format("itemmove.move", spec));
			LOG(LOG_DEBUG, "UIItemMove: SoftKey MoveItem " << spec);
		}, []() {}, "iv,src,dst[,qty]");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIItemMove::get_type() const { return TYPE; }

UIFadeYesNo::UIFadeYesNo()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("fadeyesno.y_enter_yes_n_esc_no"))
{
	position = Point<int16_t>(300, 200); dimension = Point<int16_t>(240, 120); dragarea = Point<int16_t>(240, 20);
	has_assets = load_window_bg("FadeYesNo", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIFadeYesNo: missing");
	center_on_screen();
}
void UIFadeYesNo::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIFadeYesNo::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIFadeYesNo::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (!pressed) return;
	if (escape || keycode == 'N' || keycode == 'n') {
		NpcTalkMorePacket(1, 0).dispatch();
		deactivate();
		return;
	}
	if (keycode == 13 || keycode == 'Y' || keycode == 'y') {
		NpcTalkMorePacket(1, 1).dispatch();
		deactivate();
		return;
	}
	if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			std::string t = tip;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t == "y" || t == "yes" || t == "ok" || t == "1") {
				NpcTalkMorePacket(1, 1).dispatch();
				deactivate();
			} else if (t == "n" || t == "no" || t == "0") {
				NpcTalkMorePacket(1, 0).dispatch();
				deactivate();
			} else {
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
			}
		}, []() {}, "y|n|tip");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIFadeYesNo::get_type() const { return TYPE; }

UIContextMenu::UIContextMenu()
	: title_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, "")
{
	position = Point<int16_t>(320, 200); dimension = Point<int16_t>(120, 72); dragarea = Point<int16_t>(120, 20);
	has_assets = load_window_bg("ContextMenu", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIContextMenu: missing bg — text menu only");
}
void UIContextMenu::open_at(Point<int16_t> screen_pos, int32_t cid, const std::string& name)
{
	target_cid = cid;
	target_name = name;
	title_label.change_text(name.empty() ? "Menu" : name);
	position = screen_pos;
	dimension = Point<int16_t>(120, 202);
	makeactive();
	LOG(LOG_DEBUG, "UIContextMenu: open cid=" << cid);
}
void UIContextMenu::activate_row(int row)
{
	switch (row) {
		case 0:
			if (target_cid > 0)
				CharInfoRequestPacket(target_cid).dispatch();
			else
				UI::get().emplace<UICharInfo>(Stage::get().get_player().get_oid());
			deactivate();
			break;
		case 1: {
			std::string n = target_name;
			if (auto chat = UI::get().get_element<UIChatBar>())
				chat->focus_whisper(n);
			else if (!n.empty()) {
				UI::get().emplace<UISoftKey>([n](const std::string& msg) {
					if (!msg.empty())
						WhisperPacket(n, msg).dispatch();
				}, []() {}, "Whisper msg");
			}
			deactivate();
			break;
		}
		case 2:
			if (!target_name.empty())
				PartyInvitePacket(target_name).dispatch();
			deactivate();
			break;
		case 3: {
			auto rep = UI::get().get_element<UIReport>();
			if (!rep) rep = UI::get().emplace<UIReport>();
			else if (!rep->is_active()) rep->makeactive();
			if (rep) rep->set_target(target_name);
			deactivate();
			break;
		}
		case 4:
			if (target_cid > 0) {
				TradeInvitePacket(target_cid).dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: TradeInvite cid=" << target_cid);
			}
			deactivate();
			break;
		case 5:
			if (!target_name.empty()) {
				BuddyAddPacket(target_name, "Default Group").dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: BuddyAdd name=" << target_name);
			}
			deactivate();
			break;
		case 6:
			if (!target_name.empty()) {
				MessengerInvitePacket(target_name).dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: MessengerInvite name=" << target_name);
			}
			deactivate();
			break;
		case 7:
			if (!target_name.empty()) {
				GuildInvitePacket(target_name).dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: GuildInvite name=" << target_name);
			}
			deactivate();
			break;
		case 8:
			if (!target_name.empty()) {
				FamilyAddPacket(target_name).dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: FamilyAdd name=" << target_name);
			}
			deactivate();
			break;
		case 9:
			if (target_cid > 0) {
				GiveFamePacket(target_cid, true).dispatch();
				LOG(LOG_DEBUG, "UIContextMenu: GiveFame cid=" << target_cid);
			}
			deactivate();
			break;
		default:
			deactivate();
			break;
	}
}
Cursor::State UIContextMenu::send_cursor(bool clicked, Point<int16_t> cursorpos)
{
	Cursor::State st = UIDragElement<PosNOTICE>::send_cursor(clicked, cursorpos);
	if (!clicked || !active) return st;
	Point<int16_t> rel = cursorpos - position;
	if (rel.x() < 0 || rel.x() > dimension.x()) return st;
	if (rel.y() >= 22 && rel.y() < 40) activate_row(0);
	else if (rel.y() >= 40 && rel.y() < 58) activate_row(1);
	else if (rel.y() >= 58 && rel.y() < 76) activate_row(2);
	else if (rel.y() >= 76 && rel.y() < 94) activate_row(3);
	else if (rel.y() >= 94 && rel.y() < 112) activate_row(4);
	else if (rel.y() >= 112 && rel.y() < 130) activate_row(5);
	else if (rel.y() >= 130 && rel.y() < 148) activate_row(6);
	else if (rel.y() >= 148 && rel.y() < 166) activate_row(7);
	else if (rel.y() >= 166 && rel.y() < 184) activate_row(8);
	else if (rel.y() >= 184 && rel.y() < 202) activate_row(9);
	return st;
}
void UIContextMenu::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	title_label.draw(position + Point<int16_t>(8, 4));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.info")).draw(position + Point<int16_t>(8, 22));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.whisper")).draw(position + Point<int16_t>(8, 40));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.party")).draw(position + Point<int16_t>(8, 58));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.report")).draw(position + Point<int16_t>(8, 76));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.trade")).draw(position + Point<int16_t>(8, 94));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.buddy")).draw(position + Point<int16_t>(8, 112));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.messenger")).draw(position + Point<int16_t>(8, 130));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.guild")).draw(position + Point<int16_t>(8, 148));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.family")).draw(position + Point<int16_t>(8, 166));
	Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("contextmenu.fame")).draw(position + Point<int16_t>(8, 184));
}
void UIContextMenu::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') { deactivate(); return; }
	if (keycode == 'W' || keycode == 'w') {
		std::string n = target_name;
		if (n.empty()) {
			if (auto ci = UI::get().get_element<UICharInfo>())
				n = ci->get_target_name();
		}
		if (n.empty()) {
			UI::get().emplace<UISoftKey>([](const std::string& name) {
				if (name.empty()) return;
				UI::get().emplace<UISoftKey>([name](const std::string& msg) {
					if (!msg.empty()) WhisperPacket(name, msg).dispatch();
				}, []() {}, "Whisper msg");
			}, []() {}, "Whisper name");
		} else if (auto chat = UI::get().get_element<UIChatBar>()) {
			chat->focus_whisper(n);
		} else {
			UI::get().emplace<UISoftKey>([n](const std::string& msg) {
				if (!msg.empty()) WhisperPacket(n, msg).dispatch();
			}, []() {}, "Whisper msg");
		}
		deactivate();
		return;
	}
	if (keycode == 'I' || keycode == 'i') {
		if (target_cid > 0) {
			CharInfoRequestPacket(target_cid).dispatch();
			deactivate();
			return;
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t cid = static_cast<int32_t>(std::stoi(s));
				if (cid > 0) CharInfoRequestPacket(cid).dispatch();
			} catch (...) {}
		}, []() {}, "Char cid");
		deactivate();
		return;
	}
	if (keycode == '1') activate_row(0);
	else if (keycode == '2') activate_row(1);
	else if (keycode == '3') activate_row(2);
	else if (keycode == '4') activate_row(3);
	else if (keycode == '5') activate_row(4);
	else if (keycode == '6') activate_row(5);
	else if (keycode == '7') activate_row(6);
	else if (keycode == '8') activate_row(7);
	else if (keycode == '9') activate_row(8);
	else if (keycode == '0') activate_row(9);
}
UIElement::Type UIContextMenu::get_type() const { return TYPE; }
Button::State UIContextMenu::button_pressed(uint16_t) { return Button::State::NORMAL; }

UIEventWindow::UIEventWindow()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("eventwindow.event_softkey_note_e_uievent"))
{
	position = Point<int16_t>(260, 80); dimension = Point<int16_t>(320, 300); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("EventWindow", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIEventWindow: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& note) {
		if (note.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		tip_label.change_text(note);
		NpcTalkMorePacket(note).dispatch();
		LOG(LOG_DEBUG, "UIEventWindow: SoftKey NpcTalkMore len=" << note.size());
	}, []() {}, "Event note");
}
void UIEventWindow::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIEventWindow::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'E' || keycode == 'e') {
		UI::get().emplace<UIEvent>();
	} else if (keycode == 'N' || keycode == 'n' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([this](const std::string& note) {
			if (note.empty()) return;
			tip_label.change_text(note);
			NpcTalkMorePacket(note).dispatch();
		}, []() {}, "Event note");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIEventWindow::get_type() const { return TYPE; }

UISlideMenu::UISlideMenu() {
	position = Point<int16_t>(20, 200); dimension = Point<int16_t>(142, 38); dragarea = Point<int16_t>(142, 20);
	expanded = false;
#ifdef USE_NX
	nl::node root = UiAssetResolve::window("SlideMenu");
	if (!root) { LOG(LOG_WARN, "UISlideMenu: missing"); return; }
	nl::node cover_n = root["0"] && root["0"]["Cover"] ? root["0"]["Cover"] : root["Cover"];
	if (!cover_n) { LOG(LOG_WARN, "UISlideMenu: no Cover"); return; }
	has_assets = true;
	cover = Texture(cover_n);
	Point<int16_t> o = cover.get_origin();
	if (o.x() || o.y()) cover.shift(o);
	dimension = cover.get_dimensions();
	dragarea = Point<int16_t>(dimension.x(), 20);
	if (root["0"] && root["0"]["backgrnd"]) {
		body = Texture(root["0"]["backgrnd"]);
		Point<int16_t> bo = body.get_origin();
		if (bo.x() || bo.y()) body.shift(bo);
	}
	nl::node bt = root["0"] ? root["0"] : root;
	if (bt["BtEvent"])
		buttons[BtEvent] = std::make_unique<MapleButton>(bt["BtEvent"], Point<int16_t>(8, 40));
	if (bt["BtPartySearch"] || bt["BtParty"])
		buttons[BtPartySearch] = std::make_unique<MapleButton>(
			bt["BtPartySearch"] ? bt["BtPartySearch"] : bt["BtParty"], Point<int16_t>(8, 70));
	for (auto& b : buttons)
		if (b.second) b.second->set_active(false);
#endif
}
void UISlideMenu::toggle_expand()
{
	expanded = !expanded;
	for (auto& b : buttons)
		if (b.second) b.second->set_active(expanded);
	if (expanded && body)
		dimension = Point<int16_t>(
			std::max(cover.get_dimensions().x(), body.get_dimensions().x()),
			cover.get_dimensions().y() + body.get_dimensions().y());
	else if (cover)
		dimension = cover.get_dimensions();
	LOG(LOG_DEBUG, "UISlideMenu: expanded=" << expanded);
}
void UISlideMenu::draw(float inter) const
{
	if (!has_assets) return;
	if (cover) cover.draw(position);
	if (expanded && body) body.draw(position + Point<int16_t>(0, cover.get_dimensions().y()));
	UIElement::draw_buttons(inter);
}
void UISlideMenu::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		if (expanded) toggle_expand();
		else deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') {
		if (expanded) toggle_expand();
		else deactivate();
		return;
	}
	if (keycode == 'F' || keycode == 'f') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			FindPlayerPacket(pre).dispatch();
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& name) {
				if (!name.empty()) FindPlayerPacket(name).dispatch();
			}, []() {}, "Find name");
		}
		return;
	}
	if (keycode == 'C' || keycode == 'c') {
		UI::get().emplace<UISoftKey>([](const std::string& ch) {
			if (ch.empty()) return;
			try {
				int n = std::stoi(ch);
				if (n >= 1 && n <= 20) {
					ChangeChannelPacket(static_cast<uint8_t>(n - 1)).dispatch();
					return;
				}
			} catch (...) {}
			auto ui = UI::get().get_element<UIChannel>();
			if (!ui) ui = UI::get().emplace<UIChannel>();
			else if (!ui->is_active()) ui->makeactive();
		}, []() {}, "Channel 1-N");
		return;
	}
	if (keycode == 'S' || keycode == 's') {
		auto ui = UI::get().get_element<UIPersonalShop>();
		if (!ui) ui = UI::get().emplace<UIPersonalShop>();
		else if (!ui->is_active()) ui->makeactive();
	} else if (keycode == 'T' || keycode == 't') {
		auto ui = UI::get().get_element<UITradingRoom>();
		if (!ui) ui = UI::get().emplace<UITradingRoom>();
		else if (!ui->is_active()) ui->makeactive();
	} else if (keycode == 'E' || keycode == 'e') {
		button_pressed(BtEvent);
	} else if (keycode == 'P' || keycode == 'p') {
		button_pressed(BtPartySearch);
	} else if (keycode == ' ' || keycode == 13) {
		toggle_expand();
	} else if (keycode == 'D' || keycode == 'd') {
		auto ui = UI::get().get_element<UIDelivery>();
		if (!ui) ui = UI::get().emplace<UIDelivery>();
		else if (!ui->is_active()) ui->makeactive();
	}
}
UIElement::Type UISlideMenu::get_type() const { return TYPE; }
Button::State UISlideMenu::button_pressed(uint16_t id)
{
	switch (id) {
		case BtEvent:
			UI::get().emplace<UIEvent>();
			break;
		case BtPartySearch:
			UI::get().emplace<UIPartySearch>();
			break;
		case BtToggle:
			toggle_expand();
			break;
		default:
			break;
	}
	return Button::State::NORMAL;
}

UIUtilDlgEx::UIUtilDlgEx() {
	position = Point<int16_t>(280, 140); dimension = Point<int16_t>(300, 220); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("UtilDlgEx", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIUtilDlgEx: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& text) {
		if (text.empty()) return;
		NpcTalkMorePacket(text).dispatch();
		LOG(LOG_DEBUG, "UIUtilDlgEx: NPC text len=" << text.size());
	}, []() {}, "UtilDlg text");
}
void UIUtilDlgEx::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIUtilDlgEx::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'T' || keycode == 't' || keycode == 'S' || keycode == 's') {
		UI::get().emplace<UISoftKey>([](const std::string& text) {
			if (text.empty()) return;
			NpcTalkMorePacket(text).dispatch();
		}, []() {}, "UtilDlg text");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIUtilDlgEx::get_type() const { return TYPE; }

UIMultiLine::UIMultiLine()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("multiline.multiline_softkey_text"))
{
	position = Point<int16_t>(280, 160); dimension = Point<int16_t>(280, 180); dragarea = Point<int16_t>(280, 20);
	has_assets = load_window_bg("MultiLine", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMultiLine: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& text) {
		if (text.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		tip_label.change_text(text);
		const Inventory& inv = Stage::get().get_player().get_inventory();
		int16_t cash_slot = 0;
		int32_t cash_id = 0;
		int16_t max = inv.get_slotmax(InventoryType::Id::CASH);
		for (int16_t s = 1; s <= max; ++s) {
			int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
			if (id / 10000 == 537) { cash_slot = s; cash_id = id; break; }
		}
		if (cash_slot <= 0) {
			for (int16_t s = 1; s <= max; ++s) {
				int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
				if (id / 10000 == 508) { cash_slot = s; cash_id = id; break; }
			}
		}
		if (cash_slot > 0) {
			if (cash_id / 10000 == 537)
				UseChalkboardPacket(cash_slot, cash_id, text).dispatch();
			else
				UseKitePacket(cash_slot, cash_id, text).dispatch();
			LOG(LOG_DEBUG, "UIMultiLine: chalk/kite id=" << cash_id);
		} else {
			tip_label.change_text(Locale::text("multiline.no_chalk_kite"));
		}
	}, []() {}, "MultiLine");
}
void UIMultiLine::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMultiLine::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'C' || keycode == 'c') {
		UI::get().emplace<UISoftKey>([this](const std::string& text) {
			if (text.empty()) return;
			tip_label.change_text(text);
			const Inventory& inv = Stage::get().get_player().get_inventory();
			int16_t cash_slot = 0;
			int32_t cash_id = 0;
			int16_t max = inv.get_slotmax(InventoryType::Id::CASH);
			for (int16_t s = 1; s <= max; ++s) {
				int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
				if (id / 10000 == 537) { cash_slot = s; cash_id = id; break; }
			}
			if (cash_slot <= 0) {
				for (int16_t s = 1; s <= max; ++s) {
					int32_t id = inv.get_item_id(InventoryType::Id::CASH, s);
					if (id / 10000 == 508) { cash_slot = s; cash_id = id; break; }
				}
			}
			if (cash_slot > 0) {
				if (cash_id / 10000 == 537)
					UseChalkboardPacket(cash_slot, cash_id, text).dispatch();
				else
					UseKitePacket(cash_slot, cash_id, text).dispatch();
			} else {
				tip_label.change_text(Locale::text("multiline.no_chalk_kite"));
			}
		}, []() {}, "MultiLine");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMultiLine::get_type() const { return TYPE; }
void UIMultiLine::set_tip(const std::string& tip) { tip_label.change_text(tip); }

UIGameMenu::UIGameMenu()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK,
		"1-9 shells Esc=close X=face SoftKey hub")
{
	position = Point<int16_t>(300, 200); dimension = Point<int16_t>(200, 160); dragarea = Point<int16_t>(200, 20);
	has_assets = load_window_bg("GameMenu", backgrnd, dimension);
	if (has_assets)
	{
		dragarea = Point<int16_t>(dimension.x(), 20);

#ifdef USE_NX
		// 底板 93x140 只放 **4** 颗 81x25 真按钮：频道变更 / 游戏设置 / 系统设置 /
		// 游戏结束。参考端截帧（目录按钮点击后的列表.png，82x182）的蓝色掩膜行剖
		// 面给出 4 段实心 25 行、行间隙固定在 crop y 50/76/102/128（见
		// evidence/w2-hud-buttons/gamemenu-balloon.md），即步距 26；配准候选
		// x=0..1、y=51/77/103 换到画布坐标 = (8, 22+26i)，与 ShortCut 气球
		// SAD=0.00 定下的同一套版式完全一致（同族 93 宽气球）。
		// 高度也验算：22 + 3*26 + 25 = 125 < 131（下摆起点），第 5 颗放不下。
		// 包里有 BtSkin 贴图，但原生这层气球不摆它 -> 不发明按钮。
		nl::node root = UiAssetResolve::window("GameMenu");
		static constexpr Buttons ids[] = {
			Buttons::BT_CHANNEL, Buttons::BT_GAMEOPT, Buttons::BT_SYSOPT,
			Buttons::BT_QUIT
		};
		static constexpr const char* names[] = {
			"BtChannel", "BtGameOpt", "BtSysOpt", "BtQuit"
		};

		for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i)
			try_maple_btn(buttons, ids[i], root[names[i]], Point<int16_t>(8, static_cast<int16_t>(22 + i * 26)), "gamemenu btn");
#endif
	}
	else LOG(LOG_WARN, "UIGameMenu: missing");

	// 锚定交给 update_screen（气球要挂在底栏「目录」钮正上方，不是屏幕居中）。
	// 缺素材的回退壳没有下摆尖端可对，保持原来的居中。
	if (has_assets)
		update_screen(Constants::Constants::get().get_viewwidth(), Constants::Constants::get().get_viewheight());
	else
		center_on_screen();
}
void UIGameMenu::draw(float inter) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	UIElement::draw(inter);
	// 原生版式里没有这行英文；按键调试壳只在缺素材的回退窗体上可见。
	if (!has_assets) tip_label.draw(position + Point<int16_t>(12, 40));
}
Button::State UIGameMenu::button_pressed(uint16_t id)
{
	switch (id)
	{
		case Buttons::BT_CHANNEL:
			UI::get().emplace<UIChannel>();
			deactivate();
			break;
		case Buttons::BT_GAMEOPT:
			// 原生 v83 的目录气球里，「游戏设置」「系统设置」是两颗独立按钮、
			// 两个独立窗（UIWindow.img/GameOpt 与 /SysOpt）。本包无
			// StatusBar3.img/OptionMenu，UIOptionMenu 只剩画空底板的壳 —— 之前
			// 两颗都路由到它，就是用户报的「都是打开游戏设置窗口而且勾选框也没有」。
			UI::get().emplace<UIGameOpt>();
			deactivate();
			break;
		case Buttons::BT_SYSOPT:
			UI::get().emplace<UISysOpt>();
			deactivate();
			break;
		case Buttons::BT_QUIT:
			UI::get().emplace<UIQuit>(Stage::get().get_player().get_stats());
			deactivate();
			break;
	}

	return Button::State::NORMAL;
}
void UIGameMenu::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) deactivate();
	if (!pressed) return;
	if (keycode == '1') {
		auto ui = UI::get().get_element<UIRanking>();
		if (!ui) ui = UI::get().emplace<UIRanking>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '2') {
		auto ui = UI::get().get_element<UICounsel>();
		if (!ui) ui = UI::get().emplace<UICounsel>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '3') {
		auto ui = UI::get().get_element<UICoupon>();
		if (!ui) ui = UI::get().emplace<UICoupon>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '4') {
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '5') {
		auto ui = UI::get().get_element<UIRadio>();
		if (!ui) ui = UI::get().emplace<UIRadio>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '6') {
		auto ui = UI::get().get_element<UITradingRoom>();
		if (!ui) ui = UI::get().emplace<UITradingRoom>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '7') {
		auto ui = UI::get().get_element<UIPersonalShop>();
		if (!ui) ui = UI::get().emplace<UIPersonalShop>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '8') {
		auto ui = UI::get().get_element<UIDelivery>();
		if (!ui) ui = UI::get().emplace<UIDelivery>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '9') {
		auto ui = UI::get().get_element<UIFamily>();
		if (!ui) ui = UI::get().emplace<UIFamily>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '0') {
		auto ui = UI::get().get_element<UIStoreBank>();
		if (!ui) ui = UI::get().emplace<UIStoreBank>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'G' || keycode == 'g') {
		auto ui = UI::get().get_element<UIRemoteGachapon>();
		if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'A' || keycode == 'a') {
		auto ui = UI::get().get_element<UIAlliance>();
		if (!ui) ui = UI::get().emplace<UIAlliance>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'M' || keycode == 'm') {
		auto ui = UI::get().get_element<UIMaker>();
		if (!ui) ui = UI::get().emplace<UIMaker>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'C' || keycode == 'c') {
		auto ui = UI::get().get_element<UIChannel>();
		if (!ui) ui = UI::get().emplace<UIChannel>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'U' || keycode == 'u') {
		auto ui = UI::get().get_element<UIGuild>();
		if (!ui) ui = UI::get().emplace<UIGuild>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'W' || keycode == 'w') {
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'P' || keycode == 'p') {
		auto ui = UI::get().get_element<UIPartySearch>();
		if (!ui) ui = UI::get().emplace<UIPartySearch>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'N' || keycode == 'n') {
		auto ui = UI::get().get_element<UIMemo>();
		if (!ui) ui = UI::get().emplace<UIMemo>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'E' || keycode == 'e') {
		auto ui = UI::get().get_element<UIEntrustedShop>();
		if (!ui) ui = UI::get().emplace<UIEntrustedShop>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'I' || keycode == 'i') {
		auto ui = UI::get().get_element<UIMinigame>();
		if (!ui) ui = UI::get().emplace<UIMinigame>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'D' || keycode == 'd') {
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t amt = std::stoi(s);
				if (amt > 0) {
					MesoDropPacket(amt).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: MesoDrop amt=" << amt);
				}
			} catch (...) {}
		}, []() {}, "Drop meso");
		deactivate();
	} else if (keycode == 'Q' || keycode == 'q') {
		auto& player = Stage::get().get_player();
		auto ui = UI::get().get_element<UIQuestLog>();
		if (!ui) ui = UI::get().emplace<UIQuestLog>(player.get_quests());
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'B' || keycode == 'b') {
		auto ui = UI::get().get_element<UIUserList>();
		if (!ui) ui = UI::get().emplace<UIUserList>(0);
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '$' || keycode == 'S' || keycode == 's') {
		// S 冲突时仍允许 $；StatusBar 已有 BtCashShop
		EnterCashShopPacket().dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: EnterCashShop");
		deactivate();
	} else if (keycode == 'K' || keycode == 'k') {
		auto ui = UI::get().get_element<UIKeyConfig>();
		if (!ui) {
			auto& player = Stage::get().get_player();
			ui = UI::get().emplace<UIKeyConfig>(player.get_inventory(), player.get_skills());
		} else if (!ui->is_active()) {
			ui->makeactive();
		}
		deactivate();
	} else if (keycode == 'X' || keycode == 'x') {
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t emote = std::stoi(s);
				FaceExpressionPacket(emote).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: FaceExpression id=" << emote);
			} catch (...) {}
		}, []() {}, "Emote id");
		deactivate();
	} else if (keycode == 'Z' || keycode == 'z') {
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t id = std::stoi(s);
				if (id > 0) {
					UseChairPacket(id).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: UseChair id=" << id);
				}
			} catch (...) {}
		}, []() {}, "Chair itemid");
		deactivate();
	} else if (keycode == 'Y' || keycode == 'y') {
		CancelChairPacket().dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: CancelChair");
		deactivate();
	} else if (keycode == 'J' || keycode == 'j') {
		auto ui = UI::get().get_element<UIJoypad>();
		if (!ui) ui = UI::get().emplace<UIJoypad>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'V' || keycode == 'v') {
		auto ui = UI::get().get_element<UIEvent>();
		if (!ui) ui = UI::get().emplace<UIEvent>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'R' || keycode == 'r') {
		// R 与 1=Ranking 并存：直接开 Ranking
		auto ui = UI::get().get_element<UIRanking>();
		if (!ui) ui = UI::get().emplace<UIRanking>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 'L' || keycode == 'l') {
		CloseChalkboardPacket().dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: CloseChalkboard");
		deactivate();
	} else if (keycode == 'T' || keycode == 't') {
		int32_t from_inv = 0;
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			int16_t slot = uii->get_selected_slot();
			if (slot > 0)
				from_inv = Stage::get().get_player().get_inventory().get_item_id(uii->get_tab(), slot);
		}
		if (from_inv > 0) {
			UseItemEffectPacket(from_inv).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: UseItemEffect id=" << from_inv);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id > 0) {
						UseItemEffectPacket(id).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: UseItemEffect id=" << id);
					}
				} catch (...) {}
			}, []() {}, "ItemEffect id");
		}
		deactivate();
	} else if (keycode == 'O' || keycode == 'o') {
		UseDoorPacket(true).dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: UseDoor party");
		deactivate();
	} else if (keycode == 'F' || keycode == 'f') {
		// USE_SKILL(91)：SkillBook 选中则直接放；否则 SoftKey skillid
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			int32_t lv = 1;
			try {
				lv = Stage::get().get_player().get_skills().get_level(from_sb);
				if (lv <= 0) lv = 1;
			} catch (...) {}
			UseSkillPacket(from_sb, lv).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: UseSkill id=" << from_sb << " lv=" << lv);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id <= 0) return;
					int32_t lv = 1;
					try {
						lv = Stage::get().get_player().get_skills().get_level(id);
						if (lv <= 0) lv = 1;
					} catch (...) {}
					UseSkillPacket(id, lv).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: UseSkill id=" << id << " lv=" << lv);
				} catch (...) {}
			}, []() {}, "Skill id");
		}
		deactivate();
	} else if (keycode == '`') {
		// MonsterBook cover SoftKey（~ 留给 CharInfoRequest）
		// MONSTER_BOOK_COVER(57)：SoftKey cardid
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t id = std::stoi(s);
				if (id > 0) {
					MonsterBookSetCoverPacket(id).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: MonsterBookCover id=" << id);
				}
			} catch (...) {}
		}, []() {}, "Book cover id");
		deactivate();
	} else if (keycode == '=' || keycode == '+') {
		// SPEND_AP：SoftKey str|dex|int|luk
		UI::get().emplace<UISoftKey>([](const std::string& tip) {
			if (tip.empty()) return;
			std::string t = tip;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			MapleStat::Id st = MapleStat::Id::STR;
			bool ok = false;
			if (t == "str") { st = MapleStat::Id::STR; ok = true; }
			else if (t == "dex") { st = MapleStat::Id::DEX; ok = true; }
			else if (t == "int") { st = MapleStat::Id::INT; ok = true; }
			else if (t == "luk") { st = MapleStat::Id::LUK; ok = true; }
			if (ok) {
				SpendApPacket(st).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: SpendAp " << tip);
			}
		}, []() {}, "AP str|dex|int|luk");
		deactivate();
	} else if (keycode == '[') {
		// SoftKey tab：1=equip 2=use 3=setup 4=etc 5=cash → GATHER_ITEMS
		{
			int tab = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>())
				tab = static_cast<int>(inv->get_tab());
			if (tab >= 1 && tab <= 5) {
				GatherItemsPacket(static_cast<InventoryType::Id>(tab)).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: GatherItems tab=" << tab);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int t = 2;
					try { t = std::stoi(s); } catch (...) {}
					if (t < 1 || t > 5) t = 2;
					GatherItemsPacket(static_cast<InventoryType::Id>(t)).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: GatherItems tab=" << t);
				}, []() {}, "Gather tab 1-5");
			}
		}
		deactivate();
	} else if (keycode == ']') {
		{
			int tab = 0;
			if (auto inv = UI::get().get_element<UIItemInventory>())
				tab = static_cast<int>(inv->get_tab());
			if (tab >= 1 && tab <= 5) {
				SortItemsPacket(static_cast<InventoryType::Id>(tab)).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: SortItems tab=" << tab);
			} else {
				UI::get().emplace<UISoftKey>([](const std::string& s) {
					int t = 2;
					try { t = std::stoi(s); } catch (...) {}
					if (t < 1 || t > 5) t = 2;
					SortItemsPacket(static_cast<InventoryType::Id>(t)).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: SortItems tab=" << t);
				}, []() {}, "Sort tab 1-5");
			}
		}
		deactivate();
	} else if (keycode == ';' || keycode == ':') {
		auto ui = UI::get().get_element<UIRpsGame>();
		if (!ui) ui = UI::get().emplace<UIRpsGame>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '\\' || keycode == '|') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			DenyGuildRequestPacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: DenyGuild from=" << pre);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& from) {
				if (from.empty()) return;
				DenyGuildRequestPacket(from).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: DenyGuild from=" << from);
			}, []() {}, "Deny guild from");
		}
		deactivate();
	} else if (keycode == '-' || keycode == '_') {
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(3, from_ql).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: Quest forfeit id=" << from_ql);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int16_t qid = static_cast<int16_t>(std::stoi(s));
					if (qid > 0) {
						QuestActionPacket(3, qid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: Quest forfeit id=" << qid);
					}
				} catch (...) {}
			}, []() {}, "Forfeit quest id");
		}
		deactivate();
	} else if (keycode == ',' || keycode == '<') {
		int16_t scroll = 0;
		if (auto inv = UI::get().get_element<UIItemInventory>()) {
			if (inv->get_tab() == InventoryType::Id::USE)
				scroll = inv->get_selected_slot();
		}
		auto ask_equip = [](int16_t src) {
			if (auto eq = UI::get().get_element<UIEquipInventory>()) {
				EquipSlot::Id dst = eq->get_selected_eqslot();
				if (dst != EquipSlot::Id::NONE) {
					ScrollEquipPacket(src, dst).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: ScrollEquip src=" << src << " dst=" << static_cast<int>(dst));
					return;
				}
			}
			UI::get().emplace<UISoftKey>([src](const std::string& dst_s) {
				try {
					int16_t dst = static_cast<int16_t>(std::stoi(dst_s));
					if (dst > 0) {
						ScrollEquipPacket(src, static_cast<EquipSlot::Id>(dst)).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: ScrollEquip src=" << src << " dst=" << dst);
					}
				} catch (...) {}
			}, []() {}, "Equip slot id");
		};
		if (scroll > 0) {
			ask_equip(scroll);
		} else {
			UI::get().emplace<UISoftKey>([ask_equip](const std::string& src_s) {
				int16_t src = 0;
				try { src = static_cast<int16_t>(std::stoi(src_s)); } catch (...) { return; }
				if (src <= 0) return;
				ask_equip(src);
			}, []() {}, "Scroll slot");
		}
		deactivate();
	} else if (keycode == '.' || keycode == '>') {
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			int16_t slot = uii->get_selected_slot();
			auto tab = uii->get_tab();
			if (slot > 0 && (tab == InventoryType::Id::USE || tab == InventoryType::Id::ETC
				|| tab == InventoryType::Id::CASH)) {
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int32_t itemid = inv.get_item_id(tab, slot);
				if (itemid > 0) {
					if (tab == InventoryType::Id::CASH)
						UseCashItemBasicPacket(slot, itemid).dispatch();
					else
						UseItemPacket(slot, itemid).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: UseItem tab=" << (int)tab << " slot=" << slot);
					deactivate();
					return;
				}
			}
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t itemid = std::stoi(s);
				if (itemid <= 0) return;
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int16_t slot = inv.find_item(InventoryType::Id::USE, itemid);
				if (slot <= 0) slot = inv.find_item(InventoryType::Id::ETC, itemid);
				if (slot > 0) {
					UseItemPacket(slot, itemid).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: UseItem id=" << itemid << " slot=" << slot);
				}
			} catch (...) {}
		}, []() {}, "Use itemid");
		deactivate();
	} else if (keycode == '/' || keycode == '?') {
		int32_t mid = 0;
		if (auto wm = UI::get().get_element<UIWorldMap>())
			mid = wm->get_selected_map_id();
		if (mid > 0) {
			ChangeMapPacket(false, mid, "", false).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: ChangeMap id=" << mid);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t mapid = std::stoi(s);
					if (mapid > 0) {
						ChangeMapPacket(false, mapid, "", false).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: ChangeMap id=" << mapid);
					}
				} catch (...) {}
			}, []() {}, "Map id");
		}
		deactivate();
	} else if (keycode == '\'' || keycode == '"') {
		Point<int16_t> pos = Stage::get().get_player().get_position();
		auto loot = Stage::get().get_drops().find_loot_at(pos);
		if (loot.first > 0) {
			PickupItemPacket(loot.first, loot.second).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: Pickup nearest oid=" << loot.first);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t oid = std::stoi(s);
					if (oid > 0) {
						Point<int16_t> p = Stage::get().get_player().get_position();
						PickupItemPacket(oid, p).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: Pickup oid=" << oid);
					}
				} catch (...) {}
			}, []() {}, "Pickup oid");
		}
		deactivate();
	} else if (keycode == '}') {
		int32_t near = nearest_reactor_oid();
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (near > 0) {
			DamageReactorPacket(near, pos, 0, 0).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: DamageReactor nearest oid=" << near);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t oid = std::stoi(s);
					if (oid > 0) {
						Point<int16_t> p = Stage::get().get_player().get_position();
						DamageReactorPacket(oid, p, 0, 0).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: DamageReactor oid=" << oid);
					}
				} catch (...) {}
			}, []() {}, "Reactor oid");
		}
		deactivate();
	} else if (keycode == '@') {
		PlayerMapTransferPacket().dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: PlayerMapTransfer");
		deactivate();
	} else if (keycode == '!') {
		int32_t near = nearest_npc_oid();
		if (near > 0) {
			TalkToNPCPacket(near).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: TalkToNPC nearest oid=" << near);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t oid = std::stoi(s);
					if (oid > 0) {
						TalkToNPCPacket(oid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: TalkToNPC oid=" << oid);
					}
				} catch (...) {}
			}, []() {}, "NPC oid");
		}
		deactivate();
	} else if (keycode == '%') {
		// NPC_SHOP：背包选中则卖 qty=1；否则 SoftKey slot,id,qty（前缀 s, 则卖）
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			int16_t slot = uii->get_selected_slot();
			auto tab = uii->get_tab();
			if (slot > 0) {
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int32_t itemid = inv.get_item_id(tab, slot);
				if (itemid > 0) {
					NpcShopActionPacket(slot, itemid, 1, false).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: NpcShop Sell slot=" << slot << " id=" << itemid);
					deactivate();
					return;
				}
			}
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			bool sell = false;
			std::string body = s;
			if (!body.empty() && (body[0] == 's' || body[0] == 'S')) {
				sell = true;
				body = body.substr(1);
				if (!body.empty() && body[0] == ',') body = body.substr(1);
			}
			int vals[3] = { 0, 0, 1 };
			size_t start = 0; int vi = 0;
			while (vi < 3 && start <= body.size()) {
				size_t c = body.find(',', start);
				std::string part = (c == std::string::npos) ? body.substr(start) : body.substr(start, c - start);
				try { vals[vi] = std::stoi(part); } catch (...) {}
				++vi;
				if (c == std::string::npos) break;
				start = c + 1;
			}
			if (vals[0] >= 0 && vals[1] > 0) {
				NpcShopActionPacket(
					static_cast<int16_t>(vals[0]), vals[1],
					static_cast<int16_t>(vals[2]), !sell
				).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: NpcShop " << (sell ? "Sell " : "Buy ") << body);
			}
		}, []() {}, "slot,id,qty|s,slot,id,qty");
		deactivate();
	} else if (keycode == '&') {
		// NPC_TALK_MORE：SoftKey 文本回复 或 纯数字 selection
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			if (s.empty()) return;
			bool all_digit = true;
			for (char c : s) if (c < '0' || c > '9') { all_digit = false; break; }
			if (all_digit) {
				try {
					NpcTalkMorePacket(std::stoi(s)).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: NpcTalkMore sel=" << s);
				} catch (...) {}
			} else {
				NpcTalkMorePacket(s).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: NpcTalkMore text len=" << s.size());
			}
		}, []() {}, "NPC reply|sel");
		deactivate();
	} else if (keycode == '*') {
		int32_t from_ci = 0;
		if (auto ci = UI::get().get_element<UICharInfo>())
			from_ci = ci->get_target_cid();
		if (from_ci > 0) {
			CharInfoRequestPacket(from_ci).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: CharInfo cid=" << from_ci);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t cid = std::stoi(s);
					if (cid > 0) {
						CharInfoRequestPacket(cid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: CharInfo cid=" << cid);
					}
				} catch (...) {}
			}, []() {}, "Char cid");
		}
		deactivate();
	} else if (keycode == '(') {
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			int16_t slot = uii->get_selected_slot();
			if (slot > 0) {
				NpcShopActionPacket(slot).dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: NpcShopRecharge slot=" << slot);
				deactivate();
				return;
			}
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int16_t slot = static_cast<int16_t>(std::stoi(s));
				if (slot >= 0) {
					NpcShopActionPacket(slot).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: NpcShopRecharge slot=" << slot);
				}
			} catch (...) {}
		}, []() {}, "Recharge slot");
		deactivate();
	} else if (keycode == ')') {
		NpcShopActionPacket().dispatch();
		LOG(LOG_DEBUG, "UIGameMenu: NpcShopLeave");
		deactivate();
	} else if (keycode == '^') {
		if (auto uii = UI::get().get_element<UIItemInventory>()) {
			if (uii->get_tab() == InventoryType::Id::CASH) {
				int16_t slot = uii->get_selected_slot();
				if (slot > 0) {
					const Inventory& inv = Stage::get().get_player().get_inventory();
					int32_t itemid = inv.get_item_id(InventoryType::Id::CASH, slot);
					if (itemid > 0) {
						UseCashItemBasicPacket(slot, itemid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: UseCashItem slot=" << slot << " id=" << itemid);
						deactivate();
						return;
					}
				}
			}
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			try {
				int32_t itemid = std::stoi(s);
				if (itemid <= 0) return;
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int16_t slot = inv.find_item(InventoryType::Id::CASH, itemid);
				if (slot > 0) {
					UseCashItemBasicPacket(slot, itemid).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: UseCashItem id=" << itemid << " slot=" << slot);
				}
			} catch (...) {}
		}, []() {}, "Cash itemid");
		deactivate();
	} else if (keycode == '{') {
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			SpendSpPacket(from_sb).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: SpendSp id=" << from_sb);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t sid = std::stoi(s);
					if (sid > 0) {
						SpendSpPacket(sid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: SpendSp id=" << sid);
					}
				} catch (...) {}
			}, []() {}, "Skill id SP");
		}
		deactivate();
	} else if (keycode == '#') {
		int32_t from_ci = 0;
		if (auto ci = UI::get().get_element<UICharInfo>())
			from_ci = ci->get_target_cid();
		if (from_ci > 0) {
			TradeInvitePacket(from_ci).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: TradeInvite cid=" << from_ci);
			deactivate();
			return;
		}
		UI::get().emplace<UISoftKey>([](const std::string& s) {
			std::string t = s;
			for (auto& c : t) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
			if (t.empty() || t == "v" || t == "visit") {
				TradeVisitPacket().dispatch();
				LOG(LOG_DEBUG, "UIGameMenu: TradeVisit");
				return;
			}
			try {
				int32_t cid = std::stoi(s);
				if (cid > 0) {
					TradeInvitePacket(cid).dispatch();
					LOG(LOG_DEBUG, "UIGameMenu: TradeInvite cid=" << cid);
				}
			} catch (...) {}
		}, []() {}, "Trade cid|v");
		deactivate();
	} else if (keycode == '~') {
		// CharInfoRequest：CharInfo 已有 cid 则直接发（与 * 一致；与 ` BookCover 分离）
		int32_t from_ci = 0;
		if (auto ci = UI::get().get_element<UICharInfo>())
			from_ci = ci->get_target_cid();
		if (from_ci > 0) {
			CharInfoRequestPacket(from_ci).dispatch();
			LOG(LOG_DEBUG, "UIGameMenu: CharInfoRequest cid=" << from_ci);
		} else {
			UI::get().emplace<UISoftKey>([](const std::string& s) {
				try {
					int32_t cid = std::stoi(s);
					if (cid > 0) {
						CharInfoRequestPacket(cid).dispatch();
						LOG(LOG_DEBUG, "UIGameMenu: CharInfoRequest cid=" << cid);
					}
				} catch (...) {}
			}, []() {}, "CharInfo cid");
		}
		deactivate();
	} else if (keycode == 9) {
		// Tab → MonsterCarnival HUD
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 112) {
		// F1 → AriantMatch
		auto ui = UI::get().get_element<UIAriantMatch>();
		if (!ui) ui = UI::get().emplace<UIAriantMatch>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 113) {
		// F2 → PartyRace
		auto ui = UI::get().get_element<UIPartyRace>();
		if (!ui) ui = UI::get().emplace<UIPartyRace>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 114) {
		// F3 → Plant/Fishing shell
		auto ui = UI::get().get_element<UIPlant>();
		if (!ui) ui = UI::get().emplace<UIPlant>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 115) {
		// F4 → QuestAlarm SoftKey start
		auto ui = UI::get().get_element<UIQuestAlarm>();
		if (!ui) ui = UI::get().emplace<UIQuestAlarm>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 116) {
		// F5 → ShortCut bar
		auto ui = UI::get().get_element<UIShortCut>();
		if (!ui) ui = UI::get().emplace<UIShortCut>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 117) {
		// F6 → SlideMenu
		auto ui = UI::get().get_element<UISlideMenu>();
		if (!ui) ui = UI::get().emplace<UISlideMenu>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 118) {
		// F7 → MultiLine chalk/kite
		auto ui = UI::get().get_element<UIMultiLine>();
		if (!ui) ui = UI::get().emplace<UIMultiLine>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 119) {
		// F8 → MateMessage spouse chat
		auto ui = UI::get().get_element<UIMateMessage>();
		if (!ui) ui = UI::get().emplace<UIMateMessage>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 120) {
		// F9 → TreasureBox
		auto ui = UI::get().get_element<UITreasureBox>();
		if (!ui) ui = UI::get().emplace<UITreasureBox>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 121) {
		// F10 → Book / MonsterBook cover SoftKey
		auto ui = UI::get().get_element<UIBook>();
		if (!ui) ui = UI::get().emplace<UIBook>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 122) {
		// F11 → AranSkillGuide
		auto ui = UI::get().get_element<UIAranSkillGuide>();
		if (!ui) ui = UI::get().emplace<UIAranSkillGuide>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == 123) {
		// F12 → UIReset AP/SP SoftKey
		auto ui = UI::get().get_element<UIReset>();
		if (!ui) ui = UI::get().emplace<UIReset>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == ',' || keycode == '<') {
		auto ui = UI::get().get_element<UIItemSearch>();
		if (!ui) ui = UI::get().emplace<UIItemSearch>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '.' || keycode == '>') {
		auto ui = UI::get().get_element<UIGMBoard>();
		if (!ui) ui = UI::get().emplace<UIGMBoard>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '/' || keycode == '?') {
		auto ui = UI::get().get_element<UIQuestIcon>();
		if (!ui) ui = UI::get().emplace<UIQuestIcon>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '\'' || keycode == '"') {
		auto ui = UI::get().get_element<UIFadeYesNo>();
		if (!ui) ui = UI::get().emplace<UIFadeYesNo>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '[' || keycode == '{') {
		auto ui = UI::get().get_element<UIUtilDlgExAvatar>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgExAvatar>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == ']' || keycode == '}') {
		auto ui = UI::get().get_element<UIUtilDlgExMultiPet>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgExMultiPet>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == ';' || keycode == ':') {
		auto ui = UI::get().get_element<UIUtilDlgExPet>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgExPet>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	} else if (keycode == '`' || keycode == '~') {
		// SoftKey shell: UtilDlgEx (NpcTalk uses same art; this opens thin shell)
		auto ui = UI::get().get_element<UIUtilDlgEx>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgEx>();
		else if (!ui->is_active()) ui->makeactive();
		deactivate();
	}
}
UIElement::Type UIGameMenu::get_type() const { return TYPE; }

// 目录气球的落位：底板 93x140 的下摆收束到 x45..47（尖端中心 x=46，见
// evidence/w2-hud-buttons/coords.md T-2b），原生把这一尖端贴在底栏「目录」钮的正上方。
// 两张截帧互证「尖角末行 == 钮上沿」：
//   目录按钮点击后的列表.png（82x182）：气球画布末行 = crop 142，BtMenu(54x34) 掩膜配准
//     命中 crop (16,143)；
//   ”界面“按钮点击后的列表.png（94x278）：气球末行 crop 243，BtShort 命中 crop 244。
// 此前沿用 UIShortCut 的「屏高 - 底条高 - 气球高」，等于把尖角停到底条上沿，比原生高
// 36px（钮画在底条 rel y=36）—— 两条一起改。
void UIGameMenu::update_screen(int16_t new_width, int16_t new_height)
{
	if (!has_assets)
		return;	// 缺素材的回退壳没有尖角可对，保持构造里的居中

	constexpr int16_t TAIL_CX = 46;
	const ClassicBarAnchor bar = classic_bar(new_width, new_height);
	const int16_t bt_menu_cx = static_cast<int16_t>(bar.group_x + 112 + 27);	// BtMenu 54x34

	position = Point<int16_t>(
		static_cast<int16_t>(bt_menu_cx - TAIL_CX),
		static_cast<int16_t>(bar.button_top - dimension.y()));
}

UIMacro::UIMacro()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("macro.macro_softkey_name"))
{
	position = Point<int16_t>(280, 120); dimension = Point<int16_t>(289, 164); dragarea = Point<int16_t>(289, 20);
	has_assets = load_window_bg("Macro", backgrnd, dimension);
	if (!has_assets) has_assets = load_window_bg("MacroSkill", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMacro: missing");
	center_on_screen();
	// 同 UISkillMacroEx：构造时不弹安全键盘，改名走 send_key 的 S/R 分支。
}
void UIMacro::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMacro::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'R' || keycode == 'r') {
		UI::get().emplace<UISoftKey>([this](const std::string& name) {
			if (name.empty()) return;
			auto& macros = SkillMacros::get();
			SkillMacroSlot slot = macros.get_slot(0);
			slot.name = name;
			macros.set_slot(0, std::move(slot));
			ChangeSkillMacroPacket().dispatch();
			tip_label.change_text(Locale::format("common.saved", name));
		}, []() {}, "Macro");
	} else if (keycode == 'A' || keycode == 'a') {
		int32_t sid = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			sid = sb->get_selected_skill_id();
		if (sid > 0) {
			if (SkillMacros::get().assign_skill(sid)) {
				ChangeSkillMacroPacket().dispatch();
				tip_label.change_text(Locale::format("common.assigned", std::to_string(sid)));
				LOG(LOG_DEBUG, "UIMacro: assign_skill id=" << sid);
			} else {
				tip_label.change_text(Locale::text("common.macro_full"));
			}
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id <= 0) return;
					if (SkillMacros::get().assign_skill(id)) {
						ChangeSkillMacroPacket().dispatch();
						tip_label.change_text(Locale::format("common.assigned", s));
					} else {
						tip_label.change_text(Locale::text("common.macro_full"));
					}
				} catch (...) {}
			}, []() {}, "Skill id");
		}
	} else if (keycode == 'K' || keycode == 'k') {
		auto ui = UI::get().get_element<UIMacroSkill>();
		if (!ui) ui = UI::get().emplace<UIMacroSkill>();
		else if (!ui->is_active()) ui->makeactive();
	} else if (keycode == 'E' || keycode == 'e') {
		auto ui = UI::get().get_element<UISkillMacroEx>();
		if (!ui) ui = UI::get().emplace<UISkillMacroEx>();
		else if (!ui->is_active()) ui->makeactive();
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMacro::get_type() const { return TYPE; }

UIUtilDlgExAvatar::UIUtilDlgExAvatar()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("utildlgexavatar.avatar_softkey_emote"))
{
	position = Point<int16_t>(260, 120); dimension = Point<int16_t>(320, 240); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("UtilDlgEx_Avatar", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIUtilDlgExAvatar: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& tip) {
		if (tip.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		try {
			int32_t emote = std::stoi(tip);
			if (emote >= 0) {
				FaceExpressionPacket(emote).dispatch();
				tip_label.change_text(Locale::format("utildlgexavatar.emote", tip));
				LOG(LOG_DEBUG, "UIUtilDlgExAvatar: FaceExpression id=" << emote);
				return;
			}
		} catch (...) {}
		NpcTalkMorePacket(tip).dispatch();
		tip_label.change_text(tip);
		LOG(LOG_DEBUG, "UIUtilDlgExAvatar: SoftKey NpcTalkMore len=" << tip.size());
	}, []() {}, "Emote id|tip");
}
void UIUtilDlgExAvatar::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIUtilDlgExAvatar::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'E' || keycode == 'e') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			try {
				int32_t emote = std::stoi(tip);
				if (emote >= 0) {
					FaceExpressionPacket(emote).dispatch();
					tip_label.change_text(Locale::format("utildlgexavatar.emote", tip));
					return;
				}
			} catch (...) {}
			NpcTalkMorePacket(tip).dispatch();
			tip_label.change_text(tip);
		}, []() {}, "Emote id|tip");
	} else if (keycode == 'U' || keycode == 'u') {
		auto ui = UI::get().get_element<UIUtilDlgEx>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgEx>();
		else if (!ui->is_active()) ui->makeactive();
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIUtilDlgExAvatar::get_type() const { return TYPE; }

UIUtilDlgExPet::UIUtilDlgExPet() {
	position = Point<int16_t>(260, 120); dimension = Point<int16_t>(320, 240); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("UtilDlgEx_Pet", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIUtilDlgExPet: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([](const std::string& name) {
		if (name.empty()) return;
		const Inventory& inv = Stage::get().get_player().get_inventory();
		int16_t slot = 0; int32_t itemid = 0;
		for (int16_t i = 1; i <= 96; ++i) {
			int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
			if (id / 10000 == 517) { slot = i; itemid = id; break; }
		}
		if (slot <= 0) {
			LOG(LOG_DEBUG, "UIUtilDlgExPet: no 517* name tag");
			return;
		}
		UsePetNameTagPacket(slot, itemid, name).dispatch();
		LOG(LOG_DEBUG, "UIUtilDlgExPet: rename via " << itemid);
	}, []() {}, "Pet name");
}
void UIUtilDlgExPet::draw(float) const { if (has_assets && backgrnd) backgrnd.draw(position); }
void UIUtilDlgExPet::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'R' || keycode == 'r' || keycode == 'N' || keycode == 'n') {
		UI::get().emplace<UISoftKey>([](const std::string& name) {
			if (name.empty()) return;
			const Inventory& inv = Stage::get().get_player().get_inventory();
			int16_t slot = 0; int32_t itemid = 0;
			for (int16_t i = 1; i <= 96; ++i) {
				int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
				if (id / 10000 == 517) { slot = i; itemid = id; break; }
			}
			if (slot > 0)
				UsePetNameTagPacket(slot, itemid, name).dispatch();
		}, []() {}, "Pet name");
	} else if (keycode == 'M' || keycode == 'm') {
		auto ui = UI::get().get_element<UIUtilDlgExMultiPet>();
		if (!ui) ui = UI::get().emplace<UIUtilDlgExMultiPet>();
		else if (!ui->is_active()) ui->makeactive();
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIUtilDlgExPet::get_type() const { return TYPE; }

UIUtilDlgExMultiPet::UIUtilDlgExMultiPet()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("utildlgexmultipet.multipet_softkey_name"))
{
	position = Point<int16_t>(240, 100); dimension = Point<int16_t>(360, 280); dragarea = Point<int16_t>(360, 20);
	has_assets = load_window_bg("UtilDlgEx_MultiPetEquip", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIUtilDlgExMultiPet: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& name) {
		if (name.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		const Inventory& inv = Stage::get().get_player().get_inventory();
		int16_t slot = 0; int32_t itemid = 0;
		for (int16_t i = 1; i <= 96; ++i) {
			int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
			if (id / 10000 == 517) { slot = i; itemid = id; break; }
		}
		if (slot <= 0) {
			tip_label.change_text(Locale::text("utildlgexmultipet.no_517_tag"));
			return;
		}
		UsePetNameTagPacket(slot, itemid, name).dispatch();
		tip_label.change_text(Locale::format("utildlgexmultipet.rename", name));
		LOG(LOG_DEBUG, "UIUtilDlgExMultiPet: rename via " << itemid);
	}, []() {}, "MultiPet name");
}
void UIUtilDlgExMultiPet::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIUtilDlgExMultiPet::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'R' || keycode == 'r' || keycode == 'N' || keycode == 'n') {
		UI::get().emplace<UISoftKey>([this](const std::string& name) {
			if (name.empty()) return;
			const Inventory& inv = Stage::get().get_player().get_inventory();
			int16_t slot = 0; int32_t itemid = 0;
			for (int16_t i = 1; i <= 96; ++i) {
				int32_t id = inv.get_item_id(InventoryType::Id::CASH, i);
				if (id / 10000 == 517) { slot = i; itemid = id; break; }
			}
			if (slot <= 0) { tip_label.change_text(Locale::text("utildlgexmultipet.no_517_tag")); return; }
			UsePetNameTagPacket(slot, itemid, name).dispatch();
			tip_label.change_text(Locale::format("utildlgexmultipet.rename", name));
		}, []() {}, "MultiPet name");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIUtilDlgExMultiPet::get_type() const { return TYPE; }

UIExceptionItemSearch::UIExceptionItemSearch()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("exceptionitemsearch.exsearch_softkey_query"))
{
	position = Point<int16_t>(260, 140); dimension = Point<int16_t>(300, 220); dragarea = Point<int16_t>(300, 20);
	has_assets = load_window_bg("ExceptionItemSearch", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIExceptionItemSearch: missing");
	center_on_screen();
	{
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			tip_label.change_text(Locale::format("common.q", pre));
			FindPlayerPacket(pre).dispatch();
			LOG(LOG_DEBUG, "UIExceptionItemSearch: FindPlayer q=" << pre);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& q) {
				if (q.empty()) { tip_label.change_text(Locale::text("common.empty_query")); return; }
				tip_label.change_text(Locale::format("common.q", q));
				FindPlayerPacket(q).dispatch();
				LOG(LOG_DEBUG, "UIExceptionItemSearch: SoftKey FindPlayer q=" << q);
			}, []() {}, "ExSearch name");
		}
	}
}
void UIExceptionItemSearch::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIExceptionItemSearch::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'F' || keycode == 'f') {
		std::string pre;
		if (auto ci = UI::get().get_element<UICharInfo>())
			pre = ci->get_target_name();
		if (!pre.empty()) {
			tip_label.change_text(Locale::format("common.q", pre));
			FindPlayerPacket(pre).dispatch();
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& q) {
				if (q.empty()) return;
				tip_label.change_text(Locale::format("common.q", q));
				FindPlayerPacket(q).dispatch();
			}, []() {}, "ExSearch name");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIExceptionItemSearch::get_type() const { return TYPE; }

UIMobGage::UIMobGage() : tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE, Locale::text("mobgage.mob")) {
	position = Point<int16_t>(300, 40); dimension = Point<int16_t>(200, 40); dragarea = Point<int16_t>(200, 16);
	has_assets = load_window_bg("MobGage", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 16);
	else LOG(LOG_WARN, "UIMobGage: missing");
}
void UIMobGage::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIMobGage::draw(float) const {
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(8, 8));
}
void UIMobGage::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't'
		|| keycode == 'H' || keycode == 'h') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			try {
				size_t c = tip.find(',');
				if (c != std::string::npos) {
					int32_t cur = std::stoi(tip.substr(0, c));
					int32_t maxv = std::stoi(tip.substr(c + 1));
					auto boss = UI::get().get_element<UIBossHP>();
					if (!boss) boss = UI::get().emplace<UIBossHP>();
					else if (!boss->is_active()) boss->makeactive();
					boss->set_hp(1, cur, maxv, 0, 0);
					tip_label.change_text(tip);
					LOG(LOG_DEBUG, "UIMobGage: SoftKey → BossHP " << tip);
					return;
				}
				int pct = std::stoi(tip);
				tip_label.change_text(Locale::format("mobgage.txt", std::to_string(pct)));
				auto boss = UI::get().get_element<UIBossHP>();
				if (!boss) boss = UI::get().emplace<UIBossHP>();
				else if (!boss->is_active()) boss->makeactive();
				boss->set_hp(1, pct, 100, 0, 0);
			} catch (...) {
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
				LOG(LOG_DEBUG, "UIMobGage: SoftKey NpcTalkMore len=" << tip.size());
			}
		}, []() {}, "cur,max|pct|tip");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMobGage::get_type() const { return TYPE; }

UISkillUp::UISkillUp() : tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::YELLOW, Locale::text("skillup.skill_up")) {
	position = Point<int16_t>(320, 200); dimension = Point<int16_t>(160, 80); dragarea = Point<int16_t>(160, 16);
	has_assets = load_window_bg("SkillUp", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 16);
	else LOG(LOG_WARN, "UISkillUp: missing");
	center_on_screen();
	{
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			SpendSpPacket(from_sb).dispatch();
			tip_label.change_text(Locale::format("skillup.sp", std::to_string(from_sb)));
			LOG(LOG_DEBUG, "UISkillUp: SpendSp id=" << from_sb);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id > 0) {
						SpendSpPacket(id).dispatch();
						tip_label.change_text(Locale::format("skillup.sp", s));
						LOG(LOG_DEBUG, "UISkillUp: SpendSp id=" << id);
					}
				} catch (...) {}
			}, []() {}, "SkillUp id");
		}
	}
}
void UISkillUp::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UISkillUp::draw(float) const {
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 24));
}
void UISkillUp::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's') {
		int32_t from_sb = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			from_sb = sb->get_selected_skill_id();
		if (from_sb > 0) {
			SpendSpPacket(from_sb).dispatch();
			tip_label.change_text(Locale::format("skillup.sp", std::to_string(from_sb)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id > 0) {
						SpendSpPacket(id).dispatch();
						tip_label.change_text(Locale::format("skillup.sp", s));
					}
				} catch (...) {}
			}, []() {}, "SkillUp id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UISkillUp::get_type() const { return TYPE; }

UIMacroSkill::UIMacroSkill()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BLACK, Locale::text("macroskill.macroskill_softkey_name"))
{
	position = Point<int16_t>(280, 120); dimension = Point<int16_t>(289, 164); dragarea = Point<int16_t>(289, 20);
	has_assets = load_window_bg("MacroSkill", backgrnd, dimension);
	if (!has_assets) has_assets = load_window_bg("Macro", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMacroSkill: missing");
	center_on_screen();
	UI::get().emplace<UISoftKey>([this](const std::string& name) {
		if (name.empty()) { tip_label.change_text(Locale::text("common.empty")); return; }
		auto& macros = SkillMacros::get();
		SkillMacroSlot slot = macros.get_slot(0);
		slot.name = name;
		macros.set_slot(0, std::move(slot));
		ChangeSkillMacroPacket().dispatch();
		tip_label.change_text(Locale::format("common.saved", name));
		LOG(LOG_DEBUG, "UIMacroSkill: SoftKey rename+save name=" << name);
	}, []() {}, "MacroSkill");
}
void UIMacroSkill::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(12, 40));
}
void UIMacroSkill::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'R' || keycode == 'r') {
		UI::get().emplace<UISoftKey>([this](const std::string& name) {
			if (name.empty()) return;
			auto& macros = SkillMacros::get();
			SkillMacroSlot slot = macros.get_slot(0);
			slot.name = name;
			macros.set_slot(0, std::move(slot));
			ChangeSkillMacroPacket().dispatch();
			tip_label.change_text(Locale::format("common.saved", name));
		}, []() {}, "MacroSkill");
	} else if (keycode == 'A' || keycode == 'a') {
		int32_t sid = 0;
		if (auto sb = UI::get().get_element<UISkillBook>())
			sid = sb->get_selected_skill_id();
		if (sid > 0) {
			if (SkillMacros::get().assign_skill(sid)) {
				ChangeSkillMacroPacket().dispatch();
				tip_label.change_text(Locale::format("common.assigned", std::to_string(sid)));
			} else {
				tip_label.change_text(Locale::text("common.macro_full"));
			}
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				try {
					int32_t id = std::stoi(s);
					if (id <= 0) return;
					if (SkillMacros::get().assign_skill(id)) {
						ChangeSkillMacroPacket().dispatch();
						tip_label.change_text(Locale::format("common.assigned", s));
					} else {
						tip_label.change_text(Locale::text("common.macro_full"));
					}
				} catch (...) {}
			}, []() {}, "Skill id");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIMacroSkill::get_type() const { return TYPE; }

UIQuestIcon::UIQuestIcon()
	: tip_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::YELLOW, "")
{
	position = Point<int16_t>(360, 80); dimension = Point<int16_t>(64, 64); dragarea = Point<int16_t>(64, 16);
	has_assets = load_window_bg("QuestIcon", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 16);
	else LOG(LOG_WARN, "UIQuestIcon: missing");
	{
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(1, from_ql).dispatch();
			tip_label.change_text(Locale::format("questicon.q", std::to_string(from_ql)));
			LOG(LOG_DEBUG, "UIQuestIcon: QuestAction start id=" << from_ql);
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int16_t qid = static_cast<int16_t>(std::stoi(tip));
					if (qid > 0) {
						QuestActionPacket(1, qid).dispatch(); // start
						tip_label.change_text(Locale::format("questicon.q", tip));
						LOG(LOG_DEBUG, "UIQuestIcon: QuestAction start id=" << qid);
						return;
					}
				} catch (...) {}
				tip_label.change_text(tip);
				NpcTalkMorePacket(tip).dispatch();
				LOG(LOG_DEBUG, "UIQuestIcon: SoftKey NpcTalkMore len=" << tip.size());
			}, []() {}, "Quest id");
		}
	}
}
void UIQuestIcon::draw(float) const
{
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(0, 68));
}
void UIQuestIcon::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'Q' || keycode == 'q' || keycode == 'S' || keycode == 's') {
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(1, from_ql).dispatch();
			tip_label.change_text(Locale::format("questicon.q", std::to_string(from_ql)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				if (tip.empty()) return;
				try {
					int16_t qid = static_cast<int16_t>(std::stoi(tip));
					if (qid > 0) {
						QuestActionPacket(1, qid).dispatch();
						tip_label.change_text(Locale::format("questicon.q", tip));
						return;
					}
				} catch (...) {}
				NpcTalkMorePacket(tip).dispatch();
				tip_label.change_text(tip);
			}, []() {}, "Quest id|tip");
		}
	} else if (keycode == 'C' || keycode == 'c') {
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(2, from_ql, 0).dispatch();
			tip_label.change_text(Locale::format("questicon.done", std::to_string(from_ql)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				try {
					size_t comma = tip.find(',');
					int16_t qid = static_cast<int16_t>(std::stoi(comma == std::string::npos ? tip : tip.substr(0, comma)));
					int32_t npcid = 0;
					if (comma != std::string::npos)
						npcid = static_cast<int32_t>(std::stoi(tip.substr(comma + 1)));
					if (qid > 0) {
						QuestActionPacket(2, qid, npcid).dispatch();
						tip_label.change_text(Locale::format("questicon.done", tip));
					}
				} catch (...) {}
			}, []() {}, "qid or qid,npc");
		}
	} else if (keycode == 'F' || keycode == 'f') {
		int16_t from_ql = 0;
		if (auto ql = UI::get().get_element<UIQuestLog>())
			from_ql = ql->get_selected_quest_id();
		if (from_ql > 0) {
			QuestActionPacket(3, from_ql).dispatch();
			tip_label.change_text(Locale::format("questicon.forfeit", std::to_string(from_ql)));
		} else {
			UI::get().emplace<UISoftKey>([this](const std::string& tip) {
				try {
					int16_t qid = static_cast<int16_t>(std::stoi(tip));
					if (qid > 0) {
						QuestActionPacket(3, qid).dispatch();
						tip_label.change_text(Locale::format("questicon.forfeit", tip));
					}
				} catch (...) {}
			}, []() {}, "Forfeit qid");
		}
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIQuestIcon::get_type() const { return TYPE; }

UIMinigameTable::UIMinigameTable()
	: tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, Locale::text("minigametable.table"))
{
	position = Point<int16_t>(240, 100); dimension = Point<int16_t>(320, 260); dragarea = Point<int16_t>(320, 20);
	has_assets = load_window_bg("MinigameTable", backgrnd, dimension);
	if (!has_assets)
		has_assets = load_window_bg("Minigame", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 20);
	else LOG(LOG_WARN, "UIMinigameTable: missing");
#ifdef USE_NX
	// 棋子：Minigame/Omok 下常见 stone0/stone1 或 black/white
	nl::node mg = UiAssetResolve::window("Minigame");
	nl::node omok = mg["Omok"] ? mg["Omok"] : mg["omok"];
	nl::node s0 = omok["stone"]["0"] ? omok["stone"]["0"]
		: (omok["stone0"] ? omok["stone0"] : omok["black"]);
	nl::node s1 = omok["stone"]["1"] ? omok["stone"]["1"]
		: (omok["stone1"] ? omok["stone1"] : omok["white"]);
	if (s0 && s1) {
		stone_black = Texture(s0);
		stone_white = Texture(s1);
		has_stones = true;
		LOG(LOG_DEBUG, "UIMinigameTable: Omok stone textures loaded");
	}
#endif
	center_on_screen();
}
void UIMinigameTable::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIMinigameTable::set_game_kind(uint8_t kind) { game_kind = kind ? kind : 1; }
void UIMinigameTable::clear_board()
{
	for (int y = 0; y < 15; y++)
		for (int x = 0; x < 15; x++)
			board[y][x] = 0;
}
void UIMinigameTable::note_place(uint8_t x, uint8_t y, uint8_t piece)
{
	if (x < 15 && y < 15)
		board[y][x] = piece ? piece : 1;
	tip_label.change_text(Locale::format_all("minigametable.place_p", {std::to_string(x), std::to_string(y), std::to_string(piece)}));
}
void UIMinigameTable::draw(float) const {
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 20));
	if (game_kind == 1) {
		int16_t board_y0 = 36;
		int16_t board_h = static_cast<int16_t>(dimension.y() - 48);
		int16_t board_w = dimension.x();
		int16_t cell_w = static_cast<int16_t>(board_w / 15);
		int16_t cell_h = static_cast<int16_t>(board_h / 15);
		for (int y = 0; y < 15; y++) {
			for (int x = 0; x < 15; x++) {
				uint8_t p = board[y][x];
				if (!p) continue;
				Point<int16_t> cell = position + Point<int16_t>(
					static_cast<int16_t>(x * cell_w + cell_w / 2 - 6),
					static_cast<int16_t>(board_y0 + y * cell_h + cell_h / 2 - 6));
				if (has_stones) {
					if (p == 1 && stone_black) stone_black.draw(cell);
					else if (p == 2 && stone_white) stone_white.draw(cell);
				} else {
					Color::Name c = (p == 1) ? Color::Name::BLACK : Color::Name::WHITE;
					Text stone(Text::Font::A11B, Text::Alignment::CENTER, c, (p == 1) ? "B" : "W");
					stone.draw(cell + Point<int16_t>(6, 0));
				}
			}
		}
	}
}
void UIMinigameTable::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) {
		MinigameExitPacket().dispatch();
		deactivate();
		return;
	}
	if (!pressed) return;
	if (keycode == 'X' || keycode == 'x') {
		MinigameExitPacket().dispatch();
		deactivate();
		return;
	}
	if (keycode == 'O' || keycode == 'o') {
		// SoftKey "x,y" → Omok place；MatchCard 用纯数字 slot
		UI::get().emplace<UISoftKey>([this](const std::string& s) {
			if (s.empty()) return;
			if (game_kind == 2) {
				try {
					uint8_t slot = static_cast<uint8_t>(std::stoi(s));
					MinigameMatchCardSelectPacket(0, slot).dispatch();
					tip_label.change_text(Locale::format("minigametable.card", s));
				} catch (...) {}
				return;
			}
			size_t comma = s.find(',');
			if (comma == std::string::npos) return;
			try {
				uint8_t x = static_cast<uint8_t>(std::stoi(s.substr(0, comma)));
				uint8_t y = static_cast<uint8_t>(std::stoi(s.substr(comma + 1)));
				if (x > 14) x = 14;
				if (y > 14) y = 14;
				MinigameOmokPlacePacket(x, y, next_piece).dispatch();
				note_place(x, y, next_piece);
				next_piece = (next_piece == 1) ? 2 : 1;
				tip_label.change_text(Locale::format("minigametable.omok", s));
				LOG(LOG_DEBUG, "UIMinigameTable: SoftKey Omok " << (int)x << "," << (int)y);
			} catch (...) {}
		}, []() {}, game_kind == 2 ? "Card slot" : "Omok x,y");
	}
}
UIElement::Type UIMinigameTable::get_type() const { return TYPE; }
Cursor::State UIMinigameTable::send_cursor(bool clicked, Point<int16_t> cursorpos)
{
	Cursor::State st = UIDragElement<PosNOTICE>::send_cursor(clicked, cursorpos);
	if (!clicked || !active) return st;
	Point<int16_t> rel = cursorpos - position;
	// 15x15 Omok 粗映射到内容区；MatchCard 用行/列 slot
	int16_t board_y0 = 36;
	int16_t board_h = static_cast<int16_t>(dimension.y() - 48);
	int16_t board_w = dimension.x();
	if (rel.y() < board_y0 || rel.y() >= board_y0 + board_h) return st;
	if (game_kind == 2) {
		uint8_t cols = 4, rows = 5;
		uint8_t col = static_cast<uint8_t>(rel.x() * cols / (board_w ? board_w : 1));
		uint8_t row = static_cast<uint8_t>((rel.y() - board_y0) * rows / (board_h ? board_h : 1));
		uint8_t slot = static_cast<uint8_t>(row * cols + col);
		MinigameMatchCardSelectPacket(0, slot).dispatch();
		tip_label.change_text(Locale::format("minigametable.card", std::to_string(slot)));
	} else {
		uint8_t x = static_cast<uint8_t>(rel.x() * 15 / (board_w ? board_w : 1));
		uint8_t y = static_cast<uint8_t>((rel.y() - board_y0) * 15 / (board_h ? board_h : 1));
		if (x > 14) x = 14;
		if (y > 14) y = 14;
		MinigameOmokPlacePacket(x, y, next_piece).dispatch();
		note_place(x, y, next_piece);
		next_piece = (next_piece == 1) ? 2 : 1;
	}
	return st;
}

UIFloatNotice::UIFloatNotice()
	: tip_label(Text::Font::A12M, Text::Alignment::CENTER, Color::Name::WHITE, "")
{
	position = Point<int16_t>(280, 60); dimension = Point<int16_t>(240, 48); dragarea = Point<int16_t>(240, 16);
	has_assets = load_window_bg("FloatNotice", backgrnd, dimension);
	if (has_assets) dragarea = Point<int16_t>(dimension.x(), 16);
	else LOG(LOG_WARN, "UIFloatNotice: missing");
	center_on_screen();
}
void UIFloatNotice::set_tip(const std::string& tip) { tip_label.change_text(tip); }
void UIFloatNotice::draw(float) const {
	if (has_assets && backgrnd) backgrnd.draw(position);
	tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 14));
}
void UIFloatNotice::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (pressed && escape) { deactivate(); return; }
	if (!pressed) return;
	if (keycode == 'S' || keycode == 's' || keycode == 'T' || keycode == 't') {
		UI::get().emplace<UISoftKey>([this](const std::string& tip) {
			if (tip.empty()) return;
			tip_label.change_text(tip);
			auto sm = UI::get().get_element<UIStatusMessenger>();
			if (!sm) sm = UI::get().emplace<UIStatusMessenger>();
			else if (!sm->is_active()) sm->makeactive();
			sm->show_status(Color::Name::YELLOW, tip);
			LOG(LOG_DEBUG, "UIFloatNotice: SoftKey → StatusMessenger len=" << tip.size());
		}, []() {}, "FloatNotice tip");
	}
	if (keycode == 'X' || keycode == 'x') {
		deactivate();
	}
}
UIElement::Type UIFloatNotice::get_type() const { return TYPE; }

// ---------------------------------------------------------------------------
// UIWhisperTargetDlg —— 悄悄话目标弹层
//
// 版式唯一的真源是原生截帧 `whisper-dlg-native.png`（261x117）与
// `Basic.img/Notice4` 的逐行左带反查（工具 `utildlg-lookup.py`，读数
// `_utildlg_lookup.txt`）：截帧每一行都能在 Notice4 的 t/c/s 里报到出处，
// 折算关系 = 截帧坐标 + (3, 1)（件自带左 3 列、上 1 行半透明投影，截帧没切进去）。
// 中间那段 Notice4/c 在原生里被纵向拉到 22（t 止于 21、s 起于 43），所以
// MID_H 不是件高 20 而是量出来的 22。
// ---------------------------------------------------------------------------

int16_t UIWhisperTargetDlg::frame_height()
{
	return TOP_H + MID_H + BOT_H;
}

UIWhisperTargetDlg::UIWhisperTargetDlg()
	: hint_label(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::WHITE,
		Locale::text("whisper.hint_target"))
	, name_field(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::BLACK,
		Rectangle<int16_t>(Point<int16_t>(0, 0), Point<int16_t>(0, 0)), 12)
{
	dimension = Point<int16_t>(FRAME_W, frame_height());
	center_on_screen();

	nl::node notice = nl::nx::UI["Basic.img"]["Notice4"];
	// 局部量不能再叫 combo —— 会和成员 `Texture combo[3][3]` 同名，
	// 于是 `combo[s][p] = Texture(combo[states[s]][p])` 变成往 nl::node 上找 width()。
	nl::node combo_src = nl::nx::UI["Basic.img"]["ComboBox"];

	frame_top = Texture(notice["t"]);
	frame_mid = Texture(notice["c"]);
	frame_bottom = Texture(notice["s"]);

	const char* states[3] = { "normal", "mouseOver", "pressed" };

	for (uint8_t s = 0; s < 3; ++s)
		for (uint8_t p = 0; p < 3; ++p)
		{
			combo[s][p] = Texture(combo_src[states[s]][p]);

			if (p == 0) combo_left_w = combo[s][p].width();
			else if (p == 1) combo_mid_w = std::max<int16_t>(combo[s][p].width(), 1);
			else combo_right_w = combo[s][p].width();
		}

	has_assets = frame_top && frame_mid && frame_bottom && combo[0][0] && combo[0][2];

	if (!has_assets)
		UiAssetResolve::diag_once("whisperdlg.frame",
			"Basic.img/Notice4(t,c,s) 或 ComboBox 三件缺失 —— 弹层只剩文字与按钮");

	// 确定 / 取消：原生截帧里两枚实测 46x18，只有 UIWindow.img/UtilDlgEx 的
	// BtOK / BtNo 是这一档（Basic.img/BtOK 是 65x24，按截帧量会溢出右沿）。
	nl::node udex = UiAssetResolve::window("UtilDlgEx");

	try_maple_btn(buttons, Buttons::BtOk, udex["BtOK"],
		Point<int16_t>(OK_X, BTN_Y), "whisperdlg.BtOK");
	try_maple_btn(buttons, Buttons::BtNo, udex["BtNo"],
		Point<int16_t>(NO_X, BTN_Y), "whisperdlg.BtNo");

	list_back = ColorBox(FIELD_W, ROW_H, Color::Name::DUSTYGRAY, 1.0f);
	list_inner = ColorBox(FIELD_W - 2, ROW_H - 2, Color::Name::GALLERY, 1.0f);
	list_mark = ColorBox(FIELD_W - 2, ROW_H - 2, Color::Name::GRAYOLIVE, 1.0f);

	name_field.set_enter_callback([this](std::string) { confirm(); });
	// ESC 一次只退一级：列表开着先收列表，已收才关窗。焦点在输入框时按键由
	// UI::send_key 直接交给 Textfield（IO/UI.cpp:202），走不到本窗的 send_key，
	// 所以这条两级逻辑必须挂在字段的回调上。
	name_field.set_key_callback(KeyAction::Id::ESCAPE, [this]() {
		if (list_open)
			list_open = false;
		else
			deactivate();
		});
	// 与 UIChatBar 的 classic 输入行同一套基线补偿（那边真机验过）。
	name_field.set_text_shift(Point<int16_t>(0, -9));

	LOG(LOG_DEBUG, "UIWhisperTargetDlg: open assets=" << has_assets
		<< " frame=" << FRAME_W << "x" << frame_height());
}

int16_t UIWhisperTargetDlg::arrow_x() const
{
	return FIELD_X + FIELD_W - combo_right_w;
}

bool UIWhisperTargetDlg::in_arrow(Point<int16_t> cursorpos) const
{
	const Point<int16_t> lt = position + Point<int16_t>(arrow_x(), FIELD_Y);
	const Point<int16_t> rb = lt + Point<int16_t>(combo_right_w, FIELD_H);

	return Rectangle<int16_t>(lt, rb).contains(cursorpos);
}

bool UIWhisperTargetDlg::in_row(Point<int16_t> cursorpos, size_t row) const
{
	const int16_t top = FIELD_Y + FIELD_H + static_cast<int16_t>(row * ROW_H);
	const Point<int16_t> lt = position + Point<int16_t>(FIELD_X, top);
	const Point<int16_t> rb = lt + Point<int16_t>(FIELD_W, ROW_H);

	return Rectangle<int16_t>(lt, rb).contains(cursorpos);
}

std::vector<std::string> UIWhisperTargetDlg::collect_candidates() const
{
	// 候选名单 = 队友 + 好友。原生截帧是收起态，列表数据源没有取证到
	// （见 whisper-dlg-prep.md §2.3），这里只用本客户端已确定存在的两份名单，
	// 不去编造「最近密语对象」这类没核对过的来源。
	std::vector<std::string> names;

	auto push = [&names](const std::string& n) {
		if (n.empty())
			return;

		for (const auto& seen : names)
			if (seen == n)
				return;

		names.push_back(n);
	};

	if (auto party = UI::get().get_element<UIParty>())
		for (const auto& m : party->get_party().members)
			if (m.id)
				push(m.name);

	if (auto buddy = UI::get().get_element<UIBuddyList>())
		for (const auto& b : buddy->get_buddies())
			if (b.online)
				push(b.name);

	return names;
}

void UIWhisperTargetDlg::open()
{
	candidates = collect_candidates();
	candidate_text.clear();

	for (const auto& n : candidates)
		candidate_text.emplace_back(Text::Font::A12M, Text::Alignment::LEFT,
			Color::Name::BLACK, n);

	hint_label.change_text(Locale::text("whisper.hint_target"));
	list_open = false;
	refocus_field();
}

void UIWhisperTargetDlg::refocus_field()
{
	name_field.set_state(Textfield::State::NORMAL);
	name_field.set_state(Textfield::State::FOCUSED);
}

void UIWhisperTargetDlg::pick_candidate(size_t row)
{
	if (row >= candidates.size())
		return;

	name_field.change_text(candidates[row]);
	list_open = false;
	// 点列表会把键盘焦点交给 AreaButton 那条路径，重新按回输入框。
	refocus_field();
}

void UIWhisperTargetDlg::confirm()
{
	std::string name = name_field.get_text();

	while (!name.empty() && name.front() == ' ')
		name.erase(name.begin());
	while (!name.empty() && name.back() == ' ')
		name.pop_back();

	if (auto chat = UI::get().get_element<UIChatBar>())
	{
		chat->set_whisper_target(name);
		LOG(LOG_DEBUG, "UIWhisperTargetDlg: confirm len=" << name.size());
	}
	else
	{
		if (!name.empty())
			WhisperPacket(name, "").dispatch();
	}

	deactivate();
}

void UIWhisperTargetDlg::update()
{
	UIElement::draw_buttons(1.0f);

	// 命中区给的是「整条 ComboBox」，不是「箭头左侧的文字区」：UI::send_cursor
	// (IO/UI.cpp:155) 只要这一点落在 bounds 外就把焦点丢掉，于是点右侧箭头会先把
	// 输入框脱焦，随后回车（确定）就再也回不到字段 —— 真机实测「点了箭头之后
	// 回车没反应」正是这条。文字起点仍按截帧的 NAME_X，箭头那 18px 的绘制与
	// 命中在 draw()/send_cursor 里各自照旧。
	const Point<int16_t> lt = position + Point<int16_t>(FIELD_X, FIELD_Y);
	Point<int16_t> rb = position + Point<int16_t>(FIELD_X + FIELD_W, FIELD_Y + FIELD_H);

	// 下拉展开时，命中区要一路盖到最后一行 —— 否则点候选行会在第 2 步被判定
	// 「落在框外」而把焦点丢掉，pick_candidate 里的 refocus_field 也白做。
	if (list_open)
		rb = Point<int16_t>(rb.x(), rb.y() + static_cast<int16_t>(candidates.size() * ROW_H));

	name_field.update(lt, rb - lt);
}

void UIWhisperTargetDlg::remove_cursor()
{
	name_field.set_state(Textfield::State::DISABLED);
	UIElement::remove_cursor();
}

void UIWhisperTargetDlg::draw(float) const
{
	frame_top.draw(position);
	frame_mid.draw(DrawArgument(position + Point<int16_t>(0, TOP_H),
		Point<int16_t>(0, MID_H)));
	frame_bottom.draw(position + Point<int16_t>(0, TOP_H + MID_H));

	// 提示文字：截帧量到的是「墨迹顶」，绘制 y 由 Text::y_for_ink_top 反解（R27-B1b）。
	hint_label.draw(position + Point<int16_t>(HINT_X, hint_label.y_for_ink_top(HINT_INK_TOP)));

	const uint8_t st = list_open ? 2 : (arrow_hover ? 1 : 0);
	const Point<int16_t> fl = position + Point<int16_t>(FIELD_X, FIELD_Y);

	combo[st][0].draw(fl);

	int16_t x = fl.x() + combo_left_w;

	while (x + combo_mid_w <= fl.x() + FIELD_W - combo_right_w)
	{
		combo[st][1].draw(Point<int16_t>(x, fl.y()));
		x += combo_mid_w;
	}

	combo[st][2].draw(Point<int16_t>(fl.x() + FIELD_W - combo_right_w, fl.y()));

	name_field.draw(Point<int16_t>(0, 0));

	if (list_open && !candidates.empty())
	{
		const Point<int16_t> lt = position + Point<int16_t>(FIELD_X, FIELD_Y + FIELD_H);

		list_back.setheight(static_cast<int16_t>(candidates.size() * ROW_H));
		list_inner.setheight(static_cast<int16_t>(candidates.size() * ROW_H) - 2);

		list_back.draw(lt);
		list_inner.draw(lt + Point<int16_t>(1, 1));

		if (hovered < candidates.size())
			list_mark.draw(lt + Point<int16_t>(1, 1 + static_cast<int16_t>(hovered * ROW_H)));

		for (size_t i = 0; i < candidate_text.size(); ++i)
			candidate_text[i].draw(lt + Point<int16_t>(6, static_cast<int16_t>(i * ROW_H) + 2));
	}

	UIElement::draw_buttons(1.0f);
}

Cursor::State UIWhisperTargetDlg::send_cursor(bool clicked, Point<int16_t> cursorpos)
{
	if (list_open)
	{
		for (size_t i = 0; i < candidates.size(); ++i)
		{
			if (!in_row(cursorpos, i))
				continue;

			hovered = i;

			if (clicked)
			{
				pick_candidate(i);

				return Cursor::State::CLICKING;
			}

			return Cursor::State::CANCLICK;
		}
	}

	arrow_hover = in_arrow(cursorpos);

	if (arrow_hover)
	{
		if (clicked)
		{
			list_open = !list_open;
			hovered = 0;
			Sound(Sound::Name::BUTTONCLICK).play();

			return Cursor::State::CLICKING;
		}

		return Cursor::State::CANCLICK;
	}

	if (clicked && list_open && !in_arrow(cursorpos))
	{
		// 列表外、非箭头的点击：点空白只收列表，点按钮仍要先让基类命中，
		// 所以这里只在框外的空白处收列表。
		const Rectangle<int16_t> box(position, position + dimension);

		if (!box.contains(cursorpos))
			list_open = false;
	}

	if (Cursor::State tf = name_field.send_cursor(cursorpos, clicked))
		return tf;

	return UIElement::send_cursor(clicked, cursorpos);
}

void UIWhisperTargetDlg::send_key(int32_t keycode, bool pressed, bool escape)
{
	if (!pressed)
		return;

	// ESC 只收列表，列表已收才关窗 —— 原生那层就是一次只退一级。
	if (escape)
	{
		if (list_open)
			list_open = false;
		else
			deactivate();

		return;
	}

	if (keycode == 'Y' || keycode == 'y')
		confirm();
	else if (keycode == 'N' || keycode == 'n')
		deactivate();
}

Button::State UIWhisperTargetDlg::button_pressed(uint16_t buttonid)
{
	switch (buttonid)
	{
		case Buttons::BtOk:
			confirm();
			break;
		case Buttons::BtNo:
			deactivate();
			break;
	}

	return Button::State::NORMAL;
}

UIElement::Type UIWhisperTargetDlg::get_type() const { return TYPE; }
}
