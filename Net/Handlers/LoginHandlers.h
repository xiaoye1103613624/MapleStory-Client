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
	// Handler for a packet that contains the response to an attempt at logging in
	class LoginResultHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for a packet that contains the status of the requested world
	class ServerStatusHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for a packet that contains the status of the requested world
	class SelectCharacterHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handles the packet that contains information on worlds and channels
	class ServerlistHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for a packet that contains information on all chars on this world
	class CharlistHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handles the packet which contains the IP of a channel server to connect to
	class ServerIPHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for a packet which responds to the request for a character name
	class CharnameResponseHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for the packet that notifies that a char was successfully created
	class AddNewCharEntryHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// Handler for a packet that responds to the request to the delete a character
	class DeleteCharResponseHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 标准 v83：设置性别成功后的回包。
	// 注意：s9 的 SetGenderHandler 成功后回的是 LOGIN_RESULT（auth success），
	// 性别流程已在该 handler 闭环；此 opcode 在 s9 下不会下发，仅作协议完整性保留。
	class GenderDoneHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 标准 v83：EULA 确认结果。
	// 同上，s9 的 AcceptToSHandler 回 LOGIN_RESULT，此 opcode 在 s9 下不会下发。
	class ConfirmEulaResultHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 服务端 PIN 操作回包（PacketCreator.pinOperation）：
	// mode 0=接受 / 1=注册新 PIN / 2=无效重输 / 3=系统错误 / 4=请输入 PIN。
	// s9 默认 enable_pin=false 时不下发；enable_pin=true 时登录后要求 PIN。
	class CheckPincodeHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 服务端 PIN 注册结果（PacketCreator.pinRegistered）：writeByte(0) = 注册成功。
	class UpdatePincodeHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 查看全部角色：showAllCharacter(概览) + showAllCharacterInfo(各世界详情)。
	// 解析结果暂存到 UIStateLogin::viewall_worlds，由 UIViewAllChar（view-all-char 落地）消费。
	class ViewAllCharHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
}