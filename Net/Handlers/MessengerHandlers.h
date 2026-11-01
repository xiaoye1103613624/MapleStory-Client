////////////////////////////////////////////////////////////////////////////////
//	Messenger 入站：PacketSwitch MESSENGER=313，对照 s9 PacketCreator.messenger*
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	class MessengerHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
