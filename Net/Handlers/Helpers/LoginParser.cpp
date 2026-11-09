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
#include "LoginParser.h"

#include "../../Packets/LoginPackets.h"
#include "../../Session.h"

namespace ms
{
	namespace
	{
		// 见 LoginParser.h 的说明：登录期 SERVER_IP 给的 charId，换频道重连后复用。
		int32_t channel_entry_cid = 0;
		bool channel_reentry_pending = false;
	}

	Account LoginParser::parse_account(InPacket& recv)
	{
		Account account;

		recv.skip_short();

		account.accid = recv.read_int();
		account.female = recv.read_byte();
		account.admin = recv.read_bool();

		recv.skip_byte(); // Admin
		recv.skip_byte(); // Country Code

		account.name = recv.read_string();

		recv.skip_byte();

		account.muted = recv.read_bool();

		recv.skip_long(); // muted until
		recv.skip_long(); // creation date

		recv.skip_int(); // Remove "Select the world you want to play in"

		account.pin = recv.read_bool(); // 0 - Enabled, 1 - Disabled
		account.pic = recv.read_byte(); // 0 - Register, 1 - Ask, 2 - Disabled

		return account;
	}

	World LoginParser::parse_world(InPacket& recv)
	{
		int8_t wid = recv.read_byte();

		if (wid == -1)
			return { {}, {}, {}, 0, 0, wid };

		std::string name = recv.read_string();
		uint8_t flag = recv.read_byte();
		std::string event_message = recv.read_string();

		recv.skip(5);

		std::vector<int32_t> channelload;
		uint8_t channelload_size = recv.read_byte();

		for (uint8_t i = 0; i < channelload_size; ++i)
		{
			recv.skip_string(); // channel name

			channelload.push_back(recv.read_int());

			recv.skip_byte(); // world id
			recv.skip_byte(); // channel id
			recv.skip_bool(); // adult channel
		}

		recv.skip_short();

		return { name, event_message, channelload, channelload_size, flag, wid };
	}

	CharEntry LoginParser::parse_charentry(InPacket& recv)
	{
		int32_t cid = recv.read_int();
		StatsEntry stats = parse_stats(recv);
		LookEntry look = parse_look(recv);

		// v83 CharEntry：look 后先 viewAll 标志字节，再 ranking 开关。
		recv.read_bool(); // viewAll / rankinfo 前置

		stats.has_rank = recv.read_bool();

		if (stats.has_rank)
		{
			int32_t currank = recv.read_int();
			int32_t rankmv = recv.read_int();
			int32_t curjobrank = recv.read_int();
			int32_t jobrankmv = recv.read_int();
			int8_t rankmc = (rankmv > 0) ? '+' : (rankmv < 0) ? '-' : '=';
			int8_t jobrankmc = (jobrankmv > 0) ? '+' : (jobrankmv < 0) ? '-' : '=';

			stats.rank = std::make_pair(currank, rankmc);
			stats.jobrank = std::make_pair(curjobrank, jobrankmc);
		}

		return { stats, look, cid };
	}

