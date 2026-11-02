//////////////////////////////////////////////////////////////////////////////////
//	MemoryTexture — BGRA held in memory, uploaded into the GL atlas as a raw id	//
//	+ the runtime recolour kernel used for native "slot unavailable" shading	//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Point.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ms
{
	// Same contract as LocalPngTexture (ensure_raw / draw_raw) but the pixels come
	// from memory instead of a PNG on disk. Needed because the v83 client does not
	// ship a separate "disabled" artwork for UIWindow.img/Equip/backgrnd: it draws
	// the one background bitmap and recolours specific slot rects at runtime.
	//
	// Atlas ids live in the same map as nlnx bitmap ids, and ensure_raw() returns
	// early when an id is already present, so a *changed* pixel set must use a
	// *fresh* id. Callers pick ids from a private ASCII range (nlnx ids are heap
	// pointers and are far larger) and must bump the id whenever content changes.
	class MemoryTexture
	{
	public:
		MemoryTexture() = default;

		// Takes ownership of a w*h*4 BGRA buffer.
		void set(size_t atlas_id, int16_t width, int16_t height, std::vector<uint8_t> bgra);
		void clear();

		bool is_valid() const;
		size_t get_atlas_id() const;
		int16_t width() const;
		int16_t height() const;

		// Re-ensure after an atlas clear, then draw 1:1 or stretched.
		void draw(Point<int16_t> position) const;
		void draw(Point<int16_t> position, int16_t dest_w, int16_t dest_h) const;

	private:
		void ensure() const;

		size_t atlas_id = 0;
		int16_t native_w = 0;
		int16_t native_h = 0;
		std::vector<uint8_t> bgra;
		bool valid = false;
	};

	// v83 "slot unavailable" recolour, measured per-pixel off native frames:
	//   R' = snap17(0.75 * R + 68.25), G' = snap17(0.75 * G), B' = snap17(0.75 * B)
	// where snap17 puts the result back onto the art's 17-multiple lattice (4-bit
	// quantised PNG8 source) with ties rounded down - the tie direction is itself
	// measured (0.75*238 = 178.5 -> frame shows 170, not 187). Implemented as exact
	// integer arithmetic; see MemoryTexture.cpp for the derivation.
	// pred(255, 255, 255) == (255, 187, 187), the dominant colour of a tinted cell.
	uint8_t tint_disabled_channel_r(uint8_t value);
	uint8_t tint_disabled_channel_gb(uint8_t value);

	// Recolours the rect [x, x + w) x [y, y + h) of a w_img-sized BGRA buffer.
	// Out-of-range rects are clipped, not rejected, so callers can pass slot
	// geometry straight through without bounds bookkeeping.
	void tint_disabled_rect(std::vector<uint8_t>& bgra, int16_t img_w, int16_t img_h, int16_t x, int16_t y, int16_t w, int16_t h);

	// A sub-rect of an NX bitmap that should be shown in the "unavailable" shading.
	struct TintedRect
	{
		int16_t x;
		int16_t y;
		int16_t w;
		int16_t h;
	};

	// Crop rects out of src and recolour each into its own MemoryTexture.
	//
	// Why crops and not one tinted copy of the whole background: the native tint is
	// rect-scoped, so drawing a tinted 32x32 crop over the same place is pixel
	// identical to drawing a fully tinted background, and the atlas only pays for
	// the cells. The atlas id is a hash of (source bitmap, rect), so the set is
	// stable across equip/unequip and no id pool has to grow - the predicate only
	// picks which variant of a cell to draw.
	//
	// src is decoded once for all rects (nl::bitmap::data() decompresses on every
	// call). out is resized to rects.size(); entries whose crop failed (empty or
	// fully out of bounds) come back !is_valid() and the caller must fall back to
	// the untinted draw. Caveat: alpha is preserved, so a rect containing
	// transparent pixels lets the untinted background show through there.
	bool build_tinted_regions(const class Texture& src, const std::vector<TintedRect>& rects, std::vector<MemoryTexture>& out);
}
