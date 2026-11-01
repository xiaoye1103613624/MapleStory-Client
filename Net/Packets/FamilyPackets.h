////////////////////////////////////////////////////////////////////////////////
//	Family 出站：OPEN_FAMILY(146) / OPEN_FAMILY_PEDIGREE(145)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

namespace ms
{
	class OpenFamilyPacket : public OutPacket
	{
	public:
		OpenFamilyPacket() : OutPacket(OutPacket::Opcode::OPEN_FAMILY) {}
	};

	class OpenFamilyPedigreePacket : public OutPacket
	{
	public:
		OpenFamilyPedigreePacket() : OutPacket(OutPacket::Opcode::OPEN_FAMILY_PEDIGREE) {}
	};

	class FamilyAddPacket : public OutPacket
	{
	public:
		FamilyAddPacket(const std::string& name) : OutPacket(OutPacket::Opcode::ADD_FAMILY)
		{
			write_string(name);
		}
	};

	class FamilyAcceptPacket : public OutPacket
	{
	public:
		FamilyAcceptPacket(int32_t inviter_id, const std::string& inviter_name, bool accept)
			: OutPacket(OutPacket::Opcode::ACCEPT_FAMILY)
		{
			write_int(inviter_id);
			write_string(inviter_name);
			write_byte(accept ? 1 : 0);
		}
	};

	class FamilyPreceptPacket : public OutPacket
	{
	public:
		FamilyPreceptPacket(const std::string& message)
			: OutPacket(OutPacket::Opcode::CHANGE_FAMILY_MESSAGE)
		{
			write_string(message);
		}
	};

	/// s9 FamilySummonResponseHandler：string familyName + byte accept
	class FamilySummonResponsePacket : public OutPacket
	{
	public:
		FamilySummonResponsePacket(const std::string& family_name, bool accept)
			: OutPacket(OutPacket::Opcode::FAMILY_SUMMON_RESPONSE)
		{
			write_string(family_name);
			write_byte(accept ? 1 : 0);
		}
	};

	/// s9 FamilyUseHandler：int entitlementOrdinal + optional string targetName
	/// 0=FAMILY_REUINION, 1=SUMMON_FAMILY（需 name）
	class FamilyUsePacket : public OutPacket
	{
	public:
		FamilyUsePacket(int32_t entitlement, const std::string& target_name = {})
			: OutPacket(OutPacket::Opcode::USE_FAMILY)
		{
			write_int(entitlement);
			if (!target_name.empty())
				write_string(target_name);
		}
	};

	/// s9 SEPARATE_FAMILY_BY_SENIOR(0x94)：int juniorCid
	class FamilySeparateSeniorPacket : public OutPacket
	{
	public:
		explicit FamilySeparateSeniorPacket(int32_t junior_cid)
			: OutPacket(OutPacket::Opcode::SEPARATE_FAMILY_BY_SENIOR)
		{
			write_int(junior_cid);
		}
	};

	/// s9 SEPARATE_FAMILY_BY_JUNIOR(0x95)：empty
	class FamilySeparateJuniorPacket : public OutPacket
	{
	public:
		FamilySeparateJuniorPacket()
			: OutPacket(OutPacket::Opcode::SEPARATE_FAMILY_BY_JUNIOR)
		{
		}
	};
}
