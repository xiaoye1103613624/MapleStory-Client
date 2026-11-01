//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Text.h"

#include "../Constants.h"
#include "../Error.h"

#include "../Util/QuadTree.h"

#include <cstdint>
#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef USE_OPENGL_ES
#include <glew.h>
#else
#include <GLES3/gl3.h>
#endif

#ifdef USE_NX
#include <nlnx/bitmap.hpp>
#endif

namespace ms
{
	// Graphics engine which uses OpenGL
	class GraphicsGL : public Singleton<GraphicsGL>
	{
	public:
		GraphicsGL();

		// Initialize all resources
		Error init();
		// Re-initialize after changing screen modes
		void reinit();

		// Clear all bitmaps if most of the space is used up (threshold clear).
		void clear();
		// Force-unload the GPU atlas (map transfer / memory pressure).
		// Bitmaps re-upload lazily on next draw via getoffset().
		void unload_atlas();

		// Add a bitmap to the available resources
		void addbitmap(const nl::bitmap& bmp);
		// Draw the bitmap with the given parameters
		void draw(const nl::bitmap& bmp, const Rectangle<int16_t>& rect, const Range<int16_t>& vertical, const Range<int16_t>& horizontal, const Color& color, float angle);

		// Local PNG / raw BGRA upload into the shared atlas (ids should avoid nlnx collisions).
		void ensure_raw(size_t id, int16_t width, int16_t height, const uint8_t* bgra);
		void draw_raw(size_t id, int16_t x, int16_t y, int16_t width, int16_t height, float alpha = 1.0f);

		// Create a layout for the text with the parameters specified
		Text::Layout createlayout(const std::string& text, Text::Font font, Text::Alignment alignment, Color::Name color, int16_t maxwidth, bool formatted, int16_t line_adj);
		// Draw a text with the given parameters
		void drawtext(const DrawArgument& args, const Range<int16_t>& vertical, const std::string& text, const Text::Layout& layout, Text::Font font, Color::Name color, Text::Background back);

		// Draw a rectangle filled with the specified color
		void drawrectangle(int16_t x, int16_t y, int16_t width, int16_t height, float red, float green, float blue, float alpha);
		// Fill the screen with the specified color
		void drawscreenfill(float red, float green, float blue, float alpha);

		// Lock the current scene
		void lock();
		// Unlock the scene
		void unlock();
		bool is_locked() const;

		// Draw the buffer contents with the specified scene opacity
		void flush(float opacity);
		// Clear the buffer contents
		void clearscene();

	private:
		void clearinternal();
		bool addfont(const char* name, Text::Font id, FT_UInt width, FT_UInt height);

		struct Offset
		{
			GLshort left;
			GLshort right;
			GLshort top;
			GLshort bottom;
			// 最近绘制代数；atlas 满时整表 clearinternal（打包 atlas 无法局部回收）。
			uint32_t last_use;

			Offset(GLshort x, GLshort y, GLshort width, GLshort height)
			{
				left = x;
				right = x + width;
				top = y;
				bottom = y + height;
				last_use = 0;
			}

			Offset()
			{
				left = 0;
				right = 0;
				top = 0;
				bottom = 0;
				last_use = 0;
			}
		};

		// Add a bitmap to the available resources
		const Offset& getoffset(const nl::bitmap& bmp);

		struct Leftover
		{
			GLshort left;
			GLshort right;
			GLshort top;
			GLshort bottom;

			Leftover(GLshort x, GLshort y, GLshort width, GLshort height)
			{
				left = x;
				right = x + width;
				top = y;
				bottom = y + height;
			}

			Leftover()
			{
				left = 0;
				right = 0;
				top = 0;
				bottom = 0;
			}

			GLshort width() const
			{
				return right - left;
			}

			GLshort height() const
			{
				return bottom - top;
			}
		};

		struct Quad
		{
			struct Vertex
			{
				// Local Space Position
				GLshort localcoord_x;
				GLshort localcoord_y;

				// Texture Coordinates
				GLshort texcoord_x;
				GLshort texcoord_y;

				Color color;
			};

			static const size_t LENGTH = 4;
			Vertex vertices[LENGTH];

			Quad(GLshort left, GLshort right, GLshort top, GLshort bottom, const Offset& offset, const Color& color, GLfloat rotation)
			{
				vertices[0] = { left, top, offset.left, offset.top, color };
				vertices[1] = { left, bottom, offset.left, offset.bottom, color };
				vertices[2] = { right, bottom, offset.right, offset.bottom, color };
				vertices[3] = { right, top, offset.right, offset.top, color };

				if (rotation != 0.0f)
				{
					GLfloat cos = std::cos(rotation);
					GLfloat sin = std::sin(rotation);
					GLshort center_x = (left + right) / 2;
					GLshort center_y = (top + bottom) / 2;

					for (size_t i = 0; i < LENGTH; i++)
					{
						GLshort vertice_x = vertices[i].localcoord_x - center_x;
						GLshort vertice_y = vertices[i].localcoord_y - center_y;
						GLfloat rounded_x = std::roundf(vertice_x * cos - vertice_y * sin);
						GLfloat rounded_y = std::roundf(vertice_x * sin + vertice_y * cos);
						vertices[i].localcoord_x = static_cast<GLshort>(rounded_x + center_x);
						vertices[i].localcoord_y = static_cast<GLshort>(rounded_y + center_y);
					}
				}
			}
		};

