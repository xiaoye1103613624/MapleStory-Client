////////////////////////////////////////////////////////////////////////////////
//	FREDRICK(311) / FREDRICK_MESSAGE(310) — StoreBank 寄存
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	class FredrickHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	class FredrickMessageHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
