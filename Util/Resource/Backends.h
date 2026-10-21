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

// 数据源适配器（Adapter）。
//
// 每个类把一种既有物理格式的装载逻辑包装成统一的 IDataSource 契约，
// 使得 ResourceService 不需要知道任何 parser 细节。
//
// P1 阶段刻意保持**行为等价**：适配器只是调用既有的 WzFiles / NxFiles /
// ImgFiles::init()，不改动它们的装载与校验规则（例如 UI 版本校验目前只由
// wz / nx 后端执行，门面不在统一层面追加，避免在 img 数据上产生新的启动失败）。

#include "IDataSource.h"

#include <memory>

namespace ms
{
	namespace res
	{
	// 直接读取官方 .wz 归档（Util/WzRuntime.cpp 的运行时直读实现）。
	class WzBackend : public IDataSource
	{
	public:
		Format format() const noexcept override { return Format::Wz; }
		const char* name() const noexcept override { return "wz"; }

		ProbeResult probe(const std::string& candidate) const override;
		Error open(const OpenParams& params) override;
		nl::node root(RootId id) const override;

		Capabilities caps() const noexcept override
		{
			// parse_file() 一次性解析元数据，但像素/音频都是按需取用的。
			return { false, true, true, false };
		}

		void close() override;
	};

	// 读取 NoLifeNx 归档（.nx），真正的惰性：文件映射 + 按需解码。
	class NxBackend : public IDataSource
	{
	public:
		Format format() const noexcept override { return Format::Nx; }
		const char* name() const noexcept override { return "nx"; }

		ProbeResult probe(const std::string& candidate) const override;
		Error open(const OpenParams& params) override;
		nl::node root(RootId id) const override;

		Capabilities caps() const noexcept override
		{
			return { true, true, true, false };
		}

		void close() override;
	};

	// 工厂：为每种受支持格式各构造一个后端实例。
	//
	// 定位器（ResourceLocator）用它对候选目录做自省，门面则按定位结果
	// 只保留真正需要的那一个。返回向量的所有权交给调用方。
	std::vector<std::unique_ptr<IDataSource>> make_backends();

	// 读取经典独立 .img 目录。
	//
	// 注意：当前实现仍会把 .img 转换并缓存为 .nx（needs_write = true），
	// P3 将替换为"零转换"的惰性后端，届时该能力标记与实现一并更新。
	class ImgBackend : public IDataSource
	{
	public:
		Format format() const noexcept override { return Format::Img; }
		const char* name() const noexcept override { return "img"; }

		ProbeResult probe(const std::string& candidate) const override;
		Error open(const OpenParams& params) override;
		nl::node root(RootId id) const override;

		Capabilities caps() const noexcept override
		{
			// 零转换（I2）：不再落盘 .nxcache，装载即经 nl::node 直接读取；
			// 各根按需懒加载（per-root 惰性），但单根内 node 仍由 NX 层惰性访问。
			return { false, true, true, false };
		}

		void close() override;
	};
	}
}
