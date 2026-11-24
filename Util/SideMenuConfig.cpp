//////////////////////////////////////////////////////////////////////////////////
//	SideMenuConfig — JSON load/save, ini migrate, builtin seed.
//////////////////////////////////////////////////////////////////////////////////
#include "SideMenuConfig.h"
#include "SideIconConfig.h"
#include "Misc.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace ms
{
	namespace SideMenuConfig
	{
		namespace
		{
			std::string trim(std::string s)
			{
				while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t'))
					s.pop_back();
				size_t i = 0;
				while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
					++i;
				return s.substr(i);
			}

			std::string lower(std::string s)
			{
				std::transform(s.begin(), s.end(), s.begin(),
					[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
				return s;
			}

			std::string json_escape(const std::string& s)
			{
				std::string o;
				o.reserve(s.size() + 8);
				for (char c : s)
				{
					switch (c)
					{
					case '"': o += "\\\""; break;
					case '\\': o += "\\\\"; break;
					case '\n': o += "\\n"; break;
					case '\r': o += "\\r"; break;
					case '\t': o += "\\t"; break;
					default: o += c; break;
					}
				}
				return o;
			}

			// Minimal JSON scanner for our schema (object/array/string/number/bool/null).
			struct JsonVal
			{
				enum Type { Null, Bool, Number, String, Array, Object } type = Null;
				bool b = false;
				double n = 0;
				std::string s;
				std::vector<JsonVal> a;
				std::unordered_map<std::string, JsonVal> o;
			};

			struct JsonParser
			{
				const std::string& src;
				size_t i = 0;

				explicit JsonParser(const std::string& s) : src(s) {}

				void skip_ws()
				{
					while (i < src.size() && (src[i] == ' ' || src[i] == '\t'
						|| src[i] == '\r' || src[i] == '\n'))
						++i;
				}

				bool match(char c)
				{
					skip_ws();
					if (i < src.size() && src[i] == c)
					{
						++i;
						return true;
					}
					return false;
				}

				JsonVal parse_value()
				{
					skip_ws();
					if (i >= src.size())
						return {};
					const char c = src[i];
					if (c == '{') return parse_object();
					if (c == '[') return parse_array();
					if (c == '"') return parse_string();
					if (c == 't' || c == 'f') return parse_bool();
					if (c == 'n') { parse_null(); return {}; }
					return parse_number();
				}

				JsonVal parse_object()
				{
					JsonVal v;
					v.type = JsonVal::Object;
					match('{');
					skip_ws();
					if (match('}'))
						return v;
					while (i < src.size())
					{
						JsonVal key = parse_string();
						if (!match(':'))
							break;
						v.o[key.s] = parse_value();
						skip_ws();
						if (match('}'))
							break;
						if (!match(','))
							break;
					}
					return v;
				}

				JsonVal parse_array()
				{
					JsonVal v;
					v.type = JsonVal::Array;
					match('[');
					skip_ws();
					if (match(']'))
						return v;
					while (i < src.size())
					{
						v.a.push_back(parse_value());
						skip_ws();
						if (match(']'))
							break;
						if (!match(','))
							break;
					}
					return v;
				}

				JsonVal parse_string()
				{
					JsonVal v;
					v.type = JsonVal::String;
					skip_ws();
					if (i >= src.size() || src[i] != '"')
						return v;
					++i;
					while (i < src.size() && src[i] != '"')
					{
						if (src[i] == '\\' && i + 1 < src.size())
						{
							++i;
							char e = src[i++];
							switch (e)
							{
							case 'n': v.s += '\n'; break;
							case 'r': v.s += '\r'; break;
							case 't': v.s += '\t'; break;
							case '"': v.s += '"'; break;
							case '\\': v.s += '\\'; break;
							case 'u':
								// skip 4 hex
								for (int k = 0; k < 4 && i < src.size(); ++k) ++i;
								break;
							default: v.s += e; break;
							}
						}
						else
							v.s += src[i++];
					}
					if (i < src.size() && src[i] == '"')
						++i;
					return v;
				}

				JsonVal parse_number()
				{
					JsonVal v;
					v.type = JsonVal::Number;
					skip_ws();
					size_t start = i;
					if (i < src.size() && (src[i] == '-' || src[i] == '+'))
						++i;
					while (i < src.size() && (std::isdigit(static_cast<unsigned char>(src[i])) || src[i] == '.'))
						++i;
					try { v.n = std::stod(src.substr(start, i - start)); }
					catch (...) { v.n = 0; }
					return v;
				}

				JsonVal parse_bool()
				{
					JsonVal v;
					v.type = JsonVal::Bool;
					skip_ws();
					if (src.compare(i, 4, "true") == 0)
					{
						v.b = true;
						i += 4;
					}
					else if (src.compare(i, 5, "false") == 0)
					{
						v.b = false;
						i += 5;
					}
					return v;
				}

				void parse_null()
				{
					if (src.compare(i, 4, "null") == 0)
						i += 4;
				}
			};

			const JsonVal* obj_get(const JsonVal& o, const char* key)
			{
				if (o.type != JsonVal::Object)
					return nullptr;
				auto it = o.o.find(key);
				return it == o.o.end() ? nullptr : &it->second;
			}

			std::string as_str(const JsonVal* v, const std::string& def = {})
			{
				if (!v) return def;
				if (v->type == JsonVal::String) return v->s;
				if (v->type == JsonVal::Number)
				{
					std::ostringstream os;
					os << static_cast<long long>(v->n);
					return os.str();
				}
				return def;
			}

			bool as_bool(const JsonVal* v, bool def = true)
			{
				if (!v) return def;
				if (v->type == JsonVal::Bool) return v->b;
				if (v->type == JsonVal::Number) return v->n != 0;
				if (v->type == JsonVal::String)
				{
					const std::string s = lower(v->s);
					return s == "1" || s == "true";
				}
				return def;
			}

			int32_t as_i32(const JsonVal* v, int32_t def = 0)
			{
				if (!v) return def;
				if (v->type == JsonVal::Number) return static_cast<int32_t>(v->n);
				if (v->type == JsonVal::String)
				{
					try { return std::stoi(v->s); }
					catch (...) { return def; }
				}
				return def;
			}

			int64_t as_i64(const JsonVal* v, int64_t def = 0)
			{
				if (!v) return def;
				if (v->type == JsonVal::Number) return static_cast<int64_t>(v->n);
				if (v->type == JsonVal::String)
				{
					try { return std::stoll(v->s); }
					catch (...) { return def; }
				}
				return def;
			}

			bool parse_document(const std::string& text, Document& out)
			{
				JsonParser p(text);
				JsonVal root = p.parse_value();
				if (root.type != JsonVal::Object)
					return false;

				out = Document{};
				out.schema_version = as_i32(obj_get(root, "schemaVersion"), 1);
				out.revision = as_i64(obj_get(root, "revision"), 0);

				if (const JsonVal* feats = obj_get(root, "features"))
				{
					if (feats->type == JsonVal::Array)
					{
						for (const auto& f : feats->a)
						{
							if (f.type != JsonVal::Object) continue;
							FeatureSnap snap;
							snap.id = as_str(obj_get(f, "id"));
							snap.title = as_str(obj_get(f, "title"));
							snap.open_key = as_str(obj_get(f, "openKey"));
							snap.enabled = as_bool(obj_get(f, "enabled"), true);
							if (!snap.id.empty())
								out.features.push_back(std::move(snap));
						}
					}
				}

				if (const JsonVal* icons = obj_get(root, "icons"))
				{
					if (icons->type == JsonVal::Array)
					{
						for (const auto& ic : icons->a)
						{
							if (ic.type == JsonVal::String)
								out.icons.push_back(ic.s);
							else if (ic.type == JsonVal::Object)
							{
								std::string k = as_str(obj_get(ic, "key"));
								if (!k.empty())
									out.icons.push_back(std::move(k));
							}
						}
					}
				}

				if (const JsonVal* items = obj_get(root, "items"))
				{
					if (items->type == JsonVal::Array)
					{
						for (const auto& it : items->a)
						{
							if (it.type != JsonVal::Object) continue;
							Item item;
							item.id = as_str(obj_get(it, "id"));
							item.parent_id = as_str(obj_get(it, "parentId"));
							item.kind = parse_kind(as_str(obj_get(it, "kind"), "leaf"));
							item.enabled = as_bool(obj_get(it, "enabled"), true);
							item.title = as_str(obj_get(it, "title"));
							item.icon_key = as_str(obj_get(it, "iconKey"));
							item.order = as_i32(obj_get(it, "order"), as_i32(obj_get(it, "sort_order"), 0));
							item.action = parse_action(as_str(obj_get(it, "action"), "Builtin"));
							item.arg = as_str(obj_get(it, "arg"));
							if (!item.id.empty())
								out.items.push_back(std::move(item));
						}
					}
				}
				return !out.items.empty() || !out.features.empty();
			}

			void sort_items(std::vector<Item>& items)
			{
				std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
					if (a.order != b.order)
						return a.order < b.order;
					return a.id < b.id;
				});
			}
		}

		Kind parse_kind(const std::string& s)
		{
			const std::string v = lower(s);
			if (v == "folder" || v == "dir" || v == "group")
				return Kind::Folder;
			return Kind::Leaf;
		}

		Action parse_action(const std::string& s)
		{
			const std::string v = lower(s);
			if (v == "npcscript" || v == "npc")
				return Action::NpcScript;
			if (v == "chatcmd" || v == "chat")
				return Action::ChatCmd;
			return Action::Builtin;
		}

		const char* kind_name(Kind k)
		{
			return k == Kind::Folder ? "folder" : "leaf";
		}

		const char* action_name(Action a)
		{
			switch (a)
			{
			case Action::NpcScript: return "NpcScript";
			case Action::ChatCmd: return "ChatCmd";
			default: return "Builtin";
			}
		}

		Document builtin_seed()
		{
			Document d;
			d.schema_version = 1;
			d.revision = 1;
			d.icons = {
				"GalaxyStar", "EventList", "Daily", "dailyGift", "Content",
				"attendance", "Achieve", "collection", "mapleHelper", "Xmas"
			};

			for (const auto& f : FeatureRegistry::builtins())
			{
				d.features.push_back({ f.id, f.title, f.open_key, f.enabled });
			}

			auto folder = [&](const char* id, const char* title, const char* icon, int order) {
				Item it;
				it.id = id;
				it.kind = Kind::Folder;
				it.title = title;
				it.icon_key = icon;
				it.order = order;
				it.enabled = true;
				d.items.push_back(std::move(it));
			};
			auto leaf = [&](const char* id, const char* parent, const char* title,
				const char* icon, const char* feature, int order) {
				Item it;
				it.id = id;
				it.parent_id = parent ? parent : "";
				it.kind = Kind::Leaf;
				it.title = title;
				it.icon_key = icon;
				it.order = order;
				it.enabled = true;
				it.action = Action::Builtin;
				it.arg = feature;
				d.items.push_back(std::move(it));
			};

			// Root folders
			folder("folder_daily", u8"日常", "Daily", 10);
			folder("folder_combat", u8"战斗", "GalaxyStar", 20);
			folder("folder_event", u8"活动", "dailyGift", 30);
			folder("folder_tools", u8"工具", "Content", 40);

			// Root leaf (direct on bar) — quest log
			leaf("leaf_quest", nullptr, u8"任务", "EventList", "questlog", 5);

			// 日常
			leaf("leaf_checkin", "folder_daily", u8"每日签到", "attendance", "dailycheckin", 10);
			leaf("leaf_dailygift", "folder_daily", u8"每日礼物", "dailyGift", "dailygift", 20);
			leaf("leaf_event", "folder_daily", u8"活动", "EventList", "event", 30);
			leaf("leaf_medal", "folder_daily", u8"勋章", "Achieve", "medal", 40);

			// 战斗
			leaf("leaf_dpt", "folder_combat", u8"伤害统计", "GalaxyStar", "damagerank", 10);
			leaf("leaf_witch", "folder_combat", u8"魔女森林", "Absolute", "witchforest", 20);
			leaf("leaf_breakthrough", "folder_combat", u8"突破石", "collection", "breakthroughstone", 30);
			leaf("leaf_enhance", "folder_combat", u8"强化", "Content", "enhance", 40);

			// 活动
			leaf("leaf_cashwin", "folder_event", u8"现金商店窗", "19thEvent", "windowcashshop", 10);
			leaf("leaf_emoticon", "folder_event", u8"聊天表情", "13thBalloon", "chatemoticon", 20);
			leaf("leaf_mystic", "folder_event", u8"神秘徽章", "mapleHelper", "mysticbadge", 30);

			// 工具
			leaf("leaf_dye", "folder_tools", u8"装备染色", "mapleStyle", "colorprism", 10);
			leaf("leaf_worldmap", "folder_tools", u8"世界地图", "mapleAlarm", "worldmap", 20);
			leaf("leaf_mapinfo", "folder_tools", u8"地图信息", "itemCollection", "mapinfo", 30);
			leaf("leaf_mobbook", "folder_tools", u8"怪物卡片", "collection", "monsterbook", 40);
			leaf("leaf_maker", "folder_tools", u8"专业技术", "Content", "maker", 50);
			leaf("leaf_family", "folder_tools", u8"家族", "Achieve", "family", 60);
			leaf("leaf_alliance", "folder_tools", u8"联盟", "GalaxyStar", "alliance", 70);
			leaf("leaf_option", "folder_tools", u8"选项", "EventList", "optionmenu", 80);

			return d;
		}

		Document migrate_from_ini()
		{
			Document d = builtin_seed();
			d.items.clear();
			d.revision = 0;

			const auto legacy = SideIconConfig::load_or_default();
			int order = 10;
			for (const auto& slot : legacy.slots)
			{
				if (!slot.enabled)
					continue;
				Item it;
				it.id = slot.id.empty() ? ("migrated_" + std::to_string(order)) : slot.id;
				it.kind = Kind::Leaf;
				it.enabled = slot.enabled;
				it.title = it.id;
				it.order = order;
				order += 10;

				// Map local:book/bulb/star → icon keys
				std::string icon = slot.icon;
				if (icon.compare(0, 6, "local:") == 0)
					icon = icon.substr(6);
				const std::string li = lower(icon);
				if (li.find("book") != std::string::npos) it.icon_key = "EventList";
				else if (li.find("bulb") != std::string::npos) it.icon_key = "Daily";
				else if (li.find("star") != std::string::npos) it.icon_key = "GalaxyStar";
				else it.icon_key = "Content";

				switch (slot.action)
				{
				case SideIconConfig::Action::ChatCmd:
					it.action = Action::ChatCmd;
					it.arg = slot.arg;
					break;
				case SideIconConfig::Action::NpcScript:
					it.action = Action::NpcScript;
					it.arg = slot.arg;
					break;
				default:
					it.action = Action::Builtin;
					it.arg = lower(slot.arg);
					break;
				}
				d.items.push_back(std::move(it));
			}
			if (d.items.empty())
				return builtin_seed();
			return d;
		}

		bool save(const Document& doc)
		{
			std::ofstream out(FILENAME, std::ios::binary | std::ios::trunc);
			if (!out)
			{
				LOG(LOG_WARN, "SideMenuConfig: cannot write " << FILENAME);
				return false;
			}

			out << "{\n";
			out << "  \"schemaVersion\": " << doc.schema_version << ",\n";
			out << "  \"revision\": " << doc.revision << ",\n";
			out << "  \"features\": [\n";
			for (size_t i = 0; i < doc.features.size(); ++i)
			{
				const auto& f = doc.features[i];
				out << "    {\"id\":\"" << json_escape(f.id)
					<< "\",\"title\":\"" << json_escape(f.title)
					<< "\",\"openKey\":\"" << json_escape(f.open_key)
					<< "\",\"enabled\":" << (f.enabled ? "true" : "false") << "}";
				out << (i + 1 < doc.features.size() ? ",\n" : "\n");
			}
			out << "  ],\n";
			out << "  \"icons\": [\n";
			for (size_t i = 0; i < doc.icons.size(); ++i)
			{
				out << "    {\"key\":\"" << json_escape(doc.icons[i]) << "\"}";
				out << (i + 1 < doc.icons.size() ? ",\n" : "\n");
			}
			out << "  ],\n";
			out << "  \"items\": [\n";
			for (size_t i = 0; i < doc.items.size(); ++i)
			{
				const auto& it = doc.items[i];
				out << "    {\"id\":\"" << json_escape(it.id)
					<< "\",\"parentId\":" << (it.parent_id.empty() ? "null" : ("\"" + json_escape(it.parent_id) + "\""))
					<< ",\"kind\":\"" << kind_name(it.kind)
					<< "\",\"enabled\":" << (it.enabled ? "true" : "false")
					<< ",\"title\":\"" << json_escape(it.title)
					<< "\",\"iconKey\":\"" << json_escape(it.icon_key)
					<< "\",\"order\":" << it.order
					<< ",\"action\":\"" << action_name(it.action)
					<< "\",\"arg\":\"" << json_escape(it.arg) << "\"}";
				out << (i + 1 < doc.items.size() ? ",\n" : "\n");
			}
			out << "  ]\n";
			out << "}\n";
			return true;
		}

		Document load_or_default()
		{
			std::ifstream in(FILENAME, std::ios::binary);
			if (in)
			{
				std::ostringstream ss;
				ss << in.rdbuf();
				Document doc;
				if (parse_document(ss.str(), doc))
				{
					std::vector<FeatureRegistry::Info> remote;
					for (const auto& f : doc.features)
						remote.push_back({ f.id, f.title, f.open_key, f.enabled });
					FeatureRegistry::apply_remote_enable(remote);
					return doc;
				}
				LOG(LOG_WARN, "SideMenuConfig: parse failed, falling back");
			}

			std::ifstream ini(LEGACY_INI);
			if (ini.good())
			{
				Document migrated = migrate_from_ini();
				save(migrated);
				LOG(LOG_INFO, "SideMenuConfig: migrated from " << LEGACY_INI);
				return migrated;
			}

			Document seed = builtin_seed();
			save(seed);
			LOG(LOG_INFO, "SideMenuConfig: wrote builtin seed " << FILENAME);
			return seed;
		}

		bool apply_remote_json(const std::string& json_utf8)
		{
			Document incoming;
			if (!parse_document(json_utf8, incoming))
			{
				LOG(LOG_WARN, "SideMenuConfig: remote JSON parse failed");
				return false;
			}

			Document current;
			std::ifstream in(FILENAME, std::ios::binary);
			if (in)
			{
				std::ostringstream ss;
				ss << in.rdbuf();
				parse_document(ss.str(), current);
			}

			if (incoming.revision > 0 && incoming.revision <= current.revision)
			{
				LOG(LOG_INFO, "SideMenuConfig: skip remote revision=" << incoming.revision
					<< " local=" << current.revision);
				return false;
			}

			if (!save(incoming))
				return false;

			std::vector<FeatureRegistry::Info> remote;
			for (const auto& f : incoming.features)
				remote.push_back({ f.id, f.title, f.open_key, f.enabled });
			FeatureRegistry::apply_remote_enable(remote);
			LOG(LOG_INFO, "SideMenuConfig: applied remote revision=" << incoming.revision
				<< " items=" << incoming.items.size());
			return true;
		}

		std::vector<Item> root_items(const Document& doc)
		{
			std::vector<Item> out;
			for (const auto& it : doc.items)
			{
				if (it.enabled && it.parent_id.empty())
					out.push_back(it);
			}
			sort_items(out);
			return out;
		}

		std::vector<Item> children_of(const Document& doc, const std::string& folder_id)
		{
			std::vector<Item> out;
			for (const auto& it : doc.items)
			{
				if (it.enabled && it.parent_id == folder_id && it.kind == Kind::Leaf)
					out.push_back(it);
			}
			sort_items(out);
			return out;
		}
	}
}
