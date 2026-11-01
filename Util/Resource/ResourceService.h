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
#pragma once

// 资源加载门面（Facade）。
//
// 对外只暴露这一处入口：选择后端（策略）、装载、取根、诊断。
// 业务层与 ms::init() 都不再直接认识 Util/{DataFiles,ImgFiles,WzFiles,NxFiles}，
// 从而把"数据在哪里、是什么格式"收敛到资源层内部。
//
// 兼容策略：装载完成后仍然把 28 个根写回 nl::nx::* 全局变量（见 RootTable），
// 因此既有上千处 nl::nx::Map[...] 调用点无需修改。

#include "IDataSource.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>

namespace ms
{
	namespace res
	{
	class ResourceService
	{
	public:
		// 进程内唯一实例：数据源必须只装载一次，重复装载会重复映射文件。
		static ResourceService& get();

		// 按配置选择后端并完成装载。重复调用是安全的（幂等）。
		Error init();

		// img：登录成功后装选角根并重建 BodyDrawInfo；wz/nx 为 no-op。
		void ensure_game_roots();

		// img：启动后台线程预热选角根（与登录界面并行）；wz/nx 立即就绪。
		// 只启动一次，后续调用是 no-op。用于消除登录响应后同步装载
		// 冻结 Wait 弹窗半分钟级的问题（"连接加载条卡住不动"）。
		void prewarm_game_roots();

		// 预热是否完成（未启动过预热且非 img 格式时恒 true）。
		bool game_roots_ready() const noexcept;

		// img：进图前补齐 Mob 等剩余根（不含全量 Map002）；wz/nx 为 no-op。
		void ensure_map_roots();

		// img：按需装载单张地图到 Map002；wz/nx 为 no-op。
		void ensure_map(int32_t mapid);

		// 取某个标准根；未装载时返回空 node。
		nl::node root(RootId id) const;

		// 当前生效的格式；未装载时为 Unknown。
		Format format() const noexcept;

		// 是否已经装载成功。
		bool loaded() const noexcept;

		// 本次实际使用的数据根目录（定位器的命中结果）。
		const std::string& base() const noexcept;

		// 诊断信息：格式、后端名、已绑定/缺失的根、能力标记。
		// verbose 时额外列出每一个缺失的根名字。
		std::string diagnostics(bool verbose) const;

		// 释放当前后端（数据源切换前调用）。
		void close();

	private:
		ResourceService() = default;

		ResourceService(const ResourceService&) = delete;
		ResourceService& operator=(const ResourceService&) = delete;

		std::unique_ptr<IDataSource> backend_;
		std::string base_;

		std::atomic<bool> prewarm_started_{ false };
		std::atomic<bool> game_roots_ready_{ false };
	};
	}
}
