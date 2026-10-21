//////////////////////////////////////////////////////////////////////////////////
//	UiAssetResolve — prefer modern UI roots, fall back to v83 BeiDou packs.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

// Defines USE_NX. This header's include chain does not reach MapleStory.h on
// its own, so the nl::node declarations below need it pulled in explicitly.
#include "../MapleStory.h"

#ifdef USE_NX
#include <nlnx/node.hpp>
#endif

#include <string>

namespace ms
{
	namespace UiAssetResolve
	{
		// Prefer UIWindow2.img[name], else UIWindow.img[name].
		nl::node window(const char* name);

		// Prefer UIWindow4.img[name], else UIWindow2, else UIWindow.
		nl::node window4(const char* name);

		// Prefer StatusBar3.img, else StatusBar.img.
		nl::node status_bar_root();
		bool has_statusbar3();
		bool has_statusbar_classic();
		bool has_game_hud();
		// True when only classic StatusBar.img is present (BeiDou v83).
		// Shared gate for KeyConfig / StatusBar / Equip / Cursor classic layout tables.
		bool is_classic_hud();

		// Prefer UIWindow2.img/Shop2, else UIWindow.img/Shop.
		nl::node shop_root();

		// True if node exists and has at least one child or is a canvas/bitmap-bearing node.
		bool usable(nl::node n);
	}
}
