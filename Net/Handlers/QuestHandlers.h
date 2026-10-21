//////////////////////////////////////////////////////////////////////////////////
//	QuestHandlers — S2C UPDATE_QUEST_INFO (211)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	// Opcode: UPDATE_QUEST_INFO(211)
	class UpdateQuestInfoHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}
