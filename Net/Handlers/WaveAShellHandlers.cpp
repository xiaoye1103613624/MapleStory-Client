////////////////////////////////////////////////////////////////////////////////
// note
////////////////////////////////////////////////////////////////////////////////
#include "WaveAShellHandlers.h"

// UpdateHpMpAlertHandler 回灌本地设置需要 Setting<SysOptHpWarn/MpWarn>。
// QuickslotInitHandler 的真机验收判据走 perf::mark（Release 唯一观测通道）。
#include "../../Configuration.h"
#include "../../Util/PerfLog.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIViciousHammer.h"
#include "../../IO/UITypes/UIVegaSpell.h"
#include "../../IO/UITypes/UICashGachapon.h"
#include "../../IO/UITypes/UIRemoteGachapon.h"
#include "../../IO/UITypes/UIFamily.h"
#include "../../IO/UITypes/UIFamilyTree.h"
#include "../../IO/UITypes/UIMemo.h"
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UINpcTalk.h"
#include "../../IO/UITypes/UIWedding.h"
#include "../../IO/UITypes/UIMonsterCarnival.h"
#include "../../IO/UITypes/UIRpsGame.h"
#include "../../IO/UITypes/UIAriantMatch.h"
#include "../../IO/UITypes/UINewYearsCard.h"
#include "../../IO/UITypes/UIAdmin.h"
#include "../../IO/UITypes/UIAlliance.h"
#include "../../IO/UITypes/UITreasureBox.h"
#include "../../IO/UITypes/UIMapleTV.h"
#include "../../IO/UITypes/UIMegaphone.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIMerchant.h"
#include "../../IO/UITypes/UIShop.h"
#include "../../IO/UITypes/UICashShop.h"
#include "../../IO/UITypes/UIChannel.h"
#include "../../IO/UITypes/UITrade.h"
#include "../../IO/UITypes/UIWorldSelect.h"
#include "../../IO/UITypes/UIPartyRace.h"
#include "../../IO/UITypes/UIItemInventory.h"
#include "../../IO/UITypes/UIEquipInventory.h"
#include "../../IO/UITypes/UISkillBook.h"
#include "../../IO/UITypes/UIKeyConfig.h"
#include "../../IO/UITypes/UIStatsInfo.h"
#include "../../IO/UITypes/UIMaker.h"
#include "../../IO/UITypes/UIMiniMap.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIGuild.h"
#include "../../IO/UITypes/UIGuildBBS.h"
#include "../../IO/UITypes/UIGuildBoss.h"
#include "../../IO/UITypes/UIQuestLog.h"
#include "../../IO/UITypes/UIMonsterBook.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UICharInfo.h"
#include "../../IO/UITypes/UIPartySearch.h"
#include "../../IO/UITypes/UIParty.h"
#include "../../IO/UITypes/UIBuddyList.h"
#include "../../IO/UITypes/UIMessenger.h"
#include "../../IO/UITypes/UIStorage.h"
#include "../../IO/UITypes/UIDelivery.h"
#include "../../IO/UITypes/UIEvent.h"
#include "../../IO/UITypes/UITermsOfService.h"
#include "../../IO/UITypes/UIStatusBar.h"
#include "../../IO/UITypes/UIEnergyBar.h"
#include "../../IO/UITypes/UIClock.h"
#include "../../IO/UITypes/UIMTS.h"
#include "../../IO/UITypes/UIStoreBank.h"
#include "../../Character/EquipStat.h"
#include "../../Character/MapleStat.h"
#include "../../Gameplay/Stage.h"
#include "../../Gameplay/MapleMap/Npc.h"
#include "../../Net/Packets/FamilyPackets.h"
#include "../../Net/Packets/PlayerInteractionPackets.h"
#include "../../Util/Misc.h"

#include <algorithm>
#include <vector>
#include <string>
#include <unordered_set>

namespace ms
{
	namespace {
		std::unordered_set<int32_t>& obstacle_oids()
		{
			static std::unordered_set<int32_t> oids;
			return oids;
		}
		int32_t obstacle_oid_from_env(const std::string& env)
		{
			int32_t oid = 0x78000000;
			for (char c : env)
				oid = oid * 33 + static_cast<int32_t>(static_cast<unsigned char>(c));
			return oid;
		}
		Optional<UIPartyRace> ensure_party_race()
		{
			auto ui = UI::get().get_element<UIPartyRace>();
			if (!ui) ui = UI::get().emplace<UIPartyRace>();
			else if (!ui->is_active()) ui->makeactive();
			return ui;
		}
		void float_tip(const std::string& tip, Color::Name color = Color::Name::YELLOW)
		{
			// note
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		}

		// note
		bool skip_iteminfo_zeropos(InPacket& recv, int32_t& itemid_out)
		{
			if (recv.available() < 5)
				return false;
			uint8_t itemType = static_cast<uint8_t>(recv.read_byte());
			int32_t itemid = recv.read_int();
			itemid_out = itemid;
			bool isCash = recv.available() ? recv.read_bool() : false;
			if (isCash && recv.available() >= 8)
				recv.skip(8);
			if (recv.available() >= 8)
				recv.skip(8); // expiration
			if (itemType == 1)
			{
				if (recv.available() < 2 + 30)
					return false;
				recv.skip(2);
				recv.skip(30);
				if (recv.available())
					recv.skip_string();
				if (recv.available() >= 2)
					recv.skip_short();
				if (isCash)
				{
					if (recv.available() >= 10) recv.skip(10);
				}
				else if (recv.available() >= 18)
					recv.skip(18);
				if (recv.available() >= 8) recv.skip(8);
				if (recv.available() >= 4) recv.skip(4);
			}
			else if (itemid >= 5000000 && itemid <= 5000102)
			{
				if (recv.available() >= 37)
					recv.skip(37);
			}
			else
			{
				if (recv.available() >= 2) recv.skip_short();
				if (recv.available()) recv.skip_string();
				if (recv.available() >= 2) recv.skip_short();
				if ((itemid / 10000 == 233) || (itemid / 10000 == 207))
				{
					if (recv.available() >= 8) recv.skip(8);
				}
			}
			return true;
		}

		constexpr EquipStat::Id kForcedEq[12] = {
			EquipStat::Id::STR, EquipStat::Id::DEX, EquipStat::Id::INT, EquipStat::Id::LUK,
			EquipStat::Id::WATK, EquipStat::Id::WDEF, EquipStat::Id::MAGIC, EquipStat::Id::MDEF,
			EquipStat::Id::ACC, EquipStat::Id::AVOID, EquipStat::Id::SPEED, EquipStat::Id::JUMP
		};
		constexpr MapleStat::Id kForcedMs[4] = {
			MapleStat::Id::STR, MapleStat::Id::DEX, MapleStat::Id::INT, MapleStat::Id::LUK
		};
		int32_t g_forced_eq_backup[12] = {};
		uint16_t g_forced_ms_backup[4] = {};
		bool g_forced_active = false;
	}

