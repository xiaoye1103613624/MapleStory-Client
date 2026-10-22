//////////////////////////////////////////////////////////////////////////////
// NoLifeNx - Part of the NoLifeStory project                               //
// Copyright © 2013 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#include "nx.hpp"
#include "file.hpp"
#include "node.hpp"

#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>

namespace nl
{
	namespace nx
	{
		std::vector<std::unique_ptr<file>> files;

		// 可选的装载根目录前缀：非空时，所有根文件名都在该目录下解析；
		// 为空时保持既有行为（相对当前工作目录）。
		// 该前缀只在 load_all(base_dir) 执行期间有效，结束后立即清空。
		std::string base_prefix;

		// 给文件名加上根目录前缀。
		std::string with_base(std::string const& name)
		{
			if (base_prefix.empty())
				return name;

			std::string joined = base_prefix;

			const char last = joined.back();

			if (last != '/' && last != '\\')
				joined += '/';

			joined += name;

			return joined;
		}

		bool exists(std::string name)
		{
			return std::ifstream(with_base(name)).is_open();
		}

		node add_file(std::string name)
		{
			if (!exists(name))
				return {};

			files.emplace_back(new file(with_base(name)));

			return *files.back();
		}

		node Base, Character, Effect, Etc, Item, Map, Map001, Map002, Map2, Mob, Mob001, Mob002, Mob2, Morph, Npc, Quest, Reactor, Skill, Skill001, Skill002, Skill003, Sound, Sound001, Sound002, Sound2, String, TamingMob, UI, Maker;

		void load_all()
		{
			load_all(std::string());
		}

		void load_all(const std::string& base_dir)
		{
			// 装载期间生效；结束后清空，避免影响后续任何调用。
			base_prefix = base_dir;

			if (exists("Base.nx"))
			{
				Base = add_file("Base.nx");
				Character = add_file("Character.nx");
				Effect = add_file("Effect.nx");
				Etc = add_file("Etc.nx");
				Item = add_file("Item.nx");
				Map = add_file("Map.nx");
				Map001 = add_file("Map001.nx");
				Map002 = add_file("Map002.nx");
				Map2 = add_file("Map2.nx");
				Mob = add_file("Mob.nx");
				Mob001 = add_file("Mob001.nx");
				Mob002 = add_file("Mob002.nx");
				Mob2 = add_file("Mob2.nx");
				Morph = add_file("Morph.nx");
				Npc = add_file("Npc.nx");
				Quest = add_file("Quest.nx");
				Reactor = add_file("Reactor.nx");
				Skill = add_file("Skill.nx");
				Skill001 = add_file("Skill001.nx");
				Skill002 = add_file("Skill002.nx");
				Skill003 = add_file("Skill003.nx");
				Sound = add_file("Sound.nx");
				Sound001 = add_file("Sound001.nx");
				Sound002 = add_file("Sound002.nx");
				Sound2 = add_file("Sound2.nx");
				String = add_file("String.nx");
				TamingMob = add_file("TamingMob.nx");
				UI = add_file("UI.nx");
				Maker = add_file("Maker.nx");
			}
			else if (exists("Data.nx"))
			{
				Base = add_file("Data.nx");
				Character = Base["Character"];
				Effect = Base["Effect"];
				Etc = Base["Etc"];
				Item = Base["Item"];
				Map = Base["Map"];
				Map001 = Map["Map001"];
				Map002 = Map["Map002"];
				Map2 = Map["Map2"];
				Mob = Base["Mob"];
				Mob001 = Mob["Mob001"];
				Mob002 = Mob["Mob002"];
				Mob2 = Mob["Mob2"];
				Morph = Base["Morph"];
				Npc = Base["Npc"];
				Quest = Base["Quest"];
				Reactor = Base["Reactor"];
				Skill = Base["Skill"];
				Skill001 = Skill["Skill001"];
				Skill002 = Skill["Skill002"];
				Skill003 = Skill["Skill003"];
				Sound = Base["Sound"];
				Sound001 = Sound["Sound001"];
				Sound002 = Sound["Sound002"];
				Sound2 = Sound["Sound2"];
				String = Base["String"];
				TamingMob = Base["TamingMob"];
				UI = Base["UI"];
				Maker = Base["Maker"];
			}
			else
			{
				// No standard "Base.nx"/"Data.nx" aggregate. Still attempt to
				// load whichever individual root files are present so partial
				// datasets (e.g. an IMG extraction that ships no Base
				// directory) can be used; missing roots simply stay empty.
				Base = add_file("Base.nx");
				Character = add_file("Character.nx");
				Effect = add_file("Effect.nx");
				Etc = add_file("Etc.nx");
				Item = add_file("Item.nx");
				Map = add_file("Map.nx");
				Map001 = add_file("Map001.nx");
				Map002 = add_file("Map002.nx");
				Map2 = add_file("Map2.nx");
				Mob = add_file("Mob.nx");
				Mob001 = add_file("Mob001.nx");
				Mob002 = add_file("Mob002.nx");
				Mob2 = add_file("Mob2.nx");
				Morph = add_file("Morph.nx");
				Npc = add_file("Npc.nx");
				Quest = add_file("Quest.nx");
				Reactor = add_file("Reactor.nx");
				Skill = add_file("Skill.nx");
				Skill001 = add_file("Skill001.nx");
				Skill002 = add_file("Skill002.nx");
				Skill003 = add_file("Skill003.nx");
				Sound = add_file("Sound.nx");
				Sound001 = add_file("Sound001.nx");
				Sound002 = add_file("Sound002.nx");
				Sound2 = add_file("Sound2.nx");
				String = add_file("String.nx");
				TamingMob = add_file("TamingMob.nx");
				UI = add_file("UI.nx");
				Maker = add_file("Maker.nx");
			}

			base_prefix.clear();
		}
	}
}
