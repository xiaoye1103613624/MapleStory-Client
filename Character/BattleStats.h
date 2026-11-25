//////////////////////////////////////////////////////////////////////////////////
// BattleStats — 战斗统计数据源（192 原生 Battle Analysis / B2 服务端归因版）
//
// 设计（对齐 Character/DamageRank.h 的单例 + mutex + 快照范式）：
//   - 服务端为权威：exp/meso/kill/damage 均由 s9 服务端按「实得」口径聚合后下发；
//   - 客户端仅负责本地计时兜底（服务端未回数据时显示战斗时间）与渲染；
//   - 队员条目表（B2 组队维度，对应原生 button:Left/Right 翻页）。
//
// 协议（与 BeiDou-Server_s9 子代理协同定稿）：
//   - 下行 0x3BC：恒 1B 许可（对齐 192 OnServerOnCalcRequestResult@0x628900）。
//   - 下行 0x3BD：subtype 0=重置清空；1=队员快照
//       int32 charId → int32 jobId → string name → int32 partyId →
//       int64 exp → int64 meso → int64 kill → int64 damage → uint16 durationSec
//   - 上行 0x3BC：3×byte(bOn, bIsNew, bClear)（对齐 192 RequestOnCalc@0x62ee00）。
// 不触碰现有 DPT/DamageRank（伤害榜保持原样）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace ms
{
	class BattleStats
	{
	public:
		// 单个角色（自己或队员）的一份统计快照。字段与 0x3BD subtype=1 一一对应。
		struct Entry
		{
			int32_t charid = 0;
			int32_t jobid = 0;
			std::string name;
			int32_t partyid = 0;       // 0=无队伍
			uint64_t exp = 0;          // 实得经验（排除任务/道具/扭蛋）
			uint64_t meso = 0;         // 实得金币（排除交易/任务）
			uint64_t kills = 0;        // 击杀怪物数（服务端归因，int64）
			uint64_t damage = 0;       // 累计伤害
			int64_t duration_ms = 0;   // 战斗时长（由服务端 uint16 秒 ×1000 换算）
		};

		static BattleStats& get()
		{
			static BattleStats inst;
			return inst;
		}

		// ── 生命周期 ────────────────────────────────────────────────────────
		// 服务端许可：1=允许开始/继续，0=拒绝或统计结束（对齐 192 m_bServerOnCalc）。
		void set_server_enabled(bool on)
		{
			std::lock_guard<std::mutex> lock(mutex);
			server_enabled = on;
			if (!on)
				local_running = false;
		}

		bool is_server_enabled() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return server_enabled;
		}

		// 本地计时兜底：无服务端数据时也能显示战斗时间。
		void set_local_running(bool running)
		{
			std::lock_guard<std::mutex> lock(mutex);
			local_running = running;
		}

		bool is_running() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return server_enabled || local_running;
		}

		// ── 数据写入（handler 调用） ─────────────────────────────────────────
		void reset()
		{
			std::lock_guard<std::mutex> lock(mutex);
			entries.clear();
		}

		// 同 charid 覆盖，避免队员条目重复膨胀（单人增量推送语义）。
		void upsert(const Entry& e)
		{
			std::lock_guard<std::mutex> lock(mutex);
			for (auto& cur : entries)
			{
				if (cur.charid == e.charid)
				{
					cur = e;
					return;
				}
			}
			entries.push_back(e);
		}

		// ── 快照（复制，调用方无锁持有） ──────────────────────────────────────
		std::vector<Entry> snapshot() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return entries;
		}

		// 按伤害降序；原生 combo:Sort 的 7 项排序在 UI 层用同一快照做二级排序。
		std::vector<Entry> snapshot_by_damage() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::vector<Entry> out = entries;
			std::sort(out.begin(), out.end(), [](const Entry& a, const Entry& b) {
				return a.damage > b.damage;
			});
			return out;
		}

		// 找自己（charid 由调用方传入；服务端快照里自己的 charid 与角色 id 一致）。
		Entry find(int32_t charid) const
		{
			std::lock_guard<std::mutex> lock(mutex);
			for (const auto& e : entries)
				if (e.charid == charid)
					return e;
			return Entry{};
		}

		bool has_data() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return !entries.empty();
		}

	private:
		BattleStats() = default;

		mutable std::mutex mutex;
		bool server_enabled = false;
		bool local_running = false;
		std::vector<Entry> entries;
	};
}
