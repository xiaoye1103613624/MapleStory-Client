//////////////////////////////////////////////////////////////////////////////////
//	QuestData
//////////////////////////////////////////////////////////////////////////////////
#include "QuestData.h"

#include "../Character/QuestLog.h"

// Defines USE_NX (and the rest of the global config). QuestData.h's include
// chain (Template/Cache.h) does not reach MapleStory.h, so without this the
// nl::node usages below fail to compile with "use of undeclared identifier nl".
#include "../MapleStory.h"

#include <cctype>
#include <cstdlib>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		void read_int_list(nl::node parent, const char* name, std::vector<int16_t>& out)
		{
			nl::node node = parent[name];

			if (!node)
				return;

			// Single value or directory of values
			if (node.data_type() == nl::node::type::integer || node.data_type() == nl::node::type::string)
			{
				out.push_back(static_cast<int16_t>(node.get_integer()));
				return;
			}

			for (const auto& child : node)
				out.push_back(static_cast<int16_t>(child.get_integer()));
		}

		void load_check0(nl::node check0, int16_t& min_level, int16_t& max_level, int32_t& npc,
			std::vector<int16_t>& jobs, std::vector<int16_t>& prequests)
		{
			if (!check0)
				return;

			if (check0["lvmin"])
				min_level = static_cast<int16_t>(check0["lvmin"].get_integer());

			if (check0["lvmax"])
				max_level = static_cast<int16_t>(check0["lvmax"].get_integer());

			if (check0["npc"])
				npc = static_cast<int32_t>(check0["npc"].get_integer());

			read_int_list(check0, "job", jobs);
			read_int_list(check0, "quest", prequests);

			// Alternate / nested job list forms seen in some packs
			if (jobs.empty())
				read_int_list(check0, "job0", jobs);

			// Nested quest prerequisites: quest/0/id
			if (prequests.empty())
			{
				nl::node qnode = check0["quest"];

				if (qnode)
				{
					for (const auto& child : qnode)
					{
						if (child["id"])
							prequests.push_back(static_cast<int16_t>(child["id"].get_integer()));
						else if (child.data_type() == nl::node::type::integer)
							prequests.push_back(static_cast<int16_t>(child.get_integer()));
					}
				}
			}
		}
	}

	QuestData::QuestData(int32_t id) :
	questid(static_cast<int16_t>(id)),
	min_level(0),
	max_level(0),
	npc(0),
	area(0),
	valid(false)
	{
		std::string sid = std::to_string(id);

		// Quest name + description live in Quest/QuestInfo.img/<id>. The deployed
		// data does not expose a String/Quest.img, so read from QuestInfo first.
		nl::node info = nl::nx::Quest["QuestInfo.img"][sid];

		if (info)
		{
			name = info["name"].get_string();

			if (name.empty())
				name = info.get_string();

			for (const char* key : { "0", "1", "2", "3", "4" })
			{
				nl::node line = info[key];

				if (line)
				{
					std::string s = line.get_string();

					if (!s.empty())
					{
						if (!summary.empty())
							summary += "\n";

						summary += s;
					}
				}
			}

			valid = !name.empty() || !summary.empty();

			if (info["area"])
				area = static_cast<int32_t>(info["area"].get_integer());
		}

		if (name.empty())
			name = "Quest " + sid;

		// Start requirements / NPC / level gating come from Quest/Check.img.
		nl::node qsrc = nl::nx::Quest["Check.img"][sid];

		if (!qsrc)
			qsrc = nl::nx::Quest["QuestInfo.img"][sid];

		if (qsrc)
		{
			load_check0(qsrc["0"], min_level, max_level, npc, jobs, prequests);

			// Some packs store start checks at root
			if (min_level == 0 && max_level == 0 && jobs.empty() && prequests.empty())
				load_check0(qsrc, min_level, max_level, npc, jobs, prequests);
		}

		if (!valid && (info || qsrc))
			valid = true;
	}

	bool QuestData::is_valid() const
	{
		return valid;
	}

	QuestData::operator bool() const
	{
		return is_valid();
	}

	int16_t QuestData::get_id() const
	{
		return questid;
	}

	const std::string& QuestData::get_name() const
	{
		return name;
	}

	const std::string& QuestData::get_summary() const
	{
		return summary;
	}

	int16_t QuestData::get_min_level() const
	{
		return min_level;
	}

	int16_t QuestData::get_max_level() const
	{
		return max_level;
	}

	int32_t QuestData::get_npc() const
	{
		return npc;
	}

	int32_t QuestData::get_area() const
	{
		return area;
	}

	const std::vector<int16_t>& QuestData::get_jobs() const
	{
		return jobs;
	}

	const std::vector<int16_t>& QuestData::get_prequests() const
	{
		return prequests;
	}

	bool QuestData::meets_requirements(int16_t level, uint16_t job, const QuestLog& log) const
	{
		if (!valid)
			return false;

		if (min_level > 0 && level < min_level)
			return false;

		if (max_level > 0 && level > max_level)
			return false;

		if (!jobs.empty())
		{
			bool job_ok = false;

			for (int16_t j : jobs)
			{
				if (static_cast<uint16_t>(j) == job)
				{
					job_ok = true;
					break;
				}
			}

			if (!job_ok)
				return false;
		}

		for (int16_t pq : prequests)
		{
			if (pq > 0 && !log.is_completed(pq))
				return false;
		}

		return true;
	}

	std::vector<int16_t> QuestData::list_available(int16_t level, uint16_t job, const QuestLog& log)
	{
		std::vector<int16_t> result;
		nl::node check = nl::nx::Quest["Check.img"];

		if (!check)
			return result;

		result.reserve(64);

		for (const auto& child : check)
		{
			const std::string& name = child.name();

			if (name.empty() || !std::isdigit(static_cast<unsigned char>(name[0])))
				continue;

			int16_t qid = static_cast<int16_t>(std::atoi(name.c_str()));

			if (qid <= 0)
				continue;

			if (log.is_started(qid) || log.is_completed(qid))
				continue;

			const QuestData& qdata = QuestData::get(qid);

			if (qdata.meets_requirements(level, job, log))
				result.push_back(qid);
		}

		return result;
	}
}
