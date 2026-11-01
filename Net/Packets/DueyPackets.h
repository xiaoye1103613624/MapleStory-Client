////////////////////////////////////////////////////////////////////////////////
//	Duey 出站：Opcode DUEY_ACTION(65)，对照 s9 DueyHandler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"
#include <string>

namespace ms
{
	class DueyOpenRecvPacket : public OutPacket
	{
	public:
		DueyOpenRecvPacket() : OutPacket(OutPacket::Opcode::DUEY_ACTION)
		{
			write_byte(0x00);
		}
	};

	class DueySendPacket : public OutPacket
	{
	public:
		DueySendPacket(int8_t invent, int16_t slot, int16_t amount, int32_t mesos,
			const std::string& recipient, bool quick, const std::string& message)
			: OutPacket(OutPacket::Opcode::DUEY_ACTION)
		{
			write_byte(0x02);
			write_byte(invent);
			write_short(slot);
			write_short(amount);
			write_int(mesos);
			write_string(recipient);
			write_byte(quick ? 1 : 0);
			if (quick)
				write_string(message);
		}
	};

	class DueyClaimPacket : public OutPacket
	{
	public:
		DueyClaimPacket(int32_t package_id) : OutPacket(OutPacket::Opcode::DUEY_ACTION)
		{
			write_byte(0x04);
			write_int(package_id);
		}
	};

	class DueyDiscardPacket : public OutPacket
	{
	public:
		DueyDiscardPacket(int32_t package_id) : OutPacket(OutPacket::Opcode::DUEY_ACTION)
		{
			write_byte(0x05);
			write_int(package_id);
		}
	};

	class DueyClosePacket : public OutPacket
	{
	public:
		DueyClosePacket() : OutPacket(OutPacket::Opcode::DUEY_ACTION)
		{
			write_byte(0x07);
		}
	};
}
