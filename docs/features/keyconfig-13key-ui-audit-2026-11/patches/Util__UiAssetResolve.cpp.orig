//////////////////////////////////////////////////////////////////////////////////
//	UiAssetResolve
//////////////////////////////////////////////////////////////////////////////////
#include "UiAssetResolve.h"

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

		nl::node status_bar_root()
		{
			if (nl::nx::UI["StatusBar3.img"])
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
			return has_statusbar3() || has_statusbar_classic();
		}

		bool is_classic_hud()
		{
			return !has_statusbar3() && has_statusbar_classic();
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
