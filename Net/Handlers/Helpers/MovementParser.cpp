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
#include "MovementParser.h"

namespace ms
{
	std::vector<Movement> MovementParser::parse_movements(InPacket& recv)
	{
		std::vector<Movement> movements;
		uint8_t length = recv.read_byte();

		for (uint8_t i = 0; i < length; ++i)
		{
			Movement fragment;
			fragment.command = recv.read_byte();

		// s9 服务端对移动流是**逐字节原样转发**（PacketCreator.java:2276-2282
		// rebroadcastMovementList 把客户端上行字节直接回放下发），所以本解析器的每个
		// case 的消费宽度必须与服务端 AbstractMovementPacketHandler.parseMovement
		// （net/server/channel/handlers/AbstractMovementPacketHandler.java:45-157）逐字节一致。
		//
		// 修前的洞：3/4/7/8/9（teleport，服务端 9 字节）与 10（change equip，1 字节）、
		// 14（jump-down，skip 9）、18..22（relative 7 字节）没有 case、0 字节消费，
		// 片段照样 push 进表 ⇒ 同一个包后面的所有片段从错位处开始解析，
		// 表现为：受击/弹簧/椅子传送后别人看你（和看别人）整体乱飞。
		switch (fragment.command)
			{
			default:
				// 服务端对未知 command 直接弃包（EmptyMovementException，
				// AbstractMovementPacketHandler.java:147-149），不产生任何位移；
				// 客户端同样整包作废，宁可丢一次插值也不能错位解析。
				movements.clear();
				return movements;
			case 0:
			case 5:
			case 17:
				fragment.type = Movement::ABSOLUTE;
				fragment.xpos = recv.read_short();
				fragment.ypos = recv.read_short();
				fragment.lastx = recv.read_short();
				fragment.lasty = recv.read_short();
				fragment.fh = recv.read_short();
				fragment.newstate = recv.read_byte();
				fragment.duration = recv.read_short();
				break;
			case 1:
			case 2:
			case 6:
			case 12:
			case 13:
			case 16:
			case 18:
			case 19:
			case 20:
			case 22:
				fragment.type = Movement::RELATIVE;
				fragment.xpos = recv.read_short();
				fragment.ypos = recv.read_short();
				fragment.newstate = recv.read_byte();
				fragment.duration = recv.read_short();
				break;
			case 11:
				fragment.type = Movement::CHAIR;
				fragment.xpos = recv.read_short();
				fragment.ypos = recv.read_short();
				recv.skip(2);
				fragment.newstate = recv.read_byte();
				fragment.duration = recv.read_short();
				break;
			case 15:
				fragment.type = Movement::JUMPDOWN;
				fragment.xpos = recv.read_short();
				fragment.ypos = recv.read_short();
				fragment.lastx = recv.read_short();
				fragment.lasty = recv.read_short();
				recv.skip(2);
				fragment.fh = recv.read_short();
				fragment.newstate = recv.read_byte();
				fragment.duration = recv.read_short();
				break;
			case 3:
			case 4:
			case 7:
			case 8:
			case 9:
				// TeleportMovement：服务端 x,y,xwob,ywob,stance = 9 字节
				// （AbstractMovementPacketHandler.java:88-105）。位移本身交给
				// NONE 类型（不做插值），但字节必须吃满，否则后续片段错位。
				recv.skip(4);
				recv.skip(4);
				fragment.newstate = recv.read_byte();
				break;
			case 14:
				// jump-down：服务端 skip(9)（:106-108）
				recv.skip(9);
				break;
			case 10:
				// Change equip：服务端 readByte（:109-111），回放时也是 2 字节
				// （writeByte(10)+writeByte(slot)，server/movement/ChangeEquip.java）
				recv.skip_byte();
				break;
			case 21:
				// 服务端 skip(3)（AbstractMovementPacketHandler.java:139-146）
				recv.skip(3);
				break;
			}

			movements.push_back(fragment);
		}

		return movements;
	}
}