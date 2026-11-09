////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#include "GuildHandlers.h"

#include "../../Character/GuildModel.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIGuild.h"
#include "../../IO/UITypes/UIStatusMessenger.h"

#include "../../IO/UITypes/UINotice.h"
#include "../../Net/Packets/GuildPackets.h"
#include "../../Util/Misc.h"

namespace ms
{
	void GuildHandler::handle(InPacket& recv) const
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

		uint8_t mode = recv.read_byte();

		// R28-B2 收口：家族回包只写 GuildModel，绝不为数据开 UIGuild 浮窗。
		// 浮窗（状态栏钮）与共用窗家族页（G 键）都在 draw 时读模型，自取数据。
		auto guild_ui_active = [&]() {
			auto ui = UI::get().get_element<UIGuild>();
			return ui && ui->is_active();
		};

		switch (mode)
		{
			case 0x1A: // getGuildInfo — 全量下发 → 模型
			{
				uint8_t inGuild = recv.read_byte();
				if (inGuild == 0)
				{
					GuildModel::get().clear();
					while (recv.available())
						recv.skip_byte();
					break;
				}
				GuildData g;
				g.id = recv.read_int();
				g.name = recv.read_string();
				for (int i = 0; i < 5; i++)
					g.rankTitles[i] = recv.read_string();

				int16_t count = recv.read_byte();
				std::vector<int32_t> ids(count);
				for (int16_t i = 0; i < count; i++)
					ids[i] = recv.read_int();
				for (int16_t i = 0; i < count; i++)
				{
					GuildMember m;
					m.id = ids[i];
					m.name = recv.read_padded_string(13);
					m.job = recv.read_int();
					m.level = recv.read_int();
					m.rank = recv.read_int();
					m.online = (recv.read_int() == 1);
					recv.read_int(); // signature
					m.allianceRank = recv.read_int();
					g.members.push_back(m);
				}
				g.capacity = recv.read_int();
				recv.read_short(); // logoBG
				recv.read_byte();  // logoBGColor
				recv.read_short(); // logo
				recv.read_byte();  // logoColor
				g.notice = recv.read_string();
				g.gp = recv.read_int();
				g.allianceId = recv.read_int();
				GuildModel::get().set(g);
				while (recv.available())
					recv.skip_byte();
				LOG(LOG_DEBUG, "GuildHandler: model set name=" << g.name
					<< " members=" << g.members.size());
				break;
			}
			case 0x05: // guildInvite
			{
				int32_t gid = recv.read_int();
				std::string gname = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				std::string msg = "Join guild [" + gname + "]?";
				// YesNo 已显，不重复 SM
				UI::get().emplace<UIYesNo>(msg, [gid, gname](bool yes)
				{
					if (yes)
					{
						int32_t self = Stage::get().get_player().get_oid();
						GuildJoinPacket(gid, self).dispatch();
						LOG(LOG_DEBUG, "GuildHandler: accept guild=" << gname << " gid=" << gid);
					}
					else
					{
						DenyGuildRequestPacket(gname).dispatch();
						LOG(LOG_DEBUG, "GuildHandler: deny guild=" << gname);
					}
				});
				break;
			}
			case 0x27: // newGuildMember
			{
				recv.read_int(); // guildId
				GuildMember m;
				m.id = recv.read_int();
				m.name = recv.read_padded_string(13);
				m.job = recv.read_int();
				m.level = recv.read_int();
				m.rank = recv.read_int();
				m.online = (recv.read_int() == 1);
				recv.read_int(); // signature
				m.allianceRank = recv.read_int();
				while (recv.available())
					recv.skip_byte();
				GuildModel::get().add_member(m);
				if (!guild_ui_active() && !m.name.empty())
					ensure_sm(Color::Name::WHITE, m.name + " joined guild");
				break;
			}
			case 0x2c: // member left
			case 0x2f: // member expelled
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				std::string name = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				GuildModel::get().remove_member(id);
				if (!name.empty())
					ensure_sm(Color::Name::WHITE,
						name + (mode == 0x2f ? " expelled" : " left guild"));
				break;
			}
			case 0x40: // changeRank
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				int32_t rank = recv.read_byte();
				while (recv.available())
					recv.skip_byte();
				GuildModel::get().set_member_rank(id, rank);
				if (!guild_ui_active())
					ensure_sm(Color::Name::WHITE,
						"Guild rank #" + std::to_string(id) + " -> " + std::to_string(rank));
				break;
			}
			case 0x44: // guildNotice
			{
				recv.read_int(); // guildId
				std::string notice = recv.read_string();
				while (recv.available())
					recv.skip_byte();
				GuildModel::get().mutable_data().notice = notice;
				if (!guild_ui_active())
					ensure_sm(Color::Name::YELLOW, notice.empty() ? "Guild notice" : notice);
				break;
			}
			case 0x3d: // guildMemberOnline
			{
				recv.read_int(); // guildId
				int32_t id = recv.read_int();
				bool online = recv.read_bool();
				while (recv.available())
					recv.skip_byte();
				GuildModel::get().set_member_online(id, online);
				// 上线状态过密不 tip
				(void)id; (void)online;
				break;
			}
			default: // genericGuildMessage 等：仅有意义码走聊天栏（不再往窗里塞 tip）
			{
				std::string tip;
				switch (mode)
				{
					case 0x02: tip = "Guild created"; break;
					case 0x03: tip = "Guild disbanded"; break;
					case 0x05: tip = "Guild invite sent"; break;
					case 0x06: tip = "Guild invite declined"; break;
					case 0x16: tip = "Name already used"; break;
					case 0x17: tip = "Already in a guild"; break;
					case 0x18: tip = "Cannot find player"; break;
					case 0x19: tip = "Guild full"; break;
					case 0x1A: tip = "Not guild master"; break;
					case 0x1B: tip = "Cannot kick"; break;
					case 0x2A: tip = "Not in a guild"; break;
					case 0x2B: tip = "Guild rank updated"; break;
					case 0x2C: tip = "Guild capacity expanded"; break;
					default: tip = "Guild mode " + std::to_string(mode); break;
				}
				while (recv.available())
					recv.skip_byte();
				// 0x03 解散：清模型（原生此时窗随 serverNotice 关闭）。
				if (mode == 0x03)
					GuildModel::get().clear();
				if (mode != 0)
					ensure_sm(Color::Name::YELLOW, tip);
				else
					LOG(LOG_DEBUG, "Guild mode=0 rem drained");
				break;
			}
		}
	}
}
