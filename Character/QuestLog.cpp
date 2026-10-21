//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "QuestLog.h"

namespace ms
{
	void QuestLog::add_started(int16_t qid, const std::string& qdata)
	{
		started[qid] = qdata;
	}

	void QuestLog::add_completed(int16_t qid, int64_t time)
	{
		completed[qid] = time;
	}

	void QuestLog::set_started(int16_t qid, const std::string& qdata)
	{
		started[qid] = qdata;
		completed.erase(qid);
	}

	void QuestLog::set_completed(int16_t qid, int64_t time)
	{
		started.erase(qid);
		completed[qid] = time;
	}

	void QuestLog::remove_started(int16_t qid)
	{
		started.erase(qid);
	}

	void QuestLog::remove_completed(int16_t qid)
	{
		completed.erase(qid);
	}

	bool QuestLog::is_started(int16_t qid) const
	{
		return started.count(qid) > 0;
	}

	bool QuestLog::is_completed(int16_t qid) const
	{
		return completed.count(qid) > 0;
	}

	int16_t QuestLog::get_last_started() const
	{
		if (started.empty())
			return 0;

		return started.rbegin()->first;
	}

	std::string QuestLog::get_progress(int16_t qid) const
	{
		auto iter = started.find(qid);

		if (iter != started.end())
			return iter->second;

		return {};
	}

	int64_t QuestLog::get_completion_time(int16_t qid) const
	{
		auto iter = completed.find(qid);

		if (iter != completed.end())
			return iter->second;

		return 0;
	}

	const std::map<int16_t, std::string>& QuestLog::get_started() const
	{
		return started;
	}

	const std::map<int16_t, int64_t>& QuestLog::get_completed() const
	{
		return completed;
	}
}
