//////////////////////////////////////////////////////////////////////////////////
//	UiAssetResolve
//////////////////////////////////////////////////////////////////////////////////
#include "UiAssetResolve.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace UiAssetResolve
	{
		nl::node window(const char* name)
		{
			nl::node modern = nl::nx::UI["UIWindow2.img"][name];

			if (modern)
				return modern;

			return nl::nx::UI["UIWindow.img"][name];
		}

		nl::node window4(const char* name)
		{
			nl::node w4 = nl::nx::UI["UIWindow4.img"][name];

			if (w4)
				return w4;

			nl::node w2 = nl::nx::UI["UIWindow2.img"][name];

			if (w2)
				return w2;

			return nl::nx::UI["UIWindow.img"][name];
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

		bool usable(nl::node n)
		{
			return static_cast<bool>(n);
		}
	}
}
