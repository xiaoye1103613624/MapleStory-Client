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
#include <vector>

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

		// Only the img backend boots with a partial root set; wz/nx bind all at open.
		if (backend_->format() != Format::Img)
			return;

		// 选角只需外观相关根；全量 ensure_remaining 会卡登录半分钟级。
		ImgSource::ensure_char_select();
		LOG(LOG_INFO, "[RES] ensure_game_roots (char-select) " << diagnostics(false));

		// Boot 时 Character 未绑，BodyDrawInfo 会是空表 → 选角头身脚叠在一起。
		// Character 就绪后重建锚点。
		CharLook::init();
	}

	void ResourceService::ensure_map_roots()
	{
		if (!backend_)
			return;

		if (backend_->format() != Format::Img)
			return;

		ImgSource::ensure_remaining();
		LOG(LOG_INFO, "[RES] ensure_map_roots done " << diagnostics(false));
	}

	void ResourceService::ensure_map(int32_t mapid)
	{
		if (!backend_)
			return;

		if (backend_->format() != Format::Img)
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
	}
}
