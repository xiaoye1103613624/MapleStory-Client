////////////////////////////////////////////////////////////////////////////////
//	Owl of Minerva outbound — OWL_ACTION(66) / OWL_WARP(67)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// UseOwlOfMinerva：空 payload → 打开猫头鹰排行/结果
	class UseOwlPacket : public OutPacket
	{
	public:
		UseOwlPacket() : OutPacket(OutPacket::Opcode::OWL_ACTION)
		{
		}
	};

	// OwlWarp：ownerid + mapid
	class OwlWarpPacket : public OutPacket
	{
	public:
		OwlWarpPacket(int32_t ownerid, int32_t mapid) : OutPacket(OutPacket::Opcode::OWL_WARP)
		{
			write_int(ownerid);
			write_int(mapid);
		}
	};
}
