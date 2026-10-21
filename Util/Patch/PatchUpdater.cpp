//////////////////////////////////////////////////////////////////////////////
//	Patch updater — sync by final resource file
//////////////////////////////////////////////////////////////////////////////
#include "PatchUpdater.h"

#include "HttpClient.h"
#include "Manifest.h"
#include "Sha256.h"

#include "../../Configuration.h"
#include "../../MapleStory.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <sys/stat.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace ms
{
	namespace patch
	{
	namespace
	{
		uint64_t file_size(const std::string& path)
		{
#ifdef _WIN32
			struct _stat64 st;
			if (_stat64(path.c_str(), &st) != 0)
				return 0;
			return static_cast<uint64_t>(st.st_size);
#else
			struct stat st;
			if (stat(path.c_str(), &st) != 0)
				return 0;
			return static_cast<uint64_t>(st.st_size);
#endif
		}

		bool ensure_parent_dirs(const std::string& path)
		{
			std::string cur;
			for (size_t i = 0; i < path.size(); ++i)
			{
				char c = path[i];
				if (c == '/' || c == '\\')
				{
					if (cur.empty() || cur.back() == ':' )
					{
						cur.push_back(c);
						continue;
					}
#ifdef _WIN32
					_mkdir(cur.c_str());
#else
					mkdir(cur.c_str(), 0755);
#endif
				}
				cur.push_back(c);
			}
			return true;
		}

		std::string join_url(const std::string& base, const std::string& path)
		{
			if (path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0)
				return path;
			if (base.empty())
				return path;
			if (base.back() == '/')
			{
				if (!path.empty() && (path.front() == '/' || path.front() == '\\'))
					return base + path.substr(1);
				return base + path;
			}
			if (!path.empty() && (path.front() == '/' || path.front() == '\\'))
				return base + path;
			return base + "/" + path;
		}

		std::string to_lower(std::string s)
		{
			std::transform(s.begin(), s.end(), s.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			return s;
		}

		bool local_matches(const ManifestFile& f)
		{
			const uint64_t sz = file_size(f.path);
			if (f.size != 0 && sz != f.size)
				return false;
			if (sz == 0 && f.size == 0)
			{
				// Missing file: size 0 locally vs expected 0 is rare; treat as mismatch if missing.
				std::ifstream in(f.path, std::ios::binary);
				if (!in)
					return false;
			}
			const std::string hash = sha256_file_hex(f.path);
			if (hash.empty())
				return false;
			return to_lower(hash) == to_lower(f.sha256);
		}

		bool atomic_replace(const std::string& part, const std::string& dest)
		{
#ifdef _WIN32
			std::remove(dest.c_str());
			return std::rename(part.c_str(), dest.c_str()) == 0;
#else
			return std::rename(part.c_str(), dest.c_str()) == 0;
#endif
		}

		void write_state(const Manifest& m, int downloaded)
		{
			std::ofstream out("patch-state.json", std::ios::trunc);
			if (!out)
				return;
			out << "{\n"
				<< "  \"channel\": \"" << m.channel << "\",\n"
				<< "  \"clientMinVersion\": \"" << m.client_min_version << "\",\n"
				<< "  \"files\": " << m.files.size() << ",\n"
				<< "  \"downloaded\": " << downloaded << "\n"
				<< "}\n";
		}
	}

	Error PatchUpdater::sync()
	{
		if (!Setting<PatchEnabled>::get().load())
		{
			LOG(LOG_INFO, "[PATCH] disabled");
			return Error::Code::NONE;
		}

		const std::string url = Setting<PatchUrl>::get().load();
		if (url.empty())
		{
			LOG(LOG_WARN, "[PATCH] patch.url empty; skip");
			return Error::Code::NONE;
		}

		// 可写根 = 进程 cwd（Android 启动时已 chdir 到外部存储）。
		// 补丁文件按 manifest.path 相对该根落盘。
		{
#ifdef _WIN32
			char cwd[MAX_PATH] = { 0 };
			if (GetCurrentDirectoryA(MAX_PATH, cwd))
				LOG(LOG_INFO, "[PATCH] writable_root=" << cwd);
#else
			char cwd[512] = { 0 };
			if (getcwd(cwd, sizeof(cwd)))
				LOG(LOG_INFO, "[PATCH] writable_root=" << cwd);
#endif
		}

		const std::string channel = Setting<PatchChannel>::get().load();
		const uint32_t timeout = Setting<PatchTimeoutMs>::get().load();
		const uint32_t retries = Setting<PatchRetries>::get().load();
		const bool allow_skip = Setting<PatchAllowSkip>::get().load();

		HttpResult resp;
		for (uint32_t attempt = 0; attempt <= retries; ++attempt)
		{
			resp = http_get(url, timeout);
			if (resp.ok)
				break;
			LOG(LOG_WARN, "[PATCH] manifest fetch attempt " << (attempt + 1)
				<< " failed: " << resp.error);
		}

		if (!resp.ok)
		{
			if (allow_skip)
			{
				LOG(LOG_WARN, "[PATCH] allow_skip=true; continue without update");
				return Error::Code::NONE;
			}
			static std::string msg;
			msg = resp.error;
			return Error(Error::Code::CONNECTION, msg.c_str());
		}

		Manifest manifest;
		std::string perr;
		const std::string body(resp.body.begin(), resp.body.end());
		if (!parse_manifest(body, manifest, perr))
		{
			LOG(LOG_ERROR, "[PATCH] manifest parse: " << perr);
			if (allow_skip)
				return Error::Code::NONE;
			static std::string msg;
			msg = perr;
			return Error(Error::Code::MISSING_FILE, msg.c_str());
		}

		if (!manifest.channel.empty() && !channel.empty()
			&& to_lower(manifest.channel) != to_lower(channel))
		{
			LOG(LOG_WARN, "[PATCH] channel mismatch manifest=" << manifest.channel
				<< " config=" << channel << "; skip apply");
			return Error::Code::NONE;
		}

		int downloaded = 0;
		for (const ManifestFile& f : manifest.files)
		{
			if (local_matches(f))
				continue;

			const std::string file_url = join_url(manifest.base_url, f.path);
			const std::string part = f.path + ".part";
			ensure_parent_dirs(f.path);

			LOG(LOG_INFO, "[PATCH] download " << f.path);

			HttpResult fr;
			for (uint32_t attempt = 0; attempt <= retries; ++attempt)
			{
				fr = http_get_file(file_url, part, timeout);
				if (fr.ok)
					break;
			}

			if (!fr.ok)
			{
				std::remove(part.c_str());
				LOG(LOG_ERROR, "[PATCH] download failed " << f.path << ": " << fr.error);
				if (allow_skip)
					continue;
				static std::string msg;
				msg = fr.error;
				return Error(Error::Code::CONNECTION, msg.c_str());
			}

			const std::string hash = sha256_file_hex(part);
			if (to_lower(hash) != to_lower(f.sha256))
			{
				std::remove(part.c_str());
				LOG(LOG_ERROR, "[PATCH] sha256 mismatch " << f.path);
				if (allow_skip)
					continue;
				return Error(Error::Code::MISSING_FILE, "patch sha256 mismatch");
			}

			if (f.size != 0 && file_size(part) != f.size)
			{
				std::remove(part.c_str());
				LOG(LOG_ERROR, "[PATCH] size mismatch " << f.path);
				if (allow_skip)
					continue;
				return Error(Error::Code::MISSING_FILE, "patch size mismatch");
			}

			if (!atomic_replace(part, f.path))
			{
				std::remove(part.c_str());
				LOG(LOG_ERROR, "[PATCH] replace failed " << f.path);
				if (allow_skip)
					continue;
				return Error(Error::Code::MISSING_FILE, "patch replace failed");
			}

			++downloaded;
		}

		write_state(manifest, downloaded);
		LOG(LOG_INFO, "[PATCH] done downloaded=" << downloaded
			<< " total=" << manifest.files.size());
		return Error::Code::NONE;
	}
	}
}
