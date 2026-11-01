////////////////////////////////////////////////////////////////////////////////
//	Messenger 出站包：Opcode MESSENGER(122)，对照 s9 MessengerHandler mode 序。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// mode 0x00：建房 (messengerid==0) 或接受邀请进房 (messengerid!=0)
	class MessengerOpenPacket : public OutPacket
	{
	public:
		MessengerOpenPacket(int32_t messengerid) : OutPacket(OutPacket::Opcode::MESSENGER)
		{
			write_byte(0x00);
			write_int(messengerid);
		}
	};

	// mode 0x02：离房
	class MessengerLeavePacket : public OutPacket
	{
	public:
		MessengerLeavePacket() : OutPacket(OutPacket::Opcode::MESSENGER)
		{
			write_byte(0x02);
		}
	};

	// mode 0x03：按角色名邀请
	class MessengerInvitePacket : public OutPacket
	{
	public:
		MessengerInvitePacket(const std::string& name) : OutPacket(OutPacket::Opcode::MESSENGER)
		{
			write_byte(0x03);
			write_string(name);
		}
	};

	// mode 0x05：拒绝邀请（带邀请方名字）
	class MessengerDeclinePacket : public OutPacket
	{
	public:
		MessengerDeclinePacket(const std::string& sender) : OutPacket(OutPacket::Opcode::MESSENGER)
		{
			write_byte(0x05);
			write_string(sender);
		}
	};

	// mode 0x06：聊天文本（常含 "Name : msg" 前缀由客户端拼）
	class MessengerChatPacket : public OutPacket
	{
	public:
		MessengerChatPacket(const std::string& text) : OutPacket(OutPacket::Opcode::MESSENGER)
		{
			write_byte(0x06);
			write_string(text);
		}
	};
}
