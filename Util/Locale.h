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

		/// 键是否存在（当前语言或 zh-CN 兜底表里任一命中）。
		bool has(const std::string& key) const;

		/// 静态便捷入口（避免与 Singleton::get 混淆）。
		static std::string text(const std::string& key)
		{
			return Locale::get().tr(key);
		}

		/// 有键用文案，没有则回退到外部原文。用于「WZ/String 里已有文案，但原生
		/// 客户端用另一套措辞」这类覆盖场景：只登记有证据的那几个键，其余仍走数据，
		/// 避免把没核对过的职业名一并编出来。
		static std::string text_or(const std::string& key, const std::string& fallback)
		{
			return Locale::get().has(key) ? Locale::get().tr(key) : fallback;
		}

		/// 枚举原文折成键后缀再取文案：`enum_key("equip.type.", "ONE-HANDED SWORD")`
		/// 得 `equip.type.ONE_HANDED_SWORD`。连字符一起折是因为 ini 的键名里带 `-`
		/// 读起来像前缀分隔符，且「ONE-HANDED SWORD」原文同时含两种分隔符。
		static std::string enum_key(const std::string& prefix, const std::string& raw)
		{
			std::string key = prefix;
			for (char c : raw)
				key += (c == ' ' || c == '-') ? '_' : c;

			return key;
		}

		/// 取文案。空原文直接回空 —— 武器类型表里确实有空串（数据表 index 4/5/6/9），
		/// 拼进键名会得到 `equip.type.` 这种「查不到就回显键名」的脏文案。
		static std::string enum_text(const std::string& prefix, const std::string& raw)
		{
			if (raw.empty())
				return raw;

			return Locale::get().tr(enum_key(prefix, raw));
		}

		/// 单占位符文案：值填进 `{}`。加载器会 trim 掉值两端空白，所以「Type: 」这种
		/// 尾随空格没法直接存 —— 空格写在占位符两侧，由各语言自己决定要不要。
		static std::string format(const std::string& key, const std::string& value)
		{
			std::string out = Locale::get().tr(key);
			size_t ph = out.find("{}");

			if (ph == std::string::npos)
				out += value;
			else
				out.replace(ph, 2, value);

			return out;
		}

	private:
		bool load_file(const std::string& path, std::unordered_map<std::string, std::string>& out) const;

		std::string lang;
		std::unordered_map<std::string, std::string> primary;
		std::unordered_map<std::string, std::string> fallback_zh;
	};
}
