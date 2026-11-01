//////////////////////////////////////////////////////////////////////////////////
//	Locale / i18n — 登录链文案与后续国际化骨架
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Singleton.h"

#include <string>
#include <unordered_map>

namespace ms
{
	/// 轻量本地化：从 `locale/<lang>.ini` 加载键值，支持热重载。
	/// 默认语言 zh-CN；缺键时回退到 zh-CN，再回退到 key 本身。
	class Locale : public Singleton<Locale>
	{
	public:
		Locale();

		/// 按 Configuration::ui.language 重新加载（热重载 / 启动时调用）。
		void reload();

		/// 当前语言码（如 zh-CN / en-US）。
		const std::string& language() const
		{
			return lang;
		}

		/// 取文案；缺失时回退。
		std::string tr(const std::string& key) const;

		/// 静态便捷入口（避免与 Singleton::get 混淆）。
		static std::string text(const std::string& key)
		{
			return Locale::get().tr(key);
		}

	private:
		bool load_file(const std::string& path, std::unordered_map<std::string, std::string>& out) const;

		std::string lang;
		std::unordered_map<std::string, std::string> primary;
		std::unordered_map<std::string, std::string> fallback_zh;
	};
}
