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

#include "../../InPacket.h"
#include "../../Login.h"

namespace ms
{
	namespace LoginParser
	{
		Account parse_account(InPacket& recv);
		World parse_world(InPacket& recv);
		CharEntry parse_charentry(InPacket& recv);
		StatsEntry parse_stats(InPacket& recv);
		LookEntry parse_look(InPacket& recv);
		void parse_login(InPacket& recv);

		// 换频道重连后要补的入场券。
		//
		// 正常登录：SERVER_IP 回包 = [skip][IP][port][charId]，由 ServerIPHandler
		// 重连后立刻发 PLAYER_LOGIN(charId)，频道服才下发 SET_FIELD。
		// 换频道：CHANGE_CHANNEL 回包（PacketCreator.getChannelChange）只有
		// [byte 1][IP][port]，**没有 charId**，而且它的 handler 不做任何后续 ——
		// 于是客户端连上新频道服以后一句话不说，服务端等 PLAYER_LOGIN，
		// 双方干瞪眼：人还站在旧图，重开频道窗当前格仍亮原来那条。
		//
		// 所以把登录期拿到的 charId 存下来，换频道时挂一次「待重入」标记，
		// 由 parse_login 在重连成功后补发 PLAYER_LOGIN。
		void note_channel_entry_cid(int32_t cid);
		void expect_channel_reentry();

		// 正常登录（SERVER_IP）自己会发 PLAYER_LOGIN，进这条路径前必须先把
		// 标记清掉：上一次换频道若没走到回包（服务端拒绝、掉线），标记会残留，
		// 而那时存的 charId 属于上一个角色，抢发一次就是把角色换错人。
		void clear_channel_reentry();
	}
}