//////////////////////////////////////////////////////////////////////////////
//	Patch updater — HTTP GET
//////////////////////////////////////////////////////////////////////////////
#include "HttpClient.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

namespace ms
{
	namespace patch
	{
	namespace
	{
		bool parse_url(const std::string& url, bool& https, std::string& host, int& port, std::string& path)
		{
			https = false;
			host.clear();
			path = "/";
			port = 80;

			std::string rest = url;
			if (rest.rfind("https://", 0) == 0)
			{
				https = true;
				port = 443;
				rest = rest.substr(8);
			}
			else if (rest.rfind("http://", 0) == 0)
			{
				rest = rest.substr(7);
			}
			else
			{
				return false;
			}

			const size_t slash = rest.find('/');
			std::string hostport = slash == std::string::npos ? rest : rest.substr(0, slash);
			path = slash == std::string::npos ? "/" : rest.substr(slash);

			const size_t colon = hostport.find(':');
			if (colon == std::string::npos)
			{
				host = hostport;
			}
			else
			{
				host = hostport.substr(0, colon);
				port = std::atoi(hostport.substr(colon + 1).c_str());
			}

			return !host.empty();
		}
	}

#ifdef _WIN32
	HttpResult http_get(const std::string& url, uint32_t timeout_ms)
	{
		HttpResult result;

		bool https = false;
		std::string host, path;
		int port = 80;
		if (!parse_url(url, https, host, port, path))
		{
			result.error = "invalid url";
			return result;
		}

		std::wstring whost(host.begin(), host.end());
		std::wstring wpath(path.begin(), path.end());

		HINTERNET session = WinHttpOpen(L"MapleStory-Client-Patch/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!session)
		{
			result.error = "WinHttpOpen failed";
			return result;
		}

		WinHttpSetTimeouts(session, timeout_ms, timeout_ms, timeout_ms, timeout_ms);

		HINTERNET conn = WinHttpConnect(session, whost.c_str(), static_cast<INTERNET_PORT>(port), 0);
		if (!conn)
		{
			result.error = "WinHttpConnect failed";
			WinHttpCloseHandle(session);
			return result;
		}

		DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
		HINTERNET req = WinHttpOpenRequest(conn, L"GET", wpath.c_str(), nullptr,
			WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
		if (!req)
		{
			result.error = "WinHttpOpenRequest failed";
			WinHttpCloseHandle(conn);
			WinHttpCloseHandle(session);
			return result;
		}

		if (!WinHttpSendRequest(req, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
			|| !WinHttpReceiveResponse(req, nullptr))
		{
			result.error = "WinHttp request failed";
			WinHttpCloseHandle(req);
			WinHttpCloseHandle(conn);
			WinHttpCloseHandle(session);
			return result;
		}

		DWORD status = 0;
		DWORD status_size = sizeof(status);
		WinHttpQueryHeaders(req, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX, &status, &status_size, WINHTTP_NO_HEADER_INDEX);
		result.status = static_cast<int>(status);

		for (;;)
		{
			DWORD avail = 0;
			if (!WinHttpQueryDataAvailable(req, &avail))
				break;
			if (avail == 0)
				break;
			const size_t offset = result.body.size();
			result.body.resize(offset + avail);
			DWORD read = 0;
			if (!WinHttpReadData(req, result.body.data() + offset, avail, &read))
				break;
			result.body.resize(offset + read);
		}

		WinHttpCloseHandle(req);
		WinHttpCloseHandle(conn);
		WinHttpCloseHandle(session);

		result.ok = (result.status >= 200 && result.status < 300);
		if (!result.ok && result.error.empty())
			result.error = "HTTP status " + std::to_string(result.status);
		return result;
	}
#else
	HttpResult http_get(const std::string& url, uint32_t timeout_ms)
	{
		HttpResult result;
		bool https = false;
		std::string host, path;
		int port = 80;
		if (!parse_url(url, https, host, port, path))
		{
			result.error = "invalid url";
			return result;
		}
		if (https)
		{
			result.error = "HTTPS not supported on this platform yet; use http://";
			return result;
		}

		addrinfo hints{};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		addrinfo* res = nullptr;
		const std::string port_str = std::to_string(port);
		if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0 || !res)
		{
			result.error = "DNS failed";
			return result;
		}

		int fd = -1;
		for (addrinfo* p = res; p; p = p->ai_next)
		{
			fd = static_cast<int>(::socket(p->ai_family, p->ai_socktype, p->ai_protocol));
			if (fd < 0)
				continue;
			if (::connect(fd, p->ai_addr, static_cast<socklen_t>(p->ai_addrlen)) == 0)
				break;
			::close(fd);
			fd = -1;
		}
		freeaddrinfo(res);

		if (fd < 0)
		{
			result.error = "connect failed";
			return result;
		}

		// Apply SO_RCVTIMEO / SO_SNDTIMEO so hung patch hosts cannot block boot forever.
		if (timeout_ms > 0)
		{
			timeval tv{};
			tv.tv_sec = static_cast<long>(timeout_ms / 1000);
			tv.tv_usec = static_cast<long>((timeout_ms % 1000) * 1000);
			::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		}

		std::ostringstream req;
		req << "GET " << path << " HTTP/1.1\r\n"
			<< "Host: " << host << "\r\n"
			<< "User-Agent: MapleStory-Client-Patch/1.0\r\n"
			<< "Connection: close\r\n\r\n";
		const std::string reqs = req.str();
		if (::send(fd, reqs.data(), reqs.size(), 0) < 0)
		{
			result.error = "send failed";
			::close(fd);
			return result;
		}

		std::string raw;
		char buf[4096];
		ssize_t n;
		while ((n = ::recv(fd, buf, sizeof(buf), 0)) > 0)
			raw.append(buf, static_cast<size_t>(n));
		::close(fd);

		const size_t hdr_end = raw.find("\r\n\r\n");
		if (hdr_end == std::string::npos)
		{
			result.error = "bad HTTP response";
			return result;
		}

		const std::string headers = raw.substr(0, hdr_end);
		if (headers.size() >= 12 && headers.rfind("HTTP/", 0) == 0)
			result.status = std::atoi(headers.c_str() + 9);

		result.body.assign(raw.begin() + static_cast<std::ptrdiff_t>(hdr_end + 4), raw.end());
		result.ok = (result.status >= 200 && result.status < 300);
		if (!result.ok)
			result.error = "HTTP status " + std::to_string(result.status);
		return result;
	}
#endif

	HttpResult http_get_file(const std::string& url, const std::string& file_path, uint32_t timeout_ms)
	{
		HttpResult r = http_get(url, timeout_ms);
		if (!r.ok)
			return r;

		std::ofstream out(file_path, std::ios::binary | std::ios::trunc);
		if (!out)
		{
			r.ok = false;
			r.error = "cannot write " + file_path;
			return r;
		}
		if (!r.body.empty())
			out.write(r.body.data(), static_cast<std::streamsize>(r.body.size()));
		if (!out)
		{
			r.ok = false;
			r.error = "write failed " + file_path;
		}
		return r;
	}
	}
}