	void ViciousHammerHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "ViciousHammerHandler: mode=0x" << std::hex << static_cast<int>(mode) << std::dec);

		auto ui = UI::get().get_element<UIViciousHammer>();
		if (!ui)
			ui = UI::get().emplace<UIViciousHammer>();
		else if (!ui->is_active())
			ui->makeactive();

		if (!ui)
			return;

		if (mode == 0x39)
		{
			if (recv.available() >= 8)
			{
				recv.read_int();
				ui->set_hammer_used(recv.read_int());
			}
			else
			{
				ui->set_status("Hammer data");
			}
		}
		else if (mode == 0x3D)
		{
			if (recv.available() >= 4)
				recv.read_int();
			ui->set_status("Hammer ready");
		}
		else
		{
			ui->set_status("Hammer op=0x" + std::to_string(mode));
		}
		while (recv.available())
			recv.skip_byte();
	}

	void VegaScrollHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "VegaScrollHandler: mode=0x" << std::hex << static_cast<int>(mode) << std::dec);
		while (recv.available())
			recv.skip_byte();

		auto ui = UI::get().get_element<UIVegaSpell>();
		if (!ui)
			ui = UI::get().emplace<UIVegaSpell>();
		else if (!ui->is_active())
			ui->makeactive();

		if (ui)
			ui->set_result(mode);
	}

	void CashGachaponOpenHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UICashGachapon>();
		if (!ui) ui = UI::get().emplace<UICashGachapon>();
		else if (!ui->is_active()) ui->makeactive();
		(void)mode; // note
	}

	void GachaponBoxHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		auto ui = UI::get().get_element<UIRemoteGachapon>();
		if (!ui) ui = UI::get().emplace<UIRemoteGachapon>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_result(mode);
		while (recv.available())
			recv.skip_byte();
	}

	void FamilyInfoHandler::handle(InPacket& recv) const
	{
		FamilyInfoData data;
		if (recv.available() >= 12) {
			data.cur_rep = recv.read_int();
			data.tot_rep = recv.read_int();
			data.today_rep = recv.read_int();
		}
		if (recv.available() >= 6) {
			data.juniors = recv.read_short();
			data.juniors_max = recv.read_short();
			recv.read_short();
		}
		if (recv.available() >= 4)
			data.leader_id = recv.read_int();
		if (recv.available())
			data.name = recv.read_string();
		if (recv.available())
			data.message = recv.read_string();
		std::vector<std::string> privs;
		if (recv.available() >= 4) {
			int32_t n = recv.read_int();
			if (n < 0) n = 0;
			if (n > 64) n = 64;
			for (int32_t i = 0; i < n && recv.available() >= 8; i++) {
				int32_t id = recv.read_int();
				recv.read_int();
				privs.emplace_back("#" + std::to_string(id));
			}
		}

		LOG(LOG_DEBUG, "FamilyInfoHandler: name=" << data.name << " rep=" << data.cur_rep
			<< " privs=" << privs.size());

		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_info(data);
			if (!privs.empty())
				ui->set_privileges(privs);
		}
		while (recv.available())
			recv.skip_byte();
		// note
	}

	void FamilyChartHandler::handle(InPacket& recv) const
	{
		// note
		if (recv.available() < 8)
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}

		int32_t viewed_id = recv.read_int();
		int32_t entry_count = recv.read_int();
		std::vector<FamilyTreeEntry> entries;
		entries.reserve(entry_count > 0 ? entry_count : 0);

		for (int32_t i = 0; i < entry_count && recv.available() >= 8; i++) {
			FamilyTreeEntry e;
			e.chr_id = recv.read_int();
			e.senior_id = recv.read_int();
			if (recv.available() >= 2) e.job = recv.read_short();
			if (recv.available()) e.level = static_cast<uint8_t>(recv.read_byte());
			if (recv.available()) e.online = recv.read_byte() != 0;
			if (recv.available() >= 20) {
				e.rep = recv.read_int();
				e.tot_rep = recv.read_int();
				recv.read_int(); // repsToSenior
				recv.read_int(); // todaysRep
				recv.read_int(); // channel / away flag
			}
			if (recv.available() >= 4)
				recv.read_int(); // minutes online
			if (recv.available())
				e.name = recv.read_string();
			entries.push_back(std::move(e));
		}

		// member info pairs + entitlement loop + add-button short
		int16_t add_enabled = 0;
		if (recv.available() >= 4) {
			int32_t info_n = recv.read_int();
			for (int32_t i = 0; i < info_n && recv.available() >= 8; i++) {
				recv.read_int();
				recv.read_int();
			}
		}
		std::vector<std::string> chart_privs;
		if (recv.available() >= 4) {
			int32_t ent_n = recv.read_int();
			if (ent_n < 0) ent_n = 0;
			if (ent_n > 64) ent_n = 64;
			for (int32_t i = 0; i < ent_n && recv.available() >= 8; i++) {
				int32_t id = recv.read_int();
				recv.read_int();
				chart_privs.emplace_back("#" + std::to_string(id));
			}
		}
		if (recv.available() >= 2)
			add_enabled = recv.read_short();

		const size_t n_entries = entries.size();
		LOG(LOG_DEBUG, "FamilyChartHandler: viewed=" << viewed_id
			<< " entries=" << n_entries << " add=" << add_enabled
			<< " privs=" << chart_privs.size());

		auto ui = UI::get().get_element<UIFamilyTree>();
		if (ui && ui->is_active())
			ui->set_chart(viewed_id, std::move(entries), add_enabled);
		if (!chart_privs.empty())
		{
			auto fam = UI::get().get_element<UIFamily>();
			if (fam && fam->is_active())
				fam->set_privileges(chart_privs);
		}
		while (recv.available())
			recv.skip_byte();
		// note
	}

	void FamilyJoinRequestHandler::handle(InPacket& recv) const
	{
		if (recv.available() < 4)
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}

		int32_t inviter_id = recv.read_int();
		std::string inviter = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FamilyJoinRequest: from=" << inviter << " id=" << inviter_id);
		while (recv.available())
			recv.skip_byte();

		std::string msg = inviter.empty()
			? "Someone wants you to join their family. Accept?"
			: (inviter + " wants you to join their family. Accept?");

		// note
		UI::get().emplace<UIYesNo>(msg, [inviter_id, inviter](bool yes) {
			FamilyAcceptPacket(inviter_id, inviter, yes).dispatch();
		});
	}

	void FamilyJoinResultHandler::handle(InPacket& recv) const
	{
		uint8_t accepted = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		while (recv.available())
			recv.skip_byte();
		std::string tip = accepted
			? (name.empty() ? "Family invite accepted." : (name + " joined your family."))
			: (name.empty() ? "Family invite declined." : (name + " declined the invite."));
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges({ tip });
			return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00001;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 3000);
	}

	void FamilyResultHandler::handle(InPacket& recv) const
	{
		uint8_t type = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "FamilyResult: type=" << static_cast<int>(type));
		while (recv.available())
			recv.skip_byte();
		static const char* msgs[] = {
			"Family ok",
			"Already in a family",
			"Family full",
			"Cannot accept",
			"Family not found",
			"Junior limit",
			"Level too low",
			"Request cancelled"
		};
		std::string tip = (type < 8) ? msgs[type]
			: ("Family notice (" + std::to_string(type) + ")");
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges({ tip });
			return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00000 ^ static_cast<int32_t>(type);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 3000);
	}

	void WeddingProgressHandler::handle(InPacket& recv) const
	{
		uint8_t step = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		int32_t groom = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t bride = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "WeddingProgress: step=" << static_cast<int>(step));
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_progress(step, groom, bride);
		while (recv.available())
			recv.skip_byte();
		// note
		(void)step;
	}

	void WeddingCeremonyEndHandler::handle(InPacket&) const
	{
		LOG(LOG_DEBUG, "WeddingCeremonyEnd");
		if (auto ui = UI::get().get_element<UIWedding>())
			ui->deactivate();
		// note
	}

	void WeddingGiftResultHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		int32_t item = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "WeddingGiftResult mode=" << (int)mode << " item=" << item
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		std::string tip = item
			? ("Wedding gift #" + std::to_string(item))
			: ("Wedding gift m" + std::to_string(mode));
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_invitation(true, tip);
		// note
	}

	void MonsterCarnivalStartHandler::handle(InPacket& recv) const
	{
		// note
		int8_t team = recv.available() ? recv.read_byte() : 0;
		int16_t self_avail = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t self_total = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t enemy_avail = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t enemy_total = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t obstacl = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "MonsterCarnivalStart team=" << static_cast<int>(team)
			<< " cp=" << self_avail << "/" << self_total
			<< " en=" << enemy_avail << "/" << enemy_total
			<< " obs=" << obstacl);
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) {
			ui->set_cp(team, self_total, self_avail);
			ui->set_enemy_cp(enemy_total, enemy_avail);
			ui->set_message(obstacl > 0
				? ("Carnival start obs=" + std::to_string(obstacl))
				: "Carnival start");
		}
		while (recv.available())
			recv.skip_byte();
	}

	void MonsterCarnivalObtainedCPHandler::handle(InPacket& recv) const
	{
		int16_t total = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t avail = recv.available() >= 2 ? recv.read_short() : 0;
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_cp(ui->get_local_team(), total, avail);
		// note
		(void)total; (void)avail;
	}

	void MonsterCarnivalPartyCPHandler::handle(InPacket& recv) const
	{
		int8_t team = recv.available() ? recv.read_byte() : 0;
		int16_t total = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t avail = recv.available() >= 2 ? recv.read_short() : 0;
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
		{
			if (team == ui->get_local_team())
				ui->set_cp(team, total, avail);
			else
				ui->set_enemy_cp(total, avail);
		}
		(void)team; (void)total; (void)avail;
	}

	void MonsterCarnivalMessageHandler::handle(InPacket& recv) const
	{
		uint8_t msg = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		while (recv.available())
			recv.skip_byte();
		static const char* k_mc_msg[] = {
			"CPQ ok", "Not enough CP", "Can't use here", "Party full",
			"Wrong map", "Already summoned", "Dojo closed", "Request denied"
		};
		std::string tip = (msg < 8) ? k_mc_msg[msg]
			: ("MC msg " + std::to_string(msg));
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
		{
			ui->set_message(tip);
			return;
		}
		// note fallback
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33C00000 ^ static_cast<int32_t>(msg);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -40), tip, 0, 2500);
	}

	void MonsterCarnivalLeaveHandler::handle(InPacket&) const
	{
		if (auto ui = UI::get().get_element<UIMonsterCarnival>())
			ui->deactivate();
	}

	void RpsGameHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "RpsGameHandler: mode=" << static_cast<int>(mode));

		auto ui = UI::get().get_element<UIRpsGame>();
		if (!ui) ui = UI::get().emplace<UIRpsGame>();
		else if (!ui->is_active()) ui->makeactive();
		if (!ui) return;

		switch (mode) {
			case 8: // open npc
				if (recv.available() >= 4) recv.read_int();
				ui->set_status("RPS ready");
				break;
			case 0x06:
			{
				std::string tip = recv.available() >= 4
					? ("Need " + std::to_string(recv.read_int()) + " mesos")
					: "Not enough mesos";
				ui->set_status(tip);
				break;
			}
			case 0x0B:
			{
				uint8_t sel = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				uint8_t ans = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				ui->set_round_result(sel, ans);
				break;
			}
			case 0x0D:
				ui->set_status("Game closed");
				ui->deactivate();
				break;
			default:
			{
				std::string tip = "RPS mode=" + std::to_string(mode);
				ui->set_status(tip);
				break;
			}
		}
		while (recv.available())
			recv.skip_byte();
	}

	void AriantResultHandler::handle(InPacket& recv) const
	{
		int8_t n = recv.available() ? recv.read_byte() : 0;
		std::string line;
		for (int8_t i = 0; i < n && recv.available(); ++i)
		{
			std::string name = recv.available() ? recv.read_string() : "";
			int8_t score = recv.available() ? recv.read_byte() : 0;
			if (!line.empty()) line += " | ";
			line += name + ":" + std::to_string(score);
		}
		LOG(LOG_DEBUG, "AriantResult n=" << (int)n << " " << line);
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIAriantMatch>();
		if (!ui) ui = UI::get().emplace<UIAriantMatch>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) {
			ui->show_result_frame(10);
			ui->set_score(line.empty() ? "Ariant result" : line);
		}
		// note
	}

	void AriantScoreHandler::handle(InPacket& recv) const
	{
		// note
		std::string line;
		int count = 0;
		while (recv.available() >= 3 && count < 8) {
			std::string name = recv.read_string();
			uint8_t score = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
			if (name.empty()) break;
			if (!line.empty()) line += " | ";
			line += name + ":" + std::to_string(score);
			++count;
		}
		while (recv.available())
			recv.skip_byte();
		std::string tip = line.empty() ? "Score update" : line;
		auto ui = UI::get().get_element<UIAriantMatch>();
		if (!ui) ui = UI::get().emplace<UIAriantMatch>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_score(tip);
		// note
	}

	void NewYearCardResHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		std::string tip = "New Year card mode=" + std::to_string(mode);
		if (recv.available() >= 4) {
			int32_t cardid = recv.read_int();
			tip += " id=" + std::to_string(cardid);
		}
		if (recv.available() >= 2) {
			std::string from = recv.read_string();
			if (!from.empty()) tip += " from=" + from;
		}
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "NewYearCardRes: " << tip);
		auto ui = UI::get().get_element<UINewYearsCard>();
		if (!ui) ui = UI::get().emplace<UINewYearsCard>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_tip(tip);
		// note
	}

	void AdminResultHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		std::string payload = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "AdminResult: mode=" << static_cast<int>(mode)
			<< " len=" << payload.size());
		while (recv.available())
			recv.skip_byte();
		const char* msg = "Admin result";
		switch (mode)
		{
			case 0x04: msg = "Admin hide"; break;
			case 0x0F: msg = "Admin enter"; break;
			case 0x1A: msg = "Admin alert"; break;
			case 0x1C: msg = "Admin notice"; break;
			case 0x11: msg = "Admin ban"; break;
			case 0x12: msg = "Admin unban"; break;
			case 0x15: msg = "Admin mute"; break;
			default: break;
		}
		std::string tip = payload.empty()
			? (std::string(msg) + " m" + std::to_string(mode))
			: payload;
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_message(tip);
		// note
	}

	void MapleTVUseResHandler::handle(InPacket& recv) const
	{
		bool ok = !recv.available() || recv.read_byte() == 0;
		std::string tip = ok ? "MapleTV ok" : "MapleTV failed";
		LOG(LOG_DEBUG, "MapleTVUseRes ok=" << ok << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIMapleTV>();
		if (!ui) ui = UI::get().emplace<UIMapleTV>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_tip(tip);
		// note
	}

	namespace
	{
// note
		void skip_tv_charlook(InPacket& recv)
		{
			recv.skip_byte();
			recv.skip_byte();
			recv.skip_int();
			recv.skip_byte();
			recv.skip_int();
			while (recv.available() && recv.read_byte() != -1)
				recv.skip_int();
			while (recv.available() && recv.read_byte() != -1)
				recv.skip_int();
			recv.skip_int();
			recv.skip_int();
			recv.skip_int();
			recv.skip_int();
		}
	}

	void EnableTVHandler::handle(InPacket& recv) const
	{
		recv.read_int();
		recv.read_byte();
		LOG(LOG_DEBUG, "EnableTV");
		while (recv.available())
			recv.skip_byte();
		// note
		auto ui = UI::get().get_element<UIMapleTV>();
		if (!ui)
			ui = UI::get().emplace<UIMapleTV>();
		else if (!ui->is_active())
			ui->makeactive();
	}

	void SendTVHandler::handle(InPacket& recv) const
	{
		// note
		int8_t flag = recv.read_byte();
		int8_t type = recv.read_byte();
		skip_tv_charlook(recv);
		std::string name = recv.read_string();
		std::string partner;
		if (flag == 3)
			partner = recv.read_string();
		else
			recv.read_short();
		std::string tip = name;
		if (!partner.empty())
			tip += " & " + partner;
		tip += " [";
		tip += std::to_string(static_cast<int>(type));
		tip += "] ";
		// note
		for (int i = 0; i < 5; i++)
		{
			std::string line = recv.read_string();
			if (!line.empty())
			{
				if (i) tip += " | ";
				tip += line;
			}
		}
		recv.read_int(); // time
		if (flag == 3)
			skip_tv_charlook(recv);
		while (recv.available())
			recv.skip_byte();

		auto ui = UI::get().get_element<UIMapleTV>();
		if (!ui) ui = UI::get().emplace<UIMapleTV>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) ui->set_tip(tip);
		LOG(LOG_DEBUG, "SendTV tip=" << tip);
	}

	void RemoveTVHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIMapleTV>())
			ui->deactivate();
		LOG(LOG_DEBUG, "RemoveTV");
	}

	void MapleLifeResultHandler::handle(InPacket& recv) const
	{
		// sendMapleLifeCharacterInfo: int 0
		// sendMapleLifeNameError: int 2 + int 3 + byte 0
		int32_t code = recv.read_int();
		if (code == 2 && recv.available() >= 5)
		{
			int32_t sub = recv.read_int();
			recv.read_byte();
			LOG(LOG_DEBUG, "MapleLifeResult nameError sub=" << sub);
			while (recv.available())
				recv.skip_byte();
			std::string tip = "MapleLife name error " + std::to_string(sub);
			auto ui = UI::get().get_element<UIAdmin>();
			if (!ui) ui = UI::get().emplace<UIAdmin>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_message(tip);
			return;
		}
		LOG(LOG_DEBUG, "MapleLifeResult code=" << code);
		while (recv.available())
			recv.skip_byte();
		if (code != 0)
		{
			std::string tip = "MapleLife code=" + std::to_string(code);
			auto ui = UI::get().get_element<UIAdmin>();
			if (!ui) ui = UI::get().emplace<UIAdmin>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_message(tip);
		}
	}

	void MapleLifeErrorHandler::handle(InPacket& recv) const
	{
		recv.read_byte();
		int32_t code = recv.read_int();
		LOG(LOG_DEBUG, "MapleLifeError code=" << code);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "MapleLife error " + std::to_string(code);
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) ui->set_message(tip);
	}

	void AvatarMegaphoneHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		std::string line1 = recv.available() ? recv.read_string() : "";
		std::string tip = name.empty() ? "Avatar Megaphone" : (name + ": " + line1);
		LOG(LOG_DEBUG, "AvatarMegaphone from=" << name << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIAvatarMegaphone>();
		if (!ui) ui = UI::get().emplace<UIAvatarMegaphone>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_tip(tip);
		// note
	}

	void ClaimResultHandler::handle(InPacket& recv) const
	{
		int8_t result = recv.available() ? recv.read_byte() : 0;
		std::string tip = result ? ("Claim fail " + std::to_string(result)) : "Claim ok";
		LOG(LOG_DEBUG, "ClaimResult=" << (int)result << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		auto claim = UI::get().get_element<UIClaim>();
		if (!claim) claim = UI::get().emplace<UIClaim>();
		else if (!claim->is_active()) claim->makeactive();
		if (claim)
		{
			claim->set_tip(tip);
			return;
		}
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) ui->set_message(tip);
	}

	void IncubatorResultHandler::handle(InPacket& recv) const
	{
		int32_t item = recv.available() >= 4 ? recv.read_int() : 0;
		std::string tip = item ? ("Incubator 鈫?#" + std::to_string(item)) : "Incubator result";
		LOG(LOG_DEBUG, "IncubatorResult item=" << item << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIIncubator>();
		if (!ui) ui = UI::get().emplace<UIIncubator>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_tip(tip);
		// note
	}

	void MemoResultHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "MemoResultHandler: mode=" << static_cast<int>(mode));

		switch (mode) {
			case 3: // note
			{
				auto ui = UI::get().get_element<UIMemo>();
				if (!ui) ui = UI::get().emplace<UIMemo>();
				else if (!ui->is_active()) ui->makeactive();
				if (!ui) return;
				uint8_t count = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				std::vector<MemoEntry> notes;
				notes.reserve(count);
				for (uint8_t i = 0; i < count && recv.available(); i++) {
					MemoEntry n;
					n.id = recv.read_int();
					n.from = recv.read_string();
					n.message = recv.read_string();
					if (recv.available() >= 8) recv.read_long();
					if (recv.available()) n.fame = recv.read_byte();
					notes.push_back(n);
				}
				const size_t n = notes.size();
				ui->set_notes(std::move(notes));
				// note
				break;
			}
			case 4:
			{
				auto ui = UI::get().get_element<UIMemo>();
				if (!ui) ui = UI::get().emplace<UIMemo>();
				else if (!ui->is_active()) ui->makeactive();
				if (ui) ui->show_tip("Note sent.");
				break;
			}
			case 5:
			{
				uint8_t err = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
				std::string tip = "Note error " + std::to_string(err);
				auto ui = UI::get().get_element<UIMemo>();
				if (!ui) ui = UI::get().emplace<UIMemo>();
				else if (!ui->is_active()) ui->makeactive();
				if (ui) ui->show_tip(tip);
				break;
			}
			default:
				// note
				break;
		}
		while (recv.available())
			recv.skip_byte();
	}

	void CoconutScoreHandler::handle(InPacket& recv) const
	{
		int16_t maple = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t story = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "CoconutScore maple=" << maple << " story=" << story);
		while (recv.available())
			recv.skip_byte();
		if (auto ui = ensure_party_race())
			ui->set_score(maple, story);
		// note
	}

	void HitSnowballHandler::handle(InPacket& recv) const
	{
		int8_t what = recv.available() ? recv.read_byte() : 0;
		int32_t dmg = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "HitSnowball what=" << (int)what << " dmg=" << dmg);
		while (recv.available())
			recv.skip_byte();
		if (auto ui = ensure_party_race())
			ui->set_stage(what);
		(void)dmg;
	}

	void SnowballMessageHandler::handle(InPacket& recv) const
	{
		int8_t team = recv.available() ? recv.read_byte() : 0;
		int32_t msg = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "SnowballMessage team=" << (int)team << " msg=" << msg);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto ui = ensure_party_race())
			ui->set_stage(team);
	}

	void CoconutHitHandler::handle(InPacket& recv) const
	{
		int16_t id = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t delay = recv.available() >= 2 ? recv.read_short() : 0;
		int8_t type = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "CoconutHit id=" << id << " delay=" << delay << " type=" << (int)type);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Coconut #" + std::to_string(id) + " t" + std::to_string(type);
		const int32_t oid = 0x73000000 ^ id;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, tip);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::SUMMON,
				pos + Point<int16_t>(id * 2, -40), tip, 0, 1500 + delay);
		}
		(void)delay; (void)type;
		// note
	}

	void SnowballStateHandler::handle(InPacket& recv) const
	{
		// note
		if (recv.available() == 21)
		{
			recv.skip(21);
			LOG(LOG_DEBUG, "SnowballState enterMap pad");
			return;
		}
		int8_t state = recv.available() ? recv.read_byte() : 0;
		int32_t hp0 = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t hp1 = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t pos0 = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available()) recv.read_byte();
		int16_t pos1 = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available()) recv.read_byte();
		LOG(LOG_DEBUG, "SnowballState state=" << (int)state
			<< " hp=" << hp0 << "/" << hp1
			<< " pos=" << pos0 << "/" << pos1);
		while (recv.available())
			recv.skip_byte();
		if (auto ui = ensure_party_race()) {
			ui->set_stage(state);
			ui->set_score(static_cast<int16_t>(hp0), static_cast<int16_t>(hp1));
		}
		// note
	}

	void OxQuizHandler::handle(InPacket& recv) const
	{
		// note
		uint8_t ask = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		uint8_t qset = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		int16_t qid = recv.available() >= 2 ? recv.read_short() : 0;
		while (recv.available())
			recv.skip_byte();
		std::string tip = ask
			? ("OX Q set=" + std::to_string(qset) + " id=" + std::to_string(qid))
			: ("OX A set=" + std::to_string(qset) + " id=" + std::to_string(qid));
		LOG(LOG_DEBUG, "OxQuiz: " << tip);
		auto quiz = UI::get().get_element<UISpeedQuiz>();
		if (!quiz) quiz = UI::get().emplace<UISpeedQuiz>();
		else if (!quiz->is_active()) quiz->makeactive();
		if (quiz)
			quiz->set_tip(tip);
		// note
	}

	void HorntailCaveHandler::handle(InPacket& recv) const
	{
		uint8_t mode = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		LOG(LOG_DEBUG, "HorntailCave mode=" << static_cast<int>(mode));
		// note
		if (auto ui = ensure_party_race())
			ui->set_stage(mode);
		while (recv.available())
			recv.skip_byte();
	}

	void OpenUIHandler::handle(InPacket& recv) const
	{
// note
		int8_t ui = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "OpenUI id=" << (int)ui);
		Player& player = Stage::get().get_player();
		switch (ui)
		{
			case 1:
				UI::get().emplace<UIItemInventory>(player.get_inventory());
				break;
			case 2:
				UI::get().emplace<UIEquipInventory>(player.get_inventory());
				break;
			case 3:
				UI::get().emplace<UISkillBook>(player.get_stats(), player.get_skills());
				break;
			case 4:
				UI::get().emplace<UIKeyConfig>(player.get_inventory(), player.get_skills());
				break;
			case 5:
				UI::get().emplace<UIStatsInfo>(player.get_stats());
				break;
			case 6:
			{
				auto q = UI::get().get_element<UIQuestLog>();
				if (!q) UI::get().emplace<UIQuestLog>(player.get_quests());
				else if (!q->is_active()) q->makeactive();
				break;
			}
			case 7:
			{
				auto g = UI::get().get_element<UIGuild>();
				if (!g) UI::get().emplace<UIGuild>();
				else if (!g->is_active()) g->makeactive();
				break;
			}
			case 8:
			{
				auto f = UI::get().get_element<UIFamily>();
				if (!f) UI::get().emplace<UIFamily>();
				else if (!f->is_active()) f->makeactive();
				break;
			}
			case 9:
			{
				auto m = UI::get().get_element<UIMaker>();
				if (!m) UI::get().emplace<UIMaker>();
				else if (!m->is_active()) m->makeactive();
				break;
			}
			case 10: // note
			{
				auto b = UI::get().get_element<UIMonsterBook>();
				if (!b) UI::get().emplace<UIMonsterBook>();
				else if (!b->is_active()) b->makeactive();
				break;
			}
			case 0x0B: // note
			{
				auto g = UI::get().get_element<UIGuild>();
				if (!g) UI::get().emplace<UIGuild>();
				else if (!g->is_active()) g->makeactive();
				break;
			}
			case 0x0C: // Party
			{
				auto p = UI::get().get_element<UIParty>();
				if (!p) UI::get().emplace<UIParty>();
				else if (!p->is_active()) p->makeactive();
				break;
			}
			case 0x0D: // Buddy
			{
				auto b = UI::get().get_element<UIBuddyList>();
				if (!b) UI::get().emplace<UIBuddyList>();
				else if (!b->is_active()) b->makeactive();
				break;
			}
			case 0x0E: // Messenger
			{
				auto m = UI::get().get_element<UIMessenger>();
				if (!m) m = UI::get().emplace<UIMessenger>();
				else if (!m->is_active()) m->makeactive();
				if (m) m->try_create_room();
				break;
			}
			case 0x0F: // Storage
			{
				auto s = UI::get().get_element<UIStorage>();
				if (!s) UI::get().emplace<UIStorage>();
				else if (!s->is_active()) s->makeactive();
				break;
			}
			case 0x10: // Delivery / Duey
			{
				auto d = UI::get().get_element<UIDelivery>();
				if (!d) UI::get().emplace<UIDelivery>();
				else if (!d->is_active()) d->makeactive();
				break;
			}
			case 0x11: // PersonalShop
			{
				auto ps = UI::get().get_element<UIPersonalShop>();
				if (!ps) UI::get().emplace<UIPersonalShop>();
				else if (!ps->is_active()) ps->makeactive();
				break;
			}
			case 0x12: // Monster Carnival
			{
				auto c = UI::get().get_element<UIMonsterCarnival>();
				if (!c) UI::get().emplace<UIMonsterCarnival>();
				else if (!c->is_active()) c->makeactive();
				break;
			}
			case 0x16: // Party Search
			{
				auto p = UI::get().get_element<UIPartySearch>();
				if (!p) UI::get().emplace<UIPartySearch>();
				else if (!p->is_active()) p->makeactive();
				break;
			}
			case 0x17: // Item Creation / Maker
			{
				auto m = UI::get().get_element<UIMaker>();
				if (!m) UI::get().emplace<UIMaker>();
				else if (!m->is_active()) m->makeactive();
				break;
			}
			case 0x1B: // Family
			{
				auto f = UI::get().get_element<UIFamily>();
				if (!f) UI::get().emplace<UIFamily>();
				else if (!f->is_active()) f->makeactive();
				break;
			}
			case 0x1C: // Family Pedigree / Tree
			{
				auto t = UI::get().get_element<UIFamilyTree>();
				if (!t) UI::get().emplace<UIFamilyTree>();
				else if (!t->is_active()) t->makeactive();
				break;
			}
			case 0x20: // Maple Event
			{
				auto e = UI::get().get_element<UIEvent>();
				if (!e) UI::get().emplace<UIEvent>();
				else if (!e->is_active()) e->makeactive();
				break;
			}
			case 0x21: // Char Info
			{
				auto ci = UI::get().get_element<UICharInfo>();
				if (!ci) UI::get().emplace<UICharInfo>(player.get_oid());
				else if (!ci->is_active()) ci->makeactive();
				break;
			}
			case 0x22: // Memo / Note
			{
				auto memo = UI::get().get_element<UIMemo>();
				if (!memo) UI::get().emplace<UIMemo>();
				else if (!memo->is_active()) memo->makeactive();
				break;
			}
			case 0x25: // Admin
			{
				auto ad = UI::get().get_element<UIAdmin>();
				if (!ad) UI::get().emplace<UIAdmin>();
				else if (!ad->is_active()) ad->makeactive();
				break;
			}
			case 0x13: // Alliance
			{
				auto a = UI::get().get_element<UIAlliance>();
				if (!a) UI::get().emplace<UIAlliance>();
				else if (!a->is_active()) a->makeactive();
				break;
			}
			case 0x14: // Wedding
			{
				auto w = UI::get().get_element<UIWedding>();
				if (!w) UI::get().emplace<UIWedding>();
				else if (!w->is_active()) w->makeactive();
				break;
			}
			case 0x15: // RPS
			{
				auto r = UI::get().get_element<UIRpsGame>();
				if (!r) UI::get().emplace<UIRpsGame>();
				else if (!r->is_active()) r->makeactive();
				break;
			}
			case 0x18: // StoreBank / Fredrick
			{
				auto sb = UI::get().get_element<UIStoreBank>();
				if (!sb) UI::get().emplace<UIStoreBank>();
				else if (!sb->is_active()) sb->makeactive();
				break;
			}
			case 0x19: // Ariant
			{
				auto ar = UI::get().get_element<UIAriantMatch>();
				if (!ar) UI::get().emplace<UIAriantMatch>();
				else if (!ar->is_active()) ar->makeactive();
				break;
			}
			case 0x1A: // MiniMap
			{
				auto mm = UI::get().get_element<UIMiniMap>();
				if (!mm) UI::get().emplace<UIMiniMap>(player.get_stats());
				else if (!mm->is_active()) mm->makeactive();
				break;
			}
			default:
				// note
				if (auto ad = UI::get().get_element<UIAdmin>())
				{
					if (ad->is_active())
						ad->set_message("OpenUI id=" + std::to_string(static_cast<int>(ui)));
				}
				break;
		}
		while (recv.available())
			recv.skip_byte();
	}

	void LockUIHandler::handle(InPacket& recv) const
	{
		bool lock = recv.available() && recv.read_byte() != 0;
		LOG(LOG_DEBUG, "LockUI lock=" << lock);
		while (recv.available())
			recv.skip_byte();
		if (lock)
			UI::get().disable();
		else
			UI::get().enable();
		// note
	}

	void DisableUIHandler::handle(InPacket& recv) const
	{
		bool disable = recv.available() && recv.read_byte() != 0;
		LOG(LOG_DEBUG, "DisableUI disable=" << disable);
		while (recv.available())
			recv.skip_byte();
		if (disable)
			UI::get().disable();
		else
			UI::get().enable();
		// note
	}

	void PlayerHintHandler::handle(InPacket& recv) const
	{
		std::string hint = recv.read_string();
		int16_t width = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t height = recv.available() >= 2 ? recv.read_short() : 0;
		if (recv.available()) recv.read_byte();
		LOG(LOG_DEBUG, "PlayerHint w=" << width << " h=" << height << " " << hint);
		while (recv.available())
			recv.skip_byte();
		if (!hint.empty())
		{
			if (auto talk = UI::get().get_element<UINpcTalk>())
			{
				if (talk->is_active())
					return;
			}
			auto alarm = UI::get().get_element<UIQuestAlarm>();
			if (!alarm) alarm = UI::get().emplace<UIQuestAlarm>();
			else if (!alarm->is_active()) alarm->makeactive();
			if (alarm)
				alarm->set_tip(hint);
			// note
		}
		(void)width; (void)height;
	}

	void SpawnGuideHandler::handle(InPacket& recv) const
	{
		bool spawn = recv.available() && recv.read_bool();
		LOG(LOG_DEBUG, "SpawnGuide spawn=" << spawn);
		while (recv.available())
			recv.skip_byte();
		const int32_t guide_oid = 0x70000001;
		auto& markers = Stage::get().get_field_markers();
		if (spawn)
		{
			if (markers.contains(guide_oid))
				markers.set_label(guide_oid, "Guide");
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(guide_oid, FieldMarker::Kind::DOOR,
					pos + Point<int16_t>(40, 0), "Guide");
			}
		}
		else
			markers.remove(guide_oid);
		// note
	}

	void TalkGuideHandler::handle(InPacket& recv) const
	{
		// note
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		if (mode == 0)
		{
			std::string talk = recv.read_string();
			recv.skip(std::min<size_t>(8, recv.available()));
			LOG(LOG_DEBUG, "TalkGuide: " << talk);
			while (recv.available())
				recv.skip_byte();
			if (!talk.empty())
			{
				auto alarm = UI::get().get_element<UIQuestAlarm>();
				if (!alarm) alarm = UI::get().emplace<UIQuestAlarm>();
				else if (!alarm->is_active()) alarm->makeactive();
				if (alarm)
					alarm->set_tip(talk);
			}
		}
		else
		{
			int32_t hint = recv.available() >= 4 ? recv.read_int() : 0;
			if (recv.available() >= 4) recv.read_int();
			LOG(LOG_DEBUG, "GuideHint id=" << hint);
			while (recv.available())
				recv.skip_byte();
			// note
		}
	}

	void DojoWarpUpHandler::handle(InPacket& recv) const
	{
		// note
		int8_t a = recv.available() ? recv.read_byte() : 0;
		int8_t b = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "DojoWarpUp " << (int)a << "," << (int)b);
		while (recv.available())
			recv.skip_byte();
