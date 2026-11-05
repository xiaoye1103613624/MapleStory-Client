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
#include "GraphicsGL.h"
#include "Utf8.h"

#include "../Configuration.h"
#include "../MapleStory.h"
#include "../IO/Window.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <vector>

namespace ms
{
	// [DBG-ATLAS] 临时联调诊断：图集异常事件落文件（限 200 条）。
	// 排查「登录页竖条纹/错贴」用，闭环后删除。
	static void dbgatlas(const char* kind, size_t id, int bw, int bh, int ow, int oh)
	{
		static std::ofstream log;
		static int count = 0;

		if (count >= 200)
			return;

		if (!log.is_open())
			log.open("atlas_dbg.log", std::ios::app);

		if (log.is_open())
		{
			log << kind << " id=" << id << " bmp=" << bw << "x" << bh
				<< " cached=" << ow << "x" << oh << '\n';
			log.flush();
			count++;
		}
	}

	GraphicsGL::GraphicsGL()
	{
		locked = false;
		atlas_clear_pending = false;

		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();
		SCREEN = Rectangle<int16_t>(0, VWIDTH, 0, VHEIGHT);
	}

	Error GraphicsGL::init()
	{
		// Setup parameters
		// ----------------
		const char* vertexShaderSource =
			"#version 120\n"
			"attribute vec4 coord;"
			"attribute vec4 color;"
			"varying vec2 texpos;"
			"varying vec4 colormod;"
			"uniform vec2 screensize;"
			"uniform int yoffset;"

			"void main(void)"
			"{"
			"	float x = -1.0 + coord.x * 2.0 / screensize.x;"
			"	float y = 1.0 - (coord.y + yoffset) * 2.0 / screensize.y;"
			"   gl_Position = vec4(x, y, 0.0, 1.0);"
			"	texpos = coord.zw;"
			"	colormod = color;"
			"}";

		const char* fragmentShaderSource =
			"#version 120\n"
			"varying vec2 texpos;"
			"varying vec4 colormod;"
			"uniform sampler2D texture;"
			"uniform vec2 atlassize;"
			"uniform int fontregion;"

			"void main(void)"
			"{"
			"	if (texpos.y == 0)"
			"	{"
			"		gl_FragColor = colormod;"
			"	}"
			"	else if (texpos.y <= fontregion)"
			"	{"
			"		gl_FragColor = vec4(1, 1, 1, texture2D(texture, texpos / atlassize).r) * colormod;"
			"	}"
			"	else"
			"	{"
			"		gl_FragColor = texture2D(texture, texpos / atlassize) * colormod;"
			"	}"
			"}";

		const GLsizei bufSize = 512;

		GLint success;
		GLchar infoLog[bufSize];

		// Initialize and configure
		// ------------------------
		LOG(LOG_INFO, "Using OpenGL " << glGetString(GL_VERSION));
#ifdef USE_OPENGL_ES
		// GLES has no GLEW loader: the driver exports every core function we use.
#else
		if (GLenum error = glewInit())
			return Error(Error::Code::GLEW, (const char*)glewGetErrorString(error));

		LOG(LOG_INFO, "Using GLEW " << glewGetString(GLEW_VERSION));
#endif

		if (FT_Init_FreeType(&ftlibrary))
			return Error::Code::FREETYPE;

		FT_Int ftmajor;
		FT_Int ftminor;
		FT_Int ftpatch;

		FT_Library_Version(ftlibrary, &ftmajor, &ftminor, &ftpatch);

		LOG(LOG_INFO, "Using FreeType " << ftmajor << "." << ftminor << "." << ftpatch);

		// Build and compile our shader program
		// ------------------------------------

		// Vertex Shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		// Check for shader compile errors
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (success != GL_TRUE)
		{
			glGetShaderInfoLog(vertexShader, bufSize, NULL, infoLog);

			return Error(Error::Code::VERTEX_SHADER, infoLog);
		}

		// Fragment Shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		// Check for shader compile errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (success != GL_TRUE)
		{
			glGetShaderInfoLog(fragmentShader, bufSize, NULL, infoLog);

			return Error(Error::Code::FRAGMENT_SHADER, infoLog);
		}

		// Link Shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		// Check for linking errors
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if (success != GL_TRUE)
		{
			glGetProgramInfoLog(shaderProgram, bufSize, NULL, infoLog);

			return Error(Error::Code::SHADER_PROGRAM_LINK, infoLog);
		}

		// Validate Program
		glValidateProgram(shaderProgram);

		// Check for validation errors
		glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);

