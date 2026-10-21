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
#pragma once

#include <map>
#include <string>

namespace ms
{
	// Class that stores information on the quest log of an individual character.
	// "Started" = in-progress; completed times are Unix-style timestamps from the server.
	class QuestLog
	{
	public:
		void add_started(int16_t qid, const std::string& quest_data);
		void add_completed(int16_t qid, int64_t time);

		void set_started(int16_t qid, const std::string& quest_data);
		void set_completed(int16_t qid, int64_t time);
		void remove_started(int16_t qid);
		void remove_completed(int16_t qid);

		bool is_started(int16_t qid) const;
		bool is_completed(int16_t qid) const;
		int16_t get_last_started() const;
		std::string get_progress(int16_t qid) const;
		int64_t get_completion_time(int16_t qid) const;

		const std::map<int16_t, std::string>& get_started() const;
		const std::map<int16_t, int64_t>& get_completed() const;

	private:
		std::map<int16_t, std::string> started;
		std::map<int16_t, int64_t> completed;
	};
}
