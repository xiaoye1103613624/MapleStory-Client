//////////////////////////////////////////////////////////////////////////////////
// DamageSkinState — 见 .h。
//////////////////////////////////////////////////////////////////////////////////
#include "DamageSkinState.h"

namespace ms
{
	namespace DamageSkinState
	{
		namespace
		{
			std::map<int32_t, int64_t> g_catalog;
			std::set<int32_t> g_owned;
			int32_t g_active = 0;
			bool g_have_result = false;
			int32_t g_res_op = 0;
			bool g_res_ok = false;
			int32_t g_res_skin = 0;
			int32_t g_res_mesos = 0;
		}

		void set_catalog(const std::map<int32_t, int64_t>& prices)
		{
			g_catalog = prices;
		}

		void set_inventory(int32_t active, const std::set<int32_t>& owned)
		{
			g_active = active;
			g_owned = owned;
			g_owned.insert(0); // 默认皮肤永远拥有
		}

		void set_result(int32_t op, bool ok, int32_t skinId, int32_t mesos)
		{
			g_have_result = true;
			g_res_op = op;
			g_res_ok = ok;
			g_res_skin = skinId;
			g_res_mesos = mesos;
		}

		const std::map<int32_t, int64_t>& catalog() { return g_catalog; }
		const std::set<int32_t>& owned() { return g_owned; }
		int32_t active() { return g_active; }
		bool empty() { return g_catalog.empty() && g_owned.size() <= 1; }

		bool take_last_result(int32_t& op, bool& ok, int32_t& skinId, int32_t& mesos)
		{
			if (!g_have_result) return false;
			op = g_res_op; ok = g_res_ok; skinId = g_res_skin; mesos = g_res_mesos;
			g_have_result = false;
			return true;
		}
	}
}