// note
		if (auto ui = ensure_party_race())
			ui->set_stage(b);
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
			clock->set_clock(1, "Dojo warp", 0);
		(void)a;
	}

	void CancelChairResultHandler::handle(InPacket& recv) const
	{
		// note
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		int16_t id = 0;
		if (mode != 0 && recv.available() >= 2)
			id = recv.read_short();
		LOG(LOG_DEBUG, "CancelChairResult mode=" << (int)mode << " id=" << id);
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().set_state(Char::State::STAND);
		(void)mode; (void)id;
		// note
	}

	void LeftKnockBackHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "LeftKnockBack");
		Player& player = Stage::get().get_player();
		player.set_direction(true); // face/flip toward knock (left)
		PhysicsObject& ph = player.get_phobj();
		ph.hspeed = -8.0;
		ph.hforce = -player.get_walkforce() * 2.0;
		// note
	}

	void SheepRanchInfoHandler::handle(InPacket& recv) const
	{
		int8_t wolf = recv.available() ? recv.read_byte() : 0;
		int8_t sheep = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SheepRanch wolf=" << (int)wolf << " sheep=" << (int)sheep);
		while (recv.available())
			recv.skip_byte();
		if (auto ui = ensure_party_race())
			ui->set_score(wolf, sheep);
		// note
	}

	void SheepRanchClothesHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int8_t clothes = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SheepRanchClothes cid=" << cid << " clothes=" << (int)clothes);
		while (recv.available())
			recv.skip_byte();
