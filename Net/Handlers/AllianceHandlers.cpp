////////////////////////////////////////////////////////////////////////////////
//	Alliance ALLIANCE_OPERATION handler
////////////////////////////////////////////////////////////////////////////////
#include "AllianceHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIAlliance.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Util/Misc.h"

namespace ms
{
	namespace
	{
		// 对齐 s9 GuildPackets.getGuildInfo：解析一个公会块并返回名称
		std::string parse_guild_info_name(InPacket& recv)
		{
			if (!recv.available())
				return {};
			int32_t gid = recv.read_int();
			std::string gname = recv.read_string();
			for (int r = 0; r < 5; r++)
				recv.read_string();
			uint8_t members = recv.read_byte();
			for (uint8_t m = 0; m < members && recv.available(); m++)
				recv.read_int();
			for (uint8_t m = 0; m < members && recv.available(); m++)
			{
				recv.skip_padded_string(13);
				recv.skip_int(); // job
				recv.skip_int(); // level
				recv.skip_int(); // guildRank
				recv.skip_int(); // online
				recv.skip_int(); // signature
				recv.skip_int(); // allianceRank
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
			auto ui = UI::get().get_element<UIAlliance>();
			if (!ui)
				ui = UI::get().emplace<UIAlliance>();
			else if (!ui->is_active())
				ui->makeactive();
			return ui;
		};

		uint8_t action = recv.read_byte();

		switch (action)
		{
			case 0x0C: // getAllianceInfo
			{
				recv.read_byte(); // flag (1)
				int32_t id = recv.read_int();
				std::string name = recv.read_string();
				std::vector<std::string> ranks(5);
				for (int i = 0; i < 5; i++)
					ranks[i] = recv.read_string();
				uint8_t guildCount = recv.read_byte();
				int32_t capacity = recv.read_int();
				std::vector<int32_t> guilds;
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					guilds.push_back(recv.read_int());
				std::string notice = recv.read_string();
				(void)capacity;
				if (auto ui = ensure_ui())
					ui->set_alliance(id, name, ranks, guilds, notice);
				while (recv.available())
					recv.skip_byte();
				(void)name;
				break;
			}
			case 0x1C: // allianceNotice
			{
				recv.read_int(); // id
				std::string notice = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				if (auto ui = ensure_ui())
				{
					ui->set_notice(notice);
					break;
				}
				ensure_sm(Color::Name::YELLOW, notice.empty() ? "Alliance notice" : notice);
				break;
			}
			case 0x0F: // updateAllianceInfo
			{
				int32_t id = recv.read_int();
				std::string name = recv.read_string();
				std::vector<std::string> ranks(5);
				for (int i = 0; i < 5; i++)
					ranks[i] = recv.read_string();
				uint8_t guildCount = recv.read_byte();
				std::vector<int32_t> guilds;
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					guilds.push_back(recv.read_int());
				int32_t capacity = recv.read_int();
				recv.skip_short(); // s9 writeShort(0)
				std::vector<std::string> gnames;
				for (uint8_t i = 0; i < guildCount && recv.available(); i++)
					gnames.push_back(parse_guild_info_name(recv));
				while (recv.available())
					recv.skip_byte();
				(void)capacity;
				if (auto ui = ensure_ui())
				{
					std::string notice = ui->get_notice();
					ui->set_alliance(id, name, ranks, guilds, gnames, notice);
					LOG(LOG_DEBUG, "AllianceInfo: 0x0F guilds=" << guilds.size()
						<< " names=" << gnames.size());
					break;
				}
				if (!name.empty())
					ensure_sm(Color::Name::YELLOW, "Alliance: " + name);
				LOG(LOG_DEBUG, "AllianceInfo: 0x0F guilds=" << guilds.size()
					<< " names=" << gnames.size());
				(void)name;
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
