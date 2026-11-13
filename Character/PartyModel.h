////////////////////////////////////////////////////////////////////////////////
//	PartyModel — 组队会话级数据层（Angel 全流程 P0 / T1）
//
//	仿 GuildModel：UserList 组员页读常驻模型；PartyHandlers 写模型；
//	UIParty 浮窗仅已打开时同步，不再为刷列表强制弹窗。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace ms
{
	struct PartyMember
	{
		int32_t id = 0;
		std::string name;
		int32_t job = 0;
		int32_t level = 0;
		int32_t channel = -2;
		int32_t mapid = 0;
		bool online = false;
	};

	struct Party
	{
		int32_t id = 0;
		int32_t leader = 0;
		std::array<PartyMember, 6> members{};

		bool in_party() const { return id > 0; }
	};

	class PartyModel
	{
	public:
		static PartyModel& get();

		const Party& data() const { return party; }
		Party& mutable_data() { return party; }
		void set(const Party& p) { party = p; }
		void clear() { party = Party{}; }

		void set_leader(int32_t cid);
		const PartyMember* find(int32_t cid) const;

	private:
		Party party;
	};
}
