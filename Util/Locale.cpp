//////////////////////////////////////////////////////////////////////////////////
//	Locale / i18n — 登录链文案与后续国际化骨架
//////////////////////////////////////////////////////////////////////////////////
#include "Locale.h"

#include "../Configuration.h"

#include <fstream>
#include <sstream>

namespace ms
{
	namespace
	{
		std::string trim(const std::string& s)
		{
			size_t b = 0;
			while (b < s.size() && (s[b] == ' ' || s[b] == '\t' || s[b] == '\r'))
				++b;
			size_t e = s.size();
			while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r'))
				--e;
			return s.substr(b, e - b);
		}

		std::string strip_bom(std::string s)
		{
			if (s.size() >= 3
				&& static_cast<unsigned char>(s[0]) == 0xEF
				&& static_cast<unsigned char>(s[1]) == 0xBB
				&& static_cast<unsigned char>(s[2]) == 0xBF)
			{
				s.erase(0, 3);
			}
			return s;
		}
	}

	Locale::Locale()
	{
		reload();
	}

	bool Locale::load_file(const std::string& path, std::unordered_map<std::string, std::string>& out) const
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open())
			return false;

		std::ostringstream buffer;
		buffer << file.rdbuf();
		std::string content = strip_bom(buffer.str());
		std::istringstream stream(content);
		std::string line;

		while (std::getline(stream, line))
		{
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			std::string t = trim(line);
			if (t.empty() || t[0] == '#' || t[0] == ';')
				continue;

			size_t eq = t.find('=');
			if (eq == std::string::npos)
				continue;

			std::string key = trim(t.substr(0, eq));
			std::string val = trim(t.substr(eq + 1));
			if (!key.empty())
				out[key] = val;
		}

		return true;
	}

	void Locale::reload()
	{
		lang = Configuration::get().get_ui_language();
		if (lang.empty())
			lang = "zh-CN";

		primary.clear();
		fallback_zh.clear();

		load_file("locale/zh-CN.ini", fallback_zh);
		if (lang == "zh-CN")
			primary = fallback_zh;
		else
			load_file("locale/" + lang + ".ini", primary);

		// 代次必须在两张表都装好之后才递增：绑定表看到的永远是「这一代对应的文案已就位」。
		++gen;
	}

	std::string Locale::tr(const std::string& key) const
	{
		auto it = primary.find(key);
		if (it != primary.end())
			return it->second;

		auto fb = fallback_zh.find(key);
		if (fb != fallback_zh.end())
			return fb->second;

		return key;
	}

	bool Locale::has(const std::string& key) const
	{
		return primary.find(key) != primary.end() || fallback_zh.find(key) != fallback_zh.end();
	}
}
