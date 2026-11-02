//////////////////////////////////////////////////////////////////////////////////
//	MemoryTexture + runtime recolour kernel
//////////////////////////////////////////////////////////////////////////////////
#include "MemoryTexture.h"

#include "GraphicsGL.h"
#include "Texture.h"

#include <algorithm>
#include <cstring>

namespace ms
{
	void MemoryTexture::set(size_t id, int16_t width, int16_t height, std::vector<uint8_t> pixels)
	{
		atlas_id = id;
		native_w = width;
		native_h = height;
		bgra = std::move(pixels);
		valid = width > 0 && height > 0
			&& bgra.size() == static_cast<size_t>(width) * static_cast<size_t>(height) * 4;

		if (!valid)
		{
			bgra.clear();
			native_w = 0;
			native_h = 0;
			atlas_id = 0;
			return;
		}

		ensure();
	}

	void MemoryTexture::clear()
	{
		atlas_id = 0;
		native_w = 0;
		native_h = 0;
		bgra.clear();
		valid = false;
	}

	bool MemoryTexture::is_valid() const
	{
		return valid;
	}

	size_t MemoryTexture::get_atlas_id() const
	{
		return atlas_id;
	}

	int16_t MemoryTexture::width() const
	{
		return native_w;
	}

	int16_t MemoryTexture::height() const
	{
		return native_h;
	}

	void MemoryTexture::ensure() const
	{
		if (!valid)
			return;

		GraphicsGL::get().ensure_raw(atlas_id, native_w, native_h, bgra.data());
	}

	void MemoryTexture::draw(Point<int16_t> position) const
	{
		draw(position, native_w, native_h);
	}

	void MemoryTexture::draw(Point<int16_t> position, int16_t dest_w, int16_t dest_h) const
	{
		if (!valid)
			return;

		ensure();
		GraphicsGL::get().draw_raw(atlas_id, position.x(), position.y(), dest_w, dest_h, 1.0f);
	}

	// The source art is 4-bit quantised, so every channel it uses is a multiple of
	// 17 (0, 17, ... 255); the 0.75 scale lands off-lattice and the native frames
	// only ever show on-lattice values again, so the result is snapped back.
	//
	// Written as exact integer arithmetic instead of float so the rounding cannot
	// drift. Derivation, with v = the linear (pre-snap) value:
	//   G/B: v = 3*C/4            -> k = ceil(v/17 - 1/2) = floor((3*C + 33) / 68)
	//   R:   v = (3*R + 273)/4    -> k = floor((3*R + 306) / 68)
	// and out = 17*k. k <= 15 for every C <= 255, so the 255 clamp is implicit.
	//
	// The half-way rule is *measured*, not guessed: 0.75*238 = 178.5 sits exactly
	// between 170 and 187 and the native frame shows 170; 0.75*170 = 127.5 shows
	// 119; 0.75*187+68.25 = 208.5 shows 204. Ties round down, which is what
	// ceil(numerator/denominator) gives. (verify_tint_kernel.py checks the integer
	// kernel is identical to the float kernel used to produce that evidence.)
	static uint8_t tint_gb(uint32_t value)
	{
		return static_cast<uint8_t>(17u * ((3u * value + 33u) / 68u));
	}

	static uint8_t tint_r(uint32_t value)
	{
		return static_cast<uint8_t>(17u * ((3u * value + 306u) / 68u));
	}

	uint8_t tint_disabled_channel_r(uint8_t value)
	{
		return tint_r(value);
	}

	uint8_t tint_disabled_channel_gb(uint8_t value)
	{
		return tint_gb(value);
	}

	void tint_disabled_rect(std::vector<uint8_t>& bgra, int16_t img_w, int16_t img_h, int16_t x, int16_t y, int16_t w, int16_t h)
	{
		if (img_w <= 0 || img_h <= 0)
			return;

		if (bgra.size() < static_cast<size_t>(img_w) * static_cast<size_t>(img_h) * 4)
			return;

		const int32_t x0 = std::max<int32_t>(0, x);
		const int32_t y0 = std::max<int32_t>(0, y);
		const int32_t x1 = std::min<int32_t>(img_w, static_cast<int32_t>(x) + w);
		const int32_t y1 = std::min<int32_t>(img_h, static_cast<int32_t>(y) + h);

		for (int32_t row = y0; row < y1; ++row)
		{
			for (int32_t col = x0; col < x1; ++col)
			{
				// GL_BGRA upload order: b, g, r, a.
				uint8_t* px = bgra.data() + (static_cast<size_t>(row) * img_w + col) * 4;

				px[1] = tint_disabled_channel_gb(px[1]);
				px[2] = tint_disabled_channel_r(px[2]);
				px[0] = tint_disabled_channel_gb(px[0]);
			}
		}
	}

