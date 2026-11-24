//////////////////////////////////////////////////////////////////////////////
//	Fyms085 resource abstraction additions Copyright (C) 2026
//	IMG 零转换后端（I2）实现。
//////////////////////////////////////////////////////////////////////////////
#include "ImgSource.h"

#include "../../Configuration.h"
#include "../../IO/Window.h"
#include "../../MapleStory.h"
#include "../PerfLog.h"
#include "RootTable.h"

#include <nlnx/wz.hpp>

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace ms
{
	namespace res
	{
		std::vector<std::unique_ptr<nl::img_file>> ImgSource::s_files;
		std::vector<ImgSource::map_slot> ImgSource::s_map_files;
		std::vector<ImgSource::map_slot> ImgSource::s_query_files;
		std::map<std::string, std::string> ImgSource::s_dirs;
		std::set<std::string> ImgSource::s_loaded;
		int32_t ImgSource::s_bound_mapid = -2;
		std::atomic<bool> ImgSource::s_pump_enabled{ true };
		std::mutex ImgSource::s_load_mtx;
		std::atomic<bool> ImgSource::s_bg_loading{ false };
		std::map<int, long long> ImgSource::s_last_access;

		bool ImgSource::is_lazy(RootId id)
		{
			// 低频根：不参与登录/选角/进图即时依赖，prewarm 跳过，按需加载。
			// Effect/Etc：角色创建与游戏内特效/伤害皮肤；Quest/Reactor：任务/反应器。
			switch (id)
			{
				case RootId::Effect:
				case RootId::Etc:
				case RootId::Quest:
				case RootId::Reactor:
					return true;
				default:
					return false;
			}
		}

		void ImgSource::set_pump_enabled(bool enabled)
		{
			s_pump_enabled.store(enabled);
		}

		void ImgSource::set_bg_loading(bool loading)
		{
			s_bg_loading.store(loading);
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

		// 整文件批量读入（替代逐字节 istreambuf_iterator）：先取大小再一次性
		// read，去掉迭代器虚拟调用开销。失败 / 空文件返回空 vector（调用方跳过）。
		static std::vector<char> read_file_bulk(const fs::path& p)
		{
			std::error_code ec;
			const uintmax_t sz = fs::file_size(p, ec);
			if (ec || sz == 0 || sz > (1ull << 30))
				return {};
			std::ifstream f(p, std::ios::binary);
			if (!f)
				return {};
			std::vector<char> buf(static_cast<size_t>(sz));
			if (!f.read(buf.data(), static_cast<std::streamsize>(sz)))
				return {};
			return buf;
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
			// Wave 0.2 复核（grep 实测）：Map/Map001 **不可**移出 boot ——
			// 登录背景本体 = Map001["Back"]["login.img"]/["UI_login.img"]
			// （LoginBackground.cpp:168、UILogin.cpp:96-98、UICharSelect.cpp:214），
			// 木牌 = Map["Obj"]["login.img"]（UILogin:140 等）。曾试移除，
			// 会造成登录页空纹理，已撤销；启动占比问题改由 gather 剪枝解决。
			// Boot 仅装登录页立即需要的根（UI/Map/Map001/Sound/String）。
			// Effect/Etc 仅用于角色创建与游戏内特效/伤害皮肤等，登录页绘制
			// 不依赖，移入 prewarm 后台装载，缩短启动到登录页的时间。
			static const RootId kBootRoots[] = {
				RootId::UI,
				RootId::Map,
				RootId::Map001,
				RootId::Sound,
				RootId::String
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
				// 低频根（Effect/Etc/Quest/Reactor）延后到首次访问再装载，
				// 降低预热内存/CPU 峰值；用到时 ensure() 自动按需加载。
				if (is_lazy(id))
					continue;
				ensure(id);
				// 本函数可在后台线程跑（prewarm 第二阶段）：SDL 事件泵只许
				// 主线程，pump 关闭时跳过（与 ensure_char_select 同规）。
				if (s_pump_enabled.load())
					Window::get().poll_events();
			}

			LOG(LOG_INFO, "[IMG] ensure_remaining bound_roots="
				<< RootTable::bound_count() << "/" << RootTable::SIZE);
		}

		std::string ImgSource::memory_diagnostics()
		{
			std::ostringstream out;
			const long long now =
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now().time_since_epoch()).count();

			out << "roots=" << RootTable::bound_count() << "/" << RootTable::SIZE;

			// 低频根里尚未被实际访问、仍在延迟态的数量（预热未装载、按需才装）。
			int lazy_pending = 0;
			for (size_t i = 0; i < RootTable::SIZE; ++i)
			{
				RootId id = static_cast<RootId>(i);
				if (is_lazy(id) && !s_loaded.count(root_filename(id)))
					++lazy_pending;
			}
			if (lazy_pending)
				out << " lazy_pending=" << lazy_pending;

			out << " files=" << s_files.size() << " maps=" << s_map_files.size();

			// 最久未访问的已加载根（仅观测冷热分布；本后端不据此驱逐）。
			long long oldest = -1;
			std::string oldest_name;
			for (const auto& kv : s_last_access)
			{
				const long long age = now - kv.second;
				if (oldest < 0 || age > oldest)
				{
					oldest = age;
					oldest_name = root_filename(static_cast<RootId>(kv.first));
				}
			}
			if (oldest >= 0)
				out << " oldest=" << oldest_name << ":" << (oldest / 1000) << "s";

			return out.str();
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

			// s_map_files/LRU 环与 RootTable::bind 与后台预热线程的 s_files/bind
			// 并发（bind 直接写全局 nl::node 引用，无内部锁）——整段罩同一把
			// 装载锁。load_single_map 不回入 ensure()，无嵌套加锁。
			std::lock_guard<std::mutex> map(s_load_mtx);

			if (s_bound_mapid == mapid && RootTable::root(RootId::Map002))
				return;

			// 近张命中（回城/折返/传送往返极常见）：MAP_KEEP 环里已有该图，
			// 直接重绑走人 —— 跳过磁盘读 + img 解压，这两步是换图黑屏时长里
			// 最大的可消除项。命中后把该槽提到队尾（LRU 尾 = 最新，保活）。
			// 必须在 load_single_map 之前：放进读盘之后再判等于没有快路径。
			for (auto mi = s_map_files.begin(); mi != s_map_files.end(); ++mi)
			{
				if (mi->mapid != mapid)
					continue;

				RootTable::bind(RootId::Map002, mi->file->root());
				s_bound_mapid = mapid;

				if (mi + 1 != s_map_files.end())
				{
					map_slot hit = std::move(*mi);
					s_map_files.erase(mi);
					s_map_files.push_back(std::move(hit));
				}

				LOG(LOG_INFO, "[IMG] ensure_map rebind hit mapid=" << mapid);
				return;
			}

			// 冷路径：磁盘读 + img 解压 —— 换图黑时长里最大的可测量段。
			// perf_log 开启时落 [PERF] mapload 毫秒（Release 唯一观测通道）。
			const auto cold_t0 = std::chrono::steady_clock::now();

			load_single_map(mapid);

			{
				const auto cold_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - cold_t0).count();
				perf::mark("IMG", "mapload cold mapid=" + std::to_string(mapid)
					+ " ms=" + std::to_string(cold_ms));
			}

			Window::get().poll_events();
		}

		std::vector<std::pair<std::string, std::vector<char>>> ImgSource::gather(const std::string& dir)
		{
			std::vector<std::pair<std::string, std::vector<char>>> out;
			std::error_code ec;
			fs::path directory = fs::u8path(dir);
			if (!fs::is_directory(directory, ec))
				return out;

			// Map 根专用剪枝：顶层 Map/ 子树（实测 97MB、5364 个单图文件）
			// 全部是"读进来再被 load_map_helpers 的 Map/ 前缀过滤器丢弃"的
			// 死字节 —— 单图由 load_single_map 按路径直开，不经 gather。
			// 目录遍历阶段直接不下潜，省 IO + 省堆。
			// 仅当被扫目录本身叫 Map（= Data/Map，load_map_helpers 的传入）
			// 时生效；其余根（Character/Item/…）filename 不同，行为不变。
			// depth 语义为 MSVC 实测：根的直接子项 depth()==0 —— 第一版写成
			// ==1 永不命中（复测 Map=5733ms 无变化才暴露），已纠。
			const bool prune_map_subtree = (directory.filename() == "Map");

			// 阶段 1：单线程枚举（递归遍历 + Map 子树剪枝），收集 (rel, abs)。
			struct entry { std::string rel; fs::path abs; };
			std::vector<entry> files;
			for (auto it = fs::recursive_directory_iterator(directory,
				fs::directory_options::skip_permission_denied, ec);
				it != fs::recursive_directory_iterator(); ++it)
			{
				std::error_code lec;
				if (prune_map_subtree && it.depth() == 0 && it->is_directory(lec)
					&& it->path().filename() == "Map")
				{
					it.disable_recursion_pending();
					continue;
				}
				if (!it->is_regular_file(lec) || lec)
					continue;
				if (!iequals(it->path().extension().string(), ".img"))
					continue;
				std::string rel = it->path().lexically_relative(directory).generic_string();
				files.push_back({ std::move(rel), it->path() });
			}

			if (files.empty())
				return out;

			// 阶段 2：多线程批量读（IO 密集，文件数增长时收益明显）。
			// 限制线程数（≤8）避免句柄 / 上下文爆炸；每线程负责一段索引区间，
			// 写入互不重叠的 out[i]，无锁竞争。解析（open_merge）保持主线程单
			// 线程，仅把"读文件"这一步并行化，规避对解析器的侵入式改造。
			out.resize(files.size());
			const unsigned hw = std::thread::hardware_concurrency();
			const unsigned threads = std::max(1u, std::min(hw == 0 ? 4u : hw, 8u));
			const size_t n = files.size();
			const size_t per = (n + threads - 1) / threads;

			auto reader = [&](size_t start, size_t end)
			{
				for (size_t i = start; i < end && i < n; ++i)
				{
					std::vector<char> buf = read_file_bulk(files[i].abs);
					if (buf.empty())
						continue;
					out[i].first = std::move(files[i].rel);
					out[i].second = std::move(buf);
				}
			};

			if (threads <= 1 || n < 64)
			{
				reader(0, n);
			}
			else
			{
				std::vector<std::thread> pool;
				pool.reserve(threads);
				for (unsigned t = 0; t < threads; ++t)
				{
					const size_t start = t * per;
					const size_t end = std::min(start + per, n);
					if (start >= end)
						break;
					pool.emplace_back(reader, start, end);
				}
				for (auto& th : pool)
					th.join();
			}

			// 过滤读取失败的条目（out[i].first 为空 = 未成功读取）。
			std::vector<std::pair<std::string, std::vector<char>>> cleaned;
			cleaned.reserve(out.size());
			for (auto& kv : out)
				if (!kv.first.empty())
					cleaned.push_back(std::move(kv));
			out = std::move(cleaned);

			// 大目录扫描后主线程泵消息，避免“未响应”。
			// （后台线程装载时跳过：SDL 事件只允许主线程泵，主循环照常泵。）
			if (s_pump_enabled.load())
				Window::get().poll_events();

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
			s_map_files.push_back({ mapid, std::move(file) });

			// 长跑内存治理（2026-11-04 审计坐实的泄漏）：原先每次换图向 s_files
			// append 一个解压后的地图树（数十 KB~MB 级）且只增不减，反复换图
			// = 单调增长。但 nlnx node 引用 img_file 内部缓冲，换绑瞬间不能
			// 销毁旧 file（悬垂）——故用有界 FIFO：保留最近 MAP_KEEP 张
			// （当前图 + 前两张宽限窗，覆盖旧图 sprite/atlas 残留引用），
			// 超出即销毁最老。上界 3×单图体积，长跑恒定。
			constexpr size_t MAP_KEEP = 3;

			while (s_map_files.size() > MAP_KEEP)
				s_map_files.erase(s_map_files.begin());

			s_bound_mapid = mapid;
			s_loaded.insert("Map002");

			LOG(LOG_INFO, "[IMG] ensure_map ok mapid=" << mapid
				<< " file=" << hit.generic_string());
		}

		nl::node ImgSource::ensure_map_query(int32_t mapid)
		{
			if (mapid <= 0)
				return nl::node{};

			std::lock_guard<std::mutex> map(s_load_mtx);

			// 【预绑定后端 · 2026-11-21 回归修复】wz / nx / .nxcache mmap 在 open()
			// 时已把全部地图绑进 Map002，直接只读查询即可命中。关键：这里**绝不**
			// 调用 ensure_map() 重绑 Map002、也绝不淘汰 s_map_files —— 那会让当前
			// 渲染持有的 nl::node 悬垂，正是 0xc0000374 堆损坏的根因（见
			// docs/features/worldmap-hover-crash-rootfix-2026-11/README.md）。
			// 本分支纯读，不写任何全局根；节点在查询期间有效，调用方（get_life_on_map/
			// get_mobs_on_map）即时把字段拷贝成 std::string/数值，不长期持有。
			{
				const int pre = static_cast<int>(mapid / 100000000);
				std::ostringstream preoss;
				preoss << std::setw(9) << std::setfill('0') << mapid;
				const std::string preid = preoss.str();
				nl::node prebound = RootTable::root(RootId::Map002);
				if (prebound)
				{
					nl::node hit = prebound["Map"]["Map" + std::to_string(pre)][preid + ".img"];
					if (hit)
						return hit;
				}
			}

			// 命中查询缓存：提到队尾（MRU），不碰全局 Map002 / s_map_files。
			for (auto qi = s_query_files.begin(); qi != s_query_files.end(); ++qi)
			{
				if (qi->mapid != mapid)
					continue;

				nl::node root = qi->file->root();
				if (qi + 1 != s_query_files.end())
				{
					map_slot hit_slot = std::move(*qi);
					s_query_files.erase(qi);
					s_query_files.push_back(std::move(hit_slot));
				}
				const int prefix = static_cast<int>(mapid / 100000000);
				std::ostringstream idoss;
				idoss << std::setw(9) << std::setfill('0') << mapid;
				return root["Map"]["Map" + std::to_string(prefix)][idoss.str() + ".img"];
			}

			auto it = s_dirs.find("Map");
			if (it == s_dirs.end())
				return nl::node{};

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
				LOG(LOG_ERROR, "[IMG] ensure_map_query: map file missing mapid=" << mapid);
				return nl::node{};
			}

			std::ifstream f(hit, std::ios::binary);
			if (!f)
				return nl::node{};

			std::vector<char> bytes((std::istreambuf_iterator<char>(f)),
				std::istreambuf_iterator<char>());
			if (bytes.empty())
				return nl::node{};

			const std::string newrel = "Map/Map" + std::to_string(prefix) + "/" + basename;
			std::vector<std::pair<std::string, std::vector<char>>> maps;
			maps.emplace_back(newrel, std::move(bytes));

			auto file = std::make_unique<nl::img_file>();
			file->open_tree(std::move(maps));
			nl::node root = file->root();
			s_query_files.push_back({ mapid, std::move(file) });

			while (s_query_files.size() > QUERY_MAP_KEEP)
				s_query_files.erase(s_query_files.begin());

			LOG(LOG_INFO, "[IMG] ensure_map_query ok mapid=" << mapid
				<< " file=" << hit.generic_string());

			return root["Map"]["Map" + std::to_string(prefix)][id9 + ".img"];
		}

		void ImgSource::ensure(RootId id)
		{
			// 访问计时（内存诊断）：每次 ensure 都刷新该根最后访问时间，
			// 用于观测常驻根的冷热分布。本后端不驱逐已加载根（释放=node 悬垂），
			// 该计时仅诊断用。
			s_last_access[static_cast<int>(id)] =
				std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now().time_since_epoch()).count();

			// 并行预热改造：ensure_remaining 现在跑在后台线程，主线程的
			// ensure_char_select()/ensure_map(→ensure(Map)) 与之并发。
			// s_loaded/s_files/RootTable 是裸容器：判重必须与解析、绑定同罩
			// 一把锁，否则双线程各看见"未加载"就各解析一遍、双绑同一根。
			// 锁粒度=单根（最长秒级）；主线程最多在淡入黑场里等一根。
			std::lock_guard<std::mutex> load(s_load_mtx);

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

			// Wave 0.2 仪器：单根装载（gather 读盘 + open_merge 解析 + bind）
			// 分段计时落 boot_perf.log（debug.perf_log）。resource 段占冷启动
			// 87%，先拿每根占比再决定并行/收窄的刀口。RAII：三个出口都记。
			struct root_timer
			{
				const std::string* name;
				std::chrono::steady_clock::time_point t0;
				~root_timer()
				{
					const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::steady_clock::now() - t0).count();
					perf::mark("ROOT", *name + "=" + std::to_string(ms) + "ms");
				}
			} rtimer{ &phys, std::chrono::steady_clock::now() };

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
			// 退出/切后端与后台预热竞态防护：detached 预热线程可能正在
			// s_load_mtx 内解析绑定，此时主线程 clear() 裸容器 = UB。
			// 等 s_bg_loading 自然落定（后台在装载循环末尾清零），再持锁
			// 清表 —— 只验一次+拿锁再放，会漏"标志已置但线程还在排队拿锁"
			// 的窗口。常态（进游戏后才可能退出）标志早已为 false，零等待。
			// [退出防护] 加 60s 上限：detach 线程异常未落标志时不能永久自旋
			// 卡死退出；超时后按"装载已停滞"处理，继续清表（互斥锁仍保护容器）。
			const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
			while (s_bg_loading.load())
			{
				if (std::chrono::steady_clock::now() >= deadline)
				{
					LOG(LOG_WARN, "[IMG] close(): bg_loading wait timed out, proceed");
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}

			std::lock_guard<std::mutex> load(s_load_mtx);

			s_files.clear();
			s_map_files.clear();
			s_query_files.clear();
			s_dirs.clear();
			s_loaded.clear();
			s_bound_mapid = -2;
			RootTable::clear();
		}
	}
}
