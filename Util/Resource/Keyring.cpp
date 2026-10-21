//////////////////////////////////////////////////////////////////////////////
//	Fyms085 resource abstraction additions Copyright (C) 2026
//	密钥治理（D4）实现。
//////////////////////////////////////////////////////////////////////////////
#include "Keyring.h"

#include <fstream>
#include <iterator>

// 内置三表（来自 vendor Keys.cpp，定义于 namespace nl，链接期解析）。
// 注意必须放在 namespace nl 内，否则与 Keys.cpp 的 nl::key_* 符号不匹配。
namespace nl {
extern uint8_t key_bms[];
extern uint8_t key_gms[];
extern uint8_t key_kms[];
}

namespace ms
{
	namespace res
	{
		KeyResolver::KeyResolver(KeyMode mode, const std::string & external_path)
			: mode_(mode)
		{
			// 外部密钥文件：二进制 65536 字节为一个 key，可串联多个。默认关闭（空路径）。
			if (!external_path.empty())
			{
				std::ifstream f(external_path, std::ios::binary);
				if (f)
				{
					std::vector<uint8_t> buf((std::istreambuf_iterator<char>(f)),
						std::istreambuf_iterator<char>());
					for (size_t off = 0; off + 65536 <= buf.size(); off += 65536)
						external_.emplace_back(buf.begin() + off, buf.begin() + off + 65536);
				}
			}
		}

		std::vector<const uint8_t *> KeyResolver::candidates(const uint8_t * embedded) const
		{
			std::vector<const uint8_t *> out;

			if (mode_ == KeyMode::External)
			{
				for (auto & k : external_)
					out.push_back(k.data());
				return out;
			}

			// 内置模式：归档自带 key 优先，其次 gms/bms/kms，最后外部（若有）。
			if (embedded)
				out.push_back(embedded);
			out.push_back(nl::key_gms);
			out.push_back(nl::key_bms);
			out.push_back(nl::key_kms);
			for (auto & k : external_)
				out.push_back(k.data());
			return out;
		}

		std::vector<const uint8_t *> KeyResolver::external_pointers() const
		{
			std::vector<const uint8_t *> out;
			for (auto & k : external_)
				out.push_back(k.data());
			return out;
		}

		const uint8_t * KeyResolver::cached(const std::string & archive) const
		{
			auto it = cache_.find(archive);
			return it == cache_.end() ? nullptr : it->second;
		}

		void KeyResolver::cache(const std::string & archive, const uint8_t * key)
		{
			cache_[archive] = key;
		}

		void KeyResolver::note_trial(const std::string & archive, uint32_t attempts) const
		{
			trials_[archive] = attempts;
		}

		std::string KeyResolver::diagnostics() const
		{
			std::string s;
			for (auto & kv : trials_)
				s += "  " + kv.first + ": " + std::to_string(kv.second) + " trial(s)\n";
			return s;
		}
	}
}