	// nlnx bitmap ids are heap pointers xor an index, and LocalPngTexture uses bare
	// fourcc values; tagging the high bytes keeps tinted crops out of both ranges
	// while the low 48 bits carry the identity of (source bitmap, rect).
	static size_t tinted_atlas_id(size_t source_id, const TintedRect& rect)
	{
		uint64_t hash = 0xCBF29CE484222325ull;
		const uint64_t geometry =
			(static_cast<uint64_t>(static_cast<uint16_t>(rect.x)))
			| (static_cast<uint64_t>(static_cast<uint16_t>(rect.y)) << 16)
			| (static_cast<uint64_t>(static_cast<uint16_t>(rect.w)) << 32)
			| (static_cast<uint64_t>(static_cast<uint16_t>(rect.h)) << 48);

		const uint64_t keys[2] = { static_cast<uint64_t>(source_id), geometry };

		for (uint64_t key : keys)
		{
			for (int byte = 0; byte < 8; ++byte)
			{
				hash ^= (key >> (8 * byte)) & 0xFFull;
				hash *= 0x100000001B3ull;
			}
		}

		return static_cast<size_t>(0x54494E00000000ull | (hash & 0x0000FFFFFFFFFFFFull));
	}

	bool build_tinted_regions(const Texture& src, const std::vector<TintedRect>& rects, std::vector<MemoryTexture>& out)
	{
		out.assign(rects.size(), MemoryTexture());

		if (rects.empty())
			return false;

		std::vector<uint8_t> full;

		if (!src.copy_pixels(full))
			return false;

		const int16_t img_w = src.width();
		const int16_t img_h = src.height();

		if (img_w <= 0 || img_h <= 0
			|| full.size() < static_cast<size_t>(img_w) * static_cast<size_t>(img_h) * 4)
		{
			return false;
		}

		size_t built = 0;

		for (size_t i = 0; i < rects.size(); ++i)
		{
			const TintedRect& rect = rects[i];

			const int32_t x0 = std::max<int32_t>(0, rect.x);
			const int32_t y0 = std::max<int32_t>(0, rect.y);
			const int32_t x1 = std::min<int32_t>(img_w, static_cast<int32_t>(rect.x) + rect.w);
			const int32_t y1 = std::min<int32_t>(img_h, static_cast<int32_t>(rect.y) + rect.h);

			if (x1 <= x0 || y1 <= y0)
				continue;

			const int16_t crop_w = static_cast<int16_t>(x1 - x0);
			const int16_t crop_h = static_cast<int16_t>(y1 - y0);

			std::vector<uint8_t> crop(static_cast<size_t>(crop_w) * static_cast<size_t>(crop_h) * 4);

			for (int32_t row = 0; row < crop_h; ++row)
			{
				const uint8_t* src_row = full.data()
					+ (static_cast<size_t>(y0 + row) * img_w + x0) * 4;
				uint8_t* dst_row = crop.data() + static_cast<size_t>(row) * crop_w * 4;

				std::memcpy(dst_row, src_row, static_cast<size_t>(crop_w) * 4);
			}

			tint_disabled_rect(crop, crop_w, crop_h, 0, 0, crop_w, crop_h);

			// Tinted crops keep the source bitmap id in the key, so a texture
			// reload (different bitmap) cannot reuse a stale atlas entry.
			out[i].set(tinted_atlas_id(src.get_bitmap_id(), TintedRect{
				static_cast<int16_t>(x0), static_cast<int16_t>(y0), crop_w, crop_h }),
				crop_w, crop_h, std::move(crop));
			++built;
		}

		return built > 0;
	}
}
