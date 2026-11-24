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
#include "Misc.h"
#include "MobElemAttr.h"
#include "Locale.h"

#include "Resource/ImgSource.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <algorithm>
#include <locale>
#include <map>
#include <sstream>
#include <unordered_set>

namespace ms
{
	namespace string_format
	{
		std::string format_exp_compact(int64_t n)
		{
			if (n < 0)
				n = 0;

			// 亿（1e8）：保留两位小数 + 中文单位
			if (n >= 100000000LL)
			{
				char buf[32];
				std::snprintf(buf, sizeof(buf), "%.2f", static_cast<double>(n) / 100000000.0);
				return Locale::format("misc.billion", buf);
			}
			// 万（1e4）
			if (n >= 10000LL)
			{
				char buf[32];
				std::snprintf(buf, sizeof(buf), "%.2f", static_cast<double>(n) / 10000.0);
				return Locale::format("misc.ten_thousand", buf);
			}
			return std::to_string(n);
		}

		void split_number(std::string& input)
		{
			for (size_t i = input.size(); i > 3; i -= 3)
				input.insert(i - 3, 1, ',');
		}

		std::string extend_id(int32_t id, size_t length)
		{
			return pad_string(std::to_string(id), length);
		}

		std::string skill_namespace(int32_t skillid)
		{
			// 末尾补 0 到 3 位：新手（Beginner）技能 id 本身就是 4 位（0001000 Three Snails
			// 按整数存成 1000），除以 10000 得到 0，不补零会去找不存在的 "0.img"。
			return pad_string(std::to_string(skillid / 10000), 3);
		}

		std::string pad_string(std::string str, size_t length)
		{
			if (str.size() < length)
				str.insert(0, length - str.size(), '0');

			return str;
		}

		void format_with_ellipsis(Text& input, int16_t width, uint16_t ellipsis_width, bool word_break)
		{
			if (input.get_text().empty() || width == 0 || input.width() <= width)
				return;

			uint16_t i = 0;
			std::string ellipsis;
			std::string text = input.get_text();

			while (i < ellipsis_width)
			{
				ellipsis += ".";
				i++;
			}

			while (input.width() > width)
			{
				if (word_break)
				{
					size_t space = text.find_last_of(' ');

					if (space != std::string::npos)
						text = text.substr(0, space + 1);

					if (text.back() == ' ')
						text.pop_back();
				}
				else
				{
					text.pop_back();
				}

				input.change_text(text + ' ' + ellipsis);
			}
		}

		std::string tolower(std::string str)
		{
			std::locale loc;
			std::stringstream ss;

			for (std::string::size_type i = 0; i < str.length(); ++i)
				ss << std::tolower(str[i], loc);

			return ss.str();
		}
	}

	namespace bytecode
	{
		bool compare(int32_t mask, int32_t value)
		{
			return (mask & value) != 0;
		}
	}

	namespace NxHelper
	{
		namespace Map
		{
			// TODO: nl::nx::String["Map.img"]["victoria"][160070000] is returning invalid data, yet the WZ file is fine.
			MapInfo get_map_info_by_id(int32_t mapid)
			{
				std::string map_category = get_map_category(mapid);
				nl::node map_info = nl::nx::String["Map.img"][map_category][mapid];

				return {
					map_info["mapDesc"],
					map_info["mapName"],
					map_info["streetName"],
					map_info["streetName"] + " : " + map_info["mapName"]
				};
			}

			// 前向声明：life_map_index() 定义于本文件后段（扫一次 Map002 建 (kind,id)->map 表）。
			static const std::map<std::pair<uint8_t, int32_t>, int32_t>& life_map_index();

