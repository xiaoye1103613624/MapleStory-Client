////////////////////////////////////////////////////////////////////////////////
//	PartyModel — 组队会话级数据层（Angel 全流程 P0 / T1）
//
//	仿 GuildModel：UserList 组员页读常驻模型；PartyHandlers 写模型；
//	UIParty 浮窗仅已打开时同步，不再为刷列表强制弹窗。
//
//	r3（2026-11-16）：血量改常驻存储。旧路径 PartyMemberHPHandler 只在 UIParty
//	浮窗活着时把 hp 写进浮窗成员 map——「先收血包后开窗」血量永久丢失。现在
//	handler 写模型（member_hp），UIUserList 组员页血条与 UIParty 都读模型。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <utility>

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
		void clear();

		void set_leader(int32_t cid);
		const PartyMember* find(int32_t cid) const;

		// 同族成员血量（cid → {cur, max}），s9 UPDATE_PARTYMEMBER_HP 常驻下发。
		void set_member_hp(int32_t cid, int32_t cur, int32_t max);
		const std::pair<int32_t, int32_t>* member_hp(int32_t cid) const;
		void drop_member_hp(int32_t cid) { member_hps.erase(cid); }

	private:
		Party party;
		std::map<int32_t, std::pair<int32_t, int32_t>> member_hps;
	};
}
