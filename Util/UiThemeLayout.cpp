//////////////////////////////////////////////////////////////////////////////////
//	UiThemeLayout — 策略实现与注册表
//////////////////////////////////////////////////////////////////////////////////
#include "UiThemeLayout.h"

#include "UiTheme.h"
#include "UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace UiThemeLayout
	{
		namespace
		{
			bool usable_root(const char* img)
			{
#ifdef USE_NX
				return UiAssetResolve::usable(nl::nx::UI[img]);
#else
				(void)img;
				return false;
#endif
			}

			nl::node ui_node(const char* img)
			{
#ifdef USE_NX
				return nl::nx::UI[img];
#else
				(void)img;
				return nl::node();
#endif
			}

			nl::node first_usable(const char* a, const char* b, const char* c = nullptr)
			{
				nl::node n = ui_node(a);
				if (UiAssetResolve::usable(n)) return n;
				n = ui_node(b);
				if (UiAssetResolve::usable(n)) return n;
				if (c) return ui_node(c);
				return n;
			}

			// ---- 策略：083 Classic（StatusBar.img + UIWindow.img）----
			class Classic083Layout final : public IThemeLayout
			{
			public:
				UiTheme::ThemeId id() const override { return UiTheme::ThemeId::Classic083; }
				UiTheme::LayoutFamily family() const override { return UiTheme::LayoutFamily::ClassicV83; }
				bool available() const override { return UiAssetResolve::has_statusbar_classic(); }

				nl::node status_bar_root() const override { return ui_node("StatusBar.img"); }
				nl::node window_root() const override { return ui_node("UIWindow.img"); }
				nl::node window4_root() const override { return ui_node("UIWindow.img"); }
				nl::node shop_root() const override { return ui_node("UIWindow.img")["Shop"]; }

				LayoutMetrics metrics() const override
				{
					// v83 布局基准在 UIStatusBar::init_classic 内以常量写死；此处给通用默认值，
					// 保留策略接口供后续把魔数迁入，避免一次性大改影响已闭环的经典布局。
					return LayoutMetrics{ Point<int16_t>(0, 0), Point<int16_t>(0, 0), 33, 2 };
				}
			};

			// ---- 策略：Modern StatusBar3（当前 live 的 modern 包）----
			class ModernStatusBar3Layout final : public IThemeLayout
			{
			public:
				UiTheme::ThemeId id() const override { return UiTheme::ThemeId::ModernStatusBar3; }
				UiTheme::LayoutFamily family() const override { return UiTheme::LayoutFamily::ModernMainBar; }
				bool available() const override { return UiAssetResolve::has_statusbar3(); }

				nl::node status_bar_root() const override { return ui_node("StatusBar3.img"); }
				nl::node window_root() const override { return first_usable("UIWindow2.img", "UIWindow.img"); }
				nl::node window4_root() const override
				{
					return first_usable("UIWindow4.img", "UIWindow2.img", "UIWindow.img");
				}
				nl::node shop_root() const override { return first_usable("UIWindow2.img", "UIWindow.img")["Shop2"]; }

				LayoutMetrics metrics() const override
				{
					return LayoutMetrics{ Point<int16_t>(0, 0), Point<int16_t>(0, 0), 33, 2 };
				}
			};

			// ---- 策略：192 StatusBar2（MapleStoryT/怀旧岛192 分卷）----
			// 资源契约由 IDA(0x16BE4E0 OnCreate) + orange-wz(UI_002/004/005.wz) 钉死。
			// live 端当前无 StatusBar2.img，available()=false → 自动回退；导入后即刻生效。
			class StatusBar2Layout final : public IThemeLayout
			{
			public:
				UiTheme::ThemeId id() const override { return UiTheme::ThemeId::StatusBar2_192; }
				UiTheme::LayoutFamily family() const override { return UiTheme::LayoutFamily::ModernMainBar; }
				bool available() const override { return usable_root("StatusBar2.img"); }

				nl::node status_bar_root() const override { return ui_node("StatusBar2.img"); }
				nl::node window_root() const override { return first_usable("UIWindow2.img", "UIWindow.img"); }
				nl::node window4_root() const override
				{
					return first_usable("UIWindow4.img", "UIWindow2.img", "UIWindow.img");
				}
				nl::node shop_root() const override { return first_usable("UIWindow2.img", "UIWindow.img")["Shop2"]; }

				LayoutMetrics metrics() const override
				{
					// 192 IDA 硬证据：CLayoutMan::Init(512,577)@0x16BE531；
					// s_ptShortKeyPos@0x279C390 首格(7,15) pitch=33 第二排 y=48。
					return LayoutMetrics{
						Point<int16_t>(512, 577),
						Point<int16_t>(7, 15),
						33,
						2
					};
				}
			};

			struct Registry
			{
				Classic083Layout classic;
				ModernStatusBar3Layout modern3;
				StatusBar2Layout sb2;
			};

			Registry g_registry;
		}

		const IThemeLayout* get(UiTheme::ThemeId id)
		{
			switch (id)
			{
			case UiTheme::ThemeId::Classic083:        return &g_registry.classic;
			case UiTheme::ThemeId::ModernStatusBar3:  return &g_registry.modern3;
			case UiTheme::ThemeId::StatusBar2_192:    return &g_registry.sb2;
			case UiTheme::ThemeId::Auto:
			default:                                   return nullptr;
			}
		}

		const IThemeLayout* active()
		{
			const IThemeLayout* p = get(UiTheme::resolve());
			return p ? p : &g_registry.classic;
		}
	}
}
