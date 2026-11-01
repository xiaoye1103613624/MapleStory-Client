////////////////////////////////////////////////////////////////////////////////
//	PartySearch 出站：START(222) / UPDATE-Stop(223)，对照 s9 PartySearch*Handler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	// 队长开始自动匹配：min/max 等级、members（服务端忽略）、jobs 职业位掩码
	class PartySearchStartPacket : public OutPacket
	{
	public:
		PartySearchStartPacket(int32_t min_level, int32_t max_level, int32_t members, int32_t jobs)
			: OutPacket(OutPacket::Opcode::PARTY_SEARCH_START)
		{
			write_int(min_level);
			write_int(max_level);
			write_int(members);
			write_int(jobs);
		}
	};

	// 停止搜索（空包体）
	class PartySearchStopPacket : public OutPacket
	{
	public:
		PartySearchStopPacket() : OutPacket(OutPacket::Opcode::PARTY_SEARCH_UPDATE) {}
	};

	// 拒绝组队/PartySearch 邀请（DENY_PARTY_REQUEST=125）
	class DenyPartyRequestPacket : public OutPacket
	{
	public:
		DenyPartyRequestPacket(const std::string& from_name) : OutPacket(OutPacket::Opcode::DENY_PARTY_REQUEST)
		{
			write_byte(0);
			write_string(from_name);
		}
	};
}
