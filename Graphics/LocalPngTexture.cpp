//////////////////////////////////////////////////////////////////////////////////
//	LocalPngTexture
//////////////////////////////////////////////////////////////////////////////////
#include "LocalPngTexture.h"

#include "GraphicsGL.h"

#include <stb_image.h>

#include <fstream>

namespace ms
{
	bool LocalPngTexture::load(size_t id, const std::vector<std::string>& candidate_paths)
	{
		valid = false;
		bgra.clear();
		native_w = 0;
		native_h = 0;
		atlas_id = id;

		for (const std::string& path : candidate_paths)
		{
			std::ifstream probe(path, std::ios::binary);

			if (!probe.good())
				continue;

			probe.close();

			int w = 0;
			int h = 0;
			int comp = 0;
			unsigned char* rgba = stbi_load(path.c_str(), &w, &h, &comp, 4);

			if (!rgba || w <= 0 || h <= 0)
			{
				if (rgba)
					stbi_image_free(rgba);

				continue;
			}

			native_w = static_cast<int16_t>(w);
			native_h = static_cast<int16_t>(h);
			bgra.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 4);

			for (size_t i = 0; i < bgra.size(); i += 4)
			{
				bgra[i + 0] = rgba[i + 2];
				bgra[i + 1] = rgba[i + 1];
				bgra[i + 2] = rgba[i + 0];
				bgra[i + 3] = rgba[i + 3];
			}

			stbi_image_free(rgba);
			valid = true;
			ensure();
			return true;
		}

		return false;
	}

	bool LocalPngTexture::is_valid() const
	{
		return valid;
	}

	int16_t LocalPngTexture::width() const
	{
		return native_w;
	}

	int16_t LocalPngTexture::height() const
	{
		return native_h;
	}

	void LocalPngTexture::ensure() const
	{
		if (!valid)
			return;

		GraphicsGL::get().ensure_raw(atlas_id, native_w, native_h, bgra.data());
	}

	void LocalPngTexture::draw(Point<int16_t> position) const
	{
		draw(position, native_w, native_h);
	}

	void LocalPngTexture::draw(Point<int16_t> position, int16_t dest_w, int16_t dest_h) const
	{
		if (!valid)
			return;

		ensure();
		GraphicsGL::get().draw_raw(atlas_id, position.x(), position.y(), dest_w, dest_h, 1.0f);
	}
}
