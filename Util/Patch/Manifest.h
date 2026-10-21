//////////////////////////////////////////////////////////////////////////////
//	Patch updater — manifest model + minimal JSON parse
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	namespace patch
	{
		struct ManifestFile
		{
			std::string path;
			uint64_t size = 0;
			std::string sha256;
			std::string format;
		};

		struct Manifest
		{
			std::string client_min_version;
			std::string channel;
			std::string base_url;
			std::vector<ManifestFile> files;
		};

		// Parse a constrained manifest JSON. Returns false and sets error on failure.
		bool parse_manifest(const std::string& json, Manifest& out, std::string& error);
	}
}