		if (success != GL_TRUE)
		{
			glGetProgramInfoLog(shaderProgram, bufSize, NULL, infoLog);

			return Error(Error::Code::SHADER_PROGRAM_VALID, infoLog);
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		attribute_coord = glGetAttribLocation(shaderProgram, "coord");
		attribute_color = glGetAttribLocation(shaderProgram, "color");
		uniform_texture = glGetUniformLocation(shaderProgram, "texture");
		uniform_atlassize = glGetUniformLocation(shaderProgram, "atlassize");
		uniform_screensize = glGetUniformLocation(shaderProgram, "screensize");
		uniform_yoffset = glGetUniformLocation(shaderProgram, "yoffset");
		uniform_fontregion = glGetUniformLocation(shaderProgram, "fontregion");

		if (attribute_coord == -1 || attribute_color == -1 || uniform_texture == -1 || uniform_atlassize == -1 || uniform_screensize == -1 || uniform_yoffset == -1)
			return Error::Code::SHADER_VARS;

		// Vertex Buffer Object
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glGenTextures(1, &atlas);
		glBindTexture(GL_TEXTURE_2D, atlas);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLASW, ATLASH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		fontborder.set_y(1);

		const std::string FONT_NORMAL = Setting<FontPathNormal>().get().load();
		const std::string FONT_BOLD = Setting<FontPathBold>().get().load();

		if (FONT_NORMAL.empty() || FONT_BOLD.empty())
			return Error::Code::FONT_PATH;

		const char* FONT_NORMAL_STR = FONT_NORMAL.c_str();
		const char* FONT_BOLD_STR = FONT_BOLD.c_str();

		addfont(FONT_NORMAL_STR, Text::Font::A11M, 0, 11);
		addfont(FONT_BOLD_STR, Text::Font::A11B, 0, 11);
		addfont(FONT_NORMAL_STR, Text::Font::A12M, 0, 12);
		addfont(FONT_BOLD_STR, Text::Font::A12B, 0, 12);
		addfont(FONT_NORMAL_STR, Text::Font::A13M, 0, 13);
		addfont(FONT_BOLD_STR, Text::Font::A13B, 0, 13);
		addfont(FONT_NORMAL_STR, Text::Font::A14M, 0, 14);
		addfont(FONT_BOLD_STR, Text::Font::A14B, 0, 14);
		addfont(FONT_NORMAL_STR, Text::Font::A15M, 0, 15);
		addfont(FONT_BOLD_STR, Text::Font::A15B, 0, 15);
		addfont(FONT_NORMAL_STR, Text::Font::A18M, 0, 18);
		addfont(FONT_BOLD_STR, Text::Font::A18B, 0, 18);

		// 预留 CJK 字形区（每字体一块 WIDE_BLOCK²，紧跟 ASCII 条带之下），
		// 并把总字体条带高度计入 fontymax——位图打包从 border=(0,fontymax)
		// 起，逐出机制不会回收该保留区。
		const GLshort wide_origin_y = static_cast<GLshort>(fontborder.y() + fontymax);
		fontymax = static_cast<GLshort>(fontborder.y() + fontymax + WIDE_ROW_SPAN);

		for (uint8_t fid = 0; fid < Text::Font::NUM_FONTS; ++fid)
		{
			fonts[fid].wide_x = static_cast<GLshort>((fid % (ATLASW / WIDE_BLOCK)) * WIDE_BLOCK);
			fonts[fid].wide_y = wide_origin_y;
		}

		// 位图打包必须从字体区下方开始。若 border 仍为 (0,0)，后续 UI 贴图会
		// 覆盖 FreeType 字形，且被 shader 当成字体（只用 .r）→ 登录输入乱码、
		// charInfo 等底板「看不见」。clearinternal/unload_atlas 才会设 border，
		// 冷启动若不触发它们就会踩坑。
		border = Point<GLshort>(0, fontymax);
		yrange = Range<GLshort>();

		leftovers = QuadTree<size_t, Leftover>(
			[](const Leftover& first, const Leftover& second)
			{
				bool width_comparison = first.width() >= second.width();
				bool height_comparison = first.height() >= second.height();

				if (width_comparison && height_comparison)
					return QuadTree<size_t, Leftover>::Direction::RIGHT;
				else if (width_comparison)
					return QuadTree<size_t, Leftover>::Direction::DOWN;
				else if (height_comparison)
					return QuadTree<size_t, Leftover>::Direction::UP;
				else
					return QuadTree<size_t, Leftover>::Direction::LEFT;
			}
		);

		return Error::Code::NONE;
	}

	bool GraphicsGL::addfont(const char* name, Text::Font id, FT_UInt pixelw, FT_UInt pixelh)
	{
		FT_Face face;

		if (FT_New_Face(ftlibrary, name, 0, &face))
			return false;

		// 必须显式选 Unicode charmap：若 face 默认 charmap 是 Mac Roman/Latin 等，
		// FT_Load_Char 对 CJK 码点查不到字形 → get_wide_glyph 每个字都渲染成
		// 同一个 .notdef 方块（实测表现为「新手」→「ÆÆ」乱码）。
		FT_Select_Charmap(face, FT_ENCODING_UNICODE);

		if (FT_Set_Pixel_Sizes(face, pixelw, pixelh))
			return false;

		FT_GlyphSlot g = face->glyph;

		GLshort width = 0;
		GLshort height = 0;

		for (uint8_t c = 32; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			GLshort w = static_cast<GLshort>(g->bitmap.width);
			GLshort h = static_cast<GLshort>(g->bitmap.rows);

			width += w;

			if (h > height)
				height = h;
		}

		if (fontborder.x() + width > ATLASW)
		{
			fontborder.set_x(0);
			fontborder.set_y(fontymax);
			fontymax = 0;
		}

		GLshort x = fontborder.x();
		GLshort y = fontborder.y();

		fontborder.shift_x(width);

		if (height > fontymax)
			fontymax = height;

		fonts[id] = Font(width, height);

		GLshort ox = x;
		GLshort oy = y;

		for (uint8_t c = 32; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			GLshort ax = static_cast<GLshort>(g->advance.x >> 6);
			GLshort ay = static_cast<GLshort>(g->advance.y >> 6);
			GLshort l = static_cast<GLshort>(g->bitmap_left);
			GLshort t = static_cast<GLshort>(g->bitmap_top);
			GLshort w = static_cast<GLshort>(g->bitmap.width);
			GLshort h = static_cast<GLshort>(g->bitmap.rows);

			// FreeType pitch 常大于 width（行对齐）；窄字形（*、i、l）不按 pitch
			// 上传会串行成白勾/短横。pitch 以字节计，GL_RED 下 1px=1B。
			const int pitch = g->bitmap.pitch;
			const unsigned char* pixels = g->bitmap.buffer;
			std::vector<unsigned char> tight;

			if (pixels && w > 0 && h > 0 && pitch != 0 && std::abs(pitch) != w)
			{
				tight.resize(static_cast<size_t>(w) * static_cast<size_t>(h));

				for (GLshort row = 0; row < h; ++row)
				{
					const unsigned char* src = (pitch > 0)
						? (pixels + row * pitch)
						: (pixels + (h - 1 - row) * (-pitch));
					std::memcpy(tight.data() + static_cast<size_t>(row) * static_cast<size_t>(w), src, static_cast<size_t>(w));
				}

				pixels = tight.data();
			}

			if (pixels && w > 0 && h > 0)
				glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, w, h, GL_RED, GL_UNSIGNED_BYTE, pixels);

			Offset offset = Offset(ox, oy, w, h);
			fonts[id].chars[c] = { ax, ay, w, h, l, t, offset };

			ox += w;
		}

		// face 不释放（与旧行为一致——原实现也未 Done），供 CJK 字形按需生成。
		fonts[id].face = face;

		// R27-B1b：空文字的墨迹兜底用数字 '0' 的 bitmap_top（现场探测，非手抄表）。
		// 真实一行的墨迹顶由本行最高字形决定，见 glyph_bt / get_ink_offset。
		fonts[id].bt_short = fonts[id].chars['0'].bt;

