////////////////////////////////////////////////////////////////////////////////
//	Fredrick 出站：Opcode FREDRICK_ACTION(64)，对照 s9 FredrickHandler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class FredrickGetAllPacket : public OutPacket
	{
	public:
		FredrickGetAllPacket() : OutPacket(OutPacket::Opcode::FREDRICK_ACTION)
		{
			write_byte(0x1A);
		}
	};

	class FredrickExitPacket : public OutPacket
	{
	public:
		FredrickExitPacket() : OutPacket(OutPacket::Opcode::FREDRICK_ACTION)
		{
			write_byte(0x1C);
		}
	};
}
