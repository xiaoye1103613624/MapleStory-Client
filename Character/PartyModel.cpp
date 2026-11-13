////////////////////////////////////////////////////////////////////////////////
//	PartyModel 实现
////////////////////////////////////////////////////////////////////////////////
#include "PartyModel.h"

namespace ms
{
	PartyModel& PartyModel::get()
	{
		static PartyModel model;
		return model;
	}

	void PartyModel::set_leader(int32_t cid)
	{
		party.leader = cid;
	}

	const PartyMember* PartyModel::find(int32_t cid) const
	{
		for (const auto& m : party.members)
			if (m.id == cid && cid != 0)
				return &m;
		return nullptr;
	}
}
