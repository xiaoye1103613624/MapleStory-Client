////////////////////////////////////////////////////////////////////////////////
//	Alliance ALLIANCE_OPERATION handler
//	W1.5：回包写 AllianceModel + 刷新 UIUserList UNION 页（R28-B2 不为回包开窗）。
////////////////////////////////////////////////////////////////////////////////
#include "AllianceHandlers.h"

#include "../../Character/AllianceModel.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIAlliance.h"
#include "../../IO/UITypes/UIAllianceGrade.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UINotice.h"
#include "../../IO/UITypes/UIOptionMenu.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UIUserList.h"
#include "../../Net/Packets/AlliancePackets.h"
#include "../../Util/Misc.h"

namespace ms
{
	namespace
	{
		void refresh_userlist()
		{
			if (auto ui = UI::get().get_element<UIUserList>())
				ui->refresh_from_models();
		}

		// 对齐 s9 GuildPackets.getGuildInfo：解析一个公会块，写入 AllianceModel 成员。
		std::string parse_guild_block(InPacket& recv, AllianceData& out)
		{
			if (!recv.available())
				return {};

			int32_t gid = recv.read_int();
			std::string gname = recv.read_string();
			for (int r = 0; r < 5; r++)
				recv.read_string();

			uint8_t members = recv.read_byte();
			std::vector<int32_t> ids;
			ids.reserve(members);
			for (uint8_t m = 0; m < members && recv.available(); m++)
				ids.push_back(recv.read_int());

			for (uint8_t m = 0; m < members && recv.available(); m++)
			{
				AllianceMember am;
				am.guild_id = gid;
				am.guild_name = gname;
				am.id = (m < ids.size()) ? ids[m] : 0;
				am.name = recv.read_padded_string(13);
				am.job = recv.read_int();
				am.level = recv.read_int();
				am.guild_rank = recv.read_int();
				am.online = (recv.read_int() == 1);
				recv.skip_int(); // signature
				am.alliance_rank = recv.read_int();
				if (am.id > 0)
					out.members.push_back(am);
			}

			recv.skip_int();   // capacity
			recv.skip_short(); // logoBG
			recv.skip_byte();  // logoBGColor
			recv.skip_short(); // logo
			recv.skip_byte();  // logoColor
			recv.skip_string(); // notice
			recv.skip_int();   // GP
			recv.skip_int();   // allianceId
			(void)gid;
			return gname;
		}

		std::string parse_guild_info_name(InPacket& recv)
		{
			AllianceData tmp;
			return parse_guild_block(recv, tmp);
		}
	}

	void AllianceInfoHandler::handle(InPacket& recv) const
	{
		auto ensure_sm = [](Color::Name color, const std::string& tip) {
			auto mt = (color == Color::Name::RED) ? UIChatBar::MessageType::RED
				: (color == Color::Name::WHITE) ? UIChatBar::MessageType::WHITE
				: UIChatBar::MessageType::YELLOW;
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(tip.c_str(), mt);
				return;
			}
		};

		auto ensure_ui = []() -> Optional<UIAlliance> {
			// R28-B2：联盟浮窗只回写「已在场且活动」；UNION 页读 AllianceModel。
			auto ui = UI::get().get_element<UIAlliance>();
			if (ui && ui->is_active())
				return ui;
			return {};
		};

		uint8_t action = recv.read_byte();