// note
		if (auto ui = ensure_party_race())
			ui->set_stage(clothes);
		if (auto character = Stage::get().get_character(cid))
			character->show_iron_body();
		(void)clothes;
	}

	void PyramidGaugeHandler::handle(InPacket& recv) const
	{
		int32_t gauge = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "PyramidGauge=" << gauge);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Pyramid " + std::to_string(gauge);
		if (auto ui = ensure_party_race())
			ui->set_stage(gauge);
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
			clock->set_clock(1, tip, 0);
	}

	void PyramidScoreHandler::handle(InPacket& recv) const
	{
		int8_t score = recv.available() ? recv.read_byte() : 0;
		int32_t exp = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "PyramidScore rank=" << (int)score << " exp=" << exp);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto ui = ensure_party_race())
		{
			ui->set_stage(score);
			ui->show_result(score <= 3);
		}
		(void)exp;
	}

	void ClockHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		std::string tip;
		int32_t seconds = 0;
		if (type == 1)
		{
			int8_t hour = recv.available() ? recv.read_byte() : 0;
			int8_t min = recv.available() ? recv.read_byte() : 0;
			int8_t sec = recv.available() ? recv.read_byte() : 0;
			tip = std::to_string(hour) + ":"
				+ std::to_string(min) + ":"
				+ std::to_string(sec);
			LOG(LOG_DEBUG, "Clock time " << tip);
		}
		else if (type == 2)
		{
			seconds = recv.available() >= 4 ? recv.read_int() : 0;
			tip = "Timer " + std::to_string(seconds) + "s";
			LOG(LOG_DEBUG, "Clock countdown=" << seconds);
		}
		else if (type == 3)
		{
			if (recv.available())
				recv.read_byte();
			int32_t ts = recv.available() >= 4 ? recv.read_int() : 0;
			tip = "Clock ts=" + std::to_string(ts);
			LOG(LOG_DEBUG, "Clock type3 ts=" << ts);
		}
		else
		{
			tip = "Clock type=" + std::to_string(type);
			LOG(LOG_DEBUG, tip);
		}
		while (recv.available())
			recv.skip_byte();
		if (!tip.empty())
		{
			auto clock = UI::get().get_element<UIClock>();
			if (!clock)
				clock = UI::get().emplace<UIClock>();
			else if (!clock->is_active())
				clock->makeactive();
			if (clock)
				clock->set_clock(type, tip, seconds);
		}
	}

	void StopClockHandler::handle(InPacket& recv) const
	{
		if (recv.available())
			recv.read_byte();
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "StopClock");
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
			clock->stop();
	}

	void BlowWeatherHandler::handle(InPacket& recv) const
	{
		bool clear = recv.available() ? recv.read_bool() : true;
		int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
		std::string msg;
		if (!clear && recv.available())
			msg = recv.read_string();
		LOG(LOG_DEBUG, "BlowWeather clear=" << clear << " item=" << itemid << " len=" << msg.size());
		while (recv.available())
			recv.skip_byte();
		const int32_t weather_oid = 0x77000001;
		if (clear || itemid == 0)
		{
			Stage::get().get_field_markers().remove(weather_oid);
			Stage::get().clear_effect();
		}
		else if (itemid > 0)
		{
			const std::string label = msg.empty() ? "Weather" : msg;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(weather_oid))
				markers.set_label(weather_oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				// ItemEff/CashEff via FieldMarker skill_id=itemid (MapEffect expects Map002/Effect.img path)
				markers.spawn(weather_oid, FieldMarker::Kind::KITE,
					pos + Point<int16_t>(0, -80), label, itemid, 20000);
			}
		}
		// note
		if (!msg.empty() && (clear || itemid == 0))
		{
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = weather_oid ^ 1;
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (markers.contains(mid))
				markers.set_label(mid, msg);
			else
				markers.spawn(mid, FieldMarker::Kind::KITE, pos + Point<int16_t>(0, -80), msg, 0, 4000);
		}
	}

	void GmEventInstructionsHandler::handle(InPacket& recv) const
	{
		if (recv.available())
			recv.read_byte();
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "GmEventInstructions");
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev)
			ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active())
			ev->makeactive();
		// note
	}

	void ContiMoveHandler::handle(InPacket& recv) const
	{
		int8_t a = recv.available() ? recv.read_byte() : 0;
		int8_t b = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "ContiMove a=" << (int)a << " b=" << (int)b);
		std::string tip = b == 4
			? ("Boat departing #" + std::to_string(static_cast<int>(a)))
			: ("Boat arrived #" + std::to_string(static_cast<int>(a)));
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
		{
			if (b == 4)
				clock->set_clock(2, tip, 30);
			else
				clock->set_clock(1, tip, 0);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void ContiStateHandler::handle(InPacket& recv) const
	{
		int8_t state = recv.available() ? recv.read_byte() : 0;
		if (recv.available())
			recv.read_byte();
		LOG(LOG_DEBUG, "ContiState=" << (int)state);
		while (recv.available())
			recv.skip_byte();
		std::string tip = state == 1 ? "Boat docked" : "Boat left";
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
			clock->set_clock(1, tip, 0); // docked/left 均显 tip（对齐 ContiMove）
	}

	void ThrowGrenadeHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t x = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t y = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t keyDown = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skillId = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t skillLevel = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ThrowGrenade cid=" << cid << " (" << x << "," << y << ") skill="
			<< skillId << " lv=" << skillLevel << " keyDown=" << keyDown);
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().spawn_summon(cid ^ 0x60000000, cid, skillId,
			Point<int16_t>(static_cast<int16_t>(x), static_cast<int16_t>(y)),
			static_cast<int8_t>(skillLevel), 3000);
		(void)keyDown;
	}

	void FieldObstacleOnOffHandler::handle(InPacket& recv) const
	{
		std::string env = recv.available() ? recv.read_string() : "";
		int32_t mode = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "FieldObstacleOnOff env=" << env << " mode=" << mode);
		while (recv.available())
			recv.skip_byte();
		// note
		int32_t oid = obstacle_oid_from_env(env);
		auto& markers = Stage::get().get_field_markers();
		if (mode)
		{
			const std::string label = env.empty() ? "Obstacle" : env;
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR,
					pos + Point<int16_t>(0, -40), label, 0, 8000);
			}
			obstacle_oids().insert(oid);
		}
		else
		{
			markers.remove(oid);
			obstacle_oids().erase(oid);
		}
	}

	void FieldObstacleOnOffListHandler::handle(InPacket& recv) const
	{
		int32_t count = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "FieldObstacleOnOffList count=" << count);
		auto& markers = Stage::get().get_field_markers();
		for (int32_t i = 0; i < count && recv.available(); ++i)
		{
			std::string env = recv.read_string();
			int32_t mode = recv.available() >= 4 ? recv.read_int() : 0;
			int32_t oid = obstacle_oid_from_env(env);
			if (mode)
			{
				const std::string label = env.empty() ? "Obstacle" : env;
				if (markers.contains(oid))
					markers.set_label(oid, label);
				else
				{
					Point<int16_t> pos = Stage::get().get_player().get_position();
					markers.spawn(oid, FieldMarker::Kind::DOOR,
						pos + Point<int16_t>(static_cast<int16_t>(i * 8), -40),
						label, 0, 8000);
				}
				obstacle_oids().insert(oid);
			}
			else
			{
				markers.remove(oid);
				obstacle_oids().erase(oid);
			}
		}
		while (recv.available())
			recv.skip_byte();
	}

	void FieldObstacleAllResetHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "FieldObstacleAllReset n=" << obstacle_oids().size());
		for (int32_t oid : obstacle_oids())
			Stage::get().get_field_markers().remove(oid);
		obstacle_oids().clear();
	}

	void BlockedMapHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		while (recv.available())
			recv.skip_byte();
		static const char* msgs[] = {
			"Cannot enter map",
			"Portal closed",
			"Level too low",
			"Level too high",
			"Party only",
			"Cash Shop unavailable",
			"Party members only"
		};
		const char* msg = (type >= 0 && type < 7) ? msgs[type] : "Map blocked";
		LOG(LOG_DEBUG, "BlockedMap type=" << (int)type);
		// note
		if (type == 5)
		{
			if (auto cs = UI::get().get_element<UICashShop>())
			{
				if (cs->is_active())
				{
					cs->refresh_cs_meta();
					return;
				}
			}
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E00000 ^ static_cast<int32_t>(type);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, msg);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), msg, 0, 3000);
	}

	void BlockedServerHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		static const char* msgs[] = {
			"Server blocked",
			"Cannot change channel",
			"Cash Shop unavailable",
			"Trade Shop unavailable",
			"Trade Shop full",
			"Trade Shop level too low"
		};
		const char* msg = (type >= 0 && type <= 5) ? msgs[type] : "Server blocked";
		LOG(LOG_DEBUG, "BlockedServer type=" << (int)type);
		while (recv.available())
			recv.skip_byte();
		// note
		if (type == 2)
		{
			if (auto cs = UI::get().get_element<UICashShop>())
			{
				if (cs->is_active())
				{
					cs->refresh_cs_meta();
					return;
				}
			}
		}
		// note
		if (type == 1)
		{
			if (auto ch = UI::get().get_element<UIChannel>())
			{
				if (ch->is_active())
					return;
			}
		}
		// note
		if (type >= 3 && type <= 5)
		{
			if (auto mts = UI::get().get_element<UIMTS>())
			{
				if (mts->is_active())
				{
					mts->set_status(msg);
					return;
				}
			}
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E10000 ^ static_cast<int32_t>(type);
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, msg);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), msg, 0, 3000);
	}

	void ForcedMapEquipHandler::handle(InPacket& recv) const
	{
		int8_t team = recv.available() ? recv.read_byte() : -1;
		LOG(LOG_DEBUG, "ForcedMapEquip team=" << (int)team);
		while (recv.available())
			recv.skip_byte();
		if (team == 0 || team == 1)
		{
			if (auto ui = ensure_party_race())
				ui->set_score(team == 0 ? 1 : 0, team == 1 ? 1 : 0);
		}
		// note
	}

	void FamilyNotifyLoginHandler::handle(InPacket& recv) const
	{
		bool online = recv.available() ? recv.read_bool() : false;
		std::string name = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FamilyNotify " << name << (online ? " online" : " offline"));
		while (recv.available())
			recv.skip_byte();
		std::string tip = name + (online ? " logged in" : " logged out");
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges({ tip });
			return;
		}
		if (auto tree = UI::get().get_element<UIFamilyTree>())
		{
			if (tree->is_active())
				return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00003;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
	}

	void FamilySummonRequestHandler::handle(InPacket& recv) const
	{
		std::string from = recv.available() ? recv.read_string() : "";
		std::string family = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FamilySummon from=" << from << " family=" << family);
		while (recv.available())
			recv.skip_byte();
		// note
		std::string msg = from.empty()
			? "A family member is summoning you. Accept?"
			: (from + " is summoning you to their map. Accept?");
		UI::get().emplace<UIYesNo>(msg, [from, family](bool yes) {
			FamilySummonResponsePacket(family, yes).dispatch();
			(void)from;
		});
	}

	void FamilyJoinAcceptedHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		if (recv.available() >= 4)
			recv.read_int();
		LOG(LOG_DEBUG, "FamilyJoinAccepted senior=" << name);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Family senior: " + name;
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges({ tip });
			return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00002;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 3000);
	}

	void QuestClearHandler::handle(InPacket& recv) const
	{
		int16_t qid = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "QuestClear id=" << qid);
		while (recv.available())
			recv.skip_byte();
		if (qid > 0)
			Stage::get().get_player().get_quests().set_completed(qid, 0);
		auto ql = UI::get().get_element<UIQuestLog>();
		if (!ql)
			ql = UI::get().emplace<UIQuestLog>(Stage::get().get_player().get_quests());
		else if (!ql->is_active())
			ql->makeactive();
		if (ql)
		{
			ql->refresh();
			return;
		}
		{
			const int32_t oid = 0x77000009 ^ qid;
			const std::string tip = "Quest clear #" + std::to_string(qid);
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, tip);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -60), tip, 0, 3000);
			}
		}
	}

	void BridleMobCatchFailHandler::handle(InPacket& recv) const
	{
		int8_t message = recv.available() ? recv.read_byte() : 0;
		if (recv.available() >= 4)
			recv.read_int();
		if (recv.available() >= 4)
			recv.read_int();
		LOG(LOG_DEBUG, "BridleCatchFail msg=" << (int)message);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		std::string tip = message == 1 ? "Monster too strong" : "Catch failed";
		const int32_t oid = 0x53000001 ^ message;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, tip);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(0, -40), tip, 0, 2000);
		}
	}

	void CashPetFoodResultHandler::handle(InPacket& recv) const
	{
		bool fail = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "CashPetFood fail=" << fail);
		while (recv.available())
			recv.skip_byte();
		if (fail)
		{
			// note
			if (auto inv = UI::get().get_element<UIItemInventory>())
			{
				if (inv->is_active())
					return;
			}
			const int32_t oid = 0x77000004;
			const std::string label = "Pet food fail";
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(0, -35), label, 0, 2000);
			}
		}
	}

	void SpawnPortalHandler::handle(InPacket& recv) const
	{
		int32_t town = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t target = recv.available() >= 4 ? recv.read_int() : 0;
		Point<int16_t> pos = recv.available() >= 4 ? recv.read_point() : Point<int16_t>(0, 0);
		LOG(LOG_DEBUG, "SpawnPortal town=" << town << " target=" << target
			<< " (" << pos.x() << "," << pos.y() << ")");
		while (recv.available())
			recv.skip_byte();
		// s9 语义：townId/targetId = MapId.NONE(999999999) ⇒ 清除（removeDoor town
		// 分支，PC:1148-1152）；否则这是 Mystic Door 镇侧门框，每扇门一条，
		// 同屏可多扇。修前用常量 oid 0x50000001 承载 ⇒ 第二扇门的 spawn 直接
		// 覆盖第一扇（G7，组队多门互踩）。按 pos 派生实例键：同一位置重复
		// 下发=刷新（s9 对同门只发一次 spawn，位置键足够去重），NONE=全清。
		auto& markers = Stage::get().get_field_markers();
		if (town == 999999999 || target == 999999999)
			markers.remove_by_kind(FieldMarker::Kind::PORTAL);
		else
		{
			// pos 两个 short 拼 int32 键（门框坐标 ±32767 内单射）。
			// 段基址 0x55000000：全仓已用异或段（0x30..0x34/0x40/0x50..0x54/0x60/0x61）
			// 均避开；0x50000000 是 ShowItemEffect 的 cid^段，撞段会互删。
			const int32_t portal_oid = 0x55000000
				| (static_cast<uint32_t>(static_cast<uint16_t>(pos.x())) << 16)
				| static_cast<uint32_t>(static_cast<uint16_t>(pos.y()));
			const std::string label = "Portal->" + std::to_string(target);
			if (markers.contains(portal_oid))
				markers.set_label(portal_oid, label);
			else
				markers.spawn(portal_oid, FieldMarker::Kind::PORTAL, pos, label);
		}
		// note
	}

	void MapTransferResultHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		int8_t vip = recv.available() ? recv.read_byte() : 0;
		int count = vip ? 10 : 5;
		auto& rock = Stage::get().get_player().get_teleportrock();
		if (vip)
			rock.clearviplocations();
		else
			rock.clearlocations();
		for (int i = 0; i < count && recv.available() >= 4; ++i)
		{
			int32_t mapid = recv.read_int();
			if (vip)
				rock.addviplocation(mapid);
			else
				rock.addlocation(mapid);
		}
		LOG(LOG_DEBUG, "MapTransferResult mode=" << (int)mode << " vip=" << (int)vip);
		while (recv.available())
			recv.skip_byte();
		// note
		(void)mode;
	}

	void SetGenderHandler::handle(InPacket& recv) const
	{
		int8_t gender = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SetGender=" << (int)gender);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto ci = UI::get().get_element<UICharInfo>())
		{
			if (ci->is_active())
				CharInfoRequestPacket(Stage::get().get_player().get_oid()).dispatch();
		}
		(void)gender;
	}

	void TradeMoneyLimitHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "TradeMoneyLimit");
		const char* tip = "Daily meso trade limit (Lv15)";
		auto trade = UI::get().get_element<UITrade>();
		if (!trade) trade = UI::get().emplace<UITrade>();
		else if (!trade->is_active()) trade->makeactive();
		if (trade)
		{
			trade->add_chat(tip);
			return;
		}
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E20001;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), tip, 0, 3000);
	}

	void EntrustedShopCheckHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "EntrustedShopCheck mode=0x" << std::hex << (int)(uint8_t)mode << std::dec
			<< " rem=" << recv.available());
		if (mode == 0x07 || mode == 0x09)
		{
			while (recv.available())
				recv.skip_byte();
			auto box = UI::get().get_element<UIStoreBank>();
			if (!box)
				box = UI::get().emplace<UIStoreBank>();
			else if (!box->is_active())
				box->makeactive();
			if (box && mode == 0x09)
				box->show_notice("Retrieve first message");
			// note
		}
		else if (mode == 0x10)
		{
			while (recv.available())
				recv.skip_byte();
// note
			if (auto box = UI::get().get_element<UIStoreBank>())
			{
				if (box->is_active())
				{
					box->show_notice("Store channel change?");
					return;
				}
			}
			UI::get().emplace<UIYesNo>("Store channel change?", [](bool) {});
		}
		else
		{
			while (recv.available())
				recv.skip_byte();
			std::string tip = "Store check 0x"
				+ std::to_string(static_cast<int>(static_cast<uint8_t>(mode)));
			LOG(LOG_DEBUG, "EntrustedShopCheck unhandled " << tip);
			auto box = UI::get().get_element<UIStoreBank>();
			if (!box) box = UI::get().emplace<UIStoreBank>();
			else if (!box->is_active()) box->makeactive();
			if (box)
				box->show_notice(tip);
		}
	}

	void ShopLinkResultHandler::handle(InPacket& recv) const
	{
		int8_t msg = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "ShopLinkResult msg=" << (int)msg);
		while (recv.available())
			recv.skip_byte();
		static const char* texts[] = {
			"Success", "Room closed", "Room full", "Busy", "You are dead",
			"", "", "Cannot trade now"
		};
		const char* t = (msg >= 0 && msg <= 7) ? texts[msg] : nullptr;
		if (!t || !*t)
		{
			if (msg == 17) t = "May not enter store";
			else if (msg == 18) t = "Store under maintenance";
			else if (msg == 23) t = "Free Market only";
			else t = "Shop link denied";
		}
		if (msg != 0)
		{
			auto box = UI::get().get_element<UIStoreBank>();
			if (!box) box = UI::get().emplace<UIStoreBank>();
			else if (!box->is_active()) box->makeactive();
			if (box)
			{
				box->show_notice(t);
				return;
			}
			if (auto merch = UI::get().get_element<UIMerchant>())
			{
				if (merch->is_active())
				{
					if (auto chatbar = UI::get().get_element<UIChatBar>())
						chatbar->show_message(t, UIChatBar::MessageType::RED);
					return;
				}
			}
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = 0x33E30000 ^ static_cast<int32_t>(msg);
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (markers.contains(mid))
				markers.set_label(mid, t);
			else
				markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), t, 0, 3000);
		}
	}

	void SetTamingMobInfoHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t level = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t exp = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t tired = recv.available() >= 4 ? recv.read_int() : 0;
		bool levelup = recv.available() ? recv.read_bool() : false;
		LOG(LOG_DEBUG, "TamingMob cid=" << cid << " lv=" << level
			<< " exp=" << exp << " tired=" << tired << " up=" << levelup);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto ci = UI::get().get_element<UICharInfo>())
		{
			if (ci->is_active())
			{
				CharInfoRequestPacket(Stage::get().get_player().get_oid()).dispatch();
				return;
			}
		}
		if (levelup)
		{
			if (auto character = Stage::get().get_character(cid))
				character->show_iron_body();
			else
				Stage::get().get_player().show_iron_body();
			// note
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = 0x33E40000 ^ cid;
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (auto character = Stage::get().get_character(cid))
				pos = character->get_position();
			if (markers.contains(mid))
				markers.set_label(mid, "Mount level up!");
			else
				markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -70), "Mount level up!", 0, 2500);
		}
		(void)exp;
		(void)tired;
		(void)level;
	}

	void SetExtraPendantSlotHandler::handle(InPacket& recv) const
	{
		bool enable = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "SetExtraPendantSlot=" << enable);
		while (recv.available())
			recv.skip_byte();
		const char* tip = enable ? "Extra pendant on" : "Extra pendant off";
		auto ui = UI::get().get_element<UIEquipInventory>();
		if (!ui)
			ui = UI::get().emplace<UIEquipInventory>(Stage::get().get_player().get_inventory());
		else if (!ui->is_active())
			ui->makeactive();
		if (ui)
			return;
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E50001;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), tip, 0, 2500);
	}

	void FakeGmNoticeHandler::handle(InPacket& recv) const
	{
		std::string msg = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FakeGmNotice len=" << msg.size());
		while (recv.available())
			recv.skip_byte();
		std::string tip = msg.empty() ? "GM notice" : msg;
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_message(tip);
		// note
	}

	void CancelTransferWorldResultHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "CancelTransferWorld rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto cs = UI::get().get_element<UICashShop>())
		{
			if (cs->is_active())
			{
				cs->refresh_cs_meta();
				return;
			}
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E50002;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, "Transfer world cancelled");
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55),
				"Transfer world cancelled", 0, 2500);
	}

	void SetBackEffectHandler::handle(InPacket& recv) const
	{
		std::string path = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "SetBackEffect path=" << path);
		if (!path.empty())
			Stage::get().add_effect(path);
		while (recv.available())
			recv.skip_byte();
		// note
	}

	void CancelNameChangeResultHandler::handle(InPacket& recv) const
	{
		bool ok = recv.available() ? recv.read_bool() : false;
		if (!ok && recv.available())
			recv.read_byte();
		LOG(LOG_DEBUG, "CancelNameChange ok=" << ok);
		while (recv.available())
			recv.skip_byte();
		if (auto cs = UI::get().get_element<UICashShop>())
		{
			if (cs->is_active())
			{
				cs->refresh_cs_meta();
				return;
			}
		}
		std::string tip = ok ? "Name change cancelled" : "Cancel name change failed";
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E50003;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), tip, 0, 2500);
	}

	void ClaimStatusChangedHandler::handle(InPacket& recv) const
	{
		int8_t enabled = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "ClaimStatus enabled=" << (int)enabled);
		while (recv.available())
			recv.skip_byte();
		// note
	}

	void SueCharacterResultHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SueCharacter mode=" << (int)mode);
		while (recv.available())
			recv.skip_byte();
		// note
		if (mode != 0)
		{
			if (auto ui = UI::get().get_element<UIAdmin>())
			{
				if (ui->is_active())
					ui->set_message("Report failed");
			}
			// note
		}
	}

	void SessionValueHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		std::string val = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "SessionValue " << key << "=" << val);
		while (recv.available())
			recv.skip_byte();
