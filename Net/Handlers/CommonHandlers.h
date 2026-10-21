//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	class PingHandler : public PacketHandler
	{
		void handle(InPacket&) const override;
	};

	// 破功值（伤害上限）同步。
	// 对应服务端 SendOpcode.LIMIT_BREAK_SYNC(0x3730)，负载只有一个 int64。
	// 数值只用于窗口标题显示，收到后交给 Window 保存。
	class LimitBreakSyncHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
}