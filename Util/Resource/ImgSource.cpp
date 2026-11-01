//////////////////////////////////////////////////////////////////////////////
//	Fyms085 resource abstraction additions Copyright (C) 2026
//	IMG 零转换后端（I2）实现。
//////////////////////////////////////////////////////////////////////////////
#include "ImgSource.h"

#include "../../Configuration.h"
#include "../../IO/Window.h"
#include "../../MapleStory.h"
#include "RootTable.h"

#include <nlnx/wz.hpp>

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace ms
{
	namespace res
	{
		std::vector<std::unique_ptr<nl::img_file>> ImgSource::s_files;
		std::map<std::string, std::string> ImgSource::s_dirs;
		std::set<std::string> ImgSource::s_loaded;
		int32_t ImgSource::s_bound_mapid = -2;
		std::atomic<bool> ImgSource::s_pump_enabled{ true };

		void ImgSource::set_pump_enabled(bool enabled)
		{
			s_pump_enabled.store(enabled);
		}

		// 16 个标准根名（与 ImgFiles.cpp 的 ROOTS[16] 一致）。
		static const char* STANDARD_ROOTS[16] =
		{
			"Base", "Character", "Effect", "Etc", "Item", "Map", "Mob", "Morph",
			"Npc", "Quest", "Reactor", "Skill", "Sound", "String", "TamingMob", "UI"
		};

		// 大小写不敏感比较（用于扩展名匹配）。
		static bool iequals(const std::string& a, const std::string& b)
		{
			if (a.size() != b.size())
				return false;
			for (size_t i = 0; i < a.size(); ++i)
				if (std::tolower(static_cast<unsigned char>(a[i])) !=
					std::tolower(static_cast<unsigned char>(b[i])))
					return false;
			return true;
		}

		Error ImgSource::init(const std::string& base_dir)
		{
			close();

			std::string data_dir = base_dir;
			if (data_dir.empty())
				data_dir = Configuration::get().get_data_dir();
			if (data_dir.empty())
				data_dir = "Data";

			std::error_code ec;
			fs::path base = fs::absolute(data_dir);
			if (!fs::is_directory(base, ec))
			{
				static std::string dir_msg;
				dir_msg = "IMG data directory not found: " + base.string();
				return Error(Error::Code::MISSING_FILE, dir_msg.c_str());
			}

			for (const char* root : STANDARD_ROOTS)
			{
				fs::path dir = base / root;
				if (fs::is_directory(dir, ec))
					s_dirs[root] = dir.generic_string();
			}

			// 登录与核心循环依赖 UI；其余根允许缺失（BeiDou 常见缺 Base 等）。
			if (s_dirs.find("UI") == s_dirs.end())
			{
				static std::string ui_msg;
				ui_msg = "IMG layout missing required UI/ under: " + base.string();
				return Error(Error::Code::MISSING_FILE, ui_msg.c_str());
			}

			{
				std::ostringstream miss;
				bool first = true;
				for (const char* root : STANDARD_ROOTS)
				{
					if (s_dirs.find(root) != s_dirs.end())
						continue;
					if (!first)
						miss << ", ";
					first = false;
					miss << root;
				}
				if (!first)
					LOG(LOG_WARN, "[IMG] missing standard root dirs (tolerated): " << miss.str());
			}

			// Boot only the roots needed to paint the login screen quickly.
			// Sound001/002、Character、Map002 等延后，避免启动/登录卡很久。
			static const RootId kBootRoots[] = {
				RootId::UI,
				RootId::Map,
				RootId::Map001,
				RootId::Sound,
				RootId::String,
				RootId::Effect,
				RootId::Etc
			};

			for (RootId id : kBootRoots)
				ensure(id);

			LOG(LOG_INFO, "[IMG] boot dirs=" << s_dirs.size()
				<< " bound_roots=" << RootTable::bound_count()
				<< " base=" << base.string()
				<< " (heavy roots deferred)");

			return Error(Error::Code::NONE);
		}

		void ImgSource::ensure_remaining()
		{
			for (size_t i = 0; i < RootTable::SIZE; ++i)
			{
				auto id = static_cast<RootId>(i);
				// Map002 按需 ensure_map，禁止扫全图目录。
				if (id == RootId::Map002)
					continue;
				ensure(id);
				Window::get().poll_events();
			}

			LOG(LOG_INFO, "[IMG] ensure_remaining bound_roots="
				<< RootTable::bound_count() << "/" << RootTable::SIZE);
		}

		void ImgSource::ensure_char_select()
		{
			// 选角外观最少集；Skill/Quest 进图再装，显著缩短登录等待。
			static const RootId kCharSelectRoots[] = {
				RootId::Character,
				RootId::Item,
				RootId::Morph,
				RootId::TamingMob
			};

			for (RootId id : kCharSelectRoots)
			{
				ensure(id);

				if (s_pump_enabled.load())
					Window::get().poll_events();
			}

			LOG(LOG_INFO, "[IMG] ensure_char_select bound_roots="
				<< RootTable::bound_count() << "/" << RootTable::SIZE);
		}

		void ImgSource::ensure_map(int32_t mapid)
		{
			ensure(RootId::Map);

			if (s_bound_mapid == mapid && RootTable::root(RootId::Map002))
				return;

			load_single_map(mapid);
			Window::get().poll_events();
		}

		std::vector<std::pair<std::string, std::vector<char>>> ImgSource::gather(const std::string& dir)
		{
			std::vector<std::pair<std::string, std::vector<char>>> out;
			std::error_code ec;
			fs::path directory = fs::u8path(dir);
			if (!fs::is_directory(directory, ec))
				return out;

			size_t n = 0;
			for (auto it = fs::recursive_directory_iterator(directory,
				fs::directory_options::skip_permission_denied, ec);
				it != fs::recursive_directory_iterator(); ++it)
			{
				std::error_code lec;
				if (!it->is_regular_file(lec) || lec)
					continue;
				if (!iequals(it->path().extension().string(), ".img"))
					continue;

				// 读入 .img 字节（零转换：直接在内存交给 img_file 解析）。
				std::ifstream f(it->path(), std::ios::binary);
				if (!f)
					continue;
				std::vector<char> all((std::istreambuf_iterator<char>(f)),
					std::istreambuf_iterator<char>());
				if (all.empty())
					continue;

				std::string rel = it->path().lexically_relative(directory).generic_string();
				out.emplace_back(std::move(rel), std::move(all));

				// Character/Item 等大目录扫描时泵消息，避免“未响应”。
				// （后台线程装载时跳过：SDL 事件只允许主线程泵，主循环照常泵。）
				if (s_pump_enabled.load() && (++n % 64) == 0)
					Window::get().poll_events();
			}
			return out;
		}

		void ImgSource::load_standard(const std::string& name, RootId id)
		{
			auto it = s_dirs.find(name);
			if (it == s_dirs.end())
				return;
			auto bufs = gather(it->second);
			if (bufs.empty())
				return;
			auto f = std::make_unique<nl::img_file>();
			f->open_merge(std::move(bufs));
			RootTable::bind(id, f->root());
			s_files.push_back(std::move(f));
		}

		void ImgSource::load_map_helpers(const std::string& mapdir)
		{
			auto bufs = gather(mapdir);
			std::vector<std::pair<std::string, std::vector<char>>> helpers;
			for (auto& b : bufs)
			{
				// 地图 .img 位于 Map/ 子目录或平铺数字文件，留给 load_map_maps 处理。
				if (b.first.size() >= 4 && b.first.compare(0, 4, "Map/") == 0)
					continue;
				if (b.first.find('/') == std::string::npos)
				{
					fs::path p(b.first);
					std::string stem = p.stem().string();
					char* endp = nullptr;
					std::strtol(stem.c_str(), &endp, 10);
					if (endp && *endp == '\0')
						continue;
				}
				helpers.push_back(std::move(b));
			}
			if (helpers.empty())
				return;
			auto f = std::make_unique<nl::img_file>();
			f->open_merge(std::move(helpers));
			// 视图别名：Map 与 Map001 共享同一份 helper 解析（零复制，不再产生 Map.nx 复制件）。
			RootTable::bind(RootId::Map, f->root());
			RootTable::bind(RootId::Map001, f->root());
			s_files.push_back(std::move(f));
		}

		void ImgSource::load_map_maps(const std::string& mapdir)
		{
			// 保留接口：全量扫图仅作显式调试路径，正常进图走 load_single_map。
			LOG(LOG_WARN, "[IMG] load_map_maps bulk skipped (use ensure_map); dir=" << mapdir);
		}

		void ImgSource::load_single_map(int32_t mapid)
		{
			auto it = s_dirs.find("Map");
			if (it == s_dirs.end())
			{
				LOG(LOG_WARN, "[IMG] ensure_map: Map/ dir missing");
				return;
			}

			const int prefix = static_cast<int>(mapid / 100000000);
			std::ostringstream idoss;
			idoss << std::setw(9) << std::setfill('0') << mapid;
			const std::string id9 = idoss.str();
			const std::string basename = id9 + ".img";

			fs::path base = fs::u8path(it->second);
			fs::path candidates[] = {
				base / "Map" / ("Map" + std::to_string(prefix)) / basename,
				base / ("Map" + std::to_string(prefix)) / basename,
				base / basename
			};

			fs::path hit;
			std::error_code ec;
			for (const fs::path& c : candidates)
			{
				if (fs::is_regular_file(c, ec))
				{
					hit = c;
					break;
				}
			}

			if (hit.empty())
			{
				LOG(LOG_ERROR, "[IMG] map file missing mapid=" << mapid);
				s_bound_mapid = -1;
				return;
			}

			std::ifstream f(hit, std::ios::binary);
			if (!f)
			{
				LOG(LOG_ERROR, "[IMG] map open failed " << hit.generic_string());
				return;
			}

			std::vector<char> bytes((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());
			if (bytes.empty())
			{
				LOG(LOG_ERROR, "[IMG] map empty " << hit.generic_string());
				return;
			}

			std::string newrel = "Map/Map" + std::to_string(prefix) + "/" + basename;
			std::vector<std::pair<std::string, std::vector<char>>> maps;
			maps.emplace_back(std::move(newrel), std::move(bytes));

			auto file = std::make_unique<nl::img_file>();
			file->open_tree(std::move(maps));
			RootTable::bind(RootId::Map002, file->root());
			s_files.push_back(std::move(file));
			s_bound_mapid = mapid;
			s_loaded.insert("Map002");

			LOG(LOG_INFO, "[IMG] ensure_map ok mapid=" << mapid
				<< " file=" << hit.generic_string());
		}

		void ImgSource::ensure(RootId id)
		{
			std::string phys;
			switch (id)
			{
				case RootId::Map:
				case RootId::Map001:
					phys = "Map";
					break;
				case RootId::Map002:
					phys = "Map002";
					break;
				default:
					phys = root_filename(id);
					break;
			}
			if (s_loaded.count(phys))
				return;

			try
			{
				if (phys == "Map")
				{
					auto it = s_dirs.find("Map");
					if (it != s_dirs.end())
						load_map_helpers(it->second);
				}
				else if (phys == "Map002")
				{
					// 全量扫图已禁用；占位标记，真实内容由 ensure_map 绑定。
					s_loaded.insert(phys);
					return;
				}
				else
				{
					load_standard(phys, id);
				}
			}
			catch (const std::exception& e)
			{
				// 单个根解析失败（多为畸形 .img）时跳过该根，
				// 避免异常冒泡导致整个装载终止；缺失根由调用方宽松容忍。
				LOG(LOG_WARN, "[IMG] skip root " << phys << ": " << e.what());
				s_loaded.insert(phys);
				return;
			}
			s_loaded.insert(phys);
		}

		void ImgSource::close()
		{
			s_files.clear();
			s_dirs.clear();
			s_loaded.clear();
			s_bound_mapid = -2;
			RootTable::clear();
		}
	}
}
