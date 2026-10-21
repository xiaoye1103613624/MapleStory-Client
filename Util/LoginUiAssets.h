//////////////////////////////////////////////////////////////////////////////////
//	Login UI asset probes for modern (RaceSelect_new / CustomizeChar) vs classic
//	BeiDou (RaceSelect / NewChar*) dual-path create-character flow.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

#include <nlnx/node.hpp>

namespace ms
{
	namespace LoginUiAssets
	{
		/// True when Login.img/RaceSelect_new has a usable order list (modern Journey UI).
		bool has_race_select_new();

		/// True when Login.img/CustomizeChar exists (modern creation board).
		bool has_customize_char();

		/// Classic path when modern race-select assets are missing.
		bool use_classic_race_select();

		/// Classic creation when CustomizeChar is missing.
		bool use_classic_creation();

		/// NewChar / NewCharKnight / NewCharAran root for classType "000"/"1000"/"2000".
		std::string classic_newchar_root(const std::string& classType);

		/// Map/Obj/login.img/NewChar/signboard index (0 explorer, 1 knight, 2 aran).
		int classic_signboard_index(const std::string& classType);

		/// MakeCharInfo male/female roots for classic pools (Info / Premium / Orient).
		nl::node classic_makechar_male(const std::string& classType);
		nl::node classic_makechar_female(const std::string& classType);

		/// Append a line to _create_char.log (Release-visible node log).
		void log_info(const std::string& message);
		void log_error(const std::string& message);
	}
}
