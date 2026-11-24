//////////////////////////////////////////////////////////////////////////////////
//	SideMenuConfig — side_menu.json tree (folder/leaf) with order + Feature snapshot.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FeatureRegistry.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	namespace SideMenuConfig
	{
		enum class Kind : uint8_t { Folder, Leaf };
		enum class Action : uint8_t { Builtin, NpcScript, ChatCmd };

		struct FeatureSnap
		{
			std::string id;
			std::string title;
			std::string open_key;
			bool enabled = true;
		};

		struct Item
		{
			std::string id;
			std::string parent_id;   // empty = root
			Kind kind = Kind::Leaf;
			bool enabled = true;
			std::string title;
			std::string icon_key;    // MainNotice style key; auto Default/Notify
			int32_t order = 0;
			Action action = Action::Builtin;
			std::string arg;        // feature id / "npcId" / "npcId:shop" / chat
		};

		struct Document
		{
			int schema_version = 1;
			int64_t revision = 0;
			std::vector<FeatureSnap> features;
			std::vector<std::string> icons;
			std::vector<Item> items;
		};

		inline constexpr const char* FILENAME = "side_menu.json";
		inline constexpr const char* LEGACY_INI = "side_icons.ini";

		Kind parse_kind(const std::string& s);
		Action parse_action(const std::string& s);
		const char* kind_name(Kind k);
		const char* action_name(Action a);

		// Load: JSON cache → migrate legacy ini → built-in seed. Always returns usable doc.
		Document load_or_default();
		bool save(const Document& doc);
		bool apply_remote_json(const std::string& json_utf8); // write cache if newer revision

		// Sorted helpers (stable by order, then id).
		std::vector<Item> root_items(const Document& doc);
		std::vector<Item> children_of(const Document& doc, const std::string& folder_id);

		Document builtin_seed();
		Document migrate_from_ini();
	}
}
