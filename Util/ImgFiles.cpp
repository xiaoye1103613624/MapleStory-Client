////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	Fyms085 IMG backend additions Copyright (C) 2026							//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero Public License as published by			//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero Public License			//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
////////////////////////////////////////////////////////////////////////////////
#include "ImgFiles.h"
#include "../Configuration.h"

#include <nlnx/wz.hpp>
#include <nlnx/nx.hpp>
#include <nlnx/node.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace ms
{
	namespace ImgFiles
	{
		// The sixteen standard MapleStory roots, in the same order NoLifeNx
		// assigns them through nl::nx::load_all().
		static const char* ROOTS[16] =
		{
			"Base", "Character", "Effect", "Etc", "Item", "Map", "Mob", "Morph",
			"Npc", "Quest", "Reactor", "Skill", "Sound", "String", "TamingMob", "UI"
		};

		static bool iequals(const std::string& a, const std::string& b)
		{
			if (a.size() != b.size())
				return false;

			for (size_t i = 0; i < a.size(); ++i)
				if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i])))
					return false;

			return true;
		}

		Error init(const std::string& base_dir)
		{
			// 数据目录：门面定位到的目录优先，否则回退到用户配置（data.dir）。
			// The data directory is configured by the user (BeiDou Data folder).
			std::string data_dir = base_dir;

			if (data_dir.empty())
				data_dir = Configuration::get().get_data_dir();

			if (data_dir.empty())
				data_dir = "Data";

			std::error_code ec;
			fs::path base = fs::absolute(data_dir);

			if (!fs::is_directory(base, ec))
				return Error(Error::Code::MISSING_FILE,
					("IMG data directory not found: " + base.string()).c_str());

			// Converted .nx files live in a hidden subfolder so the source tree
			// stays untouched and repeated launches skip the (potentially very
			// expensive) re-parsing of tens of thousands of small .img files.
			fs::path cache = base / ".nxcache";
			fs::create_directories(cache, ec);

			// NoLifeNx locates its files relative to the current working
			// directory, so point it at the cache while loading, then restore.
			fs::path original = fs::current_path(ec);
			fs::current_path(cache, ec);

			try
			{
				auto gather = [&](fs::path const & dir) -> std::vector<std::string>
				{
					std::vector<std::string> out;
					try
					{
						for (auto it = fs::recursive_directory_iterator(
								dir, fs::directory_options::skip_permission_denied);
							it != fs::recursive_directory_iterator(); ++it)
						{
							std::error_code lec;
							if (!it->is_regular_file(lec) || lec)
								continue;
							if (iequals(it->path().extension().string(), ".img"))
								out.push_back(it->path().string());
						}
					}
					catch (const fs::filesystem_error&) {}
					return out;
				};

				for (const char* root : ROOTS)
				{
					fs::path rootdir = base / root;
					if (!fs::is_directory(rootdir, ec))
						continue;

					if (strcmp(root, "Map") != 0)
					{
						fs::path nxpath = cache / (std::string(root) + ".nx");
						if (fs::exists(nxpath, ec))
							continue; // reuse the cached conversion

						std::vector<std::string> imgs = gather(rootdir);
						if (imgs.empty())
							continue;

						nl::convert_img_files_to_nx(imgs, (cache / root).string(), rootdir.string());
					}
					else
					{
						// Map: the original WZ splits into Map.wz (helpers: Back/ Obj/
						// Tile/ WorldMap/ Effect.img/ MapHelper.img/ Physics.img),
						// Map001.wz (login/UI helpers, accessed as Map001["Back"]) and
						// Map002.wz (the actual maps, accessed as
						// Map002["Map"]["Map"+prefix][mapid+".img"]). The .img export
						// merged them under Data/Map/ as: helper folders/files + the
						// individual maps under Map/Map/<id>.img. We rebuild the three
						// roots expected by the game:
						//   - Map001.nx / Map.nx : the helper graphics
						//   - Map002.nx          : the per-map .img files reorganised as
						//                          Map/Map<prefix>/<id>.img
						// (prefix = floor(mapid / 100000000), matching Misc.cpp).
						std::vector<std::string> helper_imgs;
						std::vector<std::pair<std::string, std::string>> map_relpaths;
						try
						{
							for (auto it = fs::recursive_directory_iterator(
									rootdir, fs::directory_options::skip_permission_denied);
								it != fs::recursive_directory_iterator(); ++it)
							{
								std::error_code lec;
								if (!it->is_regular_file(lec) || lec)
									continue;
								if (!iequals(it->path().extension().string(), ".img"))
									continue;
								fs::path rel = it->path().lexically_relative(rootdir);
								if (rel.begin() != rel.end()
									&& iequals(rel.begin()->string(), "Map"))
								{
									// Individual map archive: Map/Map/<id>.img
									std::string fname = it->path().stem().string();
									std::string prefix = "0";
									bool numeric = !fname.empty();
									for (char c : fname)
										if (!::isdigit(static_cast<unsigned char>(c)))
											numeric = false;
									if (numeric)
									{
										long long id = 0;
										try { id = std::stoll(fname); }
										catch (...) { id = 0; }
										prefix = std::to_string(id / 100000000);
									}
									map_relpaths.emplace_back(
										"Map/Map" + prefix + "/" + fname + ".img",
										it->path().string());
								}
								else
								{
									helper_imgs.push_back(it->path().string());
								}
							}
						}
						catch (const fs::filesystem_error&) {}

						// Map001.nx = helpers (login background etc.)
						fs::path map001nx = cache / "Map001.nx";
						if (!fs::exists(map001nx, ec) && !helper_imgs.empty())
						{
							nl::convert_img_files_to_nx(helper_imgs, (cache / "Map001").string(), rootdir.string());
						}
						// Map.nx = same helpers (game uses bare nl::nx::Map for Obj/Tile/...)
						fs::path mapnx = cache / "Map.nx";
						if (!fs::exists(mapnx, ec) && fs::exists(map001nx, ec))
						{
							std::error_code cec;
							fs::copy_file(map001nx, mapnx, fs::copy_options::overwrite_existing, cec);
						}
						// Map002.nx = per-map .img files
						fs::path map002nx = cache / "Map002.nx";
						if (!fs::exists(map002nx, ec) && !map_relpaths.empty())
						{
							nl::convert_img_files_to_nx(map_relpaths, (cache / "Map002").string());
						}
					}
				}

				// Top-level loose .img files (smap.img, StandardPDD.img, zmap.img)
				// become their own nl::nx roots too.
				try
				{
					for (auto it = fs::directory_iterator(base, fs::directory_options::skip_permission_denied);
						it != fs::directory_iterator(); ++it)
					{
						std::error_code lec;
						if (!it->is_regular_file(lec) || lec)
							continue;
						if (!iequals(it->path().extension().string(), ".img"))
							continue;
						std::string name = it->path().stem().string();
						fs::path nxpath = cache / (name + ".nx");
						if (fs::exists(nxpath, ec))
							continue;
						nl::convert_single_img_to_nx(it->path().string(), name);
					}
				}
				catch (const fs::filesystem_error&) {}

				// Populate nl::nx::* from the generated .nx files. load_all
				// assigns every present root file to its node and leaves absent
				// roots (e.g. BeiDou data ships no Base directory) as empty,
				// valid nodes rather than failing.
				nl::nx::load_all();
			}
			catch (const std::exception& ex)
			{
				fs::current_path(original, ec);
				static const std::string message = ex.what();
				return Error(Error::Code::NLNX, message.c_str());
			}

			fs::current_path(original, ec);
			return Error::Code::NONE;
		}
	}
}
