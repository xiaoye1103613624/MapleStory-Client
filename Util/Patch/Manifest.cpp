//////////////////////////////////////////////////////////////////////////////
//	Patch updater — constrained JSON parse (no third-party dependency)
//////////////////////////////////////////////////////////////////////////////
#include "Manifest.h"

#include <cctype>

namespace ms
{
	namespace patch
	{
	namespace
	{
		void skip_ws(const std::string& s, size_t& i)
		{
			while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
				++i;
		}

		bool match(const std::string& s, size_t& i, char c)
		{
			skip_ws(s, i);
			if (i < s.size() && s[i] == c)
			{
				++i;
				return true;
			}
			return false;
		}

		bool parse_string(const std::string& s, size_t& i, std::string& out)
		{
			skip_ws(s, i);
			if (i >= s.size() || s[i] != '"')
				return false;
			++i;
			out.clear();
			while (i < s.size())
			{
				char c = s[i++];
				if (c == '"')
					return true;
				if (c == '\\' && i < s.size())
				{
					char e = s[i++];
					if (e == '"' || e == '\\' || e == '/')
						out.push_back(e);
					else if (e == 'n')
						out.push_back('\n');
					else if (e == 't')
						out.push_back('\t');
					else
						out.push_back(e);
				}
				else
				{
					out.push_back(c);
				}
			}
			return false;
		}

		bool parse_uint64(const std::string& s, size_t& i, uint64_t& out)
		{
			skip_ws(s, i);
			if (i >= s.size() || !std::isdigit(static_cast<unsigned char>(s[i])))
				return false;
			out = 0;
			while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
			{
				out = out * 10 + static_cast<uint64_t>(s[i] - '0');
				++i;
			}
			return true;
		}

		// Find "key": and parse following string value anywhere at object level (simple scan).
		bool find_string_field(const std::string& json, const char* key, std::string& out)
		{
			const std::string needle = std::string("\"") + key + "\"";
			size_t pos = 0;
			while ((pos = json.find(needle, pos)) != std::string::npos)
			{
				size_t i = pos + needle.size();
				skip_ws(json, i);
				if (i < json.size() && json[i] == ':')
				{
					++i;
					return parse_string(json, i, out);
				}
				pos += needle.size();
			}
			return false;
		}
	}

	bool parse_manifest(const std::string& json, Manifest& out, std::string& error)
	{
		out = Manifest{};
		find_string_field(json, "clientMinVersion", out.client_min_version);
		find_string_field(json, "channel", out.channel);
		find_string_field(json, "baseUrl", out.base_url);

		const std::string files_key = "\"files\"";
		size_t pos = json.find(files_key);
		if (pos == std::string::npos)
		{
			error = "missing files array";
			return false;
		}
		size_t i = pos + files_key.size();
		skip_ws(json, i);
		if (!match(json, i, ':') || !match(json, i, '['))
		{
			error = "files is not an array";
			return false;
		}

		skip_ws(json, i);
		if (match(json, i, ']'))
			return true;

		for (;;)
		{
			skip_ws(json, i);
			if (!match(json, i, '{'))
			{
				error = "expected file object";
				return false;
			}

			ManifestFile file;
			while (true)
			{
				skip_ws(json, i);
				if (match(json, i, '}'))
					break;

				std::string key;
				if (!parse_string(json, i, key) || !match(json, i, ':'))
				{
					error = "bad file field";
					return false;
				}

				if (key == "path" || key == "sha256" || key == "format")
				{
					std::string val;
					if (!parse_string(json, i, val))
					{
						error = "bad string field " + key;
						return false;
					}
					if (key == "path")
						file.path = val;
					else if (key == "sha256")
						file.sha256 = val;
					else
						file.format = val;
				}
				else if (key == "size")
				{
					if (!parse_uint64(json, i, file.size))
					{
						error = "bad size";
						return false;
					}
				}
				else
				{
					// Skip unknown value (string / number / bool / null) roughly.
					skip_ws(json, i);
					if (i < json.size() && json[i] == '"')
					{
						std::string tmp;
						if (!parse_string(json, i, tmp))
						{
							error = "skip string failed";
							return false;
						}
					}
					else
					{
						while (i < json.size() && json[i] != ',' && json[i] != '}')
							++i;
					}
				}

				skip_ws(json, i);
				if (match(json, i, ','))
					continue;
				if (i < json.size() && json[i] == '}')
					continue;
			}

			if (file.path.empty() || file.sha256.empty())
			{
				error = "file entry missing path/sha256";
				return false;
			}
			out.files.push_back(file);

			skip_ws(json, i);
			if (match(json, i, ','))
				continue;
			if (match(json, i, ']'))
				break;

			error = "files array syntax";
			return false;
		}

		return true;
	}
	}
}
