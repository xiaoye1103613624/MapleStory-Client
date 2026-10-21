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

// 资源加载层的**策略接口**（Strategy）。
//
// 这里只定义契约，不依赖任何具体 parser：img / wz / nx 各自提供一个适配器实现，
// ResourceService 只面向本接口工作，因此新增一种物理格式不需要改动门面与业务层。
//
// 与上层的关系：业务层仍然直接使用 nl::nx::<Root>[...] 这些全局节点，
// 后端在 open() 时把根节点绑定回这些全局变量（见 RootTable），
// 所以本次抽象对上千处既有调用点是**零改动**的。

#include "../../Error.h"

#include <nlnx/node.hpp>

#include <cstdint>
#include <string>
#include <vector>

// 说明：本头文件刻意**不**使用 <filesystem> 与嵌套命名空间语法，
// 以便仍按 C++14 编译的翻译单元（例如 MapleStory.cpp）也能包含它。
// 路径统一用 std::string 表示，由各适配器的实现（C++17）自行转换。
namespace ms
{
	namespace res
	{

	// 物理数据格式。
	// Img = 经典独立 .img 目录；Nx = NoLifeNx 归档；Wz = 官方 .wz 归档。
	enum class Format : uint8_t
	{
		Unknown = 0,
		Img = 1,
		Nx = 2,
		Wz = 3
	};

	// 28 个标准根，顺序与 nl::nx::* 现有全局变量、以及 Util/*Files.h 中的
	// filenames 保持一致（顺序改动会导致根绑定错位，属破坏性变更）。
	enum class RootId : uint8_t
	{
		Base = 0,
		Character,
		Effect,
		Etc,
		Item,
		Map,
		Map001,
		Map002,
		Map2,
		Mob,
		Mob001,
		Mob002,
		Mob2,
		Morph,
		Npc,
		Quest,
		Reactor,
		Skill,
		Skill001,
		Skill002,
		Skill003,
		Sound,
		Sound001,
		Sound002,
		Sound2,
		String,
		TamingMob,
		UI,

		Count
	};

	// 后端能力描述：决定门面可以施加哪些优化（延迟装载、按需解码等）。
	struct Capabilities
	{
		// 子节点是否按需解析（惰性），而非启动时全量展开。
		bool lazy_nodes = false;
		// 位图像素是否按需解码。
		bool lazy_bitmap = false;
		// 音频是否指向归档/文件映射而不需要整包读入。
		bool audio_stream = false;
		// 装载过程是否会产生中间文件（例如把 .img 转成 .nx 缓存）。
		bool needs_write = false;
	};

	// 装载参数。base 为数据所在的根目录（P2 的自适应路径会填充它，
	// P1 阶段沿用既有行为，即由各后端自行从配置/工作目录解析）。
	struct OpenParams
	{
		// 数据根目录；为空时由后端按既有规则（配置/工作目录）自行解析。
		std::string base;
		bool verbose = false;
	};

	// 自省结果：给定候选目录，本后端能否在此装载。
	// score 供"自动选择"比较多个候选（P2），usable 表示是否可作为最终选择。
	struct ProbeResult
	{
		bool usable = false;
		int score = 0;
		std::string base;
		std::vector<RootId> roots_hit;
	};

	// 数据源策略接口。
	//
	// 生命周期固定为：probe() 自省 -> open() 装载并绑定 28 个根
	// -> root() 取用 -> close() 释放。由 ResourceService 统一驱动。
	class IDataSource
	{
	public:
		virtual ~IDataSource() = default;

		// 本后端处理的物理格式。
		virtual Format format() const noexcept = 0;

		// 稳定的名字，用于日志与诊断（"img" / "nx" / "wz"）。
		virtual const char* name() const noexcept = 0;

		// 自省：候选目录下是否存在本后端可用的数据。
		virtual ProbeResult probe(const std::string& candidate) const = 0;

		// 装载：绑定全部标准根；缺失的根留为空节点（既有宽松语义，不致命）。
		virtual Error open(const OpenParams& params) = 0;

		// 取根节点；未绑定时返回空 node（上层按 NoLifeNx 语义安全使用）。
		virtual nl::node root(RootId id) const = 0;

		virtual Capabilities caps() const noexcept = 0;

		virtual void close() = 0;
	};

	// 把 Format / RootId 转成可读文本，供诊断输出使用。
	const char* to_string(Format format) noexcept;
	const char* to_string(RootId id) noexcept;

	// RootId 与标准根文件名（不含扩展名）的映射。
	const char* root_filename(RootId id) noexcept;
	}
}