		return true;
	}

	int16_t GraphicsGL::get_linespace(Text::Font id) const
	{
		return fonts[id].linespace();
	}

	GLshort GraphicsGL::glyph_bt(Font& font, uint32_t codepoint)
	{
		if (codepoint < 128)
			return font.chars[codepoint].bt;

		// 已经上传过就直接用绘制路径认的那份度量，保证「量到的 == 画出来的」。
		auto cached = font.wide.find(codepoint);

		if (cached != font.wide.end())
			return cached->second.bt;

		auto probed = font.bt_probe.find(codepoint);

		if (probed != font.bt_probe.end())
			return probed->second;

		GLshort bt = 0;

		// 没有 face（addfont 走的是非 Windows 位图字体）时只能什么都不量，
		// 交给 bt_short 兜底；直接 FT_Get_Char_Index(nullptr) 会崩。
		// 缺字（索引 0 = .notdef）同理：绘制路径整字不画，这里必须按「无墨」
		// 参与最大值，不能把方块的高度算进来。两种情况都照旧写进缓存，
		// 免得每帧重探。
		if (font.face && FT_Get_Char_Index(font.face, codepoint) != 0)
		{
			// 只取度量、不写图集（图集可能正处于 lock 状态）；与 get_wide_glyph
			// 同一个 FT_Load_Char(..., FT_LOAD_RENDER)，bitmap_top 必然一致。
			if (!FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER))
				bt = static_cast<GLshort>(font.face->glyph->bitmap_top);
		}

		font.bt_probe.emplace(codepoint, bt);

		return bt;
	}

	int16_t GraphicsGL::get_ink_offset(Text::Font id, const std::string& text)
	{
		Font& font = fonts[id];
		GLshort maxbt = font.bt_short;
		size_t pos = 0;

		while (pos < text.size())
		{
			size_t consumed = 1;
			const uint32_t cp = utf8_next(text.c_str(), pos, text.size(), consumed);

			pos += consumed;

			if (cp == 0)
				continue;

			const GLshort bt = glyph_bt(font, cp);

			if (bt > maxbt)
				maxbt = bt;
		}

		return static_cast<int16_t>(font.linespace() - maxbt);
	}

	const GraphicsGL::Font::Char* GraphicsGL::get_wide_glyph(Font& font, uint32_t codepoint)
	{
		auto found = font.wide.find(codepoint);

		if (found != font.wide.end())
			return &found->second;

		if (!font.face || font.wide_full || locked)
			return nullptr;

		// 缺字保护：字形索引 0 = .notdef（方块）。字体无该码点时直接跳过，
		// 否则所有缺字都会画成同一个方块（用户看到的「ÆÆ」乱码）。
		if (FT_Get_Char_Index(font.face, codepoint) == 0)
			return nullptr;

		FT_GlyphSlot g = font.face->glyph;

		if (FT_Load_Char(font.face, codepoint, FT_LOAD_RENDER))
			return nullptr;

		GLshort ax = static_cast<GLshort>(g->advance.x >> 6);
		GLshort ay = static_cast<GLshort>(g->advance.y >> 6);
		GLshort l = static_cast<GLshort>(g->bitmap_left);
		GLshort t = static_cast<GLshort>(g->bitmap_top);
		GLshort w = static_cast<GLshort>(g->bitmap.width);
		GLshort h = static_cast<GLshort>(g->bitmap.rows);

		if (w > WIDE_CELL || h > WIDE_CELL)
		{
			// 超出单元格（字号过大/畸形字形）：只占位不再重试。
			font.wide_full = true;
			return nullptr;
		}

		size_t slot = font.wide.size();

		if (slot >= static_cast<size_t>(WIDE_COLS) * WIDE_ROWS)
		{
			font.wide_full = true;
			return nullptr;
		}

		GLshort gx = static_cast<GLshort>(font.wide_x + (slot % WIDE_COLS) * WIDE_CELL);
		GLshort gy = static_cast<GLshort>(font.wide_y + (slot / WIDE_COLS) * WIDE_CELL);

		// pitch 处理与 addfont 相同：FreeType 行对齐字节，窄字形须去 padding。
		const int pitch = g->bitmap.pitch;
		const unsigned char* pixels = g->bitmap.buffer;
		std::vector<unsigned char> tight;

		if (pixels && w > 0 && h > 0 && pitch != 0 && std::abs(pitch) != w)
		{
			tight.resize(static_cast<size_t>(w) * static_cast<size_t>(h));

			for (GLshort row = 0; row < h; ++row)
			{
				const unsigned char* src = (pitch > 0)
					? (pixels + row * pitch)
					: (pixels + (h - 1 - row) * (-pitch));
				std::memcpy(tight.data() + static_cast<size_t>(row) * static_cast<size_t>(w), src, static_cast<size_t>(w));
			}

			pixels = tight.data();
		}

		if (pixels && w > 0 && h > 0)
			glTexSubImage2D(GL_TEXTURE_2D, 0, gx, gy, w, h, GL_RED, GL_UNSIGNED_BYTE, pixels);

		auto& stored = font.wide.emplace(
			codepoint,
			Font::Char{ ax, ay, w, h, l, t, Offset(gx, gy, w, h) }
		).first->second;

		return &stored;
	}

	void GraphicsGL::reinit()
	{
		int32_t new_width = Constants::Constants::get().get_viewwidth();
		int32_t new_height = Constants::Constants::get().get_viewheight();

		// Only wipe the atlas when the logical view size changes. Calling
		// clearinternal() on every reinit (e.g. initwindow from check_events
		// during SetField) discarded map tiles uploaded in Stage::load while
		// GL was still locked — BGM already playing, screen black, then the
		// first unlocked frames re-uploaded the whole map and starved the pump.
		const bool size_changed = (VWIDTH != new_width || VHEIGHT != new_height);

		if (size_changed)
		{
			VWIDTH = new_width;
			VHEIGHT = new_height;
			SCREEN = Rectangle<int16_t>(0, VWIDTH, 0, VHEIGHT);
		}

		glUseProgram(shaderProgram);

		glUniform1i(uniform_fontregion, fontymax);
		glUniform2f(uniform_atlassize, ATLASW, ATLASH);
		glUniform2f(uniform_screensize, VWIDTH, VHEIGHT);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(attribute_coord, 4, GL_SHORT, GL_FALSE, sizeof(Quad::Vertex), 0);
		glVertexAttribPointer(attribute_color, 4, GL_FLOAT, GL_FALSE, sizeof(Quad::Vertex), (const void*)8);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, atlas);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (size_changed)
			clearinternal();
	}

	void GraphicsGL::clearinternal()
	{
		// 未锁定且本帧已有待绘 quad：延到 clearscene，避免已入队 UV 失效。
		// 锁定装图（SetField/Stage::load）时必须立刻回收——否则 atlas 满后
		// 持续返回 nulloffset，表现为黑屏+BGM、主线程像未响应。
		if (!quads.empty() && !locked)
		{
			atlas_clear_pending = true;
			return;
		}

		quads.clear();
		border = Point<GLshort>(0, fontymax);
		yrange = Range<GLshort>();

		offsets.clear();
		leftovers.clear();
		rlid = 1;
		wasted = 0;
		atlas_clear_pending = false;
	}

	void GraphicsGL::clear()
	{
		size_t used = ATLASW * border.y() + border.x() * yrange.second();
		double usedpercent = static_cast<double>(used) / (ATLASW * ATLASH);

		// Soft threshold: reclaim atlas when packing pressure is high.
		// Coarse eviction (full reset) doubles as LRU under atlas packing constraints.
		if (usedpercent > 60.0)
			clearinternal();
	}

	void GraphicsGL::unload_atlas()
	{
		LOG(LOG_INFO, "[PERF] unload_atlas");
		// 换图路径：允许强制清空（此时不应仍持有跨图的 draw 队列）。
		quads.clear();
		atlas_clear_pending = false;
		border = Point<GLshort>(0, fontymax);
		yrange = Range<GLshort>();
		offsets.clear();
		leftovers.clear();
		rlid = 1;
		wasted = 0;
	}

	void GraphicsGL::addbitmap(const nl::bitmap& bmp)
	{
		getoffset(bmp);
	}

	const GraphicsGL::Offset& GraphicsGL::getoffset(const nl::bitmap& bmp)
	{
		// [DBG-GO] 联调诊断桩：闭环后不再落文件（no-op），保留以便将来联调。
		auto dbggo = [](const std::string&) {};

		static uint32_t atlas_use_tick = 0;
		++atlas_use_tick;

		size_t id = bmp.id();
		auto offiter = offsets.find(id);

		if (offiter != offsets.end())
		{
			Offset& cached = offiter->second;

			// 陈旧条目自愈：bitmap id 是数据指针，片头卷轴等大量临时位图释放后
			// 堆地址会被新位图复用 → 同 id 指到旧图集区域 → 把别人的像素（窄字形列/
			// 其他贴图）拉伸画出来 = 页面上的竖条纹/错贴「白云」。尺寸对不上即重传。
			if (cached.right - cached.left != static_cast<GLshort>(bmp.width())
				|| cached.bottom - cached.top != static_cast<GLshort>(bmp.height()))
			{
				dbgatlas("stale-offset", id, bmp.width(), bmp.height(),
					cached.right - cached.left, cached.bottom - cached.top);
				offsets.erase(offiter);
			}
			else
			{
				cached.last_use = atlas_use_tick;
				return cached;
			}
		}

		GLshort x = 0;
		GLshort y = 0;
		GLshort width = bmp.width();
		GLshort height = bmp.height();

		dbggo(std::string("go:size w=") + std::to_string(width) +
			" h=" + std::to_string(height));

		if (width <= 0 || height <= 0)
			return nulloffset;

		if (!bmp.data())
			return nulloffset;

		// Atlas is 8192². Reject only what physically cannot pack — do NOT use
		// the miniMap 2048 soft-cap here: map backdrops (e.g. 1280x2540) must
		// upload or Stage draws nothing while BGM still plays (black screen).
		// Oversized miniMap canvases are gated in UIMiniMap before Texture().
		if (width > ATLASW || height > ATLASH)
		{
			LOG(LOG_WARN, "[GL] bitmap exceeds atlas " << width << "x" << height);
			return nulloffset;
		}

		// 未锁定且上一帧请求了回收：本帧仍有 quad 时跳过上传，等 clearscene。
		if (atlas_clear_pending && !quads.empty() && !locked)
			return nulloffset;

		Leftover value = Leftover(x, y, width, height);

		dbggo("go:pre-findnode");

		size_t lid = leftovers.findnode(
			value,
			[](const Leftover& val, const Leftover& leaf)
			{
				return val.width() <= leaf.width() && val.height() <= leaf.height();
			}
		);

		dbggo(std::string("go:findnode-done lid=") + std::to_string(lid));

		if (lid > 0)
		{
			const Leftover& leftover = leftovers[lid];

			x = leftover.left;
			y = leftover.top;

			GLshort width_delta = leftover.width() - width;
			GLshort height_delta = leftover.height() - height;

			leftovers.erase(lid);

			wasted -= width * height;

			if (width_delta >= MINLOSIZE && height_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x + width, y + height, width_delta, height_delta));
				rlid++;

				if (width >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x, y + height, width, height_delta));
					rlid++;
				}

				if (height >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x + width, y, width_delta, height));
					rlid++;
				}
			}
			else if (width_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x + width, y, width_delta, height + height_delta));
				rlid++;
			}
			else if (height_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x, y + height, width + width_delta, height_delta));
				rlid++;
			}
		}
		else
		{
			if (border.x() + width > ATLASW)
			{
				border.set_x(0);
				border.shift_y(yrange.second());

				if (border.y() + height > ATLASH)
				{
					clearinternal();

					// 帧内无法立刻回收：本图本帧跳过，下一帧 clearscene 后重传。
					if (atlas_clear_pending)
						return nulloffset;
				}
				else
					yrange = Range<GLshort>();
			}

			x = border.x();
			y = border.y();

			border.shift_x(width);

			if (height > yrange.second())
			{
				if (x >= MINLOSIZE && height - yrange.second() >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(0, yrange.first(), x, height - yrange.second()));
					rlid++;
				}

				wasted += x * (height - yrange.second());

				yrange = Range<int16_t>(y + height, height);
			}
			else if (height < yrange.first() - y)
			{
				if (width >= MINLOSIZE && yrange.first() - y - height >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x, y + height, width, yrange.first() - y - height));
					rlid++;
				}

				wasted += width * (yrange.first() - y - height);
			}
		}

