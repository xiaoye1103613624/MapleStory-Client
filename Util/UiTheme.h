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
			Auto = 0,			// follow pack probe (StatusBar3 if present)
			Classic083,			// StatusBar.img layout family
			ModernStatusBar3	// StatusBar3.img / mainBar layout family
			// Reserved for future packs (StatusBar2 / 192skin): extend here.
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
