////////////////////////////////////////////////////////////////////////////////
//	Wedding UI —— 请柬（RING_ACTION mode5）+ 礼物台（WEDDING_ACTION mode6/7/8）
////////////////////////////////////////////////////////////////////////////////
#include "UIWedding.h"
#include "UICharInfo.h"
#include "UIMiscShells.h"
#include "UINewYearsCard.h"
#include "UINotice.h"
#include "UISoftKey.h"

#include "../UI.h"
#include "../Components/MapleButton.h"
#include "../../Character/MarriageInfo.h"
#include "../../Character/Player.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/MessagingPackets.h"
#include "../../Net/Packets/WeddingPackets.h"
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
		// 婚礼请柬（待寄出）：s9 ItemId.INVITATION_CHAPEL / INVITATION_CATHEDRAL。
		// 原生流程 = 双击请柬道具输入被邀人（道具 desc 为证，r27-b1c 台账同结论），
		// 服务端 RingActionHandler mode5 校验该 ETC 槽位道具后扣 1 张。
		int16_t find_invitation_slot(int32_t& item_id_out)
		{
			const Inventory& inv = Stage::get().get_player().get_inventory();
			int16_t slot = inv.find_item(InventoryType::Id::ETC, 4031377);
			if (slot > 0)
			{
				item_id_out = 4031377;
				return slot;
			}
			slot = inv.find_item(InventoryType::Id::ETC, 4031395);
			if (slot > 0)
				item_id_out = 4031395;
			return slot;
		}

		// 解约候选道具（s9 RingActionHandler.breakMarriageRing 的两类判据）：
		//   婚礼戒指 11128xx（itemId/10==111280，在 EQUIP/EQUIPPED）→ breakMarriage
		//   订婚凭据 4031xxx（itemId/10000==403，在 ETC）→ breakEngagement
		// 服务端要先在对应背包真的找到该道具才继续，所以这里同样只回"存在"的候选。
		int32_t find_break_item(const MarriageInfo& m)
		{
			const Inventory& inv = Stage::get().get_player().get_inventory();
			if (m.is_married() && m.ring_item_id > 0)
			{
				if (inv.find_item(InventoryType::Id::EQUIPPED, m.ring_item_id) > 0
					|| inv.find_item(InventoryType::Id::EQUIP, m.ring_item_id) > 0)
					return m.ring_item_id;
			}
			// 订婚凭据：s9 EMPTY_ENGAGEMENT_BOX_* / ENGAGEMENT_RING_* 八项。
			static const int32_t tokens[] = {
				4031357, 4031358, 4031359, 4031360,
				4031361, 4031362, 4031363, 4031364
			};
			for (int32_t id : tokens)
				if (inv.find_item(InventoryType::Id::ETC, id) > 0)
					return id;
			return 0;
		}
	}

	UIWedding::UIWedding()
		: tip_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK),
		  progress_label(Text::Font::A12B, Text::Alignment::CENTER, Color::Name::WHITE),
		  wishlist_label(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::BLACK, "")
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
		// 开窗不再自动弹软键盘（R28：无端开窗同类问题）；入口全部走按键/按钮。
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

	void UIWedding::set_gifts(const std::vector<std::string>& wishlist, const std::vector<int32_t>& gifts)
	{
		this->wishlist = wishlist;
		this->gifts = gifts;
		// 心愿单只展示前 10 条（s9 WEDDINGWL_MAX=0xA）。
		std::string line;
		for (size_t i = 0; i < this->wishlist.size() && i < 10; i++)
		{
			line += this->wishlist[i];
			if (i + 1 < this->wishlist.size() && i + 1 < 10)
				line += " | ";
		}
		wishlist_label.change_text(line);
	}

	void UIWedding::take_gift(size_t index)
	{
		if (index >= gifts.size())
			return;
		// s9 WeddingHandler mode7：byte invType（服务端未消费）+ byte itemPos（礼物下标）。
		WeddingGiftTakePacket(0, static_cast<int8_t>(index)).dispatch();
		LOG(LOG_DEBUG, "UIWedding: take_gift index=" << index);
	}

	void UIWedding::send_gift(int16_t slot, int32_t itemid, int16_t quantity)
	{
		// s9 WeddingHandler mode6：short slot + int itemid + short qty（从 ETC 背包扣）。
		WeddingGiftSendPacket(slot, itemid, quantity).dispatch();
		LOG(LOG_DEBUG, "UIWedding: send_gift slot=" << slot << " id=" << itemid);
	}

	void UIWedding::draw(float inter) const
	{
		if (!has_assets)
			return;
		if (invite)
			invite.draw(position);
		tip_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 40));
		progress_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 60));
		if (!wishlist.empty())
			wishlist_label.draw(position + Point<int16_t>(static_cast<int16_t>(dimension.x() / 2), 80));
		UIElement::draw_buttons(inter);
	}

	void UIWedding::invite_guest(const std::string& guest)
	{
		if (guest.empty())
			return;
		const Player& player = Stage::get().get_player();
		const MarriageInfo& m = player.get_marriage();
		int32_t invitation_id = 0;
		int16_t slot = find_invitation_slot(invitation_id);
		if (slot <= 0 || invitation_id <= 0 || m.relationship_id <= 0)
		{
			// 无请柬/无订婚关系：保持可见反馈，不再假装 Whisper。
			tip_label.change_text(Locale::text("wedding.no_invitation"));
			LOG(LOG_DEBUG, "UIWedding: invite blocked slot=" << slot
				<< " rel=" << m.relationship_id);
			return;
		}
		WeddingInvitePacket(guest, m.relationship_id, slot).dispatch();
		tip_label.change_text(Locale::format("wedding.invited", guest));
		LOG(LOG_DEBUG, "UIWedding: RING_ACTION invite guest=" << guest
			<< " rel=" << m.relationship_id << " slot=" << slot);
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
				invite_guest(guest_pre);
			} else {
				UI::get().emplace<UISoftKey>([this](const std::string& guest) {
					invite_guest(guest);
				}, []() {}, Locale::text("wedding.softkey_guest"));
			}
		}
		else if (keycode == 'T' || keycode == 't')
		{
			// 取礼：输入 gifts 下标（0 起）。
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				if (s.empty()) return;
				try { take_gift(static_cast<size_t>(std::stoi(s))); } catch (...) {}
			}, []() {}, Locale::text("wedding.softkey_take"));
		}
		else if (keycode == 'S' || keycode == 's')
		{
			// 送礼：输入 "slot[,qty]"，默认 qty=1；道具须确为 ETC 栏该槽位。
			UI::get().emplace<UISoftKey>([this](const std::string& s) {
				if (s.empty()) return;
				int16_t slot = 0; int16_t qty = 1;
				size_t c = s.find(',');
				try {
					slot = static_cast<int16_t>(std::stoi(c == std::string::npos ? s : s.substr(0, c)));
					if (c != std::string::npos) qty = static_cast<int16_t>(std::stoi(s.substr(c + 1)));
				} catch (...) { return; }
				if (slot <= 0 || qty < 1) return;
				const Inventory& inv = Stage::get().get_player().get_inventory();
				int32_t itemid = inv.get_item_id(InventoryType::Id::ETC, slot);
				if (itemid <= 0) return;
				send_gift(slot, itemid, qty);
			}, []() {}, Locale::text("wedding.softkey_send"));
		}
		else if (keycode == 'X' || keycode == 'x')
		{
			// 退出礼物台（解锁）：s9 mode8 → enableActions。
			WeddingRegistryExitPacket().dispatch();
			LOG(LOG_DEBUG, "UIWedding: registry exit");
		}
		else if (keycode == 'C' || keycode == 'c')
		{
			// 撤回求婚（s9 mode1）：仅本地清了 marriageItemId 时才生效。
			WeddingCancelProposalPacket().dispatch();
			tip_label.change_text(Locale::text("wedding.proposal_cancelled"));
			LOG(LOG_DEBUG, "UIWedding: proposal cancelled");
		}
		else if (keycode == 'B' || keycode == 'b')
		{
			// 解除订婚/结婚（s9 mode3 → breakMarriageRing）。
			// s9 判据：wItemId/1000000 决定查哪个背包，且必须真的持有该道具，
			// 否则整段静默 no-op；所以这里先在本端背包里找出真正存在的候选。
			const MarriageInfo& m = Stage::get().get_player().get_marriage();
			if (!m.present)
			{
				tip_label.change_text(Locale::text("wedding.no_relationship"));
				return;
			}
			const int32_t w_item = find_break_item(m);
			if (w_item <= 0)
			{
				tip_label.change_text(Locale::text("wedding.no_break_item"));
				return;
			}
			UI::get().emplace<UIYesNo>(Locale::text("wedding.break_confirm"), [w_item](bool yes) {
				if (!yes) return;
				WeddingBreakPacket(w_item).dispatch();
				LOG(LOG_DEBUG, "UIWedding: break wItem=" << w_item);
			});
		}
		else if (keycode == 'R' || keycode == 'r')
		{
			// 婚礼对话/祝福：s9 WeddingTalkHandler 读 1 个 byte action。
			// 语义完全在服务端：action==1 且发送者是婚礼 EIM 里的「宾客」→
			// 回 WEDDING_PROGRESS step2；是新人或不在 EIM → 回 WEDDING_CEREMONY_END。
			// 本端不做判定，只发 action=1（原生按钮即发 1），服务端必回 enableActions。
			WeddingTalkPacket(1).dispatch();
			tip_label.change_text(Locale::text("wedding.talk_sent"));
			LOG(LOG_DEBUG, "UIWedding: WEDDING_TALK action=1");
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
			if (!guest_pre.empty())
			{
				invite_guest(guest_pre);
				deactivate();
			}
			else
			{
				UI::get().emplace<UISoftKey>([this](const std::string& guest) {
					if (guest.empty()) { deactivate(); return; }
					invite_guest(guest);
					deactivate();
				}, [this]() { deactivate(); }, Locale::text("wedding.softkey_guest"));
			}
		}
		return Button::State::NORMAL;
	}
}