			std::vector<MapSearchEntry> get_map_search_index()
			{
				static std::vector<MapSearchEntry> cache;
				static bool built = false;

				if (built)
					return cache;

				nl::node mapimg = nl::nx::String["Map.img"];

				if (mapimg)
				{
					for (nl::node cat : mapimg)
					{
						for (nl::node m : cat)
						{
							int32_t id = 0;

							try
							{
								id = static_cast<int32_t>(std::stoi(m.name()));
							}
							catch (...)
							{
								id = 0;
							}

							if (id <= 0)
								continue;

							std::string name = m["mapName"].get_string();
							std::string street = m["streetName"].get_string();

							if (name.empty())
								name = street;

							cache.push_back({ id, name, street });
						}
					}
				}

				// 192 原生搜索轨"搜怪物名"：追加怪物/NPC 名字条目。
				// 方案B（用户 2026-11-21 拍板）：只收录至少在一张地图 life 里出现过的
				// 实体，避免"搜到却提示未在任何地图出现"的死结果。
				{
					const auto& lifeidx = life_map_index();
					nl::node mobimg = nl::nx::String["Mob.img"];
					if (mobimg)
					{
						for (nl::node m : mobimg)
						{
							try
							{
								std::string nm = m["name"].get_string();
								int32_t id = std::stoi(m.name());
								if (id > 0 && !nm.empty()
									&& lifeidx.find(std::make_pair(static_cast<uint8_t>(1), id)) != lifeidx.end())
									cache.push_back({ id, nm, "", 1, 0 });
							}
							catch (...)
							{
								// 忽略非数字键 / 缺 name 的异常条目。
							}
						}
					}

					nl::node npcimg = nl::nx::String["Npc.img"];
					if (npcimg)
					{
						for (nl::node n : npcimg)
						{
							try
							{
								std::string nm = n["name"].get_string();
								int32_t id = std::stoi(n.name());
								if (id > 0 && !nm.empty()
									&& lifeidx.find(std::make_pair(static_cast<uint8_t>(2), id)) != lifeidx.end())
									cache.push_back({ id, nm, "", 2, 0 });
							}
							catch (...)
							{
							}
						}
					}
				}

				// 排序保序：地图在前、怪物/NPC 在后，各自按 id 升序。
				std::sort(cache.begin(), cache.end(),
					[](const MapSearchEntry& a, const MapSearchEntry& b)
					{
						if (a.kind != b.kind)
							return a.kind < b.kind;
						return a.id < b.id;
					});

				built = true;

				return cache;
			}

			// 反查某实体(mob/npc)首次出现的地图 id。
			// 首次调用**单次**扫 Map002 建好 (kind,id)->map 全量反查表，后续 O(1)；
			// 绝不每次点击都全表扫描（8M 节点会卡死 UI）。
			static const std::map<std::pair<uint8_t, int32_t>, int32_t>& life_map_index()
			{
				static std::map<std::pair<uint8_t, int32_t>, int32_t> index;
				static bool built = false;
				if (built)
					return index;

				nl::node root = nl::nx::Map002;
				if (root)
				{
					// Map002 结构：Map/Map<prefix>/<id9>.img/life/<i>/{id,type}
					for (nl::node cat : root["Map"])
					{
						for (nl::node mapimg : cat)
						{
							std::string name = mapimg.name();
							if (name.size() < 5 || name.substr(name.size() - 4) != ".img")
								continue;
							int32_t mapid = 0;
							try
							{
								mapid = std::stoi(name.substr(0, name.size() - 4));
							}
							catch (...)
							{
								continue;
							}
							for (nl::node life : mapimg["life"])
							{
								std::string t = life["type"].get_string();
								if (t != "m" && t != "n")
									continue;
								uint8_t kind = (t == "m") ? 1 : 2;
								int32_t id = static_cast<int32_t>(life["id"]);
								if (id <= 0)
									continue;
								index.emplace(std::make_pair(kind, id), mapid); // 只记首次出现
							}
						}
					}
				}
				built = true;
				return index;
			}

			int32_t find_map_of_life(int32_t life_id, uint8_t kind)
			{
				const auto& index = life_map_index();
				auto it = index.find(std::make_pair(kind, life_id));
				return it == index.end() ? 0 : it->second;
			}

			std::string get_map_category(int32_t mapid)
			{
				if (mapid < 100000000)
					return "maple";

				if (mapid < 200000000)
					return "victoria";

				if (mapid < 300000000)
					return "ossyria";

				if (mapid < 540000000)
					return "elin";

				if (mapid < 600000000)
					return "singapore";

				if (mapid < 670000000)
					return "MasteriaGL";

				if (mapid < 682000000)
				{
					int32_t prefix3 = (mapid / 1000000) * 1000000;
					int32_t prefix4 = (mapid / 100000) * 100000;

					if (prefix3 == 674000000 || prefix4 == 680100000 || prefix4 == 889100000)
						return "etc";

					if (prefix3 == 677000000)
						return "Episode1GL";

					return "weddingGL";
				}

				if (mapid < 683000000)
					return "HalloweenGL";

				if (mapid < 800000000)
					return "event";

				if (mapid < 900000000)
					return "jp";

				return "etc";
			}

			// 【查询专用地图 node】读取任意地图的 life/mob 数据时必须用这个，而不是
			// get_map_node_name()：后者走 ensure_map() 会重绑全局 nl::nx::Map002 并
			// LRU 淘汰当前渲染仍持有的地图 file，导致 node 悬垂 → 0xc0000374 堆损坏。
			// ensure_map_query() 用独立 file 缓存，绝不触碰 Map002 / s_map_files。
			// 返回的 node 仅在其 file 留在查询缓存期间有效（QUERY_MAP_KEEP=32），
			// 调用方须即时把需要的字段拷贝成 std::string/数值，不得长期持有。
			static nl::node query_map_node(int32_t mapid)
			{
				if (mapid <= 0)
					return nl::node{};
				return res::ImgSource::ensure_map_query(mapid);
			}

