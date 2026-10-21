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
#include "Backends.h"

// MapleStory.h 提供后端开关（如 USE_NX），数据层头文件依赖它。
#include "../../MapleStory.h"

#include "../../Configuration.h"
#include "../ImgFiles.h"
#include "ImgSource.h"
#include "../NxFiles.h"
#include "../WzFiles.h"
#include "RootTable.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace ms
{
	namespace res
	{
	namespace
	{
		// 统计候选目录下本格式的标准根文件数量，作为自省依据。
		// 目录里通常不会 28 个根全齐（例如 BeiDou 数据不带 Base），
		// 因此只要能命中一个根就认为该目录"可能是"本格式的部署目录。
		ProbeResult probe_files(const std::string& candidate, const char* extension)
		{
			ProbeResult result;
			result.base = candidate;

			std::error_code ec;
			const fs::path directory = fs::u8path(candidate);

			if (!fs::is_directory(directory, ec))
				return result;

			for (size_t i = 0; i < RootTable::SIZE; i++)
			{
				const RootId id = static_cast<RootId>(i);
				const fs::path file = directory / (std::string(root_filename(id)) + extension);

				if (fs::exists(file, ec))
				{
					result.roots_hit.push_back(id);
					result.score++;
				}
			}

			// img 布局不是"根名 + 扩展名"的平铺文件，而是"根名/ 目录"，
			// 由 ImgBackend::probe 单独处理；这里只针对 wz / nx 的平铺布局。
			result.usable = !result.roots_hit.empty();

			return result;
		}
	}

	std::vector<std::unique_ptr<IDataSource>> make_backends()
	{
		std::vector<std::unique_ptr<IDataSource>> backends;

		backends.push_back(std::make_unique<WzBackend>());
		backends.push_back(std::make_unique<NxBackend>());
		backends.push_back(std::make_unique<ImgBackend>());

		return backends;
	}

	ProbeResult WzBackend::probe(const std::string& candidate) const
	{
		return probe_files(candidate, ".wz");
	}

	Error WzBackend::open(const OpenParams& params)
	{
		// params.base 为空时由 WzFiles 回退到既有行为（相对工作目录）。
		return ms::WzFiles::init(params.base);
	}

	nl::node WzBackend::root(RootId id) const
	{
		return RootTable::root(id);
	}

	void WzBackend::close()
	{
		RootTable::clear();
	}

	ProbeResult NxBackend::probe(const std::string& candidate) const
	{
		return probe_files(candidate, ".nx");
	}

	Error NxBackend::open(const OpenParams& params)
	{
		// params.base 为空时由 NxFiles 回退到既有行为（相对工作目录）。
		return ms::NxFiles::init(params.base);
	}

	nl::node NxBackend::root(RootId id) const
	{
		return RootTable::root(id);
	}

	void NxBackend::close()
	{
		RootTable::clear();
	}

	ProbeResult ImgBackend::probe(const std::string& candidate) const
	{
		ProbeResult result;
		result.base = candidate;

		std::error_code ec;
		const fs::path directory = fs::u8path(candidate);

		if (!fs::is_directory(directory, ec))
			return result;

		// img 布局：候选目录下应存在若干"标准根同名子目录"。
		for (size_t i = 0; i < RootTable::SIZE; i++)
		{
			const RootId id = static_cast<RootId>(i);

			if (fs::is_directory(directory / root_filename(id), ec))
			{
				result.roots_hit.push_back(id);
				result.score++;
			}
		}

		// 至少要有数个标准根子目录，才能认为这是 .img 数据目录，
		// 避免把任意含子目录的文件夹误判成数据目录。
		result.usable = result.roots_hit.size() >= 4;

		return result;
	}

	Error ImgBackend::open(const OpenParams& params)
	{
		// params.base 为空时由 ImgSource 回退到配置中的 data.dir（零转换 IMG 后端）。
		return ImgSource::init(params.base);
	}

	nl::node ImgBackend::root(RootId id) const
	{
		// 惰性装载：首次访问某根时才解析该根（I2 零转换的 per-root 惰性）。
		ImgSource::ensure(id);
		return RootTable::root(id);
	}

	void ImgBackend::close()
	{
		ImgSource::close();
	}
	}
}
