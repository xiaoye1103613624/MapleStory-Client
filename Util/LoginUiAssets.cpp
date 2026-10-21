//////////////////////////////////////////////////////////////////////////////////
//	Login UI asset probes for create-character dual-path.
//////////////////////////////////////////////////////////////////////////////////
// MapleStory.h defines USE_NX / backend switches — must come before nx headers.
#include "../MapleStory.h"

#include "LoginUiAssets.h"

#include <chrono>
#include <fstream>
#include <mutex>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace LoginUiAssets
	{
		namespace
		{
			void append_log(const char* level, const std::string& message)
			{
				static std::mutex mtx;
				std::lock_guard<std::mutex> lock(mtx);

				std::ofstream out("_create_char.log", std::ios::app);
				if (!out)
					return;

				const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				out << "[" << level << "] " << message << "\n";
				(void)now;
			}
		}

		bool has_race_select_new()
		{
#ifdef USE_NX
			nl::node order = nl::nx::UI["Login.img"]["RaceSelect_new"]["order"];
			return static_cast<bool>(order) && order.size() > 0;
#else
			return false;
#endif
		}

		bool has_customize_char()
		{
#ifdef USE_NX
			nl::node node = nl::nx::UI["Login.img"]["CustomizeChar"];
			return static_cast<bool>(node) && node.size() > 0;
#else
			return false;
#endif
		}

		bool use_classic_race_select()
		{
			return !has_race_select_new();
		}

		bool use_classic_creation()
		{
			return !has_customize_char();
		}

		std::string classic_newchar_root(const std::string& classType)
		{
			if (classType == "1000")
				return "NewCharKnight";
			if (classType == "2000")
				return "NewCharAran";
			return "NewChar";
		}

		int classic_signboard_index(const std::string& classType)
		{
			if (classType == "1000")
				return 1;
			if (classType == "2000")
				return 2;
			return 0;
		}

		nl::node classic_makechar_male(const std::string& classType)
		{
#ifdef USE_NX
			nl::node root = nl::nx::Etc["MakeCharInfo.img"];
			// Explorer: Info/Char*; Cygnus: PremiumChar*; Aran: OrientChar* (polearm pool).
			if (classType == "1000")
				return root["PremiumCharMale"];
			if (classType == "2000")
				return root["OrientCharMale"];
			return root["Info"]["CharMale"];
#else
			return {};
#endif
		}

		nl::node classic_makechar_female(const std::string& classType)
		{
#ifdef USE_NX
			nl::node root = nl::nx::Etc["MakeCharInfo.img"];
			if (classType == "1000")
				return root["PremiumCharFemale"];
			if (classType == "2000")
				return root["OrientCharFemale"];
			return root["Info"]["CharFemale"];
#else
			return {};
#endif
		}

		void log_info(const std::string& message)
		{
			append_log("INFO", message);
		}

		void log_error(const std::string& message)
		{
			append_log("ERROR", message);
		}
	}
}
