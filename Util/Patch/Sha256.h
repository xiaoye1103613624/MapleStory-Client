//////////////////////////////////////////////////////////////////////////////
//	Patch updater — SHA-256 (public-domain compact implementation wrapper)
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	namespace patch
	{
		// Hex-lowercase SHA-256 of a memory buffer.
		std::string sha256_hex(const void* data, size_t len);

		// Hex-lowercase SHA-256 of a whole file. Returns empty string on I/O error.
		std::string sha256_file_hex(const std::string& path);
	}
}
