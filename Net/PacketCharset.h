//////////////////////////////////////////////////////////////////////////////////
//	T11 · 封包边界字符集：s9 用 GBK 收发字符串，客户端内部一律 UTF-8。
//	转换只允许发生在 InPacket::read_string / OutPacket::write_string 这一层，
//	不得散落到 UI（见 docs/features/s9-protocol-align/notes/charset.md §5）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

namespace ms
{
	// 线上字节（GBK）→ 内部表示（UTF-8）
	std::string packet_to_internal(const std::string& bytes);

	// 内部表示（UTF-8）→ 线上字节（GBK）
	std::string packet_from_internal(const std::string& text);
}
