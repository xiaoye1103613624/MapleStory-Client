//////////////////////////////////////////////////////////////////////////////
//	Fyms085 resource abstraction additions Copyright (C) 2026
//	IMG 零转换后端（I2）：把经典独立 .img 目录在内存中解析为 nl::node，
//	不落盘 .nxcache、不写中间 .nx 文件。
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IDataSource.h"

#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

// 前置声明，避免向包含本头的翻译单元暴露 nlnx 内部头（保持 C++14 兼容）。
namespace nl
{
	class img_file;
}

namespace ms
{
	namespace res
	{
		// 零转换 IMG 数据源：扫描 base 下 16 标准根目录，按需（懒加载）把每个根
		// 的内存 .img 字节经 nl::img_file 解析为 nl::node，并绑定到 RootTable /
		// nl::nx::*。
		//
		// 关键特性：
		//  - 不落盘：直接内存解析，删除 .nxcache 后仍能挂载成功；
		//  - 视图别名：Map 与 Map001 共享同一份 helper 解析结果（零复制）；
		//  - 惰性：init 只记录目录，root(id) 首次访问时才解析该根（per-root 惰性）。
		class ImgSource
		{
		public:
			// 扫描并记录 base 下 16 标准根目录（不解析内容）。
			static Error init(const std::string& base_dir);

			// 懒加载：确保某根已经解析并绑定（首次访问时触发，重复调用安全）。
			static void ensure(RootId id);

			// 装载启动未绑的剩余根（选角/进图前调用）。重复调用安全。
			static void ensure_remaining();

			// 登录成功后先装选角所需根（Character/Item/…），进图再 ensure_remaining。
			static void ensure_char_select();

			// 按需装载单张地图到 Map002（避免进图时扫全 Map/ 导致未响应）。
			static void ensure_map(int32_t mapid);

			// 释放所有 img_file 实例与目录记录，并清空 RootTable。
			static void close();

			// 后台线程装载开关：SDL 事件只允许主线程泵，后台装载期间
			// 关闭 gather()/ensure_*() 内部的 poll_events（主循环照常泵）。
			static void set_pump_enabled(bool enabled);

			// 后台装载在途标志 setter（close() 竞态防护用，由
			// ResourceService 预热线程在装载任务首尾设置）。
			static void set_bg_loading(bool loading);

		private:
			// 持久持有 img_file 实例，直到 close()，避免 node 引用的缓冲悬垂。
			static std::vector<std::unique_ptr<nl::img_file>> s_files;
			// 换图单图 file 专用队列（2026-11-04 长跑内存治理）：与标准根分离，
			// 只保留最近 MAP_KEEP 张，超出从最老端销毁——连续换图内存有界。
			// 带 mapid：回城/折返命中近张时直接重绑，不再走磁盘读+解压。
			struct map_slot
			{
				int32_t mapid;
				std::unique_ptr<nl::img_file> file;
			};
			static std::vector<map_slot> s_map_files;
			// 标准根名 -> 目录路径（字符串，避免头暴露 filesystem）。
			static std::map<std::string, std::string> s_dirs;
			// 已加载的物理根标识（标准根名；Map 的 helper 标识为 "Map"，地图标识为 "Map002"）。
			static std::set<std::string> s_loaded;
			// 当前已绑进 Map002 的 mapid（换图时重绑）。
			static int32_t s_bound_mapid;
			// 消息泵开关（见 set_pump_enabled）。
			static std::atomic<bool> s_pump_enabled;

			// 装载互斥（2026-11-04 并行预热改造）：ensure_remaining 改由后台
			// 线程跑后，主线程的 ensure()/ensure_map() 与之并发。s_loaded/
			// s_files/RootTable 都是裸容器，判重必须与解析绑定同罩在锁内，
			// 否则双线程各看见"未加载"就各解析一遍、双绑同一根。锁粒度=
			// 单根解析（最长秒级），主线程在淡入黑场里等，不构成可见卡顿。
			static std::mutex s_load_mtx;
			// 后台装载在途标志：close()（退出/切后端）先等它落定再清表，
			// 防 detached 线程正在写 s_files 时被主线程 clear（UB）。
			static std::atomic<bool> s_bg_loading;

			// 递归收集目录下所有 .img 字节，relpath 相对 dir。
			static std::vector<std::pair<std::string, std::vector<char>>> gather(const std::string& dir);

			static void load_standard(const std::string& name, RootId id);
			static void load_map_helpers(const std::string& mapdir);
			static void load_map_maps(const std::string& mapdir);
			static void load_single_map(int32_t mapid);
		};
	}
}