// note
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
			{
				ui->set_message(key.empty() ? val : (key + "=" + val));
				return;
			}
		}
		bool energy_key = false;
		const size_t klen = key.size();
		for (size_t i = 0; i + 6 <= klen; ++i)
		{
			char c0 = static_cast<char>(key[i] | 0x20);
			char c1 = static_cast<char>(key[i + 1] | 0x20);
			char c2 = static_cast<char>(key[i + 2] | 0x20);
			char c3 = static_cast<char>(key[i + 3] | 0x20);
			char c4 = static_cast<char>(key[i + 4] | 0x20);
			char c5 = static_cast<char>(key[i + 5] | 0x20);
			if (c0 == 'e' && c1 == 'n' && c2 == 'e' && c3 == 'r' && c4 == 'g' && c5 == 'y')
			{
				energy_key = true;
				break;
			}
		}
		if (energy_key)
		{
			auto eb = UI::get().get_element<UIEnergyBar>();
			if (!eb)
				eb = UI::get().emplace<UIEnergyBar>();
			else if (!eb->is_active())
				eb->makeactive();
		}
	}

	void ImitatedNpcDataHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		int32_t id = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ImitatedNpc mode=" << (int)mode << " id=" << id
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (id > 0)
		{
			const int32_t oid = 0x75000000 ^ id;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "ImNPC");
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR,
					pos + Point<int16_t>(50, 0), "ImNPC", 0, 4000);
			}
		}
		(void)mode;
	}

	void ShopScannerResultHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "ShopScanner mode=" << (int)mode << " rem=" << recv.available());
		std::string tip;
		std::vector<std::pair<int32_t, int32_t>> owl_items;
		if (mode == 7)
		{
			int8_t n = recv.available() ? recv.read_byte() : 0;
			tip = "Owl top (" + std::to_string(n) + "):";
			for (int8_t i = 0; i < n && recv.available() >= 4; ++i)
			{
				int32_t item = recv.read_int();
				tip += " #" + std::to_string(item);
				owl_items.emplace_back(item, 0);
			}
		}
		else if (mode == 6)
		{
			if (recv.available() >= 4)
				recv.read_int();
			int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
			int32_t count = recv.available() >= 4 ? recv.read_int() : 0;
			tip = "Owl item=" + std::to_string(itemid) + " hits=" + std::to_string(count);
			for (int32_t i = 0; i < count && i < 12 && recv.available() >= 8; ++i)
			{
				int32_t price = recv.read_int();
				int32_t qty = recv.available() >= 4 ? recv.read_int() : 0;
				std::string owner = recv.available() ? recv.read_string() : "";
				int32_t map = recv.available() >= 4 ? recv.read_int() : 0;
				int8_t ch = recv.available() ? recv.read_byte() : 0;
				tip += " | " + owner + " @" + std::to_string(price) + "x" + std::to_string(qty);
				if (map)
					tip += " m" + std::to_string(map);
				if (ch)
					tip += " ch" + std::to_string(ch);
				owl_items.emplace_back(itemid, price);
			}
		}
		else
		{
			tip = "Owl mode=" + std::to_string(mode);
		}
		while (recv.available())
			recv.skip_byte();
		auto mts = UI::get().get_element<UIMTS>();
		if (!mts) mts = UI::get().emplace<UIMTS>();
		else if (!mts->is_active()) mts->makeactive();
		if (mts)
		{
			mts->set_status(tip);
			if (!owl_items.empty())
			{
				mts->set_list(2, 0, 0, static_cast<int32_t>(owl_items.size()), 0);
				mts->set_items(owl_items);
			}
		}
		// note
	}

	void RelogResponseHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "RelogResponse type=" << (int)type);
		while (recv.available())
			recv.skip_byte();
		// note
		if (type != 0)
		{
			std::string tip = "Relog type=" + std::to_string(type);
			auto ui = UI::get().get_element<UIAdmin>();
			if (!ui) ui = UI::get().emplace<UIAdmin>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_message(tip);
		}
	}

	void LastConnectedWorldHandler::handle(InPacket& recv) const
	{
		int32_t world = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "LastConnectedWorld=" << world);
		while (recv.available())
			recv.skip_byte();
		// note
		(void)world;
	}

	void RecommendedWorldMessageHandler::handle(InPacket& recv) const
	{
		int8_t n = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "RecommendedWorld count=" << (int)n);
		std::string first;
		for (int8_t i = 0; i < n && recv.available(); ++i)
		{
			int32_t wid = recv.available() >= 4 ? recv.read_int() : 0;
			std::string msg = recv.available() ? recv.read_string() : "";
			LOG(LOG_DEBUG, "  world=" << wid << " msg=" << msg);
			if (first.empty() && !msg.empty())
				first = msg;
		}
		while (recv.available())
			recv.skip_byte();
		// note
		if (!first.empty())
		{
			if (auto ws = UI::get().get_element<UIWorldSelect>())
			{
				if (ws->is_active())
					return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(first.c_str(), UIChatBar::MessageType::YELLOW);
		}
		(void)n;
	}

	void ForcedStatSetHandler::handle(InPacket& recv) const
	{
		// note
		uint32_t mask = recv.available() >= 4 ? static_cast<uint32_t>(recv.read_int()) : 0;
		CharStats& stats = Stage::get().get_player().get_stats();
		if (!g_forced_active)
		{
			for (int i = 0; i < 12; ++i)
				g_forced_eq_backup[i] = stats.get_total(kForcedEq[i]);
			for (int i = 0; i < 4; ++i)
				g_forced_ms_backup[i] = stats.get_stat(kForcedMs[i]);
			g_forced_active = true;
		}
		stats.init_totalstats();
		int bits = 0;
		int16_t first = 0;
		for (int i = 0; i < 12; ++i)
		{
			if ((mask & (1u << i)) == 0)
				continue;
			int16_t v = recv.available() >= 2 ? recv.read_short() : 0;
			if (bits == 0)
				first = v;
			++bits;
			stats.set_total(kForcedEq[i], v);
			if (i < 4)
				stats.set_stat(kForcedMs[i], static_cast<uint16_t>(v));
		}
		stats.close_totalstats();
		LOG(LOG_DEBUG, "ForcedStatSet mask=0x" << std::hex << mask << std::dec
			<< " bits=" << bits << " first=" << first << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto si = UI::get().get_element<UIStatsInfo>())
		{
			if (si->is_active())
				si->update_all_stats();
		}
		(void)bits; (void)first;
	}

	void ForcedStatResetHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "ForcedStatReset");
		CharStats& stats = Stage::get().get_player().get_stats();
		if (g_forced_active)
		{
			stats.init_totalstats();
			for (int i = 0; i < 12; ++i)
				stats.set_total(kForcedEq[i], g_forced_eq_backup[i]);
			for (int i = 0; i < 4; ++i)
				stats.set_stat(kForcedMs[i], g_forced_ms_backup[i]);
			stats.close_totalstats();
			g_forced_active = false;
		}
		Stage::get().get_player().recalc_stats(true);
		// note
		if (auto st = UI::get().get_element<UIStatsInfo>())
		{
			if (st->is_active())
				st->update_all_stats();
		}
	}

	void MakerResultHandler::handle(InPacket& recv) const
	{
		int32_t result = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t mode = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MakerResult result=" << result << " mode=" << mode
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		std::string tip;
		if (mode == 0)
			tip = "Maker ready";
		else if (mode == 3)
			tip = "Maker crystal";
		else if (mode == 4)
			tip = "Maker desynth";
		else if (result == 0)
			tip = "Maker success";
		else
			tip = "Maker failed";
		auto ui = UI::get().get_element<UIMaker>();
		if (!ui) ui = UI::get().emplace<UIMaker>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_status(tip);
		// note
	}

	void FamilyPrivilegeListHandler::handle(InPacket& recv) const
	{
		int32_t n = recv.available() >= 4 ? recv.read_int() : 0;
		if (n < 0) n = 0;
		if (n > 64) n = 64;
		LOG(LOG_DEBUG, "FamilyPrivilegeList count=" << n);
		std::vector<std::string> names;
		names.reserve(static_cast<size_t>(n));
		for (int32_t i = 0; i < n && recv.available(); ++i)
		{
			if (recv.available())
				recv.read_byte();
			if (recv.available() >= 4)
				recv.read_int();
			if (recv.available() >= 4)
				recv.read_int();
			std::string a = recv.available() ? recv.read_string() : "";
			std::string b = recv.available() ? recv.read_string() : "";
			if (!a.empty())
				names.push_back(a);
			else if (!b.empty())
				names.push_back(b);
		}
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
			ui->set_privileges(names);
	}

	void FamilyRepGainHandler::handle(InPacket& recv) const
	{
		int32_t gain = recv.available() >= 4 ? recv.read_int() : 0;
		std::string from = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FamilyRep +" << gain << " from=" << from);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Family REP +" + std::to_string(gain)
			+ (from.empty() ? "" : (" (" + from + ")"));
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges({ tip });
			return;
		}
		if (auto tree = UI::get().get_element<UIFamilyTree>())
		{
			if (tree->is_active())
				return;
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00004;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
	}

	void FamilySetPrivilegeHandler::handle(InPacket& recv) const
	{
		int32_t priv = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "FamilySetPrivilege=" << priv);
		while (recv.available())
			recv.skip_byte();
		static const char* bit_names[] = {
			"Teleport", "Summon", "DropShare", "EXP",
			"PartyTP", "PartySummon", "PartyDrop", "PartyEXP",
			"BossTP", "Meso", "Item", "Pet",
			"Skill", "Shop", "Wedding", "Event"
		};
		std::vector<std::string> names;
		for (int i = 0; i < 16; ++i)
		{
			if (priv & (1 << i))
				names.emplace_back(bit_names[i]);
		}
		if (names.empty())
			names.emplace_back(priv ? ("priv#" + std::to_string(priv)) : "none");
		auto ui = UI::get().get_element<UIFamily>();
		if (ui && ui->is_active())
		{
			ui->set_privileges(names);
			return;
		}
		if (auto tree = UI::get().get_element<UIFamilyTree>())
		{
			if (tree->is_active())
				return;
		}
		std::string tip = "Family priv";
		for (size_t i = 0; i < names.size() && i < 4; ++i)
			tip += (i == 0 ? ": " : ",") + names[i];
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33F00005;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
	}


	void GuildBossHealerMoveHandler::handle(InPacket& recv) const
	{
		// 原生 CField_GuildBoss::OnPacket(287) → sub_558C0C：short 绝对 Y，
		// RelMove(delta=newY-oldY) 后覆写 field 当前 Y。本端驱动地图物件 + HUD。
		const int16_t ny = recv.available() >= 2 ? recv.read_short() : 0;
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_guild_boss().set_healer_y(ny);
		auto ui = UI::get().get_element<UIGuildBoss>();
		if (!ui)
			ui = UI::get().emplace<UIGuildBoss>();
		if (ui)
			ui->set_healer_y(ny);
	}

	void GuildBossPulleyStateHandler::handle(InPacket& recv) const
	{
		// 原生 OnPacket(288) → sub_558C3B：byte 写入 field+507（交互门控）。
		const int8_t st = recv.available() ? recv.read_byte() : 0;
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_guild_boss().set_pulley_state(
			static_cast<uint8_t>(st));
		auto ui = UI::get().get_element<UIGuildBoss>();
		if (!ui)
			ui = UI::get().emplace<UIGuildBoss>();
		if (ui)
			ui->set_pulley_state(st);
	}

	void MonsterCarnivalSummonHandler::handle(InPacket& recv) const
	{
		int8_t tab = recv.available() ? recv.read_byte() : 0;
		int8_t number = recv.available() ? recv.read_byte() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "CarnivalSummon tab=" << (int)tab << " n=" << (int)number << " " << name);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Summon " + name + " t" + std::to_string(tab)
			+ "#" + std::to_string(number);
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
		{
			ui->set_message(tip);
			return; // Carnival 已显，不叠 field marker
		}
		{
			const int32_t oid = 0x76000000 ^ (tab * 32 + number);
			const std::string label = name.empty() ? ("CPQ#" + std::to_string(number)) : name;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON,
					pos + Point<int16_t>(tab * 10, -50), label, 0, 2500);
			}
		}
	}

	void MonsterCarnivalDiedHandler::handle(InPacket& recv) const
	{
		int8_t team = recv.available() ? recv.read_byte() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		int8_t lost = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "CarnivalDied team=" << (int)team << " " << name << " -" << (int)lost);
		while (recv.available())
			recv.skip_byte();
		std::string tip = name + " died (-" + std::to_string(lost) + " CP)";
		auto ui = UI::get().get_element<UIMonsterCarnival>();
		if (!ui) ui = UI::get().emplace<UIMonsterCarnival>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
		{
			ui->set_message(tip);
			return; // Carnival 已显，不叠 field marker
		}
		{
			const int32_t oid = 0x76100000 ^ (team * 1000 + lost);
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, tip);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON,
					pos + Point<int16_t>(team * 20, -55), tip, 0, 2500);
			}
		}
	}

	void WitchTowerScoreHandler::handle(InPacket& recv) const
	{
		int8_t score = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "WitchTower score=" << (int)score);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto ui = ensure_party_race())
		{
			ui->set_stage(score);
			ui->show_result(score <= 3);
		}
	}

	void SpawnKiteHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t item = recv.available() >= 4 ? recv.read_int() : 0;
		std::string msg = recv.available() ? recv.read_string() : "";
		std::string name = recv.available() ? recv.read_string() : "";
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (recv.available() >= 4)
			pos = recv.read_point();
		LOG(LOG_DEBUG, "SpawnKite oid=" << oid << " item=" << item
			<< " by=" << name << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		std::string label = name.empty() ? ("Kite " + std::to_string(item)) : name;
		if (!msg.empty())
			label = name + ": " + msg;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, label);
		else
			markers.spawn(oid, FieldMarker::Kind::KITE, pos, label);
		// note
	}

	void RemoveKiteHandler::handle(InPacket& recv) const
	{
		int8_t anim = recv.available() ? recv.read_byte() : 0;
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RemoveKite oid=" << oid << " anim=" << (int)anim);
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_field_markers().remove(oid);
		(void)anim;
	}

	void CannotSpawnKiteHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "CannotSpawnKite");
		// note
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		{
			const int32_t oid = 0x77000002;
			const std::string label = "No kite";
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::KITE, pos + Point<int16_t>(0, -40), label, 0, 2000);
			}
		}
	}

	void QuickslotInitHandler::handle(InPacket& recv) const
	{
		// v83 语义（s9 QuickslotBinding.encode 坐实）：bool + 8×int = 8 个
		// **触发键**（非槽位绑定，槽内技能/道具走 KEYMAP(135) 既有通道）。
		// 旧实现把触发键 add_keymapping(key & 0xFF, 0, i) 写进键位表槽号——
		// type=0(NONE) 时 assign 直接吞掉是良性 no-op，但键码 >0x100 时
		// (key & 0xFF) 会命中真实键位表项，污染用户常规绑定。Wave 1-1.2
		// 改为存进 Keyboard 的专用 quickslot 触发键表。
		//
		// 等于默认时服务端只发 bool=false（不下发键值）——此时**必须清表回
		// 默认**：上一角色若设了自定义触发键，本角色走默认分支时不清就串号
		// （sendMacros 注释里同款"switching characters"问题的 quickslot 版）。
		bool custom = recv.available() ? recv.read_bool() : false;

		Keyboard& keyboard = UI::get().get_keyboard();

		if (custom)
		{
			for (int i = 0; i < 8 && recv.available() >= 4; ++i)
			{
				int32_t key = recv.read_int();
				keyboard.assign_quickslot(static_cast<uint8_t>(i), static_cast<uint32_t>(key));
			}
		}
		else
		{
			keyboard.clear_quickslots();
		}

		LOG(LOG_INFO, "QuickslotInit custom=" << custom << " rem=" << recv.available());
		// 真机验收判据（Release LOG 是 no-op，走 perf 通道）：开 debug.perf_log
		// 进图应见 [PERF] QSI custom=0|1；custom=1 时附 8 个触发键值。
		{
			std::string perf_msg = std::string("custom=") + (custom ? "1" : "0");
			if (custom)
			{
				const auto& qs = UI::get().get_keyboard().quickslots();
				for (uint32_t k : qs) perf_msg += " " + std::to_string(k);
			}
			perf::mark("QSI", perf_msg);
		}
		while (recv.available())
			recv.skip_byte();

		if (auto kc = UI::get().get_element<UIKeyConfig>())
		{
			if (kc->is_active())
				kc->makeactive();
		}
	}
	void DataCrcCheckFailedHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "DataCrcCheckFailed rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
				ui->set_message("Data CRC check failed");
		}
	}

	void NotifyHpDecByFieldHandler::handle(InPacket& recv) const
	{
		int32_t dmg = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "NotifyHpDecByField dmg=" << dmg);
		while (recv.available())
			recv.skip_byte();
		// note
		if (dmg > 0)
		{
			Player& player = Stage::get().get_player();
			player.show_damage(dmg);
			CharStats& st = player.get_stats();
			uint16_t hp = st.get_stat(MapleStat::Id::HP);
			uint16_t nhp = hp > static_cast<uint16_t>(dmg)
				? static_cast<uint16_t>(hp - static_cast<uint16_t>(dmg))
				: 0;
			st.set_stat(MapleStat::Id::HP, nhp);
		}
	}

	void SetNpcScriptableHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		std::string script = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "SetNpcScriptable oid=" << oid << " script=" << script);
		while (recv.available())
			recv.skip_byte();
		if (MapObjects* npcs = Stage::get().get_npcs().get_npcs())
		{
			if (Optional<MapObject> obj = npcs->get(oid))
			{
				if (auto* npc = dynamic_cast<Npc*>(obj.get()))
					npc->set_stance("alert");
			}
		}
		// note
		(void)script;
	}

	void MoveMonsterResponseHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t moveid = recv.available() >= 2 ? recv.read_short() : 0;
		bool useSkills = recv.available() ? recv.read_bool() : false;
		int16_t mp = recv.available() >= 2 ? recv.read_short() : 0;
		int8_t skill = recv.available() ? recv.read_byte() : 0;
		int8_t level = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "MoveMonsterResponse oid=" << oid << " move=" << moveid
			<< " skills=" << useSkills << " mp=" << mp
			<< " skill=" << (int)skill << " lv=" << (int)level);
		while (recv.available())
			recv.skip_byte();
		// 把服务端授权过的下一技能交给控制器怪本体：下一次 update_movement
		// 以 SKILL 段（42..59）上报该 skillId/level，怪物技能由服务端结算
		// （s9 MoveLifeHandler.java:150 只在 canUseSkill 通过后携带）。
		// 修前这里只是弹一个文字场标，怪本身永远不出招。
		Stage::get().get_mobs().set_next_skill(oid, useSkills && skill != 0, skill, level);

		(void)moveid;
		(void)mp;
		(void)level;
	}

	void DptTrackerHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		std::string tip;
		if (mode == 0)
		{
			LOG(LOG_DEBUG, "DPT reset");
			tip = "DPT reset";
		}
		else if (mode == 1)
		{
			int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
			int32_t job = recv.available() >= 4 ? recv.read_int() : 0;
			std::string name = recv.available() ? recv.read_string() : "";
			int64_t dmg = 0;
			if (recv.available() >= 8)
				dmg = recv.read_long();
			LOG(LOG_DEBUG, "DPT player " << name << " cid=" << cid << " job=" << job << " dmg=" << dmg);
			tip = "DPT " + name + " " + std::to_string(dmg);
			(void)cid;
			(void)job;
		}
		else if (mode == 2)
		{
			int32_t skill = recv.available() >= 4 ? recv.read_int() : 0;
			std::string sname = recv.available() ? recv.read_string() : "";
			LOG(LOG_DEBUG, "DPT skill " << sname << " id=" << skill << " rem=" << recv.available());
			tip = "DPT skill " + (sname.empty() ? std::to_string(skill) : sname);
		}
		else
		{
			LOG(LOG_DEBUG, "DPT mode=" << (int)mode);
			tip = "DPT";
		}
		while (recv.available())
			recv.skip_byte();
		if (!tip.empty())
		{
			auto ui = UI::get().get_element<UIAdmin>();
			if (!ui) ui = UI::get().emplace<UIAdmin>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_message(tip);
		}
	}

	void UpdateHpMpAlertHandler::handle(InPacket& recv) const
	{
		int8_t hp = recv.available() ? recv.read_byte() : 0;
		int8_t mp = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "HpMpAlert hp=" << (int)hp << " mp=" << (int)mp);
		while (recv.available())
			recv.skip_byte();

		// 真机验收判据（perf 通道，Release 可见）：登录链路应出
		// [PERF] HPA hp=X mp=Y（服务端 step 0..19）。
		perf::mark("HPA", "hp=" + std::to_string((int)hp)
			+ " mp=" + std::to_string((int)mp));

		// Wave 1-1.1 回灌：s9 登录时 updateClientSettings(hp_step, mp_step) 下发
		// 角色存库挡位（HpMpAlertService：step/20=百分比，每挡 5%）。换算回
		// 0..100 存进本地设置，UIStatusBar 的闪铃消费端（B4-5）直接吃这两个
		// Setting —— 阈值从此跟随角色而非本机 Settings 文件。
		// 0 挡=不报警，与本地语义一致；钳 0..19 防异常值。
		auto step_to_pct = [](int8_t step) -> uint8_t
		{
			int s = step < 0 ? 0 : (step > 19 ? 19 : step);
			return static_cast<uint8_t>(s * 5);
		};

		Setting<SysOptHpWarn>::get().save(step_to_pct(hp));
		Setting<SysOptMpWarn>::get().save(step_to_pct(mp));
	}

	void SetItcHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "SetITC rem=" << recv.available());
		auto mts = UI::get().get_element<UIMTS>();
		if (!mts)
			mts = UI::get().emplace<UIMTS>();
		else if (!mts->is_active())
			mts->makeactive();
		if (mts)
			mts->set_status("Enter MTS/ITC");
		while (recv.available())
			recv.skip_byte();
	}

	void TournamentHandler::handle(InPacket& recv) const
	{
		int8_t state = recv.available() ? recv.read_byte() : 0;
		int8_t sub = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "Tournament state=" << (int)state << " sub=" << (int)sub);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Tournament " + std::to_string(state) + "/" + std::to_string(sub);
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev) ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active()) ev->makeactive();
		(void)tip;
		// note
	}

	void TournamentMatchTableHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "TournamentMatchTable rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		// note
	}

	void TournamentSetPrizeHandler::handle(InPacket& recv) const
	{
		int8_t ok = recv.available() ? recv.read_byte() : 0;
		int8_t has = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "TournamentSetPrize ok=" << (int)ok << " has=" << (int)has);
		while (recv.available())
			recv.skip_byte();
		std::string tip = ok ? "Tournament prize set" : "Tournament prize failed";
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev) ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active()) ev->makeactive();
		(void)tip; (void)has;
		// note
	}

	void TournamentUewHandler::handle(InPacket& recv) const
	{
		int8_t state = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "TournamentUEW state=" << (int)state);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Tournament advance " + std::to_string(state);
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev) ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active()) ev->makeactive();
		(void)tip;
		// note
	}

	void MtsOperationHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		std::string tip;
		Optional<UIMTS> mts = UI::get().get_element<UIMTS>();
		switch (mode)
		{
			case 0x15: // note
			{
				if (!mts)
					mts = UI::get().emplace<UIMTS>();
				else if (!mts->is_active())
					mts->makeactive();
				int32_t pageslots = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t count = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t tab = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t type = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t page = recv.available() >= 4 ? recv.read_int() : 0;
				if (recv.available()) recv.read_byte();
				if (recv.available()) recv.read_byte();
				tip = "MTS list tab" + std::to_string(tab) + " type" + std::to_string(type)
					+ " p" + std::to_string(page) + " n=" + std::to_string(count)
					+ " slots=" + std::to_string(pageslots);
				std::vector<std::pair<int32_t, int32_t>> items;
				for (int32_t i = 0; i < count && recv.available() >= 5; ++i)
				{
					int32_t itemid = 0;
					if (!skip_iteminfo_zeropos(recv, itemid))
						break;
					// trailer: id, taxes, price, 0, end, end, seller, seller, 28 zeros
					if (recv.available() < 24)
						break;
					recv.read_int(); // listing id
					recv.read_int(); // taxes
					int32_t price = recv.read_int();
					recv.read_int(); // 0
					recv.read_int(); // end ts
					recv.read_int(); // end ts
					if (recv.available()) recv.skip_string();
					if (recv.available()) recv.skip_string();
					if (recv.available() >= 28)
						recv.skip(28);
					items.emplace_back(itemid, price);
				}
				while (recv.available())
					recv.skip_byte();
				if (mts)
				{
					mts->set_list(tab, type, page, count, pageslots);
					mts->set_items(items);
					mts->set_status(tip);
				}
				if (!items.empty())
					tip += " first=#" + std::to_string(items[0].first)
						+ "@" + std::to_string(items[0].second);
				break;
			}
			case 0x1D: tip = "MTS sell ok"; break;
			case 0x21: // note
			{
				int32_t n = recv.available() >= 4 ? recv.read_int() : 0;
				if (n < 0) n = 0;
				if (n > 100) n = 100;
				std::vector<std::pair<int32_t, int32_t>> items;
				for (int32_t i = 0; i < n && recv.available() >= 5; ++i)
				{
					int32_t itemid = 0;
					if (!skip_iteminfo_zeropos(recv, itemid))
						break;
					items.emplace_back(itemid, 0);
				}
				tip = "MTS transfer inv n=" + std::to_string(static_cast<int>(items.size()));
				if (!mts && !items.empty())
					mts = UI::get().emplace<UIMTS>();
				else if (mts && !mts->is_active() && !items.empty())
					mts->makeactive();
				if (mts && mts->is_active())
				{
					mts->set_list(1, 0, 0, static_cast<int32_t>(items.size()), 0);
					mts->set_items(items);
					mts->set_status(tip);
				}
				break;
			}
			case 0x23: // note
			{
				int32_t n = recv.available() >= 4 ? recv.read_int() : 0;
				if (n < 0) n = 0;
				if (n > 100) n = 100;
				std::vector<std::pair<int32_t, int32_t>> items;
				for (int32_t i = 0; i < n && recv.available() >= 5; ++i)
				{
					int32_t itemid = 0;
					if (!skip_iteminfo_zeropos(recv, itemid))
						break;
					if (recv.available() < 24)
						break;
					recv.read_int();
					recv.read_int();
					int32_t price = recv.read_int();
					recv.read_int();
					recv.read_int();
					recv.read_int();
					if (recv.available()) recv.skip_string();
					if (recv.available()) recv.skip_string();
					if (recv.available() >= 28)
						recv.skip(28);
					items.emplace_back(itemid, price);
				}
				tip = "MTS not-yet-sold n=" + std::to_string(static_cast<int>(items.size()));
				if (!mts)
					mts = UI::get().emplace<UIMTS>();
				else if (!mts->is_active())
					mts->makeactive();
				if (mts)
				{
					mts->set_list(0, 0, 0, static_cast<int32_t>(items.size()), 0);
					mts->set_items(items);
				}
				break;
			}
			case 0x27:
			{
				int32_t qty = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t pos = recv.available() >= 4 ? recv.read_int() : 0;
				tip = "MTS transfer x" + std::to_string(qty) + " slot" + std::to_string(pos);
				break;
			}
			case 0x33: tip = "MTS buy ok"; break; // note
			case 0x34: tip = "MTS buy fail"; break;
			case 0x3D:
			{
				int32_t nx = recv.available() >= 4 ? recv.read_int() : 0;
				int32_t items = recv.available() >= 4 ? recv.read_int() : 0;
				tip = "MTS wanted NX=" + std::to_string(nx) + " items=" + std::to_string(items);
				break;
			}
			default:
				tip = "MTS mode=0x" + std::to_string(static_cast<int>(static_cast<uint8_t>(mode)))
					+ " rem=" + std::to_string(recv.available());
				break;
		}
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "MTS_OPERATION " << tip);
		if (!mts)
			mts = UI::get().emplace<UIMTS>();
		else if (!mts->is_active())
			mts->makeactive();
		if (mts)
			mts->set_status(tip);
		// note
	}

	void MtsOperation2Handler::handle(InPacket& recv) const
	{
		int32_t nx = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t mp = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MTS_OPERATION2 nx=" << nx << " maple=" << mp);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "MTS cash NX=" + std::to_string(nx) + " MP=" + std::to_string(mp);
		auto mts = UI::get().get_element<UIMTS>();
		if (!mts) mts = UI::get().emplace<UIMTS>();
		else if (!mts->is_active()) mts->makeactive();
		if (mts)
		{
			mts->set_cash(nx, mp);
			mts->set_status(tip);
		}
		// note
	}

	void NpcActionHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		size_t rem = recv.available();
		LOG(LOG_DEBUG, "NpcAction oid=" << oid << " rem=" << rem);
		if (MapObjects* npcs = Stage::get().get_npcs().get_npcs())
		{
			if (Optional<MapObject> obj = npcs->get(oid))
			{
				if (auto* npc = dynamic_cast<Npc*>(obj.get()))
					npc->set_stance(rem > 0 ? "move" : "stand");
			}
		}
		while (recv.available())
			recv.skip_byte();
		// note
		(void)rem;
	}

	void SummonItemUnavailableHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "SummonItemUnavailable");
		// note
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		{
			const int32_t oid = 0x77000003;
			const std::string label = "No summon";
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(0, -40), label, 0, 2000);
			}
		}
	}

	void GuestIdLoginHandler::handle(InPacket& recv) const
	{
		int16_t flag = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "GuestIdLogin flag=0x" << std::hex << flag << std::dec
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		// note
		UI::get().emplace<UITermsOfService>([] {});
		(void)flag;
	}

	void HourChangedHandler::handle(InPacket& recv) const
	{
		int16_t oldh = recv.available() >= 2 ? recv.read_short() : 0;
		int16_t newh = recv.available() >= 2 ? recv.read_short() : 0;
		LOG(LOG_DEBUG, "HourChanged " << oldh << " -> " << newh);
		while (recv.available())
			recv.skip_byte();
		// note
		(void)oldh; (void)newh;
	}

	void MinimapOnOffHandler::handle(InPacket& recv) const
	{
		bool on = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "MinimapOnOff=" << on);
		while (recv.available())
			recv.skip_byte();
		auto mm = UI::get().get_element<UIMiniMap>();
		if (on)
		{
			if (!mm)
				mm = UI::get().emplace<UIMiniMap>(Stage::get().get_player().get_stats());
			else if (!mm->is_active())
				mm->makeactive();
		}
		else if (mm && mm->is_active())
		{
			mm->deactivate();
		}
		// note
	}

	void BonusExpChangedHandler::handle(InPacket& recv) const
	{
		int32_t exp = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "BonusExpChanged exp=" << exp);
		while (recv.available())
			recv.skip_byte();
