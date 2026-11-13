//////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton					//
//																			//
//	Fyms085 resource abstraction additions Copyright (C) 2026				//
//																			//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.										//
//																			//
//	This program is distributed in the hope that it will be useful,			//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.						//
//																			//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
//////////////////////////////////////////////////////////////////////////////
#include "ResourceService.h"

#include "../../Character/Look/CharLook.h"
#include "../../Configuration.h"
#include "../../MapleStory.h"
#include "Backends.h"
#include "ImgSource.h"
#include "ResourceLocator.h"
#include "RootTable.h"

#include <sstream>
#include <thread>
#include <vector>

#include <windows.h>
#include <psapi.h>

#include "../../Graphics/GraphicsGL.h"
#include "../../IO/Window.h"
#include "../../Util/PerfLog.h"

namespace ms
{
	namespace res
	{
	ResourceService& ResourceService::get()
	{
		static ResourceService instance;

		return instance;
	}

	Error ResourceService::init()
	{
		// 幂等：文件映射/归档句柄重复建立既浪费又会让节点失效。
		if (backend_)
			return Error::Code::NONE;

		const std::string setting = Configuration::get().get_data_format();

		// 期望格式：auto（或空）表示交给定位器自动挑；其余为显式指定。
		// 未知取值必须报错，不能静默回退到某个后端，
		// 否则"配置写错"会被伪装成"数据有问题"，排障方向被带偏。
		Format wanted = Format::Unknown;

		if (setting == "auto" || setting.empty())
			wanted = Format::Unknown;
		else if (setting == "img")
			wanted = Format::Img;
		else if (setting == "nx")
			wanted = Format::Nx;
		else if (setting == "wz")
			wanted = Format::Wz;
		else
			return Error(Error::Code::DATA_FORMAT, setting.c_str());

		// 自适应定位：在候选路径链中找数据；显式格式也会参与候选链寻找位置。
		const Layout layout = ResourceLocator::locate(wanted);

		if (!layout.found)
		{
			// 错误信息带上尝试过的全部候选目录，避免只说"找不到文件"而无从排障。
			// Error 只保存指针，故用静态存储持有该字符串。
			static std::string message;
			std::ostringstream out;

			out << "no usable data source found";

			if (wanted != Format::Unknown)
				out << " for format \"" << setting << "\"";

			out << "; tried: " << ResourceLocator::describe_tried(layout);

			message = out.str();

			return Error(Error::Code::MISSING_FILE, message.c_str());
		}

		// 按定位结果构造对应后端。
		std::unique_ptr<IDataSource> backend;

		switch (layout.format)
		{
		case Format::Img:
			backend = std::make_unique<ImgBackend>();
			break;
		case Format::Nx:
			backend = std::make_unique<NxBackend>();
			break;
		case Format::Wz:
			backend = std::make_unique<WzBackend>();
			break;
		default:
			return Error(Error::Code::DATA_FORMAT, setting.c_str());
		}

		// 后端装载成功即完成根绑定（写入 nl::nx::*），
		// 校验规则仍由各后端自行决定，门面不在此追加统一校验。
		OpenParams params;
		params.base = layout.base;

		if (Error error = backend->open(params))
		{
			backend->close();

			return error;
		}

		backend_ = std::move(backend);
		base_ = layout.base;

		// 上线级诊断：便于确认生产是否命中 img，以及 BeiDou 缺哪些根。
		LOG(LOG_INFO, "[RES] " << diagnostics(true) << " base=" << base_);

		return Error::Code::NONE;
	}

	void ResourceService::ensure_game_roots()
	{
		if (!backend_)
			return;

		// 仅堆式 ImgSource 需要显式装载选角根；mmap / wz / nx 在 open() 已绑好
		// 全部根。注意：这里只门控“装根”，下面的 CharLook::init() 必须无条件执行。
		if (backend_->requires_preload())
		{
			// 选角只需外观相关根；全量 ensure_remaining 会卡登录半分钟级。
			ImgSource::ensure_char_select();
			LOG(LOG_INFO, "[RES] ensure_game_roots (char-select) " << diagnostics(false));
		}

		// 关键：无论何种后端，Character 就绪后都必须重建 BodyDrawInfo 并清空空
		// 锚点缓存。此前把整段用 requires_preload 门控，导致 mmap 后端下
		// CharLook::init() 被跳过 → BodyDrawInfo 空表 → 选角头/身/脚叠在一起、
		// 进图渲染异常（2026-11-13 回归修复）。
		CharLook::init();
	}

