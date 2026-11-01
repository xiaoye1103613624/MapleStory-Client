////////////////////////////////////////////////////////////////////////////////
//	便签 NOTE_ACTION(131)：mode1=丢弃（需 note id 列表；壳阶段可发空批）
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <string>
#include <vector>

namespace ms
{
	class NoteDiscardPacket : public OutPacket
	{
	public:
		NoteDiscardPacket(const std::vector<int32_t>& ids) : OutPacket(OutPacket::Opcode::NOTE_ACTION)
		{
			write_byte(1);
			write_byte(static_cast<int8_t>(ids.size()));
			write_byte(0);
			write_byte(0);
			for (int32_t id : ids)
			{
				write_int(id);
				write_byte(0);
			}
		}
	};

	// s9 NoteActionHandler action0：string name + string message（商城便签）
	class NoteSendPacket : public OutPacket
	{
	public:
		NoteSendPacket(const std::string& to, const std::string& message)
			: OutPacket(OutPacket::Opcode::NOTE_ACTION)
		{
			write_byte(0);
			write_string(to);
			write_string(message);
		}
	};

	// s9 NewYearCardHandler reqMode0：short slot + int itemid + string receiver + string msg
	class NewYearCardSendPacket : public OutPacket
	{
	public:
		NewYearCardSendPacket(int16_t slot, int32_t itemid, const std::string& receiver, const std::string& message)
			: OutPacket(OutPacket::Opcode::NEW_YEAR_CARD_REQUEST)
		{
			write_byte(0);
			write_short(slot);
			write_int(itemid);
			write_string(receiver);
			write_string(message);
		}
	};
}