		struct Font
		{
			struct Char
			{
				GLshort ax;
				GLshort ay;
				GLshort bw;
				GLshort bh;
				GLshort bl;
				GLshort bt;
				Offset offset;
			};

			GLshort width;
			GLshort height;
			Char chars[128];

			// CJK 按需字形：码点 → 字形。addfont 只烘焙 ASCII(32..127)，
			// 中文（UTF-8 多字节）在此缓存，首用时从 face 现场生成并上传到
			// init 预留的 wide 区（与 ASCII 字形同属字体保留条带，
			// clearinternal 的位图逐出不会回收，无需失效逻辑）。
			FT_Face face = nullptr;
			std::unordered_map<uint32_t, Char> wide;
			GLshort wide_x = 0;
			GLshort wide_y = 0;
			bool wide_full = false;

			Font(GLshort w, GLshort h)
			{
				width = w;
				height = h;
			}

			Font()
			{
				width = 0;
				height = 0;
			}

			int16_t linespace() const
			{
				return static_cast<int16_t>(height * 1.35 + 1);
			}
		};

		class LayoutBuilder
		{
		public:
			LayoutBuilder(Text::Font id, const Font& font, Text::Alignment alignment, Color::Name color, int16_t maxwidth, bool formatted, int16_t line_adj);

			size_t add(const char* text, size_t prev, size_t first, size_t last);
			Text::Layout finish(size_t first, size_t last);

		private:
			void add_word(size_t first, size_t last, Text::Font font, Color::Name color);
			void add_line();

			// text[pos] 处这个字符的推进量，consumed 输出它占的字节数。
			// 与 drawtext 共用 utf8_next，保证"量的"和"画的"不会分叉。
			int16_t measure(const char* text, size_t pos, size_t last, size_t& consumed) const;
			// 非 ASCII 码点的推进量：只取度量、不生成位图（图集锁定期间也要能量）。
			int16_t wide_advance(uint32_t codepoint) const;

			const Font& font;

			Text::Alignment alignment;
			Text::Font fontid;
			Color::Name color;
			int16_t maxwidth;
			bool formatted;

			int16_t ax;
			int16_t ay;

			std::vector<Text::Layout::Line> lines;
			std::vector<Text::Layout::Word> words;
			std::vector<int16_t> advances;
			int16_t width;
			int16_t endy;
			int16_t line_adj;
		};

		int16_t VWIDTH;
		int16_t VHEIGHT;
		Rectangle<int16_t> SCREEN;

		static const GLshort ATLASW = 8192;
		static const GLshort ATLASH = 8192;
		static const GLshort MINLOSIZE = 32;

		bool locked;
		// 帧内 quads 非空且未锁定时禁止 clearinternal，延到 clearscene；
		// 锁定装图时立刻回收，避免黑屏。
		bool atlas_clear_pending;

		std::vector<Quad> quads;
		GLuint VBO;
		GLuint EBO;
		GLuint atlas;

		GLint shaderProgram;
		GLint attribute_coord;
		GLint attribute_color;
		GLint uniform_texture;
		GLint uniform_atlassize;
		GLint uniform_screensize;
		GLint uniform_yoffset;
		GLint uniform_fontregion;

		std::unordered_map<size_t, Offset> offsets;
		Offset nulloffset;

		QuadTree<size_t, Leftover> leftovers;
		size_t rlid;
		size_t wasted;
		Point<GLshort> border;
		Range<GLshort> yrange;

		FT_Library ftlibrary;
		Font fonts[Text::Font::NUM_FONTS];
		Point<GLshort> fontborder;
		GLshort fontymax;

		// CJK 按需字形区：每字体一块 WIDE_BLOCK² 的固定子区，紧跟 ASCII
		// 字形条带之下（仍在字体保留条带 [0, border) 内）。22x22 格 × 22px。
		static constexpr GLshort WIDE_CELL = 22;
		static constexpr GLshort WIDE_COLS = 22;
		static constexpr GLshort WIDE_ROWS = 22;
		static constexpr GLshort WIDE_BLOCK = WIDE_CELL * WIDE_COLS;
		static constexpr GLshort WIDE_ROW_SPAN = WIDE_CELL * WIDE_ROWS;

		// 查找/生成一个非 ASCII 码点的字形；区域满或无 face 时返回空。
		const Font::Char* get_wide_glyph(Font& font, uint32_t codepoint);
	};
}