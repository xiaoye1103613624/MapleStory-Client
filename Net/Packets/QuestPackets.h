//////////////////////////////////////////////////////////////////////////////////
//	QuestPackets — C2S quest actions (BeiDou QUEST_ACTION = 0x6B)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// Opcode: QUEST_ACTION(107)
	// Layout: byte action, short questid [, int npcid for start/complete/scripted]
	class QuestActionPacket : public OutPacket
	{
	public:
		QuestActionPacket(int8_t action, int16_t questid, int32_t npcid = 0) : OutPacket(OutPacket::Opcode::QUEST_ACTION)
		{
			write_byte(action);
			write_short(questid);

			// 1=start, 2=complete, 4/5=scripted — need npc; 3=forfeit has no npc
			if (action == 1 || action == 2 || action == 4 || action == 5)
				write_int(npcid);
		}
	};
}
