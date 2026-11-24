//////////////////////////////////////////////////////////////////////////////////
// DamageSkinState — 伤害皮肤 picker 的客户端数据源（catalog/owned/active）。
// 来自服务端 0x3746/0x3747/0x3748 下发；不发假数据，空态由 picker 显示提示。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <map>
#include <set>

namespace ms
{
	namespace DamageSkinState
	{
		struct Entry
		{
			int32_t skinId = 0;
			int64_t price = 0;
		};

		void set_catalog(const std::map<int32_t, int64_t>& prices);
		void set_inventory(int32_t active, const std::set<int32_t>& owned);
		void set_result(int32_t op, bool ok, int32_t skinId, int32_t mesos);

		const std::map<int32_t, int64_t>& catalog();
		const std::set<int32_t>& owned();
		int32_t active();
		bool empty();

		/** 最近一次购买/应用结果，供 picker 弹提示；消费后清 ok 标记。 */
		bool take_last_result(int32_t& op, bool& ok, int32_t& skinId, int32_t& mesos);
	}
}
