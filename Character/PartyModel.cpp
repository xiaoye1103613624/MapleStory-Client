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

	void PartyModel::clear()
	{
		party = Party{};
		member_hps.clear();
	}

	void PartyModel::set_member_hp(int32_t cid, int32_t cur, int32_t max)
	{
		if (cid <= 0 || max <= 0)
			return;

		if (cur < 0)
			cur = 0;
		if (cur > max)
			cur = max;

		member_hps[cid] = { cur, max };
	}

	const std::pair<int32_t, int32_t>* PartyModel::member_hp(int32_t cid) const
	{
		auto it = member_hps.find(cid);
		return it != member_hps.end() ? &it->second : nullptr;
	}
}
