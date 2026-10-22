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
#pragma once

#include "../PacketHandler.h"

#include "../../IO/UITypes/UIParty.h"

namespace ms
{
	// 解析服务端下发的组队包（PARTY_OPERATION=62）。
	// 字段序以 s9 PacketCreator.updateParty/addPartyStatus 为准。
	class PartyHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;

	private:
		void parse_party_status(InPacket& recv, Party& party) const;
	};

	// 队员 HP 更新（UPDATE_PARTYMEMBER_HP=201）。
	class PartyMemberHPHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
