//////////////////////////////////////////////////////////////////////////////////
//	Config-driven left side icon dock (UISideIconBar).
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	namespace SideIconConfig
	{
		enum class Action : uint8_t
		{
			OpenUI,
			ChatCmd,
			Packet,
			NpcScript
		};

		struct Slot
		{
			std::string id;
			bool enabled = true;
			std::string icon;   // WZ path under UI, e.g. "Basic.img/BtClaim"
			Action action = Action::OpenUI;
			std::string arg;    // window type name / chat text / opcode / script
		};

		struct Table
		{
			bool collapsed = false;
			std::vector<Slot> slots;
		};

		// File next to Settings: side_icons.ini
		inline constexpr const char* FILENAME = "side_icons.ini";

		Action parse_action(const std::string& s);
		const char* action_name(Action a);

		// Load table; if missing, write defaults then load.
		Table load_or_default();
		bool save(const Table& table);
		void ensure_default_file();
	}
}
