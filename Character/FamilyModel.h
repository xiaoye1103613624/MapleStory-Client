////////////////////////////////////////////////////////////////////////////////
//	FamilyModel — 学院（Family 系统）会话级数据层
//
//	为什么存在（2026-11-07，学院窗 UI 重做同轮）：
//	 特权定义（FAMILY_PRIVILEGE_LIST=100，s9 PacketCreator.loadFamily）与
//	 使用计数（FAMILY_INFO_RESULT=95 尾部 ordinal/used 对）在**登录时**就下发，
//	 而 R28-B2 之后 handler 不再为回包开窗。窗必须从常驻数据层取，
//	 否则「按 F 开窗 → 面板全空，直到服务端再发一次」。
//	 同理名声/同学数也先落模型。
//
//	字段序事实源（逐行核对 s9）：
//	  getFamilyInfo (PacketCreator.java:5992)：
//	    int curRep / int totRep / int todayRep / short juniors / short 2 /
//	    short 0 / int leaderId / string name / string message /
//	    int count / count × (int ordinal + int used)
//	  loadFamily (PacketCreator.java:5932)：
//	    int count / count × (byte type(≤1→1 否则2) + int repCost +
//	    int usageLimit + string name + string desc)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	struct FamilyInfoData
	{
		int32_t cur_rep = 0;
		int32_t tot_rep = 0;
		int32_t today_rep = 0;
		int16_t juniors = 0;
		int16_t juniors_max = 2;
		int32_t leader_id = 0;
		std::string name;
		std::string message;
		// s9 getFamilyInfo 尾段：ordinal → 是否已用（usageLimit 恒 1，0/1 显示）。
		// 下标 = FamilyEntitlement.ordinal（0..10）。
		std::vector<uint8_t> used;

		bool has_family() const { return !name.empty(); }
	};

	/// s9 FamilyEntitlement 的 11 条定义（loadFamily 下发）。
	struct FamilyPrivilegeDef
	{
		int32_t ordinal = 0;
		int32_t rep_cost = 0;
		int32_t usage_limit = 1;
		std::string name;
		std::string desc;
	};

	class FamilyModel
	{
	public:
		static FamilyModel& get();

		void set_info(const FamilyInfoData& data);
		void set_privileges(const std::vector<FamilyPrivilegeDef>& defs);
		/// FAMILY_REP_GAIN：名声即时增减（s9 在 repgain 后也会重发 getFamilyInfo，
		/// 这里只做乐观加，真值以 info 包为准覆盖）。
		void add_rep(int32_t gain);

		const FamilyInfoData& info() const { return data; }
		const std::vector<FamilyPrivilegeDef>& privileges() const { return defs; }
		bool is_used(int32_t ordinal) const
		{
			return ordinal >= 0 && ordinal < (int32_t)data.used.size() && data.used[ordinal] != 0;
		}

	private:
		FamilyInfoData data;
		std::vector<FamilyPrivilegeDef> defs;
	};
}