#if LOG_LEVEL >= LOG_TRACE
		size_t used = ATLASW * border.y() + border.x() * yrange.second();

		double usedpercent = static_cast<double>(used) / (ATLASW * ATLASH);
		double wastedpercent = static_cast<double>(wasted) / used;

		LOG(LOG_TRACE, "Used: [" << usedpercent << "] Wasted: [" << wastedpercent << "]");
#endif

		dbggo("go:pre-data");

#ifdef USE_OPENGL_ES
		// GLES core has no GL_BGRA; convert BGRA -> RGBA on the fly.
		{
			const uint8_t* src = reinterpret_cast<const uint8_t*>(bmp.data());
			std::vector<uint8_t> rgba(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
			for (size_t p = 0; p < rgba.size(); p += 4)
			{
				rgba[p + 0] = src[p + 2];
				rgba[p + 1] = src[p + 1];
				rgba[p + 2] = src[p + 0];
				rgba[p + 3] = src[p + 3];
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
		}
#else
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, bmp.data());
#endif

		auto& inserted = offsets.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(id),
			std::forward_as_tuple(x, y, width, height)
		).first->second;
		inserted.last_use = atlas_use_tick;

		// Keep the window responsive while a burst of bitmaps fills the atlas
		// (Stage::load tile upload / enter-game). Pump every 2 inserts, or on
		// any wide/tall upload.
		static uint32_t atlas_insert_since_pump = 0;
		++atlas_insert_since_pump;

		if (atlas_insert_since_pump >= 2 || width > 256 || height > 256)
		{
			atlas_insert_since_pump = 0;
			Window::get().poll_events();
		}

		return inserted;
	}

	void GraphicsGL::draw(const nl::bitmap& bmp, const Rectangle<int16_t>& rect, const Range<int16_t>& vertical, const Range<int16_t>& horizontal, const Color& color, float angle)
	{
		if (locked)
			return;

		if (color.invisible())
			return;

		if (!rect.overlaps(SCREEN))
			return;

		Offset offset = getoffset(bmp);

		// 上传失败（无数据/超大/图集回收挂起）时绝不发 quad：nulloffset 的 UV
		// 恒为 (0,0)-(0,0)，会把图集左上角（字体区）采样成一块实心/花屏矩形。
		// 直接跳过，等下一帧重传（实心填充走 drawrectangle/drawscreenfill，
		// 不经过本函数，不受影响）。
		if (&offset == &nulloffset)
		{
			dbgatlas("null-offset-draw", bmp.id(), bmp.width(), bmp.height(), 0, 0);
			return;
		}

		offset.top += vertical.first();
		offset.bottom -= vertical.second();
		offset.left += horizontal.first();
		offset.right -= horizontal.second();

		quads.emplace_back(
			rect.left() + horizontal.first(),
			rect.right() - horizontal.second(),
			rect.top() + vertical.first(),
			rect.bottom() - vertical.second(),
			offset, color, angle
		);
	}

	Text::Layout GraphicsGL::createlayout(const std::string& text, Text::Font id, Text::Alignment alignment, Color::Name color, int16_t maxwidth, bool formatted, int16_t line_adj)
	{
		size_t length = text.length();

		if (length == 0)
			return Text::Layout();

		LayoutBuilder builder(id, fonts[id], alignment, color, maxwidth, formatted, line_adj);

		const char* p_text = text.c_str();

		size_t first = 0;
		size_t offset = 0;

		while (offset < length)
		{
			// \r \n 必须进分词集合：网络送来的 NPC 正文里断行是真 0x0D/0x0A 字节
			// （WZ 串才是字面量 "\r\n"，由 case '\\' 处理）。旧集合里没有它们，
			// 整段正文被当成一个词连着画过去，控制字符本身宽度为 0 又什么都不画，
			// 于是多行脚本挤成一行。
			size_t last = text.find_first_of(" \\#\t\r\n", offset + 1);

			if (last == std::string::npos)
				last = length;

			first = builder.add(p_text, first, offset, last);
			offset = last;
		}

		return builder.finish(first, offset);
	}

	GraphicsGL::LayoutBuilder::LayoutBuilder(Text::Font id, const Font& f, Text::Alignment a, Color::Name c, int16_t mw, bool fm, int16_t la) : fontid(id), font(f), alignment(a), color(c), maxwidth(mw), formatted(fm), line_adj(la)
	{
		ax = 0;
		ay = font.linespace();
		width = 0;
		endy = 0;

		if (maxwidth == 0)
			maxwidth = 800;
	}

	size_t GraphicsGL::LayoutBuilder::add(const char* text, size_t prev, size_t first, size_t last)
	{
		if (first == last)
			return prev;

		Text::Font last_font = fontid;
		Color::Name last_color = color;
		size_t skip = 0;
		bool linebreak = false;

		if (formatted)
		{
			size_t next_char = first + 1;
			char c = text[next_char];

			switch (text[first])
			{
				case '\\':
				{
					if (next_char < last)
					{
						switch (c)
						{
							// \r\n - Moves down a line
							case 'rn':
							case 'RN':
							{
								// TODO: How is this treated differently than the single versions?
								skip += 4;
								break;
							}
							// \n - New line
							case 'n':
							case 'N':
							{
								linebreak = true;
								skip += 2;
								break;
							}
							// \r - Return carriage
							case 'r':
							case 'R':
							{
								linebreak = ax > 0;
								skip += 2;
								break;
							}
							// \b - Backwards
							case 'b':
							{
								// TODO: What is this?
								skip += 2;
								break;
							}
							default:
							{
								single_console::log_message("[GraphicsGL::LayoutBuilder::add] Unknown format: [" + std::to_string(c) + "]");
								break;
							}
						}
					}

					break;
				}
				case '#':
				{
					if (next_char < last)
					{
						switch (c)
						{
							// #b - Blue text
							case 'b':
							{
								color = Color::Name::PERSIANGREEN;
								skip += 2;
								break;
							}
							// #B[%]# - Shows a progress bar
							case 'B':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #c[id]# - Show how many of a given item are in the player's inventory in an orange color
							case 'c':
							{
								// TODO: Show the number of items
								color = Color::Name::ORANGE;
								skip += 2;
								break;
							}
							// #d - Purple text
							case 'd':
							{
								// TODO: Need to get the proper color
								skip += 2;
								break;
							}
							// #e - Bold text
							case 'e':
							case 'E':
							{
								switch (last_font)
								{
									case Text::Font::A11M:
										fontid = Text::Font::A11B;
										break;
									case Text::Font::A12M:
										fontid = Text::Font::A12B;
										break;
									case Text::Font::A13M:
										fontid = Text::Font::A13B;
										break;
									case Text::Font::A18M:
										fontid = Text::Font::A18B;
										break;
									default:
										single_console::log_message("[GraphicsGL::LayoutBuilder::add] Unknown Text::Font: [" + std::to_string(last_font) + "]");
										break;
								}

								skip += 2;
								break;
							}
							// #f[path]# - Shows an image within the WZ file
							case 'f':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #g - Green text
							case 'g':
							{
								// TODO: Need to get the proper color
								skip += 2;
								break;
							}
							// TODO: Is there a space between the h and ending # or not?
							// #h # - Shows the name of the player
							case 'h':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #i[id]# - Shows a picture of the given item
							case 'i':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #k - Black text
							case 'k':
							{
								color = Color::Name::DARKGREY;
								skip += 2;
								break;
							}
							// #l - Ends the list of items in the selection
							case 'l':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #L[number]# - Starts a selection for the number of items given
							case 'L':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #m[id]# - Shows the name of the given map
							case 'm':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #n - Normal text (Removes bold)
							case 'n':
							case 'N':
							{
								switch (last_font)
								{
									case Text::Font::A11B:
										fontid = Text::Font::A11M;
										break;
									case Text::Font::A12B:
										fontid = Text::Font::A12M;
										break;
									case Text::Font::A13B:
										fontid = Text::Font::A13M;
										break;
									case Text::Font::A18B:
										fontid = Text::Font::A18M;
										break;
									default:
										single_console::log_message("[GraphicsGL::LayoutBuilder::add] Unknown Text::Font: [" + std::to_string(last_font) + "]");
										break;
								}

								skip += 2;
								break;
							}
							// #o[id]# - Shows the name of the given monster
							case 'o':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #p[id]# - Shows the name of the given NPC
							case 'p':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #q[id]# - Shows the name of the given skill
							case 'q':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #r - Red text
							case 'r':
							{
								color = Color::Name::RED;
								skip += 2;
								break;
							}
							// #s[id]# - Shows the image of the given skill
							case 's':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #t[id]#
							// #z[id]#
							// Shows the name of the given item
							// TODO: Are these the same?
							case 't':
							case 'z':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #v[id]# - Shows a picture of the given item
							case 'v':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							// #x - Returns "0%" (Need more information on this)
							case 'x':
							{
								// TODO: Needs implemented
								skip += 2;
								break;
							}
							default:
							{
								single_console::log_message("[GraphicsGL::LayoutBuilder::add] Unknown format: [" + std::to_string(c) + "]");
								break;
							}
						}
					}

					break;
				}
				// \t - Tab (4 spaces)
				case '\t':
				{
					ax = font.chars[' '].ax * 4;
					skip++;
					break;
				}
				// 真 0x0D - 回车断行
				case '\r':
				{
					// \r\n 只算一处断行：这里把紧随其后的 \n 一并吃掉（text 取自
					// c_str()，first+1 至多是终止符，一定可读），否则同一处会断两次、
					// 正文之间凭空多出一行空高。
					const bool crlf = text[first + 1] == '\n';

					linebreak = crlf || ax > 0;
					skip = crlf ? 2 : 1;
					break;
				}
				// 真 0x0A - 换行断行
				case '\n':
				{
					// 词起点已经越过本字节 ⇒ 前一个 \r 已在这处断过，不能再断第二次。
					// 这里保持 skip=0：控制字符没生成过字形（addfont 只填 32..127），
					// 推进量为 0，不会额外顶开笔位，但 advances 仍按字节补齐。
					if (prev <= first)
					{
						linebreak = true;
						skip = 1;
					}
					break;
				}
			}
		}

		int16_t wordwidth = 0;

		if (!linebreak)
		{
			// 逐**字符**（不是逐字节）累加宽度：中文一个字符 3 字节，旧实现只认 32..127，
			// 于是整段中文对 wordwidth 的贡献恒为 0 —— 长中文文本只溢出不换行。
			// 拆分点 i 现在必然落在码点边界上，不会再出现"半个汉字被拆到下一行"。
			for (size_t i = first; i < last; )
			{
				size_t consumed = 1;
				const unsigned char uc = static_cast<unsigned char>(text[i]);

				if (uc == '\t')
					wordwidth += ax;
				else
					wordwidth += measure(text, i, last, consumed);

				if (wordwidth > maxwidth)
				{
					// 单个字符就已经超宽：没有可分的位置，接受溢出。
					// （旧条件 last - first == 1 只覆盖单字节，多字节会原地无限递归。）
					if (i == first)
					{
						return last;
					}
					else
					{
						prev = add(text, prev, first, i);
						return add(text, prev, i, last);
					}
				}

				i += consumed;
			}
		}

		bool newword = skip > 0;
		bool newline = linebreak || ax + wordwidth > maxwidth;

		if (newword || newline)
			add_word(prev, first, last_font, last_color);

		if (newline)
		{
			add_line();

			endy = ay;
			ax = 0;
			ay += font.linespace();

			if (lines.size() > 0)
				ay -= line_adj;
		}

		// advances 按**字节**索引（Textfield 的光标用 advance(byte) 定位），所以一个
		// 多字节字符要占 consumed 个槽位，且它们都记同一个笔位起点：指针落在半个字上
		// 也不会跳位。推进量改走 measure()，与 drawtext 画每个字形时累加的 ax 同源。
		for (size_t pos = first; pos < last; )
		{
			size_t consumed = 1;
			const int16_t ch_adv = measure(text, pos, last, consumed);

			for (size_t k = 0; k < consumed; k++)
				advances.push_back(ax);

			if (pos < first + skip || newline && static_cast<unsigned char>(text[pos]) == ' ')
			{
				pos += consumed;
				continue;
			}

			ax += ch_adv;

			if (width < ax)
				width = ax;

			pos += consumed;
		}

		if (newword || newline)
			return first + skip;
		else
			return prev;
	}

	int16_t GraphicsGL::LayoutBuilder::measure(const char* text, size_t pos, size_t last, size_t& consumed) const
	{
		const uint32_t cp = utf8_next(text, pos, last, consumed);

		if (cp == 0)
			return 0;
		if (cp < 128)
			return font.chars[cp].ax;

		return wide_advance(cp);
	}

	int16_t GraphicsGL::LayoutBuilder::wide_advance(uint32_t codepoint) const
	{
		// 已经生成过字形就直接用它自己的推进量 —— 绘制路径累加的就是这个数。
		auto cached = font.wide.find(codepoint);

		if (cached != font.wide.end())
			return cached->second.ax;

		if (!font.face)
			return font.chars[' '].ax;	// 没有 face（非 Windows 字体）：退回旧的空格宽

		// 只取度量、不渲染位图：布局可能在图集锁定期间跑，那时 get_wide_glyph 会直接拒绝。
		// 用与 addfont/get_wide_glyph 同一个表达式（advance.x >> 6），FT_LOAD_RENDER 相比
		// FT_LOAD_DEFAULT 只多生成位图，不改推进量 ⇒ 量出来的和画出来的必然相等。
		const FT_UInt gindex = FT_Get_Char_Index(font.face, codepoint);

		if (gindex == 0)
			return 0;	// 字体里没这个字：绘制同样什么都不画，不能凭空留宽

		if (FT_Load_Glyph(font.face, gindex, FT_LOAD_DEFAULT))
			return 0;

		return static_cast<int16_t>(font.face->glyph->advance.x >> 6);
	}

	Text::Layout GraphicsGL::LayoutBuilder::finish(size_t first, size_t last)
	{
		add_word(first, last, fontid, color);
		add_line();

		advances.push_back(ax);

		return Text::Layout(lines, advances, width, ay, ax, endy);
	}

	void GraphicsGL::LayoutBuilder::add_word(size_t word_first, size_t word_last, Text::Font word_font, Color::Name word_color)
	{
		words.push_back({ word_first, word_last, word_font, word_color });
	}

	void GraphicsGL::LayoutBuilder::add_line()
	{
		int16_t line_x = 0;
		int16_t line_y = ay;

		switch (alignment)
		{
			case Text::Alignment::CENTER:
				line_x -= ax / 2;
				break;
			case Text::Alignment::RIGHT:
				line_x -= ax;
				break;
		}

		lines.push_back({ words, { line_x, line_y } });
		words.clear();
	}

	void GraphicsGL::drawtext(const DrawArgument& args, const Range<int16_t>& vertical, const std::string& text, const Text::Layout& layout, Text::Font id, Color::Name colorid, Text::Background background)
	{
		if (locked)
			return;

		const Color& color = args.get_color();

		if (text.empty() || color.invisible())
			return;

		GLshort x = args.getpos().x();
		GLshort y = args.getpos().y();
		GLshort w = layout.width();
		GLshort h = layout.height();
		GLshort minheight = vertical.first() > 0 ? vertical.first() : SCREEN.top();
		GLshort maxheight = vertical.second() > 0 ? vertical.second() : SCREEN.bottom();

		switch (background)
		{
			case Text::Background::NAMETAG:
			{
				// If ever changing code in here confirm placements with map 10000
				for (const Text::Layout::Line& line : layout)
				{
					GLshort left = x + line.position.x() - 1;
					GLshort right = left + w + 3;
					// 盒顶贴「这一行最高的墨」：line.position.y() 就是基线，减 bitmap_top
					// 即墨迹顶。原来写死的 +6 等于假定 bt = linespace-6 = 11（PIL 口径），
					// SimHei 实测 CJK bt 只有 9/10 ⇒ 盒比字高出 2~5px。逐行取本行最大 bt，
					// 中文名（如「新」= 10）不会被裁，纯英文名也不会白留一截。
					GLshort maxbt = fonts[id].bt_short;

					for (const Text::Layout::Word& word : line.words)
					{
						Font& word_font = fonts[word.font];

						for (size_t pos = word.first; pos < word.last; )
						{
							size_t consumed = 1;
							const uint32_t cp = utf8_next(text.c_str(), pos, word.last, consumed);

							pos += consumed;

							if (cp == 0)
								continue;

							const GLshort bt = glyph_bt(word_font, cp);

							if (bt > maxbt)
								maxbt = bt;
						}
					}

					GLshort top = y + line.position.y() - maxbt;
					GLshort bottom = top + h - 2;
					Color ntcolor = Color(0.0f, 0.0f, 0.0f, 0.6f);

					quads.emplace_back(left, right, top, bottom, nulloffset, ntcolor, 0.0f);
					quads.emplace_back(left - 1, left, top + 1, bottom - 1, nulloffset, ntcolor, 0.0f);
					quads.emplace_back(right, right + 1, top + 1, bottom - 1, nulloffset, ntcolor, 0.0f);
				}

				break;
			}
		}

		for (const Text::Layout::Line& line : layout)
		{
			Point<int16_t> position = line.position;

			for (const Text::Layout::Word& word : line.words)
			{
				GLshort ax = position.x() + layout.advance(word.first);
				GLshort ay = position.y();

				const GLfloat* wordcolor;

				if (word.color < Color::Name::NUM_COLORS)
					wordcolor = Color::colors[word.color];
				else
					wordcolor = Color::colors[colorid];

				Color abscolor = color * Color(wordcolor[0], wordcolor[1], wordcolor[2], 1.0f);

				// 解码走 Graphics/Utf8.h 的 utf8_next —— 与 LayoutBuilder::measure 同一个函数，
				// 排版量出来的推进量和这里逐字形累加的 ax 必然一致。
				for (size_t pos = word.first; pos < word.last; )
				{
					size_t consumed = 1;
					const uint32_t cp = utf8_next(text.c_str(), pos, word.last, consumed);

					pos += consumed;

					if (cp == 0)
						continue;

					Font& word_font = fonts[word.font];
					const Font::Char* chp;

					if (cp < 128)
					{
						chp = &word_font.chars[cp];
					}
					else
					{
						// 非 ASCII 码点 → 按需 CJK 字形。（旧逻辑对 uc >= 0x80 直接 continue，
						// 导致「新手/排行不可用」等中文字符串整体渲染为空白。）
						chp = get_wide_glyph(word_font, cp);

						if (!chp)
							continue;
					}

					const Font::Char& ch = *chp;

					GLshort char_x = x + ax + ch.bl;
					GLshort char_y = y + ay - ch.bt;
					GLshort char_width = ch.bw;
					GLshort char_height = ch.bh;
					GLshort char_bottom = char_y + char_height;

					Offset offset = ch.offset;

					if (char_bottom > maxheight)
					{
						GLshort bottom_adjust = char_bottom - maxheight;

						if (bottom_adjust < 10)
						{
							offset.bottom -= bottom_adjust;
							char_bottom -= bottom_adjust;
						}
						else
						{
							continue;
						}
					}

					if (char_y < minheight)
						continue;

					if (ax == 0 && cp == ' ')
						continue;

					ax += ch.ax;

					if (char_width <= 0 || char_height <= 0)
						continue;

					quads.emplace_back(char_x, char_x + char_width, char_y, char_bottom, offset, abscolor, 0.0f);
				}
			}
		}
	}

	void GraphicsGL::drawrectangle(int16_t x, int16_t y, int16_t width, int16_t height, float red, float green, float blue, float alpha)
	{
		if (locked)
			return;

		quads.emplace_back(x, x + width, y, y + height, nulloffset, Color(red, green, blue, alpha), 0.0f);
	}

	void GraphicsGL::drawscreenfill(float red, float green, float blue, float alpha)
	{
		drawrectangle(0, 0, VWIDTH, VHEIGHT, red, green, blue, alpha);
	}

	void GraphicsGL::lock()
	{
		locked = true;
	}

	void GraphicsGL::unlock()
	{
		locked = false;
	}

	bool GraphicsGL::is_locked() const
	{
		return locked;
	}

	void GraphicsGL::flush(float opacity)
	{
		bool coverscene = opacity != 1.0f;

		if (coverscene)
		{
			float complement = 1.0f - opacity;
			Color color = Color(0.0f, 0.0f, 0.0f, complement);

			quads.emplace_back(SCREEN.left(), SCREEN.right(), SCREEN.top(), SCREEN.bottom(), nulloffset, color, 0.0f);
		}

		// 中性黑兜底：任何一帧 quads 为空（换图装图竞态、atlas_clear_pending
		// 延迟清理、Stage 尚未激活）时露出的应是黑底而非刺眼纯白。
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLsizeiptr csize = quads.size() * sizeof(Quad);
		GLsizeiptr fsize = quads.size() * Quad::LENGTH;

		glEnableVertexAttribArray(attribute_coord);
		glEnableVertexAttribArray(attribute_color);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, csize, quads.data(), GL_STREAM_DRAW);

