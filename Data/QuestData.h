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

		int16_t get_min_level() const;
		int16_t get_max_level() const;
		int32_t get_npc() const;
		const std::vector<int16_t>& get_jobs() const;
		const std::vector<int16_t>& get_prequests() const;

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
		int16_t min_level;
		int16_t max_level;
		int32_t npc;
		std::vector<int16_t> jobs;
		std::vector<int16_t> prequests;
		bool valid;
	};
}
