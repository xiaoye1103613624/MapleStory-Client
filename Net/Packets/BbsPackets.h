////////////////////////////////////////////////////////////////////////////////
//	Guild BBS outbound — Opcode BBS_OPERATION(155 / 0x9B)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>

namespace ms
{
	// mode 2：列表（start 页，服务端 start*10）
	class BbsListPacket : public OutPacket
	{
	public:
		BbsListPacket(int32_t start = 0) : OutPacket(OutPacket::Opcode::BBS_OPERATION)
		{
			write_byte(2);
			write_int(start);
		}
	};

	// mode 3：打开帖子
	class BbsOpenPacket : public OutPacket
	{
	public:
		BbsOpenPacket(int32_t localthreadid) : OutPacket(OutPacket::Opcode::BBS_OPERATION)
		{
			write_byte(3);
			write_int(localthreadid);
		}
	};

	// mode 1：删帖
	class BbsDeletePacket : public OutPacket
	{
	public:
		BbsDeletePacket(int32_t localthreadid) : OutPacket(OutPacket::Opcode::BBS_OPERATION)
		{
			write_byte(1);
			write_int(localthreadid);
		}
	};

	// mode 4：回复（text ≤25）
	class BbsReplyPacket : public OutPacket
	{
	public:
		BbsReplyPacket(int32_t localthreadid, const std::string& text)
			: OutPacket(OutPacket::Opcode::BBS_OPERATION)
		{
			write_byte(4);
			write_int(localthreadid);
			write_string(text);
		}
	};

	// mode 0：发帖 / 编辑（新建 bEdit=0）
	class BbsPostPacket : public OutPacket
	{
	public:
		BbsPostPacket(bool notice, const std::string& title, const std::string& text, int32_t icon = 0)
			: OutPacket(OutPacket::Opcode::BBS_OPERATION)
		{
			write_byte(0);
			write_byte(0); // bEdit = false
			write_byte(notice ? 1 : 0);
			write_string(title);
			write_string(text);
			write_int(icon);
		}
	};
}
