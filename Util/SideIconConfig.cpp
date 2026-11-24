//////////////////////////////////////////////////////////////////////////////////
//	SideIconConfig — side_icons.ini loader/saver
//////////////////////////////////////////////////////////////////////////////////
#include "SideIconConfig.h"

#include "Misc.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ms
{
	namespace SideIconConfig
	{
		static std::string trim(std::string s)
		{
			while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
				s.pop_back();
			size_t i = 0;
			while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
				++i;
			return s.substr(i);
		}

		static std::string lower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		Action parse_action(const std::string& s)
		{
			const std::string v = lower(s);
			if (v == "chatcmd" || v == "chat")
				return Action::ChatCmd;
			if (v == "packet")
				return Action::Packet;
			if (v == "npcscript" || v == "npc")
				return Action::NpcScript;
			return Action::OpenUI;
		}

		const char* action_name(Action a)
		{
			switch (a)
			{
			case Action::ChatCmd: return "ChatCmd";
			case Action::Packet: return "Packet";
			case Action::NpcScript: return "NpcScript";
			default: return "OpenUI";
			}
		}

		static Table defaults()
		{
			Table t;
			t.collapsed = false;
			// 高版本左侧小图标 = MainNotice 风格书/灯/星（本地 PNG），
			// 非 StatusBar3 submenu，亦非 ShortCutUI 活动圆标冒充。
			t.slots = {
				{ "book", true, "local:book", Action::OpenUI, "QuestLog" },
				{ "bulb", true, "local:bulb", Action::OpenUI, "Event" },
				{ "star", true, "local:star", Action::OpenUI, "DailyGift" },
				// 伤害统计与每日签到入口（第16/22轮功能）；无本地图标资源，
				// 用 Basic.img/BtClaim 通用占位，可在 side_icons.ini 换成 local:/菜单图标。
				{ "damagerank", true, "Basic.img/BtClaim", Action::OpenUI, "DamageRank" },
				{ "dailycheckin", true, "Basic.img/BtClaim", Action::OpenUI, "DailyCheckin" },
				// 七彩棱镜染色窗：通用占位图标，可在 side_icons.ini 自定义。
				{ "colorprism", true, "Basic.img/BtClaim", Action::OpenUI, "ColorPrism" },
			};
			return t;
		}

		bool save(const Table& table)
		{
			std::ofstream out(FILENAME, std::ios::binary | std::ios::trunc);
			if (!out)
			{
				LOG(LOG_WARN, "SideIconConfig: cannot write " << FILENAME);
				return false;
			}

			out << "# Left-side icon dock (UISideIconBar)\n";
			out << "# action = OpenUI | ChatCmd | Packet | NpcScript\n";
			out << "# icon   = path under UI namespace, e.g. Basic.img/BtClaim\n";
			out << "collapsed=" << (table.collapsed ? "1" : "0") << "\n";

			for (size_t i = 0; i < table.slots.size(); ++i)
			{
				const Slot& s = table.slots[i];
				const std::string prefix = "slot." + std::to_string(i) + ".";
				out << prefix << "id=" << (s.id.empty() ? ("slot" + std::to_string(i)) : s.id) << "\n";
				out << prefix << "enabled=" << (s.enabled ? "1" : "0") << "\n";
				out << prefix << "icon=" << s.icon << "\n";
				out << prefix << "action=" << action_name(s.action) << "\n";
				out << prefix << "arg=" << s.arg << "\n";
			}
			return true;
		}

		void ensure_default_file()
		{
			std::ifstream in(FILENAME);
			if (in.good())
				return;
			save(defaults());
			LOG(LOG_INFO, "SideIconConfig: wrote default " << FILENAME);
		}

		Table load_or_default()
		{
			ensure_default_file();

			Table t;
			t.collapsed = false;

			std::ifstream in(FILENAME);
			if (!in)
			{
				LOG(LOG_WARN, "SideIconConfig: missing " << FILENAME << ", using defaults");
				return defaults();
			}

			std::unordered_map<int, Slot> by_index;
			std::string line;
			while (std::getline(in, line))
			{
				line = trim(line);
				if (line.empty() || line[0] == '#' || line[0] == ';')
					continue;

				const size_t eq = line.find('=');
				if (eq == std::string::npos)
					continue;

				const std::string key = lower(trim(line.substr(0, eq)));
				const std::string val = trim(line.substr(eq + 1));

				if (key == "collapsed")
				{
					t.collapsed = (val == "1" || lower(val) == "true");
					continue;
				}

				// slot.N.field
				if (key.compare(0, 5, "slot.") != 0)
					continue;

				const size_t dot2 = key.find('.', 5);
				if (dot2 == std::string::npos)
					continue;

				int idx = -1;
				try { idx = std::stoi(key.substr(5, dot2 - 5)); }
				catch (...) { continue; }
				if (idx < 0 || idx > 63)
					continue;

				Slot& slot = by_index[idx];
				if (slot.id.empty())
					slot.id = "slot" + std::to_string(idx);

				const std::string field = key.substr(dot2 + 1);
				if (field == "id")
					slot.id = val;
				else if (field == "enabled")
					slot.enabled = (val == "1" || lower(val) == "true");
				else if (field == "icon")
					slot.icon = val;
				else if (field == "action")
					slot.action = parse_action(val);
				else if (field == "arg")
					slot.arg = val;
			}

			if (by_index.empty())
				return defaults();

			std::vector<int> keys;
			keys.reserve(by_index.size());
			for (const auto& kv : by_index)
				keys.push_back(kv.first);
			std::sort(keys.begin(), keys.end());

			t.slots.reserve(keys.size());
			for (int k : keys)
				t.slots.push_back(by_index[k]);

			return t;
		}
	}
}
