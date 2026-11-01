// T12b 离线断言：Graphics/Utf8.h 的解码表 + FreeType 推进量一致性
//
// 独立编译，**不进** vcxproj / CMake（和 charset_check.cpp 一样是取证工具）。
//
// 两块内容：
//   1) utf8_next() 逐条比对 gen_utf8_vectors.py 生成的期望表（期望值来自 Python codec，非手抄）。
//   2) 真字体（客户端实际用的 simhei.ttf，字号取 addfont 的 11..15）上验证一条承重假设：
//        FT_Load_Glyph(FT_LOAD_DEFAULT) 的 advance.x >> 6
//        == FT_Load_Char(FT_LOAD_RENDER) 的 advance.x >> 6
//      LayoutBuilder::wide_advance 用前者、get_wide_glyph 用后者。这条不成立，
//      "量的宽度"和"画的推进量"就还是两套数，T12b 等于没修。
//
// 用法（仓库根，先起 vcvars）：
//   cl /EHsc /std:c++17 /utf-8 /I includes\freetype\include ^
//      tools\probe\utf8_metrics_check.cpp includes\freetype\win64\freetype.lib /Fe:...\utf8_check.exe
//   utf8_check.exe [font.ttf]

#include "../../Graphics/Utf8.h"
#include "utf8_vectors.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdio>
#include <string>
#include <vector>

namespace
{
	int failures = 0;

	std::string hex(uint32_t v)
	{
		char buf[16];
		snprintf(buf, sizeof(buf), "X%04X", v);
		return std::string(buf);
	}

	void check(bool ok, const std::string& what)
	{
		if (!ok)
		{
			failures++;
			printf("FAIL %s\n", what.c_str());
		}
	}

	// 与 GraphicsGL.cpp 的 LayoutBuilder::wide_advance 同一套调用（前者）。
	int advance_default(FT_Face face, FT_UInt gindex)
	{
		if (FT_Load_Glyph(face, gindex, FT_LOAD_DEFAULT))
			return -1;

		return static_cast<int>(face->glyph->advance.x >> 6);
	}

	// 与 GraphicsGL.cpp 的 get_wide_glyph 同一套调用（后者）。
	int advance_render(FT_Face face, FT_UInt32 cp)
	{
		if (FT_Load_Char(face, cp, FT_LOAD_RENDER))
			return -1;

		return static_cast<int>(face->glyph->advance.x >> 6);
	}

	int space_advance(FT_Face face)
	{
		return advance_default(face, FT_Get_Char_Index(face, ' '));
	}
}

