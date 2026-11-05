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

#include "MovementPacket.h"

namespace ms
{
	// Opcode: CHANGE_MAP(38)
	class ChangeMapPacket : public OutPacket
	{
	public:
		// Request the server to warp the player to a different map
		ChangeMapPacket(bool died, int32_t targetid, const std::string& targetp, bool usewheel) : OutPacket(OutPacket::Opcode::CHANGEMAP)
		{
			write_byte(died);
			write_int(targetid);
			write_string(targetp);
			skip(1);
			write_short(usewheel ? 1 : 0);
		}

		// Request the server to exit the cash shop
		ChangeMapPacket() : OutPacket(OutPacket::Opcode::CHANGEMAP) {}
	};

	// Opcode: ENTER_CASHSHOP(40)
	class EnterCashShopPacket : public OutPacket
	{
	public:
		// Requests the server to warp the player into the cash shop
		EnterCashShopPacket() : OutPacket(OutPacket::Opcode::ENTER_CASHSHOP) {}
	};

	// Opcode: CHANGE_CHANNEL(39 / 0x27)
	//
	// 字段序取证自服务端本体（不是猜的）：
	//   E:\project\BeiDou-Server_s9\gms-server\src\main\java\org\gms\net\server\
	//       channel\handlers\ChangeChannelHandler.java:38-39
	//         int channel = p.readByte() + 1;   // 客户端发 0 基频道号
	//         p.readInt();                      // 读走即弃，返回值未被使用
	// 之前本包只写 1 字节，服务端那条 readInt() 直接抛异常 ⇒ 永无回包，
	// 就是用户报的「频道更换点击也没有反映」。
	//
	// 回包 PacketCreator.getChannelChange()（同库 util/PacketCreator.java:891-897）
	// 写 byte(1) + 4 字节 IP + short port，正好对上 LoginParser::parse_login 的
	// skip_byte + 4*read_byte + read_short，客户端侧无需再改。
	//
	// 预留：那 4 字节在原生 v83 客户端里的**语义**未取证（IDA 未定位到构造点）。
	// 服务端把它丢弃，所以填 0 不影响换频；若日后换服务端实现需按原生值校准。
	class ChangeChannelPacket : public OutPacket
	{
	public:
		ChangeChannelPacket(uint8_t channel) : OutPacket(OutPacket::Opcode::CHANGE_CHANNEL)
		{
			write_byte(channel);
			write_int(0);
		}
	};

	// Opcode: MOVE_PLAYER(41)
	class MovePlayerPacket : public MovementPacket
	{
	public:
		// Updates the player's position with the server
		MovePlayerPacket(const Movement& movement) : MovementPacket(OutPacket::Opcode::MOVE_PLAYER)
		{
			skip(9);
			write_byte(1);
			writemovement(movement);
		}
	};

	// Opcode: PARTY_OPERATION(124)
	class PartyOperationPacket : public OutPacket
	{
	public:
		enum Operation : int8_t
		{
			CREATE = 1,
			LEAVE = 2,
			JOIN = 3,
			INVITE = 4,
			EXPEL = 5,
			PASS_LEADER = 6
		};

	protected:
		PartyOperationPacket(Operation op) : OutPacket(OutPacket::Opcode::PARTY_OPERATION)
		{
			write_byte(op);
		}
	};

	// Creates a new party
	class CreatePartyPacket : public PartyOperationPacket
	{
	public:
		CreatePartyPacket() : PartyOperationPacket(PartyOperationPacket::Operation::CREATE) {}
	};

	// Leaves a party
	class LeavePartyPacket : public PartyOperationPacket
	{
	public:
		LeavePartyPacket() : PartyOperationPacket(PartyOperationPacket::Operation::LEAVE) {}
	};

	// Joins a party
	class JoinPartyPacket : public PartyOperationPacket
	{
	public:
		JoinPartyPacket(int32_t party_id) : PartyOperationPacket(PartyOperationPacket::Operation::JOIN)
		{
			write_int(party_id);
		}
	};

	// Invites a player to a party
	class InviteToPartyPacket : public PartyOperationPacket
	{
	public:
		InviteToPartyPacket(const std::string& name) : PartyOperationPacket(PartyOperationPacket::Operation::INVITE)
		{
			write_string(name);
		}
	};

	// Expels someone from a party
	class ExpelFromPartyPacket : public PartyOperationPacket
	{
	public:
		ExpelFromPartyPacket(int32_t cid) : PartyOperationPacket(PartyOperationPacket::Operation::EXPEL)
		{
			write_int(cid);
		}
	};

	// Passes party leadership to another character
	class ChangePartyLeaderPacket : public PartyOperationPacket
	{
	public:
		ChangePartyLeaderPacket(int32_t cid) : PartyOperationPacket(PartyOperationPacket::Operation::PASS_LEADER)
		{
			write_int(cid);
		}
	};

