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
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
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

		// Validate a cached .nx before reuse.
		//
		// A conversion writes the 52-byte header and all lookup tables first,
		// then appends the (much larger) bitmap payload with a SECOND stream.
		// If the process is killed in between, the file still carries a valid
		// PKG4 magic and plausible tables, but the bitmap data region is EMPTY.
		// nl::nx::load_all() only checks the magic, so such a truncated file used
		// to be reused verbatim; the first bitmap lookup then ran
		// LZ4_decompress_fast() past the mapped file end and crashed inside
		// liblz4.dll (0xc0000005). This is what made the character-select screen
		// die right after CHARLIST.
		//
		// Header layout (nlnx/file_impl.hpp, packed 52 bytes):
		//   u32 magic, u32 node_count, u64 node_offset,
		//   u32 string_count, u64 string_offset (string TABLE),
		//   u32 bitmap_count, u64 bitmap_offset (bitmap TABLE),
		//   u32 audio_count, u64 audio_offset (audio TABLE)
		// A bitmap block is [u32 compressed length][LZ4 block]; bitmap offsets
		// must live past the bitmap table and the last block must fit in-file.
		static bool nx_is_complete(const fs::path& path, std::error_code& ec)
		{
			ec.clear();
			uint64_t size = static_cast<uint64_t>(fs::file_size(path, ec));
			if (ec || size < 52)
				return false;

			std::ifstream in(path, std::ios::binary);
			if (!in)
				return false;

			unsigned char hdr[52];
			in.read(reinterpret_cast<char*>(hdr), sizeof(hdr));
			if (in.gcount() != static_cast<std::streamsize>(sizeof(hdr)))
				return false;

			auto u32 = [&](size_t o) { uint32_t v; std::memcpy(&v, hdr + o, 4); return v; };
			auto u64 = [&](size_t o) { uint64_t v; std::memcpy(&v, hdr + o, 8); return v; };

			if (u32(0) != 0x34474B50u) // "PKG4"
				return false;

			uint64_t node_off = u64(8);
			uint64_t str_tbl_off = u64(20);
			uint64_t bmp_tbl_off = u64(32);
			uint64_t aud_tbl_off = u64(44);
			uint32_t node_count = u32(4);
			uint32_t str_count = u32(16);
			uint32_t bmp_count = u32(28);
			uint32_t aud_count = u32(40);

			auto fits = [&](uint64_t off, uint64_t n) { return off <= size && n <= size - off; };
			auto read_at = [&](uint64_t off, void* dst, size_t n) -> bool
			{
				if (!fits(off, n))
					return false;
				in.clear();
				in.seekg(static_cast<std::streamoff>(off), std::ios::beg);
				if (!in)
					return false;
				in.read(static_cast<char*>(dst), static_cast<std::streamsize>(n));
				return in.gcount() == static_cast<std::streamsize>(n);
			};

			if (!fits(node_off, static_cast<uint64_t>(node_count) * 20)
				|| !fits(str_tbl_off, static_cast<uint64_t>(str_count) * 8)
				|| !fits(bmp_tbl_off, static_cast<uint64_t>(bmp_count) * 8)
				|| !fits(aud_tbl_off, static_cast<uint64_t>(aud_count) * 8))
				return false;

			if (bmp_count > 0)
			{
				uint64_t data_start = bmp_tbl_off + static_cast<uint64_t>(bmp_count) * 8;
				uint64_t first = 0, last = 0;
				if (!read_at(bmp_tbl_off, &first, 8)
					|| !read_at(bmp_tbl_off + static_cast<uint64_t>(bmp_count - 1) * 8, &last, 8))
					return false;
				if (first < data_start || first >= size)
					return false;
				if (last < data_start || last >= size)
					return false;
				uint32_t clen = 0;
				if (!read_at(last, &clen, 4) || clen == 0)
					return false;
				if (!fits(last + 4, clen))
					return false;
			}
			else if (aud_count > 0)
			{
				uint64_t data_start = aud_tbl_off + static_cast<uint64_t>(aud_count) * 8;
				uint64_t last = 0;
				if (!read_at(aud_tbl_off + static_cast<uint64_t>(aud_count - 1) * 8, &last, 8))
					return false;
				if (last < data_start || last >= size)
					return false;
			}

			return true;
		}

		// Convert into a temporary file and only publish it (atomic rename) once
		// the result passes nx_is_complete(). An interrupted conversion can then
		// never leave a half-written .nx behind that looks reusable.
		//
		// The nl::convert_* helpers append ".nx" to the base name they are given,
		// so emit() receives "<name>.__tmp" and produces "<name>.__tmp.nx".
		template <typename EmitFn>
		static void convert_atomic(const fs::path& final_path, EmitFn emit)
		{
			std::error_code ec;
			std::string final_str = final_path.string();
			const std::string suffix = ".nx";
			if (final_str.size() <= suffix.size()
				|| final_str.compare(final_str.size() - suffix.size(), suffix.size(), suffix) != 0)
				return;

			std::string tmp_base = final_str.substr(0, final_str.size() - suffix.size()) + ".__tmp";
			fs::path produced(tmp_base + ".nx");
			fs::remove(produced, ec);

			emit(tmp_base);

			if (nx_is_complete(produced, ec))
			{
				fs::remove(final_path, ec);
				fs::rename(produced, final_path, ec);
			}
			else
			{
				fs::remove(produced, ec);
			}
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
						if (nx_is_complete(nxpath, ec))
							continue; // reuse only a validated, complete conversion

						// A truncated/unvalidated cache is worthless and dangerous:
						// remove it, then rebuild atomically.
						fs::remove(nxpath, ec);

						std::vector<std::string> imgs = gather(rootdir);
						if (imgs.empty())
							continue;

						convert_atomic(nxpath, [&](const std::string& out_base)
						{
							nl::convert_img_files_to_nx(imgs, out_base, rootdir.string());
						});
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
						if (!nx_is_complete(map001nx, ec) && !helper_imgs.empty())
						{
							fs::remove(map001nx, ec);
							convert_atomic(map001nx, [&](const std::string& out_base)
							{
								nl::convert_img_files_to_nx(helper_imgs, out_base, rootdir.string());
							});
						}
						// Map.nx = same helpers (game uses bare nl::nx::Map for Obj/Tile/...)
						fs::path mapnx = cache / "Map.nx";
						if (!nx_is_complete(mapnx, ec) && nx_is_complete(map001nx, ec))
						{
							// Publish the copy atomically as well.
							fs::path tmpcopy = cache / "Map.__tmp.nx";
							std::error_code cec;
							fs::remove(tmpcopy, cec);
							fs::copy_file(map001nx, tmpcopy, fs::copy_options::overwrite_existing, cec);
							if (!cec && nx_is_complete(tmpcopy, cec))
							{
								fs::remove(mapnx, cec);
								fs::rename(tmpcopy, mapnx, cec);
							}
							else
							{
								fs::remove(tmpcopy, cec);
							}
						}
						// Map002.nx = per-map .img files
						fs::path map002nx = cache / "Map002.nx";
						if (!nx_is_complete(map002nx, ec) && !map_relpaths.empty())
						{
							fs::remove(map002nx, ec);
							convert_atomic(map002nx, [&](const std::string& out_base)
							{
								nl::convert_img_files_to_nx(map_relpaths, out_base);
							});
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
						if (nx_is_complete(nxpath, ec))
							continue;
						fs::remove(nxpath, ec);
						convert_atomic(nxpath, [&](const std::string& out_base)
						{
							nl::convert_single_img_to_nx(it->path().string(), out_base);
						});
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
