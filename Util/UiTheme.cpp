//////////////////////////////////////////////////////////////////////////////////
//	UiTheme
//////////////////////////////////////////////////////////////////////////////////
#include "UiTheme.h"

#include "UiAssetResolve.h"
#include "UiThemeLayout.h"
#include "../Configuration.h"
#include "Misc.h"

namespace ms
{
	namespace UiTheme
	{
		namespace
		{
			// 策略优先级：Auto 按 192 > modern3 > classic 探测（高版本资源存在即优先）。
			// 新增版本只需在这里加一项 + 注册策略，不再散落 if/else。
			static const ThemeId kProbeOrder[] = {
				ThemeId::StatusBar2_192,
				ThemeId::ModernStatusBar3,
				ThemeId::Classic083
			};

			ThemeId clamp_to_available(ThemeId want)
			{
				if (want != ThemeId::Auto)
				{
					const UiThemeLayout::IThemeLayout* p = UiThemeLayout::get(want);
					if (p && p->available())
						return want;
					// 请求的主题不可用：按优先级回退到第一个可用策略。
					want = ThemeId::Auto;
				}

				for (ThemeId id : kProbeOrder)
				{
					const UiThemeLayout::IThemeLayout* p = UiThemeLayout::get(id);
					if (p && p->available())
						return id;
				}
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
			const UiThemeLayout::IThemeLayout* p = UiThemeLayout::active();
			return p ? p->family() : LayoutFamily::ClassicV83;
		}

		bool is_classic_layout()
		{
			return layout_family() == LayoutFamily::ClassicV83;
		}

		bool can_use(ThemeId id)
		{
			if (id == ThemeId::Auto)
				return UiAssetResolve::has_game_hud();
			const UiThemeLayout::IThemeLayout* p = UiThemeLayout::get(id);
			return p && p->available();
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
			// 在所有可用策略间循环（192 > modern3 > classic），保证多版本都可切换。
			for (ThemeId id : kProbeOrder)
			{
				if (id == cur)
					continue;
				if (can_use(id))
					return set_theme(id, persist);
			}
			// 只有一个可用主题：无其它目标可切。
			LOG(LOG_WARN, "UiTheme: toggle skipped; only one theme pack available");
			return cur;
		}

		const char* to_config(ThemeId id)
		{
			switch (id)
			{
			case ThemeId::Classic083:		return "classic";
			case ThemeId::ModernStatusBar3:	return "modern";
			case ThemeId::StatusBar2_192:	return "192";
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
			if (s == "192" || s == "statusbar2" || s == "sb2")
				return ThemeId::StatusBar2_192;
			return ThemeId::Auto;
		}

		const char* display_name(ThemeId id)
		{
			switch (id)
			{
			case ThemeId::Classic083:		return "083 Classic";
			case ThemeId::ModernStatusBar3:	return "Modern";
			case ThemeId::StatusBar2_192:	return "192 Classic";
			case ThemeId::Auto:
			default:						return "Auto";
			}
		}
	}
}
