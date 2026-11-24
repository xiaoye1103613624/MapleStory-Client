//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	T11 · 封包边界 GBK <-> UTF-8 转换实现										//
//////////////////////////////////////////////////////////////////////////////////
#include "PacketCharset.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace
{
	// s9 侧 CharsetConstants.java:55 => LANGUAGE_CN(3, "GBK")，本部署账号 language 恒为 3
	// （notes/charset.md §1）。Java 的 "GBK" 与微软 CP936 在常用汉字区取同一张表，
	// 差异只在 GBK 扩展码位；本批对拍样本（tools/probe/charset_vectors.h）里两者编码结果全等。
	constexpr UINT PACKET_MB_CSET = 936;

	bool is_ascii(const std::string& s)
	{
		for (const char c : s)
		{
			if (static_cast<unsigned char>(c) >= 0x80)
				return false;
		}

		return true;
	}

	// 2026-11-20：废弃 UTF-8 快路径。GBK 双字节常与 UTF-8 2/3 字节形态重叠，
	// is_strict_utf8_with_multibyte 对 GB2312 汉字区误判率约 14%，会把 owner/
	// 聊天等 GBK 原文原样喂给 Text → 乱码（见 chat-group-charset-rca）。
	// 本部署 language=3 封包恒 GBK，非 ASCII 一律 CP936→UTF-8。
}
#endif

namespace ms
{
	std::string packet_to_internal(const std::string& bytes)
	{
#ifdef _WIN32
		// 纯 ASCII 时 CP936 与 UTF-8 同形，直接原样返回：既省一次转换，
		// 也保证既有 read_string 调用点的 ASCII 字段字节级不变。
		if (bytes.empty() || is_ascii(bytes))
			return bytes;

		const int cch = MultiByteToWideChar(PACKET_MB_CSET, 0, bytes.data(),
			static_cast<int>(bytes.size()), nullptr, 0);

		if (cch <= 0)
			return bytes;

		std::wstring wide(static_cast<size_t>(cch), L'\0');

		if (MultiByteToWideChar(PACKET_MB_CSET, 0, bytes.data(),
			static_cast<int>(bytes.size()), &wide[0], cch) != cch)
			return bytes;

		const int cb = WideCharToMultiByte(CP_UTF8, 0, wide.data(), cch,
			nullptr, 0, nullptr, nullptr);

		if (cb <= 0)
			return bytes;

		std::string utf8(static_cast<size_t>(cb), '\0');

		if (WideCharToMultiByte(CP_UTF8, 0, wide.data(), cch,
			&utf8[0], cb, nullptr, nullptr) != cb)
			return bytes;

		return utf8;
#else
		return bytes;
#endif
	}

	std::string packet_from_internal(const std::string& text)
	{
#ifdef _WIN32
		if (text.empty() || is_ascii(text))
			return text;

		const int cch = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
			static_cast<int>(text.size()), nullptr, 0);

		// 不是合法 UTF-8（历史调用点里确实存在直接塞原始字节的情况）时原样发出，
		// 保持转换前的行为，不把"修复"变成"丢字段"。
		if (cch <= 0)
			return text;

		std::wstring wide(static_cast<size_t>(cch), L'\0');

		if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
			static_cast<int>(text.size()), &wide[0], cch) != cch)
			return text;

		const int cb = WideCharToMultiByte(PACKET_MB_CSET, 0, wide.data(), cch,
			nullptr, 0, nullptr, nullptr);

		if (cb <= 0)
			return text;

		std::string gbk(static_cast<size_t>(cb), '\0');

		if (WideCharToMultiByte(PACKET_MB_CSET, 0, wide.data(), cch,
			&gbk[0], cb, nullptr, nullptr) != cb)
			return text;

		return gbk;
#else
		return text;
#endif
	}
}
