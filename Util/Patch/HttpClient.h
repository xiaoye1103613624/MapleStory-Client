//////////////////////////////////////////////////////////////////////////////
//	Patch updater — minimal HTTP(S) GET client
//	Windows: WinHTTP. Other platforms: plaintext HTTP via BSD sockets.
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	namespace patch
	{
		struct HttpResult
		{
			bool ok = false;
			int status = 0;
			std::string error;
			std::vector<char> body;
		};

		// GET url into memory. timeout_ms applies per request.
		HttpResult http_get(const std::string& url, uint32_t timeout_ms);

		// GET url and write to file_path (creates parent dirs if needed via caller).
		HttpResult http_get_file(const std::string& url, const std::string& file_path, uint32_t timeout_ms);
	}
}
