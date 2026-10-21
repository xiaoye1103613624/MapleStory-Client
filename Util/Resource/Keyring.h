//////////////////////////////////////////////////////////////////////////////
//	Fyms085 resource abstraction additions Copyright (C) 2026
//	密钥治理（D4）：内置三表 + 运行时推导 + 外部密钥文件 + per-archive 缓存。
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ms
{
	namespace res
	{
		// 密钥来源模式（对应配置 data.keymode）。
		enum class KeyMode
		{
			Auto,      // 内置三表 + 归档自带 key + 外部文件（默认）
			Builtin,   // 仅内置三表 + 归档自带 key
			External,  // 仅外部密钥文件（data.keyfile 指定的本地文件，默认关闭）
		};

		// 密钥解析器：封装候选 key 序列（按 KeyMode 排序）与每归档首次命中缓存，
		// 使同一归档的后续解码直接命中（≤1 次试探）。外部密钥文件明文不入库，仅驻留内存。
		class KeyResolver
		{
		public:
			// @mode  密钥来源模式。
			// @external_path 外部密钥文件路径；空字符串表示关闭（默认）。
			KeyResolver(KeyMode mode, const std::string & external_path);

			// 给定归档自带 key（embedded，可能为 nullptr），返回应尝试的候选序列
			// （按 KeyMode 排序：内置模式下自带 key 优先，其次 gms/bms/kms，最后外部）。
			std::vector<const uint8_t *> candidates(const uint8_t * embedded) const;

			// per-archive 缓存（archive 为归档标识，如文件名）。缓存后该归档解码直接命中。
			const uint8_t * cached(const std::string & archive) const;
			void cache(const std::string & archive, const uint8_t * key);

			// 记录某归档最近一次解码的试探次数（供诊断）。
			void note_trial(const std::string & archive, uint32_t attempts) const;

			// 诊断：每归档的试探次数（归档名 -> 次数），用于验证"每归档 ≤1 次试探"。
			std::string diagnostics() const;

		private:
			std::vector<const uint8_t *> external_pointers() const;

			KeyMode mode_;
			std::vector<std::vector<uint8_t>> external_;  // 外部密钥（明文不入库，仅内存）
			mutable std::unordered_map<std::string, const uint8_t *> cache_;
			mutable std::unordered_map<std::string, uint32_t> trials_;
		};
	}
}