		switch (action)
		{
			case 0x0C: // getAllianceInfo
			{
				recv.read_byte(); // flag (1)
				AllianceData a;
				a.id = recv.read_int();
				a.name = recv.read_string();
				for (int i = 0; i < 5; i++)
					a.rank_titles[i] = recv.read_string();
				uint8_t guildCount = recv.read_byte();
				a.capacity = recv.read_int();
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					a.guild_ids.push_back(recv.read_int());
				a.notice = recv.read_string();
				AllianceModel::get().set(a);
				refresh_userlist();
				if (auto ui = ensure_ui())
					ui->set_alliance(a.id, a.name,
						std::vector<std::string>(a.rank_titles, a.rank_titles + 5),
						a.guild_ids, a.notice);
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "AllianceInfo: 0x0C id=" << a.id << " name=" << a.name);
				break;
			}
			case 0x1C: // allianceNotice
			{
				recv.read_int(); // id
				std::string notice = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				AllianceModel::get().set_notice(notice);
				refresh_userlist();
				if (auto ui = ensure_ui())
				{
					ui->set_notice(notice);
					break;
				}
				ensure_sm(Color::Name::YELLOW, notice.empty() ? "Alliance notice" : notice);
				break;
			}
			case 0x02: // sendShowInfo echo
			{
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "Alliance: show info echo (0x02)");
				break;
			}
			case 0x03: // allianceInvite
			{
				int32_t aid = recv.read_int();
				std::string from = recv.available() ? recv.read_string() : "";
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "AllianceInvite aid=" << aid << " from=" << from);
				if (!UIGameOpt::accepts(UIGameOpt::UNION_APPLY))
				{
					LOG(LOG_DEBUG, "AllianceInvite: GameOpt refuse UNION_APPLY from=" << from);
					break;
				}
				std::string msg = from.empty()
					? std::string("Guild alliance invitation.")
					: ("Invitation from " + from + ". Join the alliance?");
				UI::get().emplace<UIYesNo>(msg, [aid](bool yes) {
					if (!yes)
						return;
					AllianceAcceptPacket(aid).dispatch();
					LOG(LOG_DEBUG, "AllianceInvite: accept aid=" << aid);
				});
				break;
			}
			case 0x0D: // getGuildAlliances — 全量公会块（含成员）
			{
				const int32_t count = recv.read_int();
				AllianceData a = AllianceModel::get().data();
				a.members.clear();
				a.guild_names.clear();
				for (int32_t i = 0; i < count && recv.available(); i++)
					a.guild_names.push_back(parse_guild_block(recv, a));
				while (recv.available())
					recv.skip_byte();
				AllianceModel::get().set(a);
				refresh_userlist();
				LOG(LOG_DEBUG, "AllianceInfo: 0x0D guilds=" << a.guild_names.size()
					<< " members=" << a.members.size());
				if (auto ui = UI::get().get_element<UIAlliance>())
				{
					if (ui->is_active())
						ui->set_guild_names(a.guild_names);
				}
				break;
			}
			case 0x1D: // disband / expel
			{
				if (recv.available() >= 4)
					recv.read_int();
				while (recv.available())
					recv.skip_byte();
				AllianceModel::get().clear();
				refresh_userlist();
				LOG(LOG_DEBUG, "Alliance: disband/expel (0x1D)");
				if (auto ui = UI::get().get_element<UIAlliance>())
				{
					if (ui->is_active())
					{
						ui->set_alliance(0, "", std::vector<std::string>{},
							std::vector<int32_t>{}, std::vector<std::string>{}, "");
					}
				}
				break;
			}
			case 0x0E: // member online/offline：aid + gid + cid + bool
			{
				if (recv.available() >= 13)
				{
					recv.read_int(); // aid
					recv.read_int(); // gid
					int32_t cid = recv.read_int();
					bool online = recv.read_bool();
					AllianceModel::get().set_member_online(cid, online);
					refresh_userlist();
				}
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "Alliance: member online/offline (0x0E)");
				break;
			}
			case 0x18: // updateAllianceJobLevel：aid + gid + cid + level + job
			{
				if (recv.available() >= 20)
				{
					recv.read_int(); // aid
					recv.read_int(); // gid
					int32_t cid = recv.read_int();
					int32_t level = recv.read_int();
					int32_t job = recv.read_int();
					AllianceModel::get().set_member_job_level(cid, level, job);
					refresh_userlist();
				}
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "Alliance: member job/level update (0x18)");
				break;
			}
			case 0x1A: // changeAllianceRankTitle
			{
				if (recv.available() >= 4)
					recv.read_int();
				std::vector<std::string> ranks;
				for (int i = 0; i < 5 && recv.available(); i++)
					ranks.push_back(recv.read_string());
				while (recv.available())
					recv.skip_byte();
				AllianceModel::get().set_ranks(ranks);
				refresh_userlist();
				LOG(LOG_DEBUG, "Alliance: rank titles (0x1A) n=" << ranks.size());
				if (auto ui = UI::get().get_element<UIAlliance>())
				{
					if (ui->is_active())
						ui->set_ranks(ranks);
				}
				if (auto grade = UI::get().get_element<UIAllianceGrade>())
				{
					if (grade->is_active())
						grade->reload_from_model();
				}
				break;
			}
			case 0x0F: // updateAllianceInfo
			{
				AllianceData a;
				a.id = recv.read_int();
				a.name = recv.read_string();
				for (int i = 0; i < 5; i++)
					a.rank_titles[i] = recv.read_string();
				uint8_t guildCount = recv.read_byte();
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					a.guild_ids.push_back(recv.read_int());
				a.capacity = recv.read_int();
				recv.skip_short(); // s9 writeShort(0)
				a.notice = AllianceModel::get().data().notice;
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					a.guild_names.push_back(parse_guild_block(recv, a));
				while (recv.available())
					recv.skip_byte();
				AllianceModel::get().set(a);
				refresh_userlist();
				if (auto ui = ensure_ui())
				{
					ui->set_alliance(a.id, a.name,
						std::vector<std::string>(a.rank_titles, a.rank_titles + 5),
						a.guild_ids, a.guild_names, a.notice);
					LOG(LOG_DEBUG, "AllianceInfo: 0x0F guilds=" << a.guild_ids.size()
						<< " members=" << a.members.size());
					break;
				}
				if (!a.name.empty())
					ensure_sm(Color::Name::YELLOW, "Alliance: " + a.name);
				LOG(LOG_DEBUG, "AllianceInfo: 0x0F guilds=" << a.guild_ids.size()
					<< " members=" << a.members.size());
				break;
			}
			default:
			{
				LOG(LOG_DEBUG, "Alliance mode=" << static_cast<int>(action)
					<< " rem=" << recv.available());
				std::string extra;
				if (recv.available() >= 2)
					extra = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				std::string tip = extra.empty()
					? ("Alliance mode " + std::to_string(action))
					: extra;
				if (auto ui = ensure_ui())
				{
					ui->set_notice(tip);
					break;
				}
				ensure_sm(Color::Name::YELLOW, tip);
				break;
			}
		}
	}
}