	StatsEntry LoginParser::parse_stats(InPacket& recv)
	{
		// TODO: This is similar to CashShopParser.cpp, try and merge these.
		StatsEntry statsentry;

		statsentry.name = recv.read_padded_string(13);
		statsentry.female = recv.read_bool();

		recv.read_byte();	// skin
		recv.read_int();	// face
		recv.read_int();	// hair

		for (size_t i = 0; i < 3; i++)
			statsentry.petids.push_back(recv.read_long());

		// 北斗 gms083 服务端（已为 999 级改造）把 level 写成 **2 字节 short**。
		// 旧代码按 1 字节 read_byte() 读取，等级 >=256 会被截断（256→0），所以 999 上限必须先改这里。
		statsentry.stats[MapleStat::Id::LEVEL] = static_cast<uint16_t>(recv.read_short());
		statsentry.stats[MapleStat::Id::JOB] = recv.read_short();
		statsentry.stats[MapleStat::Id::STR] = recv.read_short();
		statsentry.stats[MapleStat::Id::DEX] = recv.read_short();
		statsentry.stats[MapleStat::Id::INT] = recv.read_short();
		statsentry.stats[MapleStat::Id::LUK] = recv.read_short();
		statsentry.stats[MapleStat::Id::HP] = recv.read_short();
		statsentry.stats[MapleStat::Id::MAXHP] = recv.read_short();
		statsentry.stats[MapleStat::Id::MP] = recv.read_short();
		statsentry.stats[MapleStat::Id::MAXMP] = recv.read_short();
		statsentry.stats[MapleStat::Id::AP] = recv.read_short();
		statsentry.stats[MapleStat::Id::SP] = recv.read_short();
		// exp 宽度必须按当前对接服务端的 PacketCreator.addCharStats 对齐：
		//  - s9（BeiDou-Server_s9，本客户端目标）：writeInt(exp)=4 字节，末尾另补 writeInt(0)=4 字节
		//    布局 exp4 / fame2 / gacha4 / map4 / spawn1 / tail4
		//  - S10 与主线版本：writeLong(exp)=8 字节，末尾无 tail
		//    布局 exp8 / fame2 / gacha4 / map4 / portal1
		// 两者从 exp 到 charLook 都是 19 字节，所以「读错 exp 宽度」不会抛 PacketError，
		// 只会把中间字段整体平移 4 字节：旧代码按 8 字节 exp 解析 s9 的包时，
		// skip(4)//gachaexp 恰好吞掉真实 mapId(00 e1 f5 05 = 100000000 射手村)，
		// 再从 spawnpoint 槽读出 16 当 mapid → Stage::load 抛 map data missing → 进图白屏。
		// 实机字节：CHARLIST(0x0b) mapId=100000000 @88、spawnpoint=16 @92、charLook @97。
		statsentry.exp = recv.read_long();
		statsentry.stats[MapleStat::Id::FAME] = recv.read_short();

		recv.skip(4); // gachaexp

		statsentry.mapid = recv.read_int();
		statsentry.portal = recv.read_byte();

		// s9 addCharStats 在 spawnpoint 之后固定补一个 int(0)；跳过它 parse_look
		// 才会落在 charLook 起点（stats+89）。旧文档的「无尾部 timestamp」只适用于
		// S10/主线的 8 字节 exp 布局，对 s9 已失效。
		recv.skip(4);
		return statsentry;
	}

	LookEntry LoginParser::parse_look(InPacket& recv)
	{
		LookEntry look;

		look.female = recv.read_bool();
		look.skin = recv.read_byte();
		look.faceid = recv.read_int();

		recv.read_bool(); // megaphone

		look.hairid = recv.read_int();

		uint8_t eqslot = recv.read_byte();

		while (eqslot != 0xFF)
		{
			look.equips[eqslot] = recv.read_int();
			eqslot = recv.read_byte();
		}

		uint8_t mskeqslot = recv.read_byte();

		while (mskeqslot != 0xFF)
		{
			look.maskedequips[mskeqslot] = recv.read_int();
			mskeqslot = recv.read_byte();
		}

		look.maskedequips[-111] = recv.read_int();

		for (uint8_t i = 0; i < 3; i++)
			look.petids.push_back(recv.read_int());

		return look;
	}

	void LoginParser::parse_login(InPacket& recv)
	{
		recv.skip_byte();

		// Read the IPv4 address in a string
		std::string addrstr;

		for (size_t i = 0; i < 4; i++)
		{
			uint8_t num = static_cast<uint8_t>(recv.read_byte());
			addrstr.append(std::to_string(num));

			if (i < 3)
				addrstr.push_back('.');
		}

		// Read the port address in a string
		std::string portstr = std::to_string(recv.read_short());

		// Attempt to reconnect to the server
		Session::get().reconnect(addrstr.c_str(), portstr.c_str());

		// 换频道走的就是这一条：CHANGE_CHANNEL 的回包不带 charId，服务端那句
		// PLAYER_LOGIN 得由我们自己补（PlayerLoggedinHandler.validateState 要求
		// !isLoggedIn()，新连接正好满足）。正常登录时 ServerIPHandler 紧接着会发
		// 同一个包，所以只在 UIChannel 挂过标记时补，避免重复入场。
		if (channel_reentry_pending)
		{
			channel_reentry_pending = false;

			if (channel_entry_cid != 0 && Session::get().is_connected())
				PlayerLoginPacket(channel_entry_cid).dispatch();
			else
				LOG(LOG_NETWORK, "channel reentry skipped: cid=" << channel_entry_cid
					<< " connected=" << Session::get().is_connected());
		}
	}

	void LoginParser::note_channel_entry_cid(int32_t cid)
	{
		channel_entry_cid = cid;
	}

	void LoginParser::expect_channel_reentry()
	{
		channel_reentry_pending = true;
	}

	void LoginParser::clear_channel_reentry()
	{
		channel_reentry_pending = false;
	}
}