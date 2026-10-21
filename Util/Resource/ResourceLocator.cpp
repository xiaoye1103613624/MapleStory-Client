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
#include "ResourceLocator.h"

#include "../../Configuration.h"
#include "Backends.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace fs = std::filesystem;

namespace ms
{
	namespace res
	{
		std::string ResourceLocator::executable_dir()
		{
#ifdef _WIN32
			// 用可执行文件的位置，而不是当前工作目录：
			// 从快捷方式/其它目录启动时，CWD 未必是部署目录。
			wchar_t buffer[MAX_PATH] = {};

			if (::GetModuleFileNameW(nullptr, buffer, MAX_PATH) == 0)
				return {};

			std::error_code ec;
			const fs::path path = fs::path(buffer).parent_path();

			return fs::absolute(path, ec).string();
#else
			// 非 Windows 平台没有统一的可执行文件路径接口，
			// 退回工作目录（Linux/macOS 下通常以程序目录启动）。
			std::error_code ec;

			return fs::current_path(ec).string();
#endif
		}

		std::vector<std::string> ResourceLocator::candidates()
		{
			std::vector<std::string> out;

			auto push = [&out](const std::string& path)
			{
				if (path.empty())
					return;

				std::error_code ec;
				const std::string normalized = fs::absolute(fs::u8path(path), ec).string();

				// 去重：同一目录只尝试一次，避免重复探测与重复报错。
				if (std::find(out.begin(), out.end(), normalized) == out.end())
					out.push_back(normalized);
			};

			// 1. 环境变量（运维/联调临时指定，优先级最高）。
			// Windows 用 _dupenv_s（避免 MSVC C4996 安全弃用报错），
			// 非 Windows 用标准 getenv。
			#ifdef _WIN32
			char* env_value = nullptr;

			if (_dupenv_s(&env_value, nullptr, "XY_DATA_DIR") == 0 && env_value)
			{
				push(env_value);
				free(env_value);
			}
			#else
			if (const char* env_value = std::getenv("XY_DATA_DIR"))
			{
				push(env_value);
			}
			#endif

			// 2. 配置中的数据目录（部署配置优先于用户配置，Configuration 已处理）。
			push(Configuration::get().get_data_dir());

			// 3~6. 可执行文件所在目录及其 Data/ 子目录、父目录及其 Data/ 子目录。
			const std::string exedir = executable_dir();
			push(exedir);

			if (!exedir.empty())
			{
				push((fs::u8path(exedir) / "Data").string());

				const std::string parent = fs::u8path(exedir).parent_path().string();
				push(parent);

				if (!parent.empty())
					push((fs::u8path(parent) / "Data").string());
			}

			// 7~8. 当前工作目录（兼容既有行为）及其 Data/ 子目录。
			std::error_code ec;
			const std::string cwd = fs::current_path(ec).string();
			push(cwd);

			if (!cwd.empty())
				push((fs::u8path(cwd) / "Data").string());

			return out;
		}

		Layout ResourceLocator::locate(Format wanted)
		{
			Layout best;

			// 自省是纯读操作（遍历目录 + 判断文件存在），可以在多个候选上安全执行。
			for (const std::string& candidate : candidates())
			{
				best.tried.push_back(candidate);

				for (const auto& backend : make_backends())
				{
					// 指定了格式时只考虑该后端，但仍然让它参与候选链寻找位置。
					if (wanted != Format::Unknown && backend->format() != wanted)
						continue;

					const ProbeResult probe = backend->probe(candidate);

					if (!probe.usable)
						continue;

					// 严格大于：同分时保留先命中的候选（候选链顺序即优先级）。
					if (probe.score > best.score)
					{
						best.found = true;
						best.format = backend->format();
						best.base = candidate;
						best.backend = backend->name();
						best.score = probe.score;
					}
				}
			}

			return best;
		}

		std::string ResourceLocator::describe_tried(const Layout& layout)
		{
			std::ostringstream out;

			for (size_t i = 0; i < layout.tried.size(); i++)
			{
				if (i)
					out << "; ";

				out << layout.tried[i];
			}

			return out.str();
		}
	}
}