// note
		std::string tip = exp ? ("Bonus EXP " + std::to_string(exp)) : "Bonus EXP changed";
		if (auto st = UI::get().get_element<UIStatsInfo>())
		{
			if (st->is_active())
			{
				st->update_all_stats();
				return;
			}
		}
		if (auto bar = UI::get().get_element<UIStatusBar>())
		{
			if (bar->is_active())
			{
				if (auto chatbar = UI::get().get_element<UIChatBar>())
					chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
				return;
			}
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E70001;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
	}

	void PlayJukeboxHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		int32_t map = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "PlayJukebox by=" << name << " map=" << map);
		while (recv.available())
			recv.skip_byte();
		std::string tip = name.empty() ? "Jukebox" : ("Jukebox: " + name);
		auto tv = UI::get().get_element<UIMapleTV>();
		if (!tv) tv = UI::get().emplace<UIMapleTV>();
		else if (!tv->is_active()) tv->makeactive();
		if (tv) tv->set_tip(tip);
		(void)map;
		// note
	}

	void MarriageRequestHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		if (mode == 0)
		{
			std::string name = recv.available() ? recv.read_string() : "";
			int32_t pid = recv.available() >= 4 ? recv.read_int() : 0;
			LOG(LOG_DEBUG, "MarriageRequest engage from=" << name << " id=" << pid);
			while (recv.available())
				recv.skip_byte();
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui)
				ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active())
				ui->makeactive();
			if (ui)
				ui->set_invitation(false, "Engage: " + name);
			(void)pid;
		}
		else if (mode == 9)
		{
			int n = 0;
			while (recv.available() >= 4)
			{
				recv.read_int();
				++n;
			}
			while (recv.available())
				recv.skip_byte();
			std::string tip = "Wishlist n=" + std::to_string(n);
			LOG(LOG_DEBUG, "MarriageRequest " << tip);
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui) ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_invitation(false, tip);
		}
		else
		{
			LOG(LOG_DEBUG, "MarriageRequest mode=" << (int)mode << " rem=" << recv.available());
			while (recv.available())
				recv.skip_byte();
			std::string tip = "Marriage request " + std::to_string(mode);
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui) ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active()) ui->makeactive();
			if (ui) ui->set_invitation(true, tip);
		}
	}

	void MarriageResultHandler::handle(InPacket& recv) const
	{
		int8_t msg = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "MarriageResult msg=" << (int)msg << " rem=" << recv.available());
		auto push_wedding = [&](const std::string& tip, bool invite) {
			auto ui = UI::get().get_element<UIWedding>();
			if (!ui)
				ui = UI::get().emplace<UIWedding>();
			else if (!ui->is_active())
				ui->makeactive();
			if (ui)
				ui->set_invitation(invite, tip);
		};
		if (msg == 11)
		{
			push_wedding("Marriage / engagement updated", false);
		}
		else if (msg == 15)
		{
			std::string groom = recv.available() ? recv.read_string() : "";
			std::string bride = recv.available() ? recv.read_string() : "";
			push_wedding(groom + " & " + bride, true);
		}
		else if (msg == 36)
		{
			if (recv.available())
				recv.read_byte();
			std::string text = recv.available() ? recv.read_string() : "You are now engaged.";
			push_wedding(text, false);
		}
		else
		{
			push_wedding("Marriage result " + std::to_string(msg), false);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void NotifyMarriedPartnerMapTransferHandler::handle(InPacket& recv) const
	{
		int32_t mapid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t partner = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "PartnerMapTransfer map=" << mapid << " partner=" << partner);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Partner -> map " + std::to_string(mapid);
		if (auto ci = UI::get().get_element<UICharInfo>())
		{
			if (ci->is_active())
				return;
		}
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui) ui->set_invitation(false, tip);
		(void)partner;
	}

	void WeddingPhotoHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "WeddingPhoto rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		auto ui = UI::get().get_element<UIWedding>();
		if (!ui) ui = UI::get().emplace<UIWedding>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_invitation(false, "Wedding photo");
	}

	void DestroyShopResultHandler::handle(InPacket& recv) const
	{
		int8_t result = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "DestroyShopResult=" << (int)result);
		while (recv.available())
			recv.skip_byte();
		if (result == 0)
		{
			int32_t cid = Stage::get().get_player().get_oid();
			Stage::get().get_field_markers().remove(cid ^ 0x30000000);
			bool closed_ui = false;
			if (auto ui = UI::get().get_element<UIMerchant>())
			{
				ui->deactivate();
				closed_ui = true;
			}
			if (auto ps = UI::get().get_element<UIPersonalShop>())
			{
				ps->deactivate();
				closed_ui = true;
			}
			if (!closed_ui)
			{
				const int32_t oid = 0x77000007;
				const std::string tip = "Shop closed";
				auto& markers = Stage::get().get_field_markers();
				if (markers.contains(oid))
					markers.set_label(oid, tip);
				else
				{
					Point<int16_t> pos = Stage::get().get_player().get_position();
					markers.spawn(oid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
				}
			}
		}
		else
		{
			if (auto merch = UI::get().get_element<UIMerchant>())
			{
				if (merch->is_active())
					return;
			}
			if (auto ps = UI::get().get_element<UIPersonalShop>())
			{
				if (ps->is_active())
					return;
			}
			const int32_t oid = 0x77000006;
			const std::string tip = "Shop destroy failed";
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, tip);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 2500);
			}
		}
	}

	void RandomMorphResHandler::handle(InPacket& recv) const
	{
		int32_t morph = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "RandomMorphRes morph=" << morph);
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().show_iron_body();
		// note
		(void)morph;
	}

	void SetPotionDiscountRateHandler::handle(InPacket& recv) const
	{
		int8_t rate = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "PotionDiscount rate=" << (int)rate);
		while (recv.available())
			recv.skip_byte();
		if (rate <= 0)
			return;
		// note
		if (auto shop = UI::get().get_element<UIShop>())
		{
			if (shop->is_active())
				return;
		}
		std::string tip = "Potion discount " + std::to_string(rate) + "%";
		const int32_t oid = 0x77000008;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, tip);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(30, -40), tip, 0, 3000);
		}
	}

	void ImitatedNpcResultHandler::handle(InPacket& recv) const
	{
		int32_t npcid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ImitatedNpcResult npc=" << npcid);
		while (recv.available())
			recv.skip_byte();
		if (npcid > 0)
		{
			const int32_t oid = 0x75000000 ^ npcid;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "ImNPC");
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR,
					pos + Point<int16_t>(50, 0), "ImNPC", 0, 4000);
			}
		}
		// note
	}

	void LimitedNpcDisableInfoHandler::handle(InPacket& recv) const
	{
		int32_t n = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "LimitedNpcDisable count=" << n);
		for (int32_t i = 0; i < n && recv.available() >= 4; ++i)
		{
			int32_t oid = recv.read_int();
			Stage::get().get_npcs().remove(oid);
		}
		while (recv.available())
			recv.skip_byte();
		// note
		(void)n;
	}

	void PartyValueHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		std::string val = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "PartyValue " << key << "=" << val);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Party " + key + "=" + val;
		auto party = UI::get().get_element<UIParty>();
		if (!party) party = UI::get().emplace<UIParty>();
		else if (!party->is_active()) party->makeactive();
		if (auto ps = UI::get().get_element<UIPartySearch>())
		{
			if (!ps->is_active())
				ps->makeactive();
		}
		// Party 无 notice API：ChatBar 显 key=val
		if (auto chatbar = UI::get().get_element<UIChatBar>())
		{
			chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
			return;
		}
		{
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = 0x33E60001;
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (markers.contains(mid))
				markers.set_label(mid, tip);
			else
				markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -50), tip, 0, 3000);
		}
	}

	void FieldSetVariableHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		std::string val = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FieldSetVariable " << key << "=" << val);
		while (recv.available())
			recv.skip_byte();
		// note
		int32_t oid = 0x79000000;
		for (char c : key)
			oid = oid * 33 + static_cast<int32_t>(static_cast<unsigned char>(c));
		const std::string label = key.empty() ? "FieldVar" : (key + "=" + val);
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, label);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::DOOR,
				pos + Point<int16_t>(0, -60), label, 0, 5000);
		}
	}

	void SetMapObjectVisibleHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		bool visible = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "SetMapObjectVisible " << name << " vis=" << visible);
		while (recv.available())
			recv.skip_byte();
		int32_t oid = 0x7A000000;
		for (char c : name)
			oid = oid * 33 + static_cast<int32_t>(static_cast<unsigned char>(c));
		auto& markers = Stage::get().get_field_markers();
		if (visible)
		{
			const std::string label = name.empty() ? "MapObj" : name;
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR,
					pos + Point<int16_t>(20, -40), label, 0, 6000);
			}
		}
		else
			markers.remove(oid);
	}

	void ClearBackEffectHandler::handle(InPacket& recv) const
	{
		while (recv.available())
			recv.skip_byte();
		LOG(LOG_DEBUG, "ClearBackEffect");
		Stage::get().clear_effect();
	}

	void GuildBbsHandler::handle(InPacket& recv) const
	{
		// s9 GuildPackets：0x06=BBSThreadList，0x07=showThread（字段序与 Odin 系一致）
		const int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "[GuildBBS] s2c mode=0x" << std::hex << (int)(uint8_t)mode
			<< std::dec << " rem=" << recv.available());

		std::vector<std::string> summary_lines;
		auto ensure_bbs = []() -> Optional<UIGuildBBS>
		{
			auto bbs = UI::get().get_element<UIGuildBBS>();
			if (!bbs)
				bbs = UI::get().emplace<UIGuildBBS>();
			else if (!bbs->is_active())
				bbs->makeactive();
			return bbs;
		};

		auto read_summary = [&](bool is_notice) -> BbsThreadSummary
		{
			BbsThreadSummary row;
			row.is_notice = is_notice;
			row.local_id = recv.available() >= 4 ? recv.read_int() : 0;
			row.poster_cid = recv.available() >= 4 ? recv.read_int() : 0;
			row.subject = recv.available() ? recv.read_string() : "";
			row.timestamp = recv.available() >= 8 ? recv.read_long() : 0;
			row.icon = recv.available() >= 4 ? recv.read_int() : 0;
			row.reply_count = recv.available() >= 4 ? recv.read_int() : 0;
			return row;
		};

		if (mode == 0x06)
		{
			// hasNotice(byte) [+ notice thread] + total(int) + pageCount(int) + threads
			const bool has_notice = recv.available() ? (recv.read_byte() != 0) : false;
			BbsThreadSummary notice;
			if (has_notice)
				notice = read_summary(true);
			const int32_t total = recv.available() >= 4 ? recv.read_int() : 0;
			const int32_t page_n = recv.available() >= 4 ? recv.read_int() : 0;
			std::vector<BbsThreadSummary> page;
			page.reserve(static_cast<size_t>(std::max(0, page_n)));
			for (int32_t i = 0; i < page_n && recv.available() >= 4; ++i)
			{
				BbsThreadSummary row = read_summary(false);
				if (!row.subject.empty())
					summary_lines.push_back(row.subject);
				page.push_back(std::move(row));
			}
			if (auto bbs = ensure_bbs())
				bbs->apply_list(has_notice, notice, total, std::move(page));
			if (has_notice && !notice.subject.empty())
				summary_lines.insert(summary_lines.begin(), std::string("[N] ") + notice.subject);
		}
		else if (mode == 0x07)
		{
			BbsThreadDetail detail;
			detail.local_id = recv.available() >= 4 ? recv.read_int() : 0;
			detail.poster_cid = recv.available() >= 4 ? recv.read_int() : 0;
			detail.timestamp = recv.available() >= 8 ? recv.read_long() : 0;
			detail.subject = recv.available() ? recv.read_string() : "";
			detail.body = recv.available() ? recv.read_string() : "";
			detail.icon = recv.available() >= 4 ? recv.read_int() : 0;
			const int32_t reply_n = recv.available() >= 4 ? recv.read_int() : 0;
			for (int32_t i = 0; i < reply_n && recv.available() >= 4; ++i)
			{
				BbsReply r;
				r.reply_id = recv.read_int();
				r.poster_cid = recv.available() >= 4 ? recv.read_int() : 0;
				r.timestamp = recv.available() >= 8 ? recv.read_long() : 0;
				r.content = recv.available() ? recv.read_string() : "";
				detail.replies.push_back(std::move(r));
			}
			if (!detail.subject.empty())
				summary_lines.push_back(detail.subject);
			if (!detail.body.empty())
			{
				std::string short_body = detail.body.size() > 72
					? detail.body.substr(0, 72) + "..." : detail.body;
				summary_lines.push_back(short_body);
			}
			if (auto bbs = ensure_bbs())
				bbs->apply_thread(std::move(detail));
		}
		else
		{
			LOG(LOG_WARN, "[GuildBBS] unhandled s2c mode=" << (int)(uint8_t)mode);
		}

		while (recv.available())
			recv.skip_byte();

		// SoftKey 时代摘要栏仍同步到公会窗，便于未开 BBS 窗时一眼看到
		auto guild = UI::get().get_element<UIGuild>();
		if (guild && !summary_lines.empty())
			guild->set_bbs_summary(summary_lines);
	}

	void SetQuestClearHandler::handle(InPacket& recv) const
	{
		int32_t qid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "SetQuestClear qid=" << qid);
		while (recv.available())
			recv.skip_byte();
		if (qid > 0 && qid < 65536)
			Stage::get().get_player().get_quests().set_completed(static_cast<int16_t>(qid), 0);
		auto ql = UI::get().get_element<UIQuestLog>();
		if (!ql)
			ql = UI::get().emplace<UIQuestLog>(Stage::get().get_player().get_quests());
		else if (!ql->is_active())
			ql->makeactive();
		if (ql)
		{
			ql->refresh();
			return;
		}
		{
			const int32_t oid = 0x7700000A ^ qid;
			const std::string tip = qid ? ("Quest clear #" + std::to_string(qid)) : std::string("Quest map clear");
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, tip);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -60), tip, 0, 3000);
			}
		}
	}

	void SetQuestTimeHandler::handle(InPacket& recv) const
	{
		int32_t start = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t end = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t span = end - start;
		LOG(LOG_DEBUG, "SetQuestTime " << start << " -> " << end);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Quest timer " + std::to_string(span) + "s";
		auto clock = UI::get().get_element<UIClock>();
		if (!clock)
			clock = UI::get().emplace<UIClock>();
		else if (!clock->is_active())
			clock->makeactive();
		if (clock)
			clock->set_clock(2, tip, span > 0 ? span : 0);
	}

	void WarnMessageHandler::handle(InPacket& recv) const
	{
		std::string msg = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "WarnMessage len=" << msg.size());
		while (recv.available())
			recv.skip_byte();
		if (msg.empty())
			msg = "Warning";
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_message(msg);
		// note
	}

	void SetObjectStateHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		int32_t state = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "SetObjectState " << name << "=" << state);
		while (recv.available())
			recv.skip_byte();
		int32_t oid = 0x7B000000;
		for (char c : name)
			oid = oid * 33 + static_cast<int32_t>(static_cast<unsigned char>(c));
		const std::string label = (name.empty() ? "Obj" : name) + "=" + std::to_string(state);
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, label);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::DOOR,
				pos + Point<int16_t>(-20, -40), label, 0, 5000);
		}
	}

	void ShowConsumeEffectHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t itemid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ShowConsumeEffect cid=" << cid << " item=" << itemid
			<< " rem=" << recv.available());
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (auto character = Stage::get().get_character(cid))
		{
			pos = character->get_position();
			character->show_iron_body();
		}
		if (itemid > 0)
		{
			const int32_t oid = cid ^ 0x51000000;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "Use");
			else
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, "Use", itemid, 2000);
		}
		while (recv.available())
			recv.skip_byte();
	}

	void ChatText1Handler::handle(InPacket& recv) const
	{
		// Same family as CHATTEXT / CHAT_RECEIVED
		int32_t charid = recv.available() >= 4 ? recv.read_int() : 0;
		if (recv.available())
			recv.read_bool(); // gm
		std::string message = recv.available() ? recv.read_string() : "";
		int8_t type = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "ChatText1 cid=" << charid << " len=" << message.size());
		while (recv.available())
			recv.skip_byte();
		if (auto character = Stage::get().get_character(charid))
		{
			std::string line = character->get_name() + " : " + message;
			character->speak(line);
			message = line;
		}
		if (!message.empty())
		{
			auto linetype = static_cast<UIChatBar::MessageType>(type);
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(message.c_str(), linetype);
			// note
		}
	}

	void SkillUseResultHandler::handle(InPacket& recv) const
	{
		int8_t result = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "SkillUseResult=" << (int)result);
		while (recv.available())
			recv.skip_byte();
		// note
		if (result != 0)
		{
			if (auto sb = UI::get().get_element<UISkillBook>())
			{
				if (sb->is_active())
					return;
			}
			std::string tip = "Skill use fail " + std::to_string(result);
			auto su = UI::get().get_element<UISkillUp>();
			if (!su) su = UI::get().emplace<UISkillUp>();
			else if (!su->is_active()) su->makeactive();
			if (su)
			{
				su->set_tip(tip);
				return;
			}
			const int32_t oid = 0x77000005 ^ result;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, tip);
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos + Point<int16_t>(0, -45), tip, 0, 2000);
			}
		}
	}

	void CancelNameChangeByOtherHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "CancelNameChangeByOther rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto cs = UI::get().get_element<UICashShop>())
		{
			if (cs->is_active())
				cs->refresh_cs_meta();
		}
		// note
	}

	void ConsultAuthkeyUpdateHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "ConsultAuthkey rem=" << recv.available() << " len=" << key.size());
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
				ui->set_message(key.empty() ? "Consult authkey" : ("Consult: " + key.substr(0, 24)));
		}
		// note
	}

	void ClassCompetitionAuthkeyUpdateHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "ClassCompetitionAuthkey rem=" << recv.available() << " len=" << key.size());
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
				ui->set_message(key.empty() ? "ClassComp authkey" : ("ClassComp: " + key.substr(0, 24)));
		}
	}

	void WebBoardAuthkeyUpdateHandler::handle(InPacket& recv) const
	{
		std::string key = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "WebBoardAuthkey rem=" << recv.available() << " len=" << key.size());
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
				ui->set_message(key.empty() ? "WebBoard authkey" : ("WebBoard: " + key.substr(0, 24)));
		}
	}

	void OpenFullClientDownloadLinkHandler::handle(InPacket& recv) const
	{
		std::string url = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "FullClientDownload url=" << url);
		while (recv.available())
			recv.skip_byte();
		std::string tip = url.empty() ? "Full client download available." : ("Download: " + url);
		// note
		UI::get().emplace<UIYesNo>(tip, [](bool) {});
	}

	void ClaimAvailableTimeHandler::handle(InPacket& recv) const
	{
		int32_t minutes = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ClaimAvailableTime min=" << minutes);
		while (recv.available())
			recv.skip_byte();
		std::string tip = minutes ? ("Report in " + std::to_string(minutes) + "m") : "Report available";
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
				ui->set_message(tip);
		}
		// note
	}

	void SetBuyEquipExtHandler::handle(InPacket& recv) const
	{
		bool enable = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "SetBuyEquipExt=" << enable);
		while (recv.available())
			recv.skip_byte();
		const char* tip = enable ? "Extra equip slot on" : "Extra equip slot off";
		if (auto ui = UI::get().get_element<UIEquipInventory>())
		{
			if (ui->is_active())
			{
				ui->makeactive();
				return;
			}
		}
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E80001;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), tip, 0, 2500);
	}

	void GuildNameChangedHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		std::string name = recv.available() ? recv.read_string() : "";
		LOG(LOG_DEBUG, "GuildNameChanged cid=" << cid << " name=" << name);
		while (recv.available())
			recv.skip_byte();
		std::string tip = name.empty() ? "Guild name cleared" : ("Guild: " + name);
		auto ui = UI::get().get_element<UIGuild>();
		if (ui && ui->is_active())
		{
			ui->set_notice(tip);
			return;
		}
		if (auto character = Stage::get().get_character(cid))
			character->show_iron_body();
		else
		{
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = cid ^ 0x32000000;
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (markers.contains(mid))
				markers.set_label(mid, tip);
			else
				markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -100), tip, 0, 2500);
		}
	}

	void GuildMarkChangedHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t bg = recv.available() >= 2 ? recv.read_short() : 0;
		int8_t bgc = recv.available() ? recv.read_byte() : 0;
		int16_t logo = recv.available() >= 2 ? recv.read_short() : 0;
		int8_t logoc = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "GuildMarkChanged cid=" << cid
			<< " bg=" << bg << "/" << (int)bgc << " logo=" << logo << "/" << (int)logoc);
		while (recv.available())
			recv.skip_byte();
		std::string tip = "Guild mark updated";
		auto ui = UI::get().get_element<UIGuild>();
		if (ui && ui->is_active())
		{
			ui->set_notice(tip);
			return;
		}
		if (auto character = Stage::get().get_character(cid))
			character->show_iron_body();
		else
		{
			auto& markers = Stage::get().get_field_markers();
			const int32_t mid = cid ^ 0x32100000;
			Point<int16_t> pos = Stage::get().get_player().get_position();
			if (markers.contains(mid))
				markers.set_label(mid, tip);
			else
				markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -100), tip, 0, 2500);
		}
		(void)bg; (void)bgc; (void)logo; (void)logoc;
	}

	void LucksackPassHandler::handle(InPacket& recv) const
	{
		int32_t item = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "LucksackPass item=" << item);
		while (recv.available())
			recv.skip_byte();
		if (item > 0)
		{
			const int32_t oid = 0x73000000 ^ item;
			auto& markers = Stage::get().get_field_markers();
			if (markers.contains(oid))
				markers.set_label(oid, "Luck");
			else
			{
				Point<int16_t> pos = Stage::get().get_player().get_position();
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, "Luck", item, 2500);
			}
			// note
			return;
		}
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		const int32_t oid = 0x73000002;
		const std::string tip = "Lucksack success";
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, tip);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, tip, 0, 2000);
		}
	}

	void LucksackFailHandler::handle(InPacket& recv) const
	{
		int8_t reason = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "LucksackFail reason=" << (int)reason);
		while (recv.available())
			recv.skip_byte();
		// note
		if (auto inv = UI::get().get_element<UIItemInventory>())
		{
			if (inv->is_active())
				return;
		}
		std::string tip = "Lucksack fail " + std::to_string(reason);
		const int32_t oid = 0x73000001 ^ reason;
		auto& markers = Stage::get().get_field_markers();
		if (markers.contains(oid))
			markers.set_label(oid, tip);
		else
		{
			Point<int16_t> pos = Stage::get().get_player().get_position();
			markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, tip, 0, 2000);
		}
	}

	void MesoBagMessageHandler::handle(InPacket& recv) const
	{
		int32_t meso = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "MesoBagMessage meso=" << meso);
		while (recv.available())
			recv.skip_byte();
