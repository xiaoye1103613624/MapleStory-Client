//////////////////////////////////////////////////////////////////////////////////
//	LocaleBindings — 「静态文案绑定表」的唯一实现
//
//	从 UIElement 里抽出来的原因：世界地图那三块吸附面板（Mob / Drop / Info）
//	按设计不是 UIElement（由 UIWorldMap 直接持有并逐帧 update），但它们同样
//	需要「切语言后已开着的窗自己换文案」。如果在每个面板里再写一份
//	代次比较，机制就有了第二套实现，迟早走样。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/SpecialText.h"
#include "../Graphics/Text.h"
#include "../Util/Locale.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace ms
{
	/// 宿主登记「值在会话内恒定」的文案对象与 Locale 键，语言重载后由宿主在自己的
	/// update 里调 refresh() 重取。
	///
	/// 判据（与 UIElement::bind_text 同一把尺）：全文搜这个对象还有没有别的写入点。
	/// 有 —— 且那个改写是事件驱动的（选中变化、数据到达）—— 就不许进表：切语言时
	/// 刷回初值之后没人再把它写回去，界面会一直停在初值上。那种走 Locale::text 就地现算。
	class LocaleBindings
	{
	public:
		explicit LocaleBindings(std::string host = "UIElement")
			: host(std::move(host))
		{
		}

		// 表里存的是指向宿主自己按值持有的 Text 的裸指针；一旦宿主被拷贝，指针就指到
		// 原件身上（改动落在看不见的地方）。直接禁拷贝，把这件事变成编译期错误。
		LocaleBindings(const LocaleBindings&) = delete;
		LocaleBindings& operator=(const LocaleBindings&) = delete;

		void bind(Text& target, std::string key)
		{
			bind(target, std::move(key), std::string());
		}

		void bind(Text& target, std::string key, std::string value)
		{
			entries.push_back({ &target, nullptr, std::move(key), std::move(value) });
			commit();
		}

		void bind(OutlinedText& target, std::string key)
		{
			bind(target, std::move(key), std::string());
		}

		void bind(OutlinedText& target, std::string key, std::string value)
		{
			entries.push_back({ nullptr, &target, std::move(key), std::move(value) });
			commit();
		}

		/// 代次变了才逐条重取；未变时成本只有一次整数比较。返回 true = 本次真的重取过，
		/// 供宿主决定要不要打那条上线级日志（日志文案带宿主名，UIElement 那边还想带 type）。
		bool refresh()
		{
			if (entries.empty())
				return false;

			const std::uint32_t next = Locale::get().generation();

			if (next == gen)
				return false;

			gen = next;

			for (const Entry& entry : entries)
			{
				const std::string text = text_of(entry);

				if (entry.target)
					entry.target->change_text(text);
				else
					entry.outlined->change_text(text);
			}

			return true;
		}

		bool empty() const
		{
			return entries.empty();
		}

		size_t count() const
		{
			return entries.size();
		}

		const std::string& hostname() const
		{
			return host;
		}

	private:
		struct Entry
		{
			Text* target = nullptr;
			OutlinedText* outlined = nullptr;
			std::string key;
			std::string value;
		};

		static std::string text_of(const Entry& entry)
		{
			return entry.value.empty()
				? Locale::text(entry.key)
				: Locale::format(entry.key, entry.value);
		}

		/// 登记完立刻取一次文案，并把代次对齐到现在 —— 于是不出现「登记了但当前还是
		/// 英文字面量」的空窗，也不会让紧接着的第一帧白重取一遍。
		void commit()
		{
			const Entry& entry = entries.back();

			if (entry.target)
				entry.target->change_text(text_of(entry));
			else
				entry.outlined->change_text(text_of(entry));

			gen = Locale::get().generation();
		}

		std::vector<Entry> entries;
		std::uint32_t gen = 0;
		std::string host;
	};
}
