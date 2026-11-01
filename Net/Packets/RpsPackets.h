////////////////////////////////////////////////////////////////////////////////
//	RPS_ACTION(136) — s9 RPSActionHandler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class RpsActionPacket : public OutPacket
	{
	public:
		explicit RpsActionPacket(uint8_t mode) : OutPacket(OutPacket::Opcode::RPS_ACTION)
		{
			write_byte(mode);
		}

		RpsActionPacket(uint8_t mode, uint8_t answer) : OutPacket(OutPacket::Opcode::RPS_ACTION)
		{
			write_byte(mode);
			write_byte(answer);
		}
	};
}
