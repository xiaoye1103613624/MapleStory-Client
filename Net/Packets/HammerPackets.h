////////////////////////////////////////////////////////////////////////////////
//	ViciousHammer 出站：USE_HAMMER(260)，对照 s9 UseHammerHandler（空包 → sendHammerMessage）
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class UseHammerPacket : public OutPacket
	{
	public:
		UseHammerPacket() : OutPacket(OutPacket::Opcode::USE_HAMMER) {}
	};
}
