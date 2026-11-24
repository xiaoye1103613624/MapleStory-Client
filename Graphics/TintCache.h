//////////////////////////////////////////////////////////////////////////////////
// TintCache — runtime HSL recolour of a Texture's BGRA pixels (native port 2026-11-18)
//
// Ported from coloring-prism/client/weapontint.cpp RGB<->HSV + TintPixel (lines 215-292),
// but WITHOUT the v83 Detours WZ-tree swap: the native client decodes each frame into a
// Texture (BGRA in memory, Texture::copy_pixels) and the Atlas uploads raw BGRA via
// GraphicsGL::ensure_raw. So recolour = copy pixels -> HSL transform -> upload raw -> hand
// the fresh raw atlas id back to the caller. No WZ mutation, no hook, no RAII guard.
//
// HUE is a SIGNED short: positive rotates, negative is an absolute target -(degrees+1).
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Graphics/Texture.h"

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace ms
{
	class TintCache
	{
	public:
		struct HSL
		{
			int16_t hue = 0;      // signed: >=0 rotate, <0 absolute -(deg+1)
			int8_t chroma = 0;    // -100..100
			int8_t bright = 0;    // -100..100

			bool operator==(const HSL& o) const
			{
				return hue == o.hue && chroma == o.chroma && bright == o.bright;
			}
		};

		static TintCache& get();

		// Return a raw atlas id holding `src` recoloured by hsl, or 0 on failure
		// (identity tint returns 0 too = "draw the original"). Stable per (bitmap id, hsl),
		// so repeated frames reuse one upload; a changed HSL gets a fresh id.
		size_t tinted(const Texture& src, const HSL& hsl);

		// Identity check, shared with the server contract.
		static bool is_identity(const HSL& hsl)
		{
			return hsl.hue == 0 && hsl.chroma == 0 && hsl.bright == 0;
		}

	private:
		TintCache() = default;
		TintCache(const TintCache&) = delete;
		TintCache& operator=(const TintCache&) = delete;

		struct Key
		{
			size_t bitmap_id;
			int16_t hue;
			int8_t chroma;
			int8_t bright;

			bool operator==(const Key& o) const
			{
				return bitmap_id == o.bitmap_id && hue == o.hue
					&& chroma == o.chroma && bright == o.bright;
			}
		};
		struct KeyHash
		{
			size_t operator()(const Key& k) const noexcept;
		};

		std::mutex mutex;
		std::unordered_map<Key, size_t, KeyHash> cache;
		size_t next_id = 0xF0000000u;   // private ASCII-ish range, far above nlnx heap pointer ids
	};
}
