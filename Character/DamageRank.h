//////////////////////////////////////////////////////////////////////////////////
// DamageRank — 个人伤害统计数据源（原生移植自 Kaentake/Tengutake uiDamageRank）
//
// 协议来源：BeiDou-Server_s9 PacketCreator.dptPlayerUpdate/dptSkillUpdate
//   (util/PacketCreator.java:347-368)，经 DptTrackerHandler 解析后写入本结构。
// 设计：进程级单例，std::map 按 id 索引 upsert；Snapshot* 返回按总伤降序的副本，
//   供 UIDamageRank 渲染。纯数据、无 UI/资源依赖，便于单测。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace ms
{
	class DamageRank
	{
	public:
		struct PlayerEntry
		{
			int32_t char_id = 0;
			std::string name;
			int32_t job = 0;
			uint64_t total_damage = 0;
		};

		struct SkillEntry
		{
			int32_t skill_id = 0;
			std::string skill_name;
			uint64_t delta_damage = 0;
			uint64_t total_damage = 0;
			uint64_t max_damage = 0;
			uint64_t min_damage = 0;
			int32_t count = 0;
		};

		static DamageRank& get()
		{
			static DamageRank inst;
			return inst;
		}

		// 服务端 mode 0：清空全部统计（damageRankReset）。
		void reset()
		{
			std::lock_guard<std::mutex> lock(mutex);
			players.clear();
			skills.clear();
		}

		// 服务端 mode 1：玩家总伤。同 id 覆盖，避免重复条目。
		void upsert_player(int32_t char_id, const std::string& name, int32_t job, uint64_t total_damage)
		{
			std::lock_guard<std::mutex> lock(mutex);
			PlayerEntry& e = players[char_id];
			e.char_id = char_id;
			e.name = name;
			e.job = job;
			e.total_damage = total_damage;
		}

		// 服务端 mode 2：技能明细。同 id 覆盖（服务端下发的是累计值）。
		void upsert_skill(
			int32_t skill_id,
			const std::string& skill_name,
			uint64_t delta_damage,
			uint64_t total_damage,
			uint64_t max_damage,
			uint64_t min_damage,
			int32_t count)
		{
			std::lock_guard<std::mutex> lock(mutex);
			SkillEntry& e = skills[skill_id];
			e.skill_id = skill_id;
			e.skill_name = skill_name;
			e.delta_damage = delta_damage;
			e.total_damage = total_damage;
			e.max_damage = max_damage;
			e.min_damage = min_damage;
			e.count = count;
		}

		// 按总伤降序的快照（复制，调用方无锁持有）。
		std::vector<PlayerEntry> snapshot_players() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::vector<PlayerEntry> out;
			out.reserve(players.size());
			for (const auto& kv : players)
				out.push_back(kv.second);
			std::sort(out.begin(), out.end(), [](const PlayerEntry& a, const PlayerEntry& b) {
				return a.total_damage > b.total_damage;
			});
			return out;
		}

		std::vector<SkillEntry> snapshot_skills() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::vector<SkillEntry> out;
			out.reserve(skills.size());
			for (const auto& kv : skills)
				out.push_back(kv.second);
			std::sort(out.begin(), out.end(), [](const SkillEntry& a, const SkillEntry& b) {
				return a.total_damage > b.total_damage;
			});
			return out;
		}

		size_t player_count() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return players.size();
		}

		size_t skill_count() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return skills.size();
		}

	private:
		DamageRank() = default;
		DamageRank(const DamageRank&) = delete;
		DamageRank& operator=(const DamageRank&) = delete;

		mutable std::mutex mutex;
		std::map<int32_t, PlayerEntry> players;
		std::map<int32_t, SkillEntry> skills;
	};
}
