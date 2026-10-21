//////////////////////////////////////////////////////////////////////////////////
//	LocalPngTexture — load a PNG from disk into the GraphicsGL atlas (ensure_raw)
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Template/Point.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	// Custom UI chrome that is not in WZ/NX. Atlas ids must stay out of nlnx bitmap id space.
	class LocalPngTexture
	{
	public:
		LocalPngTexture() = default;

		// Try each candidate path; first readable PNG wins.
		bool load(size_t atlas_id, const std::vector<std::string>& candidate_paths);
		bool is_valid() const;
		int16_t width() const;
		int16_t height() const;

		// Re-ensure after atlas clear, then draw 1:1 or stretched.
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
}
