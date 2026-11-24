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

#include "../OutPacket.h"

namespace ms
{
	class OutPacket;

	// 联盟出站包：Opcode ALLIANCE_OPERATION(143)，对接 s9 AllianceOperationHandler(action 字节)
	class AllianceActionPacket : public OutPacket
	{
	public:
		AllianceActionPacket(int8_t action) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(action);
		}
	};

	// 修改联盟公告（action 0x0A）
	class AllianceNoticePacket : public OutPacket
	{
	public:
		AllianceNoticePacket(const std::string& notice) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x0A);
			write_string(notice);
		}
	};

	// 邀请公会加入联盟（action 0x03）
	class AllianceInvitePacket : public OutPacket
	{
	public:
		AllianceInvitePacket(const std::string& guildName) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x03);
			write_string(guildName);
		}
	};

	// 驱逐公会出盟（action 0x06）
	// Angel sub_914EB4 @0x914eb4：Encode1(6)+Encode4(guildid)+Encode4(allianceid)
	// s9 AllianceOperationHandler case 0x06：readInt guildid + readInt allianceid
	class AllianceKickPacket : public OutPacket
	{
	public:
		AllianceKickPacket(int32_t guildid, int32_t allianceid) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x06);
			write_int(guildid);
			write_int(allianceid);
		}
	};

	// 转让联盟盟主（action 0x07）
	// Angel sub_915983 @0x915983 / v95 CTabGuildAlliance::OnChangeMaster：Encode1(7)+Encode4(cid)
	// s9 case 0x07：readInt victimid（须 alliance_rank==2）
	class AllianceChangeMasterPacket : public OutPacket
	{
	public:
		explicit AllianceChangeMasterPacket(int32_t cid) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x07);
			write_int(cid);
		}
	};

	// 成员联盟职级升降（action 0x09）
	// Angel sub_91505B @0x91505b / v95 OnGradeChange(bUp)：Encode1(9)+Encode4(cid)+Encode1(bUp)
	// s9 case 0x09：readInt cid + readByte raise（>0 升职，否则降职；新职级仅 3..5）
	// 注意：与 AllianceGradeNamePacket(0x08) 不同 —— 本包改成员职级序号，不改职衔名。
	class AllianceGradeChangePacket : public OutPacket
	{
	public:
		AllianceGradeChangePacket(int32_t cid, bool raise) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x09);
			write_int(cid);
			write_byte(raise ? 1 : 0);
		}
	};

	// 联盟五段职衔名保存（action 0x08）— CWndAllianceGrade::OnSaveGradeName @0x91848d
	// Encode1(8)+5×EncodeStr；s9 case 0x08：readString×5 → updateRankTitles
	class AllianceGradeNamePacket : public OutPacket
	{
	public:
		AllianceGradeNamePacket(const std::string& g1, const std::string& g2, const std::string& g3,
			const std::string& g4, const std::string& g5)
			: OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x08);
			write_string(g1);
			write_string(g2);
			write_string(g3);
			write_string(g4);
			write_string(g5);
		}
	};

	// 接受联盟邀请（action 0x04）
	// s9 AllianceOperationHandler case 0x04：readInt(allianceid)；
	// 后续 recruiter 公会名一行被注释掉不读（AllianceOperationHandler.java:107-108）。
	// 仅公会长度 1 的会长可发（服务端 guildrank==1 校验）。
	class AllianceAcceptPacket : public OutPacket
	{
	public:
		explicit AllianceAcceptPacket(int32_t allianceid) : OutPacket(OutPacket::Opcode::ALLIANCE_OPERATION)
		{
			write_byte(0x04);
			write_int(allianceid);
		}
	};

	// Opcode: DENY_ALLIANCE_REQUEST(144 / 0x90) — byte(0) + inviter + guild
	class DenyAllianceRequestPacket : public OutPacket
	{
	public:
		DenyAllianceRequestPacket(const std::string& inviter, const std::string& guild)
			: OutPacket(OutPacket::Opcode::DENY_ALLIANCE_REQUEST)
		{
			write_byte(0);
			write_string(inviter);
			write_string(guild);
		}
	};
}
