//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
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

#include <cstddef>
#include <cstdint>

namespace ms
{
	// 从 text[pos] 起解码一个 UTF-8 字符，consumed 输出它占用的字节数（含首字节）。
	// 返回码点；控制字节、落在字符中间的续字节、被截断的序列一律返回 0 且 consumed = 1，
	// 调用方按"什么都不画、也不占推进量"处理。
	//
	// 排版（GraphicsGL::LayoutBuilder）与绘制（GraphicsGL::drawtext）必须共用这一个口径：
	// T12b 的成因就是两边各手写了一遍解码，量出来的宽度和画出来的推进量对不上。
	//
	// 刻意**不做**严格校验（尾字节是否 0x80..0xBF、引导字节是否合法、是否过长编码）——
	// 改动前的绘制路径就是 `(cp << 6) | (b & 0x3F)` 直接拼，加校验等于顺手改渲染行为。
	// 入站字符串自 T11 起已是合法 UTF-8，畸形序列只可能来自损坏数据；
	// 这里的宽松语义由 tools/probe/utf8_metrics_check.cpp 的 `lax:` 向量钉住，不是没发现。
	inline uint32_t utf8_next(const char* text, size_t pos, size_t last, size_t& consumed)
	{
		consumed = 1;

		const unsigned char uc = static_cast<unsigned char>(text[pos]);

		if (uc < 0x20)
			return 0;
		if (uc < 0x80)
			return uc;
		if (uc < 0xC0)
			return 0;

		const size_t extra = (uc < 0xE0) ? 1 : (uc < 0xF0) ? 2 : 3;

		// 尾字节越界：与旧绘制逻辑一致，剩余字节逐个当无效跳过。
		if (pos + extra >= last)
			return 0;

		uint32_t cp = uc & (extra == 1 ? 0x1F : extra == 2 ? 0x0F : 0x07);

		for (size_t k = 1; k <= extra; k++)
			cp = (cp << 6) | (static_cast<unsigned char>(text[pos + k]) & 0x3F);

		consumed = extra + 1;

		return cp;
	}
}
