//////////////////////////////////////////////////////////////////////////////////
//	UiTheme
//////////////////////////////////////////////////////////////////////////////////
#include "UiTheme.h"

#include "UiAssetResolve.h"
#include "../Configuration.h"
#include "Misc.h"

namespace ms
{
	namespace UiTheme
	{
		namespace
		{
			ThemeId clamp_to_available(ThemeId want)
			{
				const bool has_classic = UiAssetResolve::has_statusbar_classic();
				const bool has_modern = UiAssetResolve::has_statusbar3();

				if (want == ThemeId::Classic083)
				{
					if (has_classic)
						return ThemeId::Classic083;
					if (has_modern)
						return ThemeId::ModernStatusBar3;
					return ThemeId::Auto;
				}

				if (want == ThemeId::ModernStatusBar3)
				{
					if (has_modern)
						return ThemeId::ModernStatusBar3;
					if (has_classic)
						return ThemeId::Classic083;
					return ThemeId::Auto;
				}

				// Auto: prefer modern when present (legacy UiAssetResolve::is_classic_hud).
				if (has_modern)
					return ThemeId::ModernStatusBar3;
				if (has_classic)
					return ThemeId::Classic083;
				return ThemeId::Auto;
			}
		}

		ThemeId configured()
		{
			return from_config(Setting<HudTheme>::get().load());
		}

		ThemeId resolve()
		{
			return clamp_to_available(configured());
		}

		LayoutFamily layout_family()
		{
			return resolve() == ThemeId::Classic083
				? LayoutFamily::ClassicV83
				: LayoutFamily::ModernMainBar;
		}

		bool is_classic_layout()
		{
			return layout_family() == LayoutFamily::ClassicV83;
		}

		bool can_use(ThemeId id)
		{
			if (id == ThemeId::Auto)
				return UiAssetResolve::has_game_hud();
			if (id == ThemeId::Classic083)
				return UiAssetResolve::has_statusbar_classic();
			if (id == ThemeId::ModernStatusBar3)
				return UiAssetResolve::has_statusbar3();
			return false;
		}

		ThemeId set_theme(ThemeId id, bool persist)
		{
			ThemeId applied = clamp_to_available(id);

			if (persist)
			{
				// Store the user intent when possible; Auto stays Auto.
				const ThemeId store = (id == ThemeId::Auto) ? ThemeId::Auto : applied;
				Setting<HudTheme>::get().save(to_config(store));
			}

			LOG(LOG_INFO, "UiTheme: set intent=" << to_config(id)
				<< " applied=" << to_config(applied));
			return applied;
		}

		ThemeId toggle(bool persist)
		{
			const ThemeId cur = resolve();
			ThemeId next = (cur == ThemeId::Classic083)
				? ThemeId::ModernStatusBar3
				: ThemeId::Classic083;

			if (!can_use(next))
			{
				LOG(LOG_WARN, "UiTheme: toggle skipped; target pack missing ("
					<< to_config(next) << ")");
				return cur;
			}

			return set_theme(next, persist);
		}

		const char* to_config(ThemeId id)
		{
			switch (id)
			{
			case ThemeId::Classic083:		return "classic";
			case ThemeId::ModernStatusBar3:	return "modern";
			case ThemeId::Auto:
			default:						return "auto";
			}
		}

		ThemeId from_config(const std::string& s)
		{
			if (s == "classic" || s == "083" || s == "v83")
				return ThemeId::Classic083;
			if (s == "modern" || s == "statusbar3" || s == "sb3")
				return ThemeId::ModernStatusBar3;
			return ThemeId::Auto;
		}

		const char* display_name(ThemeId id)
		{
			switch (id)
			{
			case ThemeId::Classic083:		return "083 Classic";
			case ThemeId::ModernStatusBar3:	return "Modern";
			case ThemeId::Auto:
			default:						return "Auto";
			}
		}
	}
}
