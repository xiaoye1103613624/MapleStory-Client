//////////////////////////////////////////////////////////////////////////////////
//	装备成长 companion 数据：本地 WZ info/level + 服务端 0x17B 缓存
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ms
{
	class EquipGrowthData
	{
	public:
		static constexpr int BONUS_SLOTS = 15;

		struct TipEntry
		{
			int32_t item_id = 0;
			int32_t item_level = 0;
			int32_t scroll = 0;
			std::string text;			// UTF-8 行文（\n 分隔）
			std::array<int16_t, BONUS_SLOTS> bonus{};
			bool has_bonus = false;
			std::array<int16_t, BONUS_SLOTS> flame{};
			bool has_flame = false;
			bool resolved = false;
			bool from_server = false;
			bool local_only = false;
		};

		/// 本地 WZ 构图；无成长树返回 false。
		static bool build_local(int32_t item_id, int32_t item_level, int32_t scroll, TipEntry& out);

		/// 服务端 0x17B 写入缓存。
		static void set_server_tip(int32_t item_id, bool has_data, const std::string& text,
			const int16_t* bonus, bool has_bonus, const int16_t* flame, bool has_flame,
			int32_t item_level, int32_t scroll);

		static bool try_get(int32_t item_id, TipEntry& out);
		static bool has_usable(int32_t item_id);

		/// 悬停请求去重：无可用缓存且未 pending 时返回 true（调用方应发包）。
		static bool begin_request(int32_t item_id);
		static void clear_pending(int32_t item_id);
		static void invalidate(int32_t item_id);
		static void clear_all();

	private:
		static std::unordered_map<int32_t, TipEntry> cache;
		static std::unordered_map<int32_t, int> request_attempts;
		static std::unordered_map<int32_t, bool> pending;
	};
}