#ifdef USE_OPENGL_ES
		// GLES has no GL_QUADS: draw the same 4 vertices per quad as two
		// triangles via a reused index buffer (0,1,2, 0,2,3).
		{
			const size_t icount = quads.size() * 6;
			std::vector<GLushort> indices(icount);
			for (size_t i = 0; i < quads.size(); ++i)
			{
				GLushort b = static_cast<GLushort>(i * 4);
				indices[i * 6 + 0] = b;
				indices[i * 6 + 1] = b + 1;
				indices[i * 6 + 2] = b + 2;
				indices[i * 6 + 3] = b;
				indices[i * 6 + 4] = b + 2;
				indices[i * 6 + 5] = b + 3;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STREAM_DRAW);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(icount), GL_UNSIGNED_SHORT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
#else
		glDrawArrays(GL_QUADS, 0, fsize);
#endif

		glDisableVertexAttribArray(attribute_coord);
		glDisableVertexAttribArray(attribute_color);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (coverscene)
			quads.pop_back();
	}

	void GraphicsGL::read_pixels(int16_t width, int16_t height, uint8_t* out) const
	{
		// 视图宽度未必是 4 的倍数，关掉按行对齐否则回读会错位。
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, out);
		glPixelStorei(GL_PACK_ALIGNMENT, 4);
	}

	void GraphicsGL::clearscene()
	{
		if (atlas_clear_pending)
		{
			atlas_clear_pending = false;
			border = Point<GLshort>(0, fontymax);
			yrange = Range<GLshort>();
			offsets.clear();
			leftovers.clear();
			rlid = 1;
			wasted = 0;
		}

		if (!locked)
			quads.clear();
	}

	void GraphicsGL::ensure_raw(size_t id, int16_t width, int16_t height, const uint8_t* bgra)
	{
		if (!bgra || width <= 0 || height <= 0)
			return;

		if (offsets.find(id) != offsets.end())
			return;

		GLshort x = 0;
		GLshort y = 0;
		Leftover value = Leftover(0, 0, width, height);

		size_t lid = leftovers.findnode(
			value,
			[](const Leftover& val, const Leftover& leaf)
			{
				return val.width() <= leaf.width() && val.height() <= leaf.height();
			}
		);

		if (lid > 0)
		{
			const Leftover& leftover = leftovers[lid];

			x = leftover.left;
			y = leftover.top;

			GLshort width_delta = leftover.width() - width;
			GLshort height_delta = leftover.height() - height;

			leftovers.erase(lid);
			wasted -= width * height;

			if (width_delta >= MINLOSIZE && height_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x + width, y + height, width_delta, height_delta));
				rlid++;

				if (width >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x, y + height, width, height_delta));
					rlid++;
				}

				if (height >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x + width, y, width_delta, height));
					rlid++;
				}
			}
			else if (width_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x + width, y, width_delta, height + height_delta));
				rlid++;
			}
			else if (height_delta >= MINLOSIZE)
			{
				leftovers.add(rlid, Leftover(x, y + height, width + width_delta, height_delta));
				rlid++;
			}
		}
		else
		{
			if (border.x() + width > ATLASW)
			{
				border.set_x(0);
				border.shift_y(yrange.second());

				if (border.y() + height > ATLASH)
				{
					clearinternal();

					// 帧内无法立刻回收：本图本帧跳过，下一帧 clearscene 后重传。
					if (atlas_clear_pending)
						return;
				}
				else
					yrange = Range<GLshort>();
			}

			x = border.x();
			y = border.y();
			border.shift_x(width);

			if (height > yrange.second())
			{
				if (x >= MINLOSIZE && height - yrange.second() >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(0, yrange.first(), x, height - yrange.second()));
					rlid++;
				}

				wasted += x * (height - yrange.second());
				yrange = Range<int16_t>(y + height, height);
			}
			else if (height < yrange.first() - y)
			{
				if (width >= MINLOSIZE && yrange.first() - y - height >= MINLOSIZE)
				{
					leftovers.add(rlid, Leftover(x, y + height, width, yrange.first() - y - height));
					rlid++;
				}

				wasted += width * (yrange.first() - y - height);
			}
		}

#ifdef USE_OPENGL_ES
		{
			std::vector<uint8_t> rgba(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
			for (size_t p = 0; p < rgba.size(); p += 4)
			{
				rgba[p + 0] = bgra[p + 2];
				rgba[p + 1] = bgra[p + 1];
				rgba[p + 2] = bgra[p + 0];
				rgba[p + 3] = bgra[p + 3];
			}
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
		}
#else
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, bgra);
#endif

		offsets.emplace(id, Offset(x, y, width, height));
	}

	void GraphicsGL::draw_raw(size_t id, int16_t x, int16_t y, int16_t width, int16_t height, float alpha)
	{
		if (locked || alpha <= 0.0f)
			return;

		auto it = offsets.find(id);

		if (it == offsets.end())
			return;

		Rectangle<int16_t> rect(x, x + width, y, y + height);

		if (!rect.overlaps(SCREEN))
			return;

		Color color(1.0f, 1.0f, 1.0f, alpha);
		quads.emplace_back(rect.left(), rect.right(), rect.top(), rect.bottom(), it->second, color, 0.0f);
	}
}