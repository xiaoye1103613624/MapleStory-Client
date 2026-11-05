//////////////////////////////////////////////////////////////////////////////////
//	QuestData — cached quest name / summary / start requirements from String+Quest
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Cache.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	class QuestLog;

	class QuestData : public Cache<QuestData>
	{
	public:
		bool is_valid() const;
		explicit operator bool() const;

		int16_t get_id() const;
		const std::string& get_name() const;
		const std::string& get_summary() const;
		// QuestInfo.img/<id>/{0,1,2…} 分状态摘要；tab 0/1/2 对应可接/进行/完成。
		const std::string& get_summary_for_tab(uint16_t tab) const;

		int16_t get_min_level() const;
		int16_t get_max_level() const;
		int32_t get_npc() const;
		int32_t get_area() const;
		const std::vector<int16_t>& get_jobs() const;
		const std::vector<int16_t>& get_prequests() const;

		// 连锁任务：QuestInfo parent + order（IDA sub_83EA05 / sub_83FEC6，"%s ( %d / %d )"）。
		const std::string& get_chain_parent() const;
		int16_t get_chain_order() const;
		int16_t get_chain_total() const;
		bool is_chain_quest() const;

		// True when player level/job/prequests allow accepting (ignores started/completed).
		bool meets_requirements(int16_t level, uint16_t job, const QuestLog& log) const;

		// Scan Quest/Check.img for quests the player can accept right now.
		static std::vector<int16_t> list_available(int16_t level, uint16_t job, const QuestLog& log);

	private:
		friend Cache<QuestData>;
		QuestData(int32_t questid);

		int16_t questid;
		std::string name;
		std::string summary;
		// 分状态页原文（0=可接短述、1=详细、2=进行中…）；summary 仍是它们的换行拼接以兼容旧调用。
		std::vector<std::string> pages;
		std::string chain_parent;
		int16_t chain_order;
		int16_t min_level;
		int16_t max_level;
		int32_t npc;
		int32_t area;
		std::vector<int16_t> jobs;
		std::vector<int16_t> prequests;
		bool valid;
	};
}
