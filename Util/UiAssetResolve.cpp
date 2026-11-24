//////////////////////////////////////////////////////////////////////////////////
//	UiAssetResolve
//////////////////////////////////////////////////////////////////////////////////
#include "UiAssetResolve.h"

#include "UiTheme.h"
#include "UiThemeLayout.h"

#include <fstream>
#include <mutex>
#include <unordered_set>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// One-shot guard so a per-frame path can never append in a loop.
		std::unordered_set<std::string> g_diag_seen;
		std::mutex g_diag_mtx;
	}

	namespace UiAssetResolve
	{
		void diag_once(const std::string& tag, const std::string& message)
		{
			{
				std::lock_guard<std::mutex> lock(g_diag_mtx);

				if (!g_diag_seen.insert(tag).second)
					return;
			}

			std::ofstream out("_ui_asset_diag.log", std::ios::app);

			if (!out)
				return;

			out << "[" << tag << "] " << message << "\n";
		}

		nl::node window(const char* name)
		{
			nl::node modern = nl::nx::UI["UIWindow2.img"][name];

			if (modern)
				return modern;

			nl::node legacy = nl::nx::UI["UIWindow.img"][name];

			if (!legacy)
				diag_once(std::string("missing window ") + name, "neither UIWindow2.img nor UIWindow.img has it");

			return legacy;
		}

		nl::node window_enh(const char* name)
		{
			// D2=B: enhancement UIs live in a dedicated UIWindow2Enh.img so the
			// 64MB 192 pack never bloats UI.nx. Fall back to the classic window
			// root for nodes the standalone pack does not carry.
			nl::node enh = nl::nx::UI["UIWindow2Enh.img"][name];

			if (enh)
				return enh;

			nl::node win = nl::nx::UI["UIWindow2.img"][name];

			if (win)
				return win;

			return nl::nx::UI["UIWindow.img"][name];
		}

		nl::node tooltip()
		{
			// D4=B: 192 graphical tooltip root (Item/Equip/{Star,AddOption,Summary,...}).
			nl::node tt = nl::nx::UI["UIToolTip.img"];

			if (tt)
				return tt;

			return nl::node();
		}

		nl::node window4(const char* name)
		{
			nl::node w4 = nl::nx::UI["UIWindow4.img"][name];

			if (w4)
				return w4;

			nl::node w2 = nl::nx::UI["UIWindow2.img"][name];

			if (w2)
				return w2;

			nl::node legacy = nl::nx::UI["UIWindow.img"][name];

			if (!legacy)
				diag_once(std::string("missing window4 ") + name, "no UIWindow4/2/img root");

			return legacy;
		}

		namespace
		{
			bool looks_like_native_equip_shell(nl::node equip)
			{
				// 192 UIWindow4 Equip：多页签壳（Tab / Android / Cash / Pet…），
				// 与 classic UIWindow.img/Equip（单页 + pet / BtPet*）结构不同。
				if (!equip)
					return false;

				if (equip["Tab"] || equip["Android"] || equip["Dragon"] || equip["Mechanic"])
					return true;

				// 原生壳根级无 pet 子节点；classic 有。
				if (equip["backgrnd"] && !equip["pet"] && (equip["Equip"] || equip["Cash"]))
					return true;

				return false;
			}
		}

		nl::node equip_native_root()
		{
			nl::node w4 = nl::nx::UI["UIWindow4.img"]["Equip"];

			if (looks_like_native_equip_shell(w4))
				return w4;

			return window4("Equip");
		}

		nl::node equip_panel_root()
		{
			nl::node classic = nl::nx::UI["UIWindow.img"]["Equip"];
			nl::node w4 = nl::nx::UI["UIWindow4.img"]["Equip"];

			// 192 多页签已入库时，主角色装备栏继续吃 classic，避免 backgrnd/pet 错位。
			if (looks_like_native_equip_shell(w4) && usable(classic))
				return classic;

			if (usable(w4) && !looks_like_native_equip_shell(w4))
				return w4;

			if (usable(classic))
				return classic;

			return window4("Equip");
		}

		nl::node status_bar_root()
		{
			// ThemePack 策略：资源根由当前生效策略决定；新增版本不再改这里。
			const UiThemeLayout::IThemeLayout* p = UiThemeLayout::active();
			if (p)
			{
				nl::node root = p->status_bar_root();
				if (usable(root))
					return root;
			}
			// 兜底：任何策略资源缺失时保底可用包，避免 HUD 消失。
			if (has_statusbar3())
				return nl::nx::UI["StatusBar3.img"];
			return nl::nx::UI["StatusBar.img"];
		}

		bool has_statusbar3()
		{
			return static_cast<bool>(nl::nx::UI["StatusBar3.img"]);
		}

		bool has_statusbar_classic()
		{
			return static_cast<bool>(nl::nx::UI["StatusBar.img"]["base"]);
		}

		bool has_game_hud()
		{
			const UiThemeLayout::IThemeLayout* p = UiThemeLayout::active();
			if (p && p->available())
				return true;
			return has_statusbar3() || has_statusbar_classic();
		}

		bool is_classic_hud()
		{
			// Theme-aware: when both packs ship, ui.theme selects the layout family.
			return UiTheme::is_classic_layout();
		}

		nl::node shop_root()
		{
			nl::node shop2 = nl::nx::UI["UIWindow2.img"]["Shop2"];

			if (shop2)
				return shop2;

			return nl::nx::UI["UIWindow.img"]["Shop"];
		}

		nl::node close_btn()
		{
			// BtClose first: it is the blue-square/white-X title-bar close that the native
			// v83 client draws (see header). BtClose2/3 are only empty-shell fallbacks.
			static const char* const order[] = { "BtClose", "BtClose2", "BtClose3" };

			for (const char* name : order)
			{
				nl::node btn = nl::nx::UI["Basic.img"][name];

				if (usable(btn))
					return btn;
			}

			diag_once("close_btn", "no usable Basic.img/BtClose* button");
			return nl::node();
		}

		bool has_quit_reward_dialog()
		{
			// UIQuit loads nl::nx::UI["UIWindow6.img"]["askReward"]["userLog"].
			// The BeiDou v83 pack has no UIWindow6.img at all, so this is always
			// false there and callers must fall back to UIQuitConfirm (Notice board).
			// NOTE: window() deliberately does not probe UIWindow6.img; query the
			// image directly so a pack that DOES ship it can still use UIQuit.
			nl::node reward = nl::nx::UI["UIWindow6.img"]["askReward"];
			bool ok = usable(reward) && usable(reward["userLog"]);

			if (!ok)
				diag_once("quit_reward_missing", "UIWindow6.img/askReward/userLog unavailable; use UIQuitConfirm");

			return ok;
		}

		bool usable(nl::node n)
		{
			if (!n)
				return false;

			// A node that merely exists is not usable: the v83 BeiDou pack carries
			// several empty shells (present name, no children, no bitmap) which used
			// to pass the old bool(node) test and left windows frameless.
			if (n.size() > 0)
				return true;

			return n.data_type() == nl::node::type::bitmap;
		}
	}
}
