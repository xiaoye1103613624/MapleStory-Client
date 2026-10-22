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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#include "AllianceHandlers.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIAlliance.h"

namespace ms
{
	void AllianceInfoHandler::handle(InPacket& recv) const
	{
		// 仅在联盟窗口打开时处理
		auto ui = UI::get().get_element<UIAlliance>();
		if (!ui)
			return;

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
				ui->set_alliance(id, name, ranks, guilds, notice);
				break;
			}
			case 0x1C: // allianceNotice
			{
				recv.read_int(); // id
				std::string notice = recv.read_string();
				ui->set_notice(notice);
				break;
			}
			case 0x0F: // updateAllianceInfo（含 guildInfo 块，整体跳过）
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
				while (recv.available())
					recv.skip_byte(); // 跳过 guildInfo 块
				(void)id; (void)ranks; (void)capacity;
				ui->set_alliance(id, name, ranks, guilds, ui->get_notice());
				break;
			}
			default:
				while (recv.available())
					recv.skip_byte();
				break;
		}
	}
}
