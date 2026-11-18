////////////////////////////////////////////////////////////////////////////////
//	BuddyListModel — 好友会话级数据层（Angel 全流程 P0 / T1）
//
//	仿 GuildModel：083「玩家列表」好友页签读常驻模型，不依赖 UIBuddyList 浮窗
//	是否被强开。BuddyHandlers 写模型；浮窗仅已打开时同步。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	struct Buddy
	{
		int32_t cid = 0;
		std::string name;
		std::string group;
		int32_t channel = -1;
		bool online = false;
	};

	struct BuddyListData
	{
		// 083 好友上限 25（UserList 截图 0/25）；容量包可覆盖。
		int32_t capacity = 25;
		std::vector<Buddy> buddies;
	};

	class BuddyListModel
	{
	public:
		static BuddyListModel& get();

		const BuddyListData& data() const { return list; }
		BuddyListData& mutable_data() { return list; }
		void set(const BuddyListData& d) { list = d; }
		void clear() { list = BuddyListData{}; active_group.clear(); }

		void set_capacity(int32_t cap);
		void update_channel(int32_t cid, int32_t channel);
		const Buddy* find(int32_t cid) const;

		// Angel StatusBar+3180 当前好友分组名；空串匹配未分组好友（strcmp 空组）。
		const std::string& get_active_group() const { return active_group; }
		void set_active_group(const std::string& g) { active_group = g; }

	private:
		BuddyListData list;
		std::string active_group;
	};
}
