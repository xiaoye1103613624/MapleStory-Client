//////////////////////////////////////////////////////////////////////////////////
// TintCache.cpp — HSL recolour kernel + raw atlas cache
//////////////////////////////////////////////////////////////////////////////////
#include "TintCache.h"

#include "GraphicsGL.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace ms
{
	namespace
	{
		// Ported verbatim from weapontint.cpp:215-231 (RGB in 0..255, HSV out).
		void rgb_to_hsv(int r, int g, int b, float& h, float& s, float& v)
		{
			const int mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
			const int mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
			const int d = mx - mn;
			v = mx / 255.0f;
			s = mx == 0 ? 0.0f : static_cast<float>(d) / mx;
			if (d == 0) h = 0.0f;
			else if (mx == r) h = 60.0f * std::fmod(static_cast<float>(g - b) / d, 6.0f);
			else if (mx == g) h = 60.0f * ((static_cast<float>(b - r) / d) + 2.0f);
			else h = 60.0f * ((static_cast<float>(r - g) / d) + 4.0f);
			if (h < 0.0f) h += 360.0f;
		}

		// Ported verbatim from weapontint.cpp:233-249.
		void hsv_to_rgb(float h, float s, float v, int& r, int& g, int& b)
		{
			const float c = v * s;
			const float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
			const float m = v - c;
			float rf, gf, bf;
			if (h < 60.0f) { rf = c; gf = x; bf = 0; }
			else if (h < 120.0f) { rf = x; gf = c; bf = 0; }
			else if (h < 180.0f) { rf = 0; gf = c; bf = x; }
			else if (h < 240.0f) { rf = 0; gf = x; bf = c; }
			else if (h < 300.0f) { rf = x; gf = 0; bf = c; }
			else { rf = c; gf = 0; bf = x; }
			auto to8 = [](float f) {
				int n = static_cast<int>(f * 255.0f + 0.5f);
				return n < 0 ? 0 : (n > 255 ? 255 : n);
			};
			r = to8(rf + m); g = to8(gf + m); b = to8(bf + m);
		}

		// Hue semantic: positive rotate, negative absolute -(deg+1).
		float resolve_hue(int16_t hue, float h)
		{
			if (hue >= 0)
			{
				float out = h + static_cast<float>(hue);
				while (out >= 360.0f) out -= 360.0f;
				while (out < 0.0f) out += 360.0f;
				return out;
			}
			return static_cast<float>(-(static_cast<int>(hue) + 1)); // absolute target
		}

		// Ported from weapontint.cpp:253-292 TintPixel. BGRA buffer layout is
		// [b,g,r,a] per pixel (GL_BGRA upload contract, Texture.cpp:149-150).
		void tint_bgra(std::vector<uint8_t>& bgra, const TintCache::HSL& t)
		{
			const float chroma = static_cast<float>(t.chroma) / 100.0f;
			const float bright = static_cast<float>(t.bright) / 100.0f;
			for (size_t i = 0; i + 3 < bgra.size(); i += 4)
			{
				const uint8_t a = bgra[i + 3];
				int r = bgra[i + 2], g = bgra[i + 1], b = bgra[i];
				// Transparent AND black has no colour to rotate (weapontint.cpp:264);
				// transparent-but-coloured is kept (additive skill effects need it).
				if (a == 0 && r == 0 && g == 0 && b == 0)
					continue;
				float h, s, v;
				rgb_to_hsv(r, g, b, h, s, v);
				h = resolve_hue(t.hue, h);
				// Towards the bound, not scaled by it (weapontint.cpp:272-286).
				s = chroma < 0.0f ? s * (1.0f + chroma) : s + (1.0f - s) * chroma;
				v = bright < 0.0f ? v * (1.0f + bright) : v + (1.0f - v) * bright;
				s = std::clamp(s, 0.0f, 1.0f);
				v = std::clamp(v, 0.0f, 1.0f);
				hsv_to_rgb(h, s, v, r, g, b);
				bgra[i] = static_cast<uint8_t>(b);
				bgra[i + 1] = static_cast<uint8_t>(g);
				bgra[i + 2] = static_cast<uint8_t>(r);
				// alpha untouched
			}
		}
	}

	size_t TintCache::KeyHash::operator()(const Key& k) const noexcept
	{
		size_t h = k.bitmap_id;
		h ^= static_cast<size_t>(static_cast<uint16_t>(k.hue)) * 0x9E3779B1u;
		h ^= static_cast<size_t>(static_cast<uint8_t>(k.chroma)) * 0x85EBCA6Bu;
		h ^= static_cast<size_t>(static_cast<uint8_t>(k.bright)) * 0xC2B2AE35u;
		return h;
	}

	TintCache& TintCache::get()
	{
		static TintCache inst;
		return inst;
	}

	size_t TintCache::tinted(const Texture& src, const HSL& hsl)
	{
		if (is_identity(hsl) || !src.is_valid())
			return 0;

		const Key key{src.get_bitmap_id(), hsl.hue, hsl.chroma, hsl.bright};
		{
			std::lock_guard<std::mutex> lock(mutex);
			auto it = cache.find(key);
			if (it != cache.end())
				return it->second;
		}

		std::vector<uint8_t> bgra;
		if (!src.copy_pixels(bgra) || bgra.empty())
			return 0;
		tint_bgra(bgra, hsl);

		std::lock_guard<std::mutex> lock(mutex);
		auto it = cache.find(key);
		if (it != cache.end())
			return it->second;
		const size_t id = next_id++;
		GraphicsGL::get().ensure_raw(id, src.width(), src.height(), bgra.data());
		cache.emplace(key, id);
		return id;
	}
}
