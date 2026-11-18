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

#include "Tooltip.h"

#include "../../Graphics/Geometry.h"
#include "../../Graphics/Text.h"
#include "../../Graphics/Texture.h"

#include "TooltipLattice.h"

namespace ms
{
	// Q1 定案（PLAN §6）：v83 原生 tooltip 框体是程序描边而非贴图 —— 包内无 UIToolTip.img、
	// UIWindow.img/ToolTip 无九切片，且包内全部九切片族均为浅底气泡（证据见
	// docs/features/ui-v83-window-restore/evidence/w5-tooltip/coords.md）。
	// 5 个 tooltip 组件共用。
	//
	// 2026-11-08 逐像素复校（参考帧《背包中鼠标悬浮物品上显示的物品信息内容.png》）：
	// 背板填充 = (79,79,119)，四边 1px 亮边 = (85,85,130)，**没有灰色外框**。
	// 旧实现填充 (85,85,130) + 灰边 (128,128,128) 是「背景不对」的主因（偏亮 + 多一圈灰框）。
	//
	// 2026-11-15 m00754 ⑦ 翻案（撤销 m00304 的实心错改）：079 原生 1:1 帧
	// m00751 放大后右半能透出深色洞穴纹理（紫色区 std 8.4，实心当前 std 0.9），
	// 且 docs/.../equip-native-compare.md:35 早已逐像素反算底 α=0.90 精确。
	// 白字发灰与背板 α 物理无关（正文在背板之后绘制），m00304 的归因是错的。
	// 恢复 git HEAD 已实机验证的半透明：fill α=0.88，edge α=0.92（边框略实，
	// 保住 1px 亮边不被吃掉）。
	// 透明度走 Color::a()（Geometry::draw 里 opacity * color.a() 二次相乘），
	// ColorBox 的 opacity 参数保持 1.0f，避免两处相乘产生 0.77 的过透。
	class TooltipBackdrop
	{
	public:
		// lattice=true 时叠加原生十字织纹背板（41x34 tile 平铺，见 TooltipLattice）。
		// 织纹背板为全不透明（图2 铁证：tooltip 压白格(255)/奶油格(238)上底色恒为(79,74,119)），
		// 其余 tooltip 保持既有半透明口径不变。
		void draw(Point<int16_t> topleft, int16_t width, int16_t height, bool lattice = false) const
		{
			ColorBox fill(width, height, lattice
				? Color(0.0f, 0.0f, 0.0f, FILL_ALPHA) // v083: 0xBB000000 纯黑
				: Color(0.0f, 0.0f, 0.0f, FILL_ALPHA), 1.0f); // v083: 0xBB000000 纯黑
			fill.draw(topleft);

			if (lattice)
				TooltipLattice().draw(topleft, width, height);

			const Color edge(85.0f / 255.0f, 85.0f / 255.0f, 130.0f / 255.0f, EDGE_ALPHA);

			ColorBox top(width, 1, edge, 1.0f);
			top.draw(topleft);

			ColorBox bottom(width, 1, edge, 1.0f);
			bottom.draw(topleft + Point<int16_t>(0, height - 1));

			ColorBox side(1, height - 2, edge, 1.0f);
			side.draw(topleft + Point<int16_t>(0, 1));
			side.draw(topleft + Point<int16_t>(width - 1, 1));

			// 079 原生外壳两层：最外亮边 1px + cream(238,238,221) 内环 1px + body
			// （m00877 ④「背景>白色间隔>边框线」；native m00751 左缘 x30 紫 1px /
			// x31 cream 1px / x32 body，顶缘 y85 亮线 / y86 cream / y87 body 同构）。
			const Color ring(238.0f / 255.0f, 238.0f / 255.0f, 221.0f / 255.0f, 1.0f);

			ColorBox rtop(width - 2, 1, ring, 1.0f);
			rtop.draw(topleft + Point<int16_t>(1, 1));

			ColorBox rbottom(width - 2, 1, ring, 1.0f);
			rbottom.draw(topleft + Point<int16_t>(1, height - 2));

			ColorBox rside(1, height - 4, ring, 1.0f);
			rside.draw(topleft + Point<int16_t>(1, 2));
			rside.draw(topleft + Point<int16_t>(width - 2, 2));
		}

		// 079 原生装备 tooltip 双区背板（m00877 ②③ 逐像素定案，m00751 268x310）：
		// 左侧不透明紫卡 = body x32..173（142px，(79,79,119) 恒定，含十字织纹），
		// 右侧半透紫纱 = x174..255（约 82px，alpha≈0.18 的 (79,79,119) 罩在场景上，
		// 实测 obs(23,17,57) = 0.18*79+0.82*11 ✓ / B 0.18*119+0.82*45=56 ✓），
		// 分界处有 1px 亮竖线 (85,85,130)。边框走向：顶缘亮线+cream 内环全宽贯通
		// （x200 y85/y86 实测同构），左缘同构；右缘无框（半透区渐隐入场景），
		// 底缘亮线+cream 只画到不透区为止（x200 y296+ 无 edge）。
		void draw_equip(Point<int16_t> topleft, int16_t width, int16_t height) const
		{
			// 2026-11-16 m01309 还原口径：用户实机反馈「右侧没有背景」——双区半透纱 α0.18
			// 在亮色地图上几乎不可见，被感知为缺背板。按用户 R11 拍板口径整宽还原：
			// 全宽均匀 (79,79,119) 填充 + 四边亮边 + cream 内环，不再做双区分界。
			ColorBox solid(width, height, Color(0.0f, 0.0f, 0.0f, FILL_ALPHA), 1.0f); // v083: 0xBB000000
			solid.draw(topleft);

			const Color edge(85.0f / 255.0f, 85.0f / 255.0f, 130.0f / 255.0f, 1.0f);

			ColorBox top(width, 1, edge, 1.0f);
			top.draw(topleft);

			const Color ring(238.0f / 255.0f, 238.0f / 255.0f, 221.0f / 255.0f, 1.0f);

			ColorBox rtop(width - 2, 1, ring, 1.0f);
			rtop.draw(topleft + Point<int16_t>(1, 1));

			ColorBox bottom(width, 1, edge, 1.0f);
			bottom.draw(topleft + Point<int16_t>(0, height - 1));

			ColorBox rbottom(width - 2, 1, ring, 1.0f);
			rbottom.draw(topleft + Point<int16_t>(1, height - 2));

			ColorBox side(1, height - 2, edge, 1.0f);
			side.draw(topleft);
			side.draw(topleft + Point<int16_t>(width - 1, 1));

			ColorBox rside(1, height - 4, ring, 1.0f);
			rside.draw(topleft + Point<int16_t>(1, 2));
			rside.draw(topleft + Point<int16_t>(width - 2, 2));
		}
	
	private:
		static constexpr float FILL_ALPHA = 187.0f / 255.0f; // v083: 0xBB000000 → A=0xBB(187)≈73.3% 不透明，全 tooltip 统一
		static constexpr float EDGE_ALPHA = 0.92f;
	};

	class TextTooltip : public Tooltip
	{
	public:
		TextTooltip();

		void draw(Point<int16_t> position) const override;

		bool set_text(std::string text, uint16_t maxwidth = 340, bool formatted = true, int16_t line_adj = 2);

	private:
		TooltipBackdrop backdrop;
		std::string text;
		Text text_label;
	};
}