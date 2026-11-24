//////////////////////////////////////////////////////////////////////////////////
// TintContext — per-character ColorPrism tint state + draw-time routing (2026-11-18)
//
// Why draw-time: CharEquips::cloth_cache shares one Clothing per itemId across ALL
// characters (CharEquips.cpp:39-46), so a tint baked into a Clothing frame would recolour
// every wearer of that item. Instead: Clothing construction only REGISTERS bitmap_id -> itemId;
// Char::draw pushes the drawing character's id; Texture::draw asks this context which tint
// (if any) applies to the frame's bitmap and redirects to the TintCache raw atlas id.
// No tint pushed / no entry => original draw path, zero behaviour change.
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TintCache.h"

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef USE_NX
namespace nl { class node; }
#endif

namespace ms
{
	class TintContext
	{
	public:
		static TintContext& get();

		// Clothing/Hair/Face/Body construction registers which logical tint key a bitmap belongs to.
		// tintKey uses the wire space: itemId, itemId+effect bias, HAIR/FACE/SKIN sentinel, skill+bias.
		void register_bitmap(size_t bitmap_id, int32_t tint_key);
#ifdef USE_NX
		// Recursively register every bitmap node under `root` to `tint_key` (one call per look tree).
		void register_tree(nl::node root, int32_t tint_key);
#endif

		// Snapshot from server (0x372F) sets/replaces a character's tint table.
		void set_char_tints(int32_t char_id, const std::unordered_map<int32_t, TintCache::HSL>& tints);
		void clear_char(int32_t char_id);

		// Draw-time scoped current character (RAII). Nested draws supported.
		class Scope
		{
		public:
			explicit Scope(int32_t char_id);
			~Scope();
			Scope(const Scope&) = delete;
			Scope& operator=(const Scope&) = delete;
		private:
			int32_t prev;
			bool had;
		};

		// Called by Texture::draw: resolve the active character's HSL for this frame's bitmap.
		// Returns false (hsl untouched) when no tint applies — caller then draws the original.
		bool resolve_hsl(size_t bitmap_id, TintCache::HSL& out) const;

	private:
		TintContext() = default;
		TintContext(const TintContext&) = delete;
		TintContext& operator=(const TintContext&) = delete;

		mutable std::mutex mutex;
		std::unordered_map<size_t, int32_t> bitmap_to_key;
		std::unordered_map<int32_t, std::unordered_map<int32_t, TintCache::HSL>> char_tints;
		// Draw stack: thread-local since rendering is on one thread but keeps it simple & reentrant.
		static thread_local std::vector<int32_t> stack;
	};
}