			std::unordered_map<int64_t, std::pair<std::string, std::string>> get_life_on_map(int32_t mapid)
			{
				std::unordered_map<int64_t, std::pair<std::string, std::string>> map_life;
				// 查询任意地图 life 必须走隔离缓存：绝不重绑渲染用的全局 Map002。
				if (mapid <= 0)
					return map_life;

				nl::node portal = query_map_node(mapid);
				if (!portal)
					return map_life;

				for (nl::node life : portal["life"])
				{
					int64_t life_id = life["id"];
					std::string life_type = life["type"];
					uint8_t hide_life = life["hide"];

					if (!hide_life)
					{
						if (life_type == "m")
						{
							// Mob
							nl::node life_name = nl::nx::String["Mob.img"][life_id]["name"];

							std::string life_id_str = string_format::extend_id(life_id, 7);
							nl::node life_level = nl::nx::Mob[life_id_str + ".img"]["info"]["level"];

							if (life_name && life_level)
								map_life[life_id] = { life_type, life_name + "(Lv. " + life_level + ")" };
						}
						else if (life_type == "n")
						{
							// NPC
							if (nl::node life_name = nl::nx::String["Npc.img"][life_id]["name"])
								map_life[life_id] = { life_type, life_name };
						}
					}
				}

				return map_life;
			}

			std::vector<MapMobInfo> get_mobs_on_map(int32_t mapid)
			{
				std::vector<MapMobInfo> result;
				std::unordered_set<int32_t> seen;

				// 手册/侧栏查任意地图 mob，走隔离缓存，绝不重绑渲染用 Map002。
				nl::node portal = query_map_node(mapid);

				if (!portal)
					return result;

				for (nl::node life : portal["life"])
				{
					std::string life_type = life["type"];
					uint8_t hide_life = life["hide"];

					if (hide_life || life_type != "m")
						continue;

					int32_t life_id = static_cast<int32_t>(static_cast<int64_t>(life["id"]));

					if (!seen.insert(life_id).second)
						continue;

					std::string life_id_str = string_format::extend_id(life_id, 7);
					nl::node info = nl::nx::Mob[life_id_str + ".img"]["info"];
					nl::node life_name = nl::nx::String["Mob.img"][life_id]["name"];

					MapMobInfo entry;
					entry.id = life_id;
					entry.name = life_name ? life_name.get_string() : ("Mob " + std::to_string(life_id));
					entry.level = info["level"] ? static_cast<int16_t>(info["level"].get_integer()) : 0;
					entry.max_hp = info["maxHP"] ? static_cast<int32_t>(info["maxHP"].get_integer()) : 0;
					entry.exp = info["exp"] ? static_cast<int32_t>(info["exp"].get_integer()) : 0;
					entry.boss = info["boss"] ? (info["boss"].get_integer() != 0) : false;
					if (const char* code = data::mob_elem_attr(life_id))
						entry.elem_attr = code;
					result.push_back(std::move(entry));
				}

				std::sort(result.begin(), result.end(), [](const MapMobInfo& a, const MapMobInfo& b)
				{
					if (a.level != b.level)
						return a.level < b.level;

					return a.id < b.id;
				});

				return result;
			}

			nl::node get_map_node_name(int32_t mapid)
			{
				// 世界地图悬停到无 map_ids 的点位会传 mapid=0（UIWorldMap.cpp:667 的
				// spot_map_id 兜底）。0 是非法地图，ensure_map(0)→load_single_map(0)
				// 拼出不存在的 img 路径并写全局 nl::node，实测触发 0xc0000374 堆损坏。
				// 源头拦截：非法 id 直接返回空节点，绝不起加载。
				if (mapid <= 0)
					return nl::node{};

				// Map002 是按需懒加载源（默认只绑玩家当前所在地图），
				// 世界地图/任务窗查询任意地图前必须先装载该 img，否则恒为空节点。
				res::ImgSource::ensure_map(mapid);

				std::string prefix = std::to_string(mapid / 100000000);
				std::string mapid_str = string_format::extend_id(mapid, 9);

				return nl::nx::Map002["Map"]["Map" + prefix][mapid_str + ".img"];
			}
		}
	}

	namespace single_console
	{
		void log_message(std::string message)
		{
			if (std::find(std::begin(log_history), std::end(log_history), message) == std::end(log_history))
			{
				log_history.push_back(message);

				LOG(LOG_DEBUG, message);
			}
		}
	}
}