	// Opcode: ADMIN_COMMAND(128)
	// 服务端权威：s9 AdminCommandHandler.java（switch(mode)，权限门 = isGM()）
	//   0x00 召唤袋刷怪（读 int itemId，走 ItemInformationProvider.getSummonMobs）
	//   0x02 设置经验（读 int exp）
	//   0x11 入图（其内再读 byte 子操作：0=列本图用户、12=uclip）
	//   0x15 杀怪（int mobId + int amount）
	//   0x17 刷怪（int mobId + int quantity → spawnMonsterOnGroundBelow）★
	//   0x18 报怪血量（int mobId）
	// 客户端此前只发了 0x11（子操作 0/12），0x17 这条"按 mobId 直接刷怪"没接，
	// 导致冒烟时无法在有怪/空图造测试怪（见 evidence/smoke-2026-11-05.md 阻塞2）。
	class AdminCommandPacket : public OutPacket
	{
	public:
		enum Mode : int8_t
		{
			ENTER_MAP = 0x11,
			SUMMON_MOB = 0x17,
			SET_EXP = 0x02,
			KILL_MOB = 0x15,
			MOB_HP = 0x18
		};

	protected:
		AdminCommandPacket(Mode mode) : OutPacket(OutPacket::Opcode::ADMIN_COMMAND)
		{
			write_byte(mode);
		}
	};

	// Admin has entered the map
	class AdminEnterMapPacket : public AdminCommandPacket
	{
	public:
		enum Operation : int8_t
		{
			SHOW_USERS,
			ALERT_ADMINS = 12
		};

		AdminEnterMapPacket(Operation op) : AdminCommandPacket(AdminCommandPacket::Mode::ENTER_MAP)
		{
			write_byte(op);
		}
	};

	// Wave 0.1 冒烟配套：按 mobId 在脚下刷 N 只（s9 AdminCommandHandler case 0x17）。
	// 用途 = 真机冒烟造测试怪（触怪仇恨 0xBD / 手雷广播 0x6D / 受击数字等都需要怪），
	// 也是 GM 工具箱里原本缺失的一件。服务端已是现成能力，本包只是把入口接出来。
	class AdminSummonMobPacket : public AdminCommandPacket
	{
	public:
		AdminSummonMobPacket(int32_t mobid, int32_t quantity)
			: AdminCommandPacket(AdminCommandPacket::Mode::SUMMON_MOB)
		{
			write_int(mobid);
			write_int(quantity);
		}
	};

	// s9 case 0x02：直接设经验（int），用于冒烟造升级 → 拿 AP 验一键分配。
	class AdminSetExpPacket : public AdminCommandPacket
	{
	public:
		explicit AdminSetExpPacket(int32_t exp)
			: AdminCommandPacket(AdminCommandPacket::Mode::SET_EXP)
		{
			write_int(exp);
		}
	};

	// Opcode: MOVE_MONSTER(188)
	class MoveMobPacket : public MovementPacket
	{
	public:
		// Updates a mob's position with the server
		MoveMobPacket(int32_t oid, int16_t type, int8_t skillb, int8_t skill0, int8_t skill1, int8_t skill2, int8_t skill3, int8_t skill4, Point<int16_t> startpos, const Movement& movement) : MovementPacket(OutPacket::Opcode::MOVE_MONSTER)
		{
			write_int(oid);
			write_short(type);
			write_byte(skillb);
			write_byte(skill0);
			write_byte(skill1);
			write_byte(skill2);
			write_byte(skill3);
			write_byte(skill4);

			skip(13);

			write_point(startpos);

			write_byte(1);
			writemovement(movement);
		}
	};

	// Opcode: PICKUP_ITEM(202)
	class PickupItemPacket : public OutPacket
	{
	public:
		// Requests picking up an item
		PickupItemPacket(int32_t oid, Point<int16_t> position) : OutPacket(OutPacket::Opcode::PICKUP_ITEM)
		{
			write_int(0);
			write_byte(0);
			write_point(position);
			write_int(oid);
		}
	};

	// Opcode: DAMAGE_REACTOR(205)
	class DamageReactorPacket : public OutPacket
	{
	public:
		// Requests damaging a reactor
		DamageReactorPacket(int32_t oid, Point<int16_t> position, int16_t stance, int skillid) : OutPacket(OutPacket::Opcode::DAMAGE_REACTOR)
		{
			write_int(oid);
			write_point(position);
			write_short(stance);
			skip(4);
			write_int(skillid);
		};
	};

	// Opcode: PLAYER_MAP_TRANSFER(207)
	class PlayerMapTransferPacket : public OutPacket
	{
	public:
		// Requests the server to set map transition complete
		PlayerMapTransferPacket() : OutPacket(OutPacket::Opcode::PLAYER_MAP_TRANSFER) {}
	};

	// 历史误用：Opcode 223 实为 s9 PARTY_SEARCH_UPDATE（停止匹配）。
	// 进图后勿再 dispatch；保留空类以免旧 include 断编译。
	class PlayerUpdatePacket
	{
	public:
		PlayerUpdatePacket() = default;
		void dispatch() const
		{
			/* no-op：禁止对 223 发空包，否则会注销 PartySearch */
		}
	};
}