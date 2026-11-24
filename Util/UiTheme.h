//////////////////////////////////////////////////////////////////////////////////
//	UiTheme — HUD / UI ThemePack selection (classic 083 vs modern StatusBar3).
//	Presentation only; protocol and gameplay stay on the current s9/083 stack.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>

namespace ms
{
	namespace UiTheme
	{
		enum class ThemeId : uint8_t
		{
			Auto = 0,			// follow pack probe (best available pack)
			Classic083,			// StatusBar.img layout family (083)
			ModernStatusBar3,	// StatusBar3.img / mainBar layout family
			StatusBar2_192		// 192 / MapleStoryT: StatusBar2.img + UIWindow2/4（ResMan 分卷）
			// 新增版本：加枚举 + 在 UiThemeLayout.cpp 注册一个策略即可，勿在消费端堆 if/else。
		};

		enum class LayoutFamily : uint8_t
		{
			ClassicV83,
			ModernMainBar
		};

		// Configured preference from ui.theme (may be Auto).
		ThemeId configured();
		// Effective theme after asset fallback.
		ThemeId resolve();
		LayoutFamily layout_family();
		bool is_classic_layout();

		// Persist preference and return the theme actually applied (after fallback).
		ThemeId set_theme(ThemeId id, bool persist);
		// Flip Classic083 <-> ModernStatusBar3 when both packs exist; otherwise no-op.
		ThemeId toggle(bool persist);

		bool can_use(ThemeId id);
		const char* to_config(ThemeId id);
		ThemeId from_config(const std::string& s);
		const char* display_name(ThemeId id);
	}
}
