////////////////////////////////////////////////////////////////////////////////
//	AllianceModel — 家族联盟（Guild Alliance）会话级数据层
//
//	W1.5（2026-11-20）：083 联盟成员不在独立 CUIAlliance 浮窗，而在
//	CUIUserList::CTabGuildAlliance（Angel LoadData @0x9126f4 / GUILDITEM）。
//	R28-B2 之后 handler 不为回包开窗 → 共用窗 UNION 页必须读常驻模型，
//	否则「登录/入盟包到达 → 无人开 UIAlliance → 数据丢失 → 开页全空」。
//
//	字段事实源：s9 ALLIANCE_OPERATION getAllianceInfo(0x0C)/updateAllianceInfo(0x0F)
//	+ getGuildAlliances(0x0D) 公会块（与 GuildPackets.getGuildInfo 同构，含成员）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	struct AllianceMember
	{
		int32_t id = 0;
		std::string name;
		int32_t job = 0;
		int32_t level = 0;
		int32_t guild_rank = 0;
		int32_t alliance_rank = 0;
		bool online = false;
		int32_t guild_id = 0;
		std::string guild_name;
	};

	struct AllianceData
	{
		int32_t id = 0;
		std::string name;
		std::string rank_titles[5];
		std::vector<int32_t> guild_ids;
		std::vector<std::string> guild_names;
		std::vector<AllianceMember> members;
		int32_t capacity = 0;
		std::string notice;

		bool in_alliance() const { return id > 0; }
	};

	class AllianceModel
	{
	public:
		static AllianceModel& get();

		const AllianceData& data() const { return alliance; }
		AllianceData& mutable_data() { return alliance; }
		void set(const AllianceData& a) { alliance = a; }
		void clear() { alliance = AllianceData{}; }

		void set_notice(const std::string& notice) { alliance.notice = notice; }
		void set_ranks(const std::vector<std::string>& ranks);
		void set_guild_names(const std::vector<std::string>& names);
		void set_member_online(int32_t cid, bool online);
		void set_member_job_level(int32_t cid, int32_t level, int32_t job);
		const AllianceMember* find(int32_t cid) const;

	private:
		AllianceData alliance;
	};
}
