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

		// The shared window-close button: Basic.img/BtClose (12x12) — a solid blue square
		// carrying a white X. Pixel-matched against four native v83 title bars (角色能力 /
		// 背包 / 好友列表 / 技能), see evidence/r15-closebtn/. BtClose2 (14x14) is only a
		// black outline X over a transparent interior, which reads as a blank white box;
		// it stays as a fallback for packs that ship BtClose without bitmaps.
		nl::node close_btn();

		// True if node exists and has at least one child or is a canvas/bitmap-bearing node.
		bool usable(nl::node n);

		// Release-visible UI asset diagnostic: appends one line to _ui_asset_diag.log.
		// Each distinct tag is written once, so construction-time probes cannot flood the file.
		void diag_once(const std::string& tag, const std::string& message);
	}
}