int main(int argc, char** argv)
{
	const char* fontpath = argc > 1 ? argv[1] : "E:/MXD/BeiDou-Client-xy/fonts/SimHei/simhei.ttf";

	// ---- 1) 解码表 ----

	size_t decoded_bytes = 0;

	for (size_t v = 0; v < UTF8_VECTOR_COUNT; v++)
	{
		const Utf8Vector& vec = UTF8_VECTORS[v];
		const char* text = reinterpret_cast<const char*>(vec.bytes);

		std::vector<uint32_t> got_cps;
		std::vector<uint32_t> got_consumed;

		for (size_t pos = 0; pos < vec.nbytes; )
		{
			size_t consumed = 1;
			const uint32_t cp = ms::utf8_next(text, pos, vec.nbytes, consumed);

			check(consumed >= 1 && pos + consumed <= vec.nbytes,
				std::string(vec.label) + ": consumed out of range");

			got_cps.push_back(cp);
			got_consumed.push_back(static_cast<uint32_t>(consumed));

			pos += consumed;
		}

		check(got_cps.size() == vec.nchars,
			std::string(vec.label) + ": char count " + std::to_string(got_cps.size())
			+ " != expected " + std::to_string(vec.nchars));

		for (size_t i = 0; i < vec.nchars && i < got_cps.size(); i++)
		{
			check(got_cps[i] == vec.cps[i],
				std::string(vec.label) + ": cp[" + std::to_string(i) + "] = U+"
				+ hex(got_cps[i]) + " != U+" + hex(vec.cps[i]));
			check(got_consumed[i] == vec.consumed[i],
				std::string(vec.label) + ": consumed[" + std::to_string(i) + "] = "
				+ std::to_string(got_consumed[i]) + " != " + std::to_string(vec.consumed[i]));
		}

		size_t sum = 0;

		for (uint32_t c : got_consumed)
			sum += c;

		check(sum == vec.nbytes, std::string(vec.label) + ": consumed sum != nbytes");

		decoded_bytes += sum;
	}

	printf("decode: vectors=%d bytes=%d failures=%d\n",
		static_cast<int>(UTF8_VECTOR_COUNT), static_cast<int>(decoded_bytes), failures);

	// ---- 2) FreeType 推进量假设 ----

	FT_Library library;

	if (FT_Init_FreeType(&library))
	{
		printf("freetype: INIT FAILED\n");
		return failures + 1;
	}

	FT_Face face;

	if (FT_New_Face(library, fontpath, 0, &face))
	{
		printf("freetype: OPEN FAILED %s\n", fontpath);
		return failures + 1;
	}

	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	// 样本码点：解码表里所有合法码点 + 一段 CJK 抽样 + 已知缺字
	std::vector<FT_UInt32> corpus;

	for (uint32_t cp = 0x20; cp < 0x7F; cp++)
		corpus.push_back(cp);

	for (size_t v = 0; v < UTF8_VECTOR_COUNT; v++)
		for (size_t i = 0; i < UTF8_VECTORS[v].nchars; i++)
			if (UTF8_VECTORS[v].cps[i] >= 0x80)
				corpus.push_back(UTF8_VECTORS[v].cps[i]);

	for (FT_UInt32 cp = 0x4E00; cp <= 0x9FA5; cp += 37)
		corpus.push_back(cp);

	corpus.push_back(0xAC00);	// 한
	corpus.push_back(0x1F383);	// emoji，字体必缺

	int compared = 0;
	int missing = 0;
	int mismatched = 0;
	int width_stats_min = 0x7FFFFFFF;
	int width_stats_max = 0;
	int ascii_min = 0x7FFFFFFF;
	int ascii_max = 0;

	const int sizes[] = { 11, 12, 13, 14, 15 };

	for (int px : sizes)
	{
		if (FT_Set_Pixel_Sizes(face, 0, px))
		{
			check(false, "FT_Set_Pixel_Sizes " + std::to_string(px));
			continue;
		}

		const int space = space_advance(face);

		for (FT_UInt32 cp : corpus)
		{
			const FT_UInt gi = FT_Get_Char_Index(face, cp);

			if (gi == 0)
			{
				missing++;
				continue;
			}

			const int a_default = advance_default(face, gi);
			const int a_render = advance_render(face, cp);

			compared++;

			if (a_default != a_render)
			{
				mismatched++;
				printf("FAIL advance mismatch px=%d cp=U+%04X default=%d render=%d\n",
					px, cp, a_default, a_render);
			}

			if (cp >= 0x20 && cp < 0x7F)
			{
				if (a_render < ascii_min) ascii_min = a_render;
				if (a_render > ascii_max) ascii_max = a_render;
			}
			else if (cp >= 0x2E80)
			{
				if (a_render < width_stats_min) width_stats_min = a_render;
				if (a_render > width_stats_max) width_stats_max = a_render;
			}

			// 旧实现给一个汉字记 3 个空格宽（UTF-8 3 字节），新实现记它自己的推进量。
			// 这里只断言方向成立：全角字宽必然 > 空格宽，否则"3 倍空格"的误差量级不成立。
			if (cp >= 0x4E00 && cp <= 0x9FA5)
				check(a_render > space,
					"CJK narrower than space px=" + std::to_string(px)
					+ " cp=U+" + std::to_string(cp));
		}

		printf("freetype: px=%d space=%d\n", px, space);
	}

	printf("freetype: compared=%d missing=%d mismatched=%d ascii_ax=[%d,%d] cjk_ax=[%d,%d]\n",
		compared, missing, mismatched, ascii_min, ascii_max, width_stats_min, width_stats_max);

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	printf("RESULT failures=%d\n", failures);

	return failures;
}