	void ResourceService::prewarm_game_roots()
	{
		if (prewarm_started_.exchange(true))
			return;

		// 预绑定后端（wz / nx / .nxcache mmap）已在 open() 绑好全部根，直接放行；
		// 仅堆式 ImgSource 需要后台 ensure_remaining。
		if (!backend_ || !backend_->requires_preload())
		{
			game_roots_ready_.store(true);
			return;
		}

		// 与登录界面并行装载选角根。装载全程禁用 ImgSource 内部的
		// poll_events（SDL 事件只允许主线程泵，主循环照常泵）。
		std::thread([this]()
		{
			ImgSource::set_bg_loading(true);
			ImgSource::set_pump_enabled(false);
			ensure_game_roots();
			// 就绪标志必须紧跟选角根：LoginResultHandler 登录成功时泵 Wait
			// 弹窗等的就是这个门。
			// 【2026-11-04 自纠回归】上一批我把 store(true) 挪到了
			// ensure_remaining 之后 —— 等于让登录等待吃满全量预热（ROOT 实测
			// 链 Mob 7583ms / Character 5293ms / Npc 1706ms…合计 ~25s），
			// 把"消除进图 7.5s"的收益反向加进了登录段。现在恢复早放行：
			// 选角根就绪即放行，全量根继续后台跑（ensure() 已互斥，主线程
			// ensure_map_roots 撞上进行中的后台装载 = 拿锁等一根，仍优于
			// 旧的整段同步 7.5s）。
			game_roots_ready_.store(true);
			ImgSource::ensure_remaining();
			ImgSource::set_pump_enabled(true);
			ImgSource::set_bg_loading(false);
			LOG(LOG_INFO, "[RES] prewarm done (char-select released early, remaining in bg)");
		}).detach();
	}

	bool ResourceService::game_roots_ready() const noexcept
	{
		return game_roots_ready_.load();
	}

	void ResourceService::ensure_map_roots()
	{
		if (!backend_)
			return;

		// 预绑定后端（wz / nx / .nxcache）已在 open() 绑好全部根，无需全量 ensure_remaining。
		if (!backend_->requires_preload())
			return;

		ImgSource::ensure_remaining();
		LOG(LOG_INFO, "[RES] ensure_map_roots done " << diagnostics(false));
	}

	void ResourceService::ensure_map(int32_t mapid)
	{
		if (!backend_)
			return;

		// 预绑定后端（wz / nx / .nxcache）已在 open() 把全部地图绑入 Map002 根，
		// 无需逐图 ensure_map（堆式 ImgSource 才需要按需装载单图）。
		if (!backend_->requires_preload())
			return;

		ImgSource::ensure_map(mapid);
	}

	nl::node ResourceService::root(RootId id) const
	{
		return backend_ ? backend_->root(id) : nl::node{};
	}

	Format ResourceService::format() const noexcept
	{
		return backend_ ? backend_->format() : Format::Unknown;
	}

	bool ResourceService::loaded() const noexcept
	{
		return backend_ != nullptr;
	}

	const std::string& ResourceService::base() const noexcept
	{
		return base_;
	}

	std::string ResourceService::diagnostics(bool verbose) const
	{
		std::ostringstream out;

		out << "format=" << to_string(format());

		if (backend_)
			out << " backend=" << backend_->name();
		else
			out << " backend=none";

		out << " roots=" << RootTable::bound_count() << "/" << RootTable::SIZE;

		if (backend_)
		{
			const Capabilities caps = backend_->caps();

			out << " caps=["
				<< (caps.lazy_nodes ? "lazy-nodes " : "")
				<< (caps.lazy_bitmap ? "lazy-bitmap " : "")
				<< (caps.audio_stream ? "audio-stream " : "")
				<< (caps.needs_write ? "writes-cache" : "")
				<< "]";
		}

		const std::vector<RootId> missing = RootTable::missing_roots();

		if (!missing.empty())
		{
			out << " missing=" << missing.size();

			if (verbose)
			{
				out << " (";

				for (size_t i = 0; i < missing.size(); i++)
				{
					if (i)
						out << ", ";

					out << to_string(missing[i]);
				}

				out << ")";
			}
		}

		return out.str();
	}

	void ResourceService::close()
	{
		if (backend_)
		{
			backend_->close();

			backend_.reset();
		}
	}

	void ResourceService::sample_memory()
	{
		// 仅当 debug.perf_log 开启时落盘（Release 下 LOG 为 no-op，perf 通道独立）。
		// 复用 PerfLog 的开关类型，避免新增配置项；perf::mark 内部另有 2s 缓存门控。
		if (!Setting<PerfLogEnabled>::get().load())
			return;

		// 进程私有提交内存（Private Bytes）。用扩展结构 PROCESS_MEMORY_COUNTERS_EX
		// 取 PrivateUsage；动态取 psapi，避免链接 Psapi.lib。
		SIZE_T private_bytes = 0;
		HMODULE hpsapi = LoadLibraryA("psapi.dll");
		if (hpsapi)
		{
			using Fn = BOOL(WINAPI*)(HANDLE, PPROCESS_MEMORY_COUNTERS_EX, DWORD);
			Fn fn = reinterpret_cast<Fn>(GetProcAddress(hpsapi, "GetProcessMemoryInfo"));
			if (fn)
			{
				PROCESS_MEMORY_COUNTERS_EX pmc;
				pmc.cb = sizeof(pmc);
				if (fn(GetCurrentProcess(), &pmc, sizeof(pmc)))
					private_bytes = pmc.PrivateUsage;
			}
			FreeLibrary(hpsapi);
		}

		const double atlas = GraphicsGL::get().atlas_usage_percent();
		const int fps = Window::get().get_perf_fps();
		const std::string roots = ImgSource::memory_diagnostics();

		std::ostringstream oss;
		oss << "privateMB=" << (static_cast<double>(private_bytes) / (1024.0 * 1024.0))
			<< " atlas%=" << (atlas * 100.0)
			<< " fps=" << fps
			<< " " << roots;
		perf::mark("MEM", oss.str());
	}
	}
	}
