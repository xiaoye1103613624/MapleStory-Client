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

#include "../OutPacket.h"

namespace ms
{
	// Packet which notifies the server that the connection is still alive.
	// Opcode: PONG(24)
	class PongPacket : public OutPacket
	{
	public:
		PongPacket() : OutPacket(PONG) {}
	};

	// Opcode: CANCEL_BUFF(92 / 0x5C) — s9 CancelBuffHandler body 只有一个
	// int sourceid（增益来源技能 id）。服务端白名单技能走专用取消，default
	// 分支 cancelEffect(skill(sourceid))；结算后回 s2c CANCEL_BUFF(33) 移除
	// 列表图标。Wave 3：UIBuffList 右键点图标即发本包。
	class CancelBuffPacket : public OutPacket
	{
	public:
		CancelBuffPacket(int32_t sourceid) : OutPacket(OutPacket::Opcode::CANCEL_BUFF)
		{
			write_int(sourceid);
		}
	};

	// Opcode: CANCEL_DEBUFF(99 / 0x63) — 按 s9 协议在疾病到期时发空包。
	// 083 IDB 无对应到期检查符号（OnTemporaryStatSet @0xa202be / OnTemporaryStatReset @0xa2071f 仅作对照）；
	// s9 CancelDebuffHandler 当前为空实现（注释块带 "TIP: BAD STUFF LOL!"，不解禁），本包仅做协议对齐与可观测性。
	class CancelDebuffPacket : public OutPacket
	{
	public:
		CancelDebuffPacket() : OutPacket(OutPacket::Opcode::CANCEL_DEBUFF) {}
	};
}