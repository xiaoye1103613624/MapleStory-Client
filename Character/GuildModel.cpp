////////////////////////////////////////////////////////////////////////////////
//	GuildModel 实现
////////////////////////////////////////////////////////////////////////////////
#include "GuildModel.h"

namespace ms
{
	GuildModel& GuildModel::get()
	{
		static GuildModel model;
		return model;
	}

	void GuildModel::add_member(const GuildMember& m)
	{
		for (const auto& e : guild.members)
			if (e.id == m.id)
				return;
		guild.members.push_back(m);
	}

	void GuildModel::remove_member(int32_t id)
	{
		for (size_t i = 0; i < guild.members.size(); i++)
			if (guild.members[i].id == id)
			{
				guild.members.erase(guild.members.begin() + static_cast<long>(i));
				return;
			}
	}

	void GuildModel::set_member_rank(int32_t id, int32_t rank)
	{
		for (auto& m : guild.members)
			if (m.id == id)
				m.rank = rank;
	}

	void GuildModel::set_member_online(int32_t id, bool online)
	{
		for (auto& m : guild.members)
			if (m.id == id)
				m.online = online;
	}

	const GuildMember* GuildModel::find(int32_t id) const
	{
		for (const auto& m : guild.members)
			if (m.id == id)
				return &m;
		return nullptr;
	}
}
