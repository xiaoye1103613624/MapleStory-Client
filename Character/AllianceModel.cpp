////////////////////////////////////////////////////////////////////////////////
//	AllianceModel 实现
////////////////////////////////////////////////////////////////////////////////
#include "AllianceModel.h"

namespace ms
{
	AllianceModel& AllianceModel::get()
	{
		static AllianceModel model;
		return model;
	}

	void AllianceModel::set_ranks(const std::vector<std::string>& ranks)
	{
		for (int i = 0; i < 5; i++)
			alliance.rank_titles[i] = (i < static_cast<int>(ranks.size())) ? ranks[i] : std::string();
	}

	void AllianceModel::set_guild_names(const std::vector<std::string>& names)
	{
		alliance.guild_names = names;
		if (alliance.guild_ids.size() < names.size())
			alliance.guild_ids.resize(names.size(), 0);

		// 同步已解析成员行上的公会名（按 guild_ids 下标对齐）。
		for (auto& m : alliance.members)
		{
			for (size_t i = 0; i < alliance.guild_ids.size(); i++)
			{
				if (alliance.guild_ids[i] == m.guild_id && i < alliance.guild_names.size())
				{
					m.guild_name = alliance.guild_names[i];
					break;
				}
			}
		}
	}

	void AllianceModel::set_member_online(int32_t cid, bool online)
	{
		for (auto& m : alliance.members)
			if (m.id == cid)
				m.online = online;
	}

	void AllianceModel::set_member_job_level(int32_t cid, int32_t level, int32_t job)
	{
		for (auto& m : alliance.members)
		{
			if (m.id == cid)
			{
				m.level = level;
				m.job = job;
			}
		}
	}

	const AllianceMember* AllianceModel::find(int32_t cid) const
	{
		for (const auto& m : alliance.members)
			if (m.id == cid)
				return &m;
		return nullptr;
	}
}
