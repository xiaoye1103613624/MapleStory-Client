//////////////////////////////////////////////////////////////////////////////////
//	FeatureRegistry — unified builtin UI entry ids for SideMenu bindings.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <functional>
#include <string>
#include <vector>

namespace ms
{
	namespace FeatureRegistry
	{
		struct Info
		{
			std::string id;       // stable key used by side_menu.json / Web
			std::string title;    // display name (zh fallback)
			std::string open_key; // legacy alias accepted by open()
			bool enabled = true;
		};

		// Built-in openers registered at startup (client-side implementations).
		const std::vector<Info>& builtins();

		// Open by registry id or legacy open_key / alias. Returns false if unknown.
		bool open(const std::string& id_or_key);

		// Merge enable flags from server snapshot (id -> enabled). Unknown ids ignored.
		void apply_remote_enable(const std::vector<Info>& remote);

		// Enabled builtins (after remote mask) for Web seed / debug.
		std::vector<Info> list_bindable();
	}
}
