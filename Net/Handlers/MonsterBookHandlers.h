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
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	// 怪物图鉴封面更新（服务端 MONSTER_BOOK_SET_COVER=0x54）
	class MonsterBookCoverHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// 怪物图鉴新增卡片（服务端 MONSTER_BOOK_SET_CARD=0x53）
	class MonsterBookCardHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