// note
		bool inv_open = false;
		if (auto inv = UI::get().get_element<UIItemInventory>())
			inv_open = inv->is_active();
		bool bar_open = false;
		if (auto bar = UI::get().get_element<UIStatusBar>())
			bar_open = bar->is_active();
		bool stats_open = false;
		if (auto st = UI::get().get_element<UIStatsInfo>())
			stats_open = st->is_active();
		if (inv_open || bar_open || stats_open)
			return;
		std::string tip = meso != 0 ? ("Meso bag +" + std::to_string(meso)) : "Meso bag";
		auto& markers = Stage::get().get_field_markers();
		const int32_t mid = 0x33E80002;
		Point<int16_t> pos = Stage::get().get_player().get_position();
		if (markers.contains(mid))
			markers.set_label(mid, tip);
		else
			markers.spawn(mid, FieldMarker::Kind::DOOR, pos + Point<int16_t>(0, -55), tip, 0, 2500);
	}

	void KoreanEventHandler::handle(InPacket& recv) const
	{
		int8_t mode = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "KoreanEvent mode=" << (int)mode << " rem=" << recv.available());
		std::string tip = "Korean event m" + std::to_string(mode);
		while (recv.available())
			recv.skip_byte();
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev) ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active()) ev->makeactive();
		(void)tip;
		// note
	}

	void ShowMagnetHandler::handle(InPacket& recv) const
	{
		int32_t moboid = recv.available() >= 4 ? recv.read_int() : 0;
		bool success = recv.available() ? recv.read_bool() : false;
		LOG(LOG_DEBUG, "ShowMagnet oid=" << moboid << " ok=" << success);
		while (recv.available())
			recv.skip_byte();
		if (success)
		{
			const int32_t oid = moboid ^ 0x52000000;
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(moboid);
			if (markers.contains(oid))
				markers.set_label(oid, "Magnet");
			else
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, "Magnet", 0, 2000);
		}
		else
		{
			if (auto inv = UI::get().get_element<UIItemInventory>())
			{
				if (inv->is_active())
					return;
			}
			const int32_t oid = moboid ^ 0x52000000;
			const std::string label = "Magnet fail";
			auto& markers = Stage::get().get_field_markers();
			Point<int16_t> pos = Stage::get().get_mobs().get_mob_position(moboid);
			if (markers.contains(oid))
				markers.set_label(oid, label);
			else
				markers.spawn(oid, FieldMarker::Kind::SUMMON, pos, label, 0, 1500);
		}
	}

	void ResetMonsterAnimationHandler::handle(InPacket& recv) const
	{
		int32_t oid = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ResetMonsterAnimation oid=" << oid);
		// Soft reset: clear any magnet marker tied to this oid
		Stage::get().get_field_markers().remove(oid ^ 0x52000000);
		while (recv.available())
			recv.skip_byte();
	}

	void AriantThingHandler::handle(InPacket& recv) const
	{
		int32_t item = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "AriantThing item=" << item << " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		std::string tip = item != 0 ? ("Ariant item #" + std::to_string(item)) : std::string("Ariant");
		// note
		auto ui = UI::get().get_element<UIAriantMatch>();
		if (!ui)
			ui = UI::get().emplace<UIAriantMatch>();
		else if (!ui->is_active())
			ui->makeactive();
		if (ui)
			ui->set_score(tip);
	}

	void AccountInfoHandler::handle(InPacket& recv) const
	{
		std::string name = recv.available() ? recv.read_string() : "";
		int32_t id = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "AccountInfo name=" << name << " id=" << id
			<< " rem=" << recv.available());
		while (recv.available())
			recv.skip_byte();
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
			{
				std::string tip = name.empty()
					? ("Account #" + std::to_string(id))
					: ("Account: " + name);
				ui->set_message(tip);
			}
		}
		// note
	}

	void ChannelSelectedHandler::handle(InPacket& recv) const
	{
		int32_t ch = recv.available() >= 4 ? recv.read_int() : 0;
		LOG(LOG_DEBUG, "ChannelSelected ch=" << ch);
		while (recv.available())
			recv.skip_byte();
		if (auto chui = UI::get().get_element<UIChannel>())
		{
			if (chui->is_active())
				return; // note
		}
		(void)ch;
	}

	void HackshieldRequestHandler::handle(InPacket& recv) const
	{
		LOG(LOG_DEBUG, "HackshieldRequest rem=" << recv.available());
// note
		while (recv.available())
			recv.skip_byte();
	}

	void CheckCrcResultHandler::handle(InPacket& recv) const
	{
		bool ok = recv.available() ? recv.read_bool() : true;
		LOG(LOG_DEBUG, "CheckCrcResult ok=" << ok);
		while (recv.available())
			recv.skip_byte();
		if (!ok)
		{
			if (auto ui = UI::get().get_element<UIAdmin>())
			{
				if (ui->is_active())
					ui->set_message("CRC check failed");
			}
			// note
		}
	}

	void KoreanInternetCafeHandler::handle(InPacket& recv) const
	{
		int8_t type = recv.available() ? recv.read_byte() : 0;
		LOG(LOG_DEBUG, "KoreanInternetCafe type=" << (int)type);
		while (recv.available())
			recv.skip_byte();
		// note
		if (type == 0)
			return;
		std::string tip = "PC Cafe type=" + std::to_string(type);
		auto ev = UI::get().get_element<UIEvent>();
		if (!ev) ev = UI::get().emplace<UIEvent>();
		else if (!ev->is_active()) ev->makeactive();
		if (ev)
			return;
		auto ui = UI::get().get_element<UIAdmin>();
		if (!ui) ui = UI::get().emplace<UIAdmin>();
		else if (!ui->is_active()) ui->makeactive();
		if (ui)
			ui->set_message(tip);
	}
}

