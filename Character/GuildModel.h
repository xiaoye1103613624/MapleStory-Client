////////////////////////////////////////////////////////////////////////////////
//	GuildModel — 家族（Guild 系统）会话级数据层
//
//	为什么存在（2026-11-07 家族页切片）：
//	 083 原版「家族」不是独立浮窗，而是「玩家列表 & 家族」共用窗（UIUserList）
//	 第 3 页签（好友/组员/家族/家族联盟/拒绝列表，原生截图为证）。
//	 数据原先只活在 UIGuild 浮窗实例里；R28-B2 之后 handler 不再为回包开窗，
//	 共用窗页签必须从常驻模型取数，否则「登录时 showGuildInfo 到达 → 无人开窗
//	 → 数据丢失 → 按 G 开页全空」。
//	 UIGuild 浮窗（状态栏钮入口）保留，同样改读模型。
//
//	数据事实源：s9 GuildPackets.showGuildInfo（mode 0x1A）字段序已在
//	 GuildHandlers.cpp case 0x1A 逐字段核对（id/name/5 rankTitles/members…）。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	struct GuildMember
	{
		int32_t id;
		std::string name;
		int32_t job;
		int32_t level;
		int32_t rank;
		bool online;
		int32_t allianceRank;
	};

	struct GuildData
	{
		int32_t id = 0;
		std::string name;
		std::string rankTitles[5];
		std::vector<GuildMember> members;
		int32_t capacity = 0;
		int32_t gp = 0;
		int32_t allianceId = 0;
		std::string notice;

		bool in_guild() const { return id > 0 && !name.empty(); }
	};

	class GuildModel
	{
	public:
		static GuildModel& get();

		const GuildData& data() const { return guild; }
		GuildData& mutable_data() { return guild; }
		void set(const GuildData& g) { guild = g; }
		void clear() { guild = GuildData{}; }

		// 增量维护（handler 用；找不到 id 时静默，等全量 0x1A 覆盖）
		void add_member(const GuildMember& m);
		void remove_member(int32_t id);
		void set_member_rank(int32_t id, int32_t rank);
		void set_member_online(int32_t id, bool online);
		const GuildMember* find(int32_t id) const;

	private:
		GuildData guild;
	};
}
