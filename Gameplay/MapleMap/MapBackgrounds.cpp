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
#include "MapBackgrounds.h"

#include "../../Graphics/GraphicsGL.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <cmath>
#include <vector>

namespace ms
{
	namespace
	{
		// 高分 type=0 半透明淡出延展（docs/features/map-bg-viewport-fallback）。
		constexpr float kBgFadeRatio = 0.55f;
		constexpr int kBgMaxHCopies = 3;   // 单侧最多水平级数
		constexpr int kBgMaxVCopies = 4;   // 单侧最多竖直级数
		constexpr float kBgMinOpc = 0.05f;
	}
	Background::Background(nl::node src)
	{
		// 视口相关成员统一交给 refresh_viewport()：构造期先按当前 Constants 初始化一次，
		// 之后每次 draw 再按"当时的" Constants 重算 —— 游戏视口在本对象构造之后才切到
		// game_* 档位、且游戏内可随时切档，构造期快照必然过期（高分露底根因）。
		refresh_viewport();

		nl::node backsrc = nl::nx::Map001["Back"];

		if (!backsrc)
			backsrc = nl::nx::Map["Back"];

		animated = src["ani"].get_bool();
		nl::node bS = src["bS"];
		std::string bs_name = bS.get_string();
		nl::node sheet = backsrc[bs_name + ".img"];

		if (!sheet)
			sheet = backsrc[bs_name];

		animation = sheet[animated ? "ani" : "back"][src["no"]];
		opacity = src["a"];
		flipped = src["f"].get_bool();
		cx = src["cx"];
		cy = src["cy"];
		rx = src["rx"];
		ry = src["ry"];

		moveobj.set_x(src["x"]);
		moveobj.set_y(src["y"]);

		type = typebyid(src["type"]);

		settype(type);
	}

	void Background::refresh_viewport()
	{
		// 每次绘制/视口变化时调用：视口尺寸来自 Constants 的当前值（进游戏后由
		// SetFieldHandlers::apply_game_viewsize 写成 game_* 档位，游戏内切档亦会更新）。
		// 平铺块数用 ceil 而非整除：VWIDTH/cx 向下取整会少铺一列，高分辨率下右/下露底。
		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();
		WOFFSET = VWIDTH / 2;
		HOFFSET = VHEIGHT / 2;

		if (cx > 0 && cy > 0)
		{
			const auto ceil_div = [](int32_t total, int32_t unit) -> int16_t {
				return static_cast<int16_t>((total + unit - 1) / unit);
			};

			// +3 保留原生视差冗余（rx/ry 平移与滚动余量），ceil 保证任何档位都铺满。
			switch (type)
			{
			case Type::HTILED:
			case Type::HMOVEA:
				htile = static_cast<int16_t>(ceil_div(VWIDTH, cx) + 3);
				break;
			case Type::VTILED:
			case Type::VMOVEA:
				vtile = static_cast<int16_t>(ceil_div(VHEIGHT, cy) + 3);
				break;
			case Type::TILED:
			case Type::HMOVEB:
			case Type::VMOVEB:
				htile = static_cast<int16_t>(ceil_div(VWIDTH, cx) + 3);
				vtile = static_cast<int16_t>(ceil_div(VHEIGHT, cy) + 3);
				break;
			default:
				break;
			}
		}
	}

	void Background::settype(Type type)
	{
		// 保存类型供 refresh_viewport 每次按视口重算 htile/vtile。
		this->type = type;

		int16_t dim_x = animation.get_dimensions().x();
		int16_t dim_y = animation.get_dimensions().y();

		// TODO: Double check for zero. Is this a WZ reading issue?
		// Also reject negatives: Background::draw uses while (x > 0) x -= cx;
		// with cx < 0 that loop never ends (client appears Not Responding).
		if (cx <= 0)
			cx = (dim_x > 0) ? dim_x : 1;

		if (cy <= 0)
			cy = (dim_y > 0) ? dim_y : 1;

		htile = 1;
		vtile = 1;

		// 构造期也算一次（refresh_viewport 里是同一套 ceil 公式）。
		refresh_viewport();

		switch (type)
		{
		case Type::HMOVEA:
		case Type::HMOVEB:
			moveobj.hspeed = rx / 16;
			break;
		case Type::VMOVEA:
		case Type::VMOVEB:
			moveobj.vspeed = ry / 16;
			break;
		}
	}

	void Background::draw_normal_with_fade(int16_t ix, int16_t iy, float alpha) const
	{
		const Point<int16_t> origin = animation.get_origin();
		const Point<int16_t> dim = animation.get_dimensions();
		const int16_t dim_x = dim.x();
		const int16_t dim_y = dim.y();
		const float base_opc = opacity / 255.0f;

		// 无有效画布时只画本体，避免步进 0 死循环。
		if (dim_x <= 0 || dim_y <= 0)
		{
			animation.draw(DrawArgument(Point<int16_t>(ix, iy), flipped, base_opc), alpha);
			return;
		}

		struct Copy
		{
			int16_t x;
			int16_t y;
			bool flip;
			float opc;
		};

		const auto screen_rect = [&](int16_t px, int16_t py, bool flip_flag) -> Rectangle<int16_t> {
			return DrawArgument(Point<int16_t>(px, py), flip_flag, 1.0f)
				.get_rectangle(origin, dim);
		};

		// ---- 水平：本体 + 左右镜像淡出列（最多单侧 kBgMaxHCopies）----
		std::vector<Copy> cols;
		cols.push_back({ ix, iy, flipped, base_opc });

		auto h_span = [&](int32_t& out_l, int32_t& out_r) {
			out_l = cols.front().x;
			out_r = cols.front().x;
			bool first = true;
			for (const Copy& c : cols)
			{
				const Rectangle<int16_t> r = screen_rect(c.x, c.y, c.flip);
				const int32_t l = r.left();
				const int32_t rr = r.right();
				if (first)
				{
					out_l = l;
					out_r = rr;
					first = false;
				}
				else
				{
					if (l < out_l)
						out_l = l;
					if (rr > out_r)
						out_r = rr;
				}
			}
		};

		int32_t span_l = 0;
		int32_t span_r = 0;
		h_span(span_l, span_r);

		for (int k = 1; k <= kBgMaxHCopies; ++k)
		{
			if (span_l <= 0 && span_r >= VWIDTH)
				break;

			const float opc = base_opc * std::pow(kBgFadeRatio, static_cast<float>(k));
			if (opc < kBgMinOpc)
				break;

			const bool mirror = (k % 2) == 1;
			const bool nf = flipped ^ mirror;
			const int32_t dx = static_cast<int32_t>(dim_x) * k;

			if (span_l > 0)
			{
				const int16_t nx = static_cast<int16_t>(ix - dx);
				cols.push_back({ nx, iy, nf, opc });
				const Rectangle<int16_t> r = screen_rect(nx, iy, nf);
				if (r.left() < span_l)
					span_l = r.left();
				if (r.right() > span_r)
					span_r = r.right();
			}

			if (span_r < VWIDTH)
			{
				const int16_t nx = static_cast<int16_t>(ix + dx);
				cols.push_back({ nx, iy, nf, opc });
				const Rectangle<int16_t> r = screen_rect(nx, iy, nf);
				if (r.left() < span_l)
					span_l = r.left();
				if (r.right() > span_r)
					span_r = r.right();
			}
		}

		// ---- 竖直：对每个水平基线实例向上下淡出（不再镜像翻倍）----
		std::vector<Copy> draws = cols;

		for (const Copy& base : cols)
		{
			Rectangle<int16_t> vr = screen_rect(base.x, base.y, base.flip);
			int32_t top = vr.top();
			int32_t bottom = vr.bottom();

			for (int k = 1; k <= kBgMaxVCopies; ++k)
			{
				if (top <= 0 && bottom >= VHEIGHT)
					break;

				const float opc = base.opc * std::pow(kBgFadeRatio, static_cast<float>(k));
				if (opc < kBgMinOpc)
					break;

				const int32_t dy = static_cast<int32_t>(dim_y) * k;

				if (top > 0)
				{
					const int16_t ny = static_cast<int16_t>(base.y - dy);
					draws.push_back({ base.x, ny, base.flip, opc });
					const Rectangle<int16_t> r = screen_rect(base.x, ny, base.flip);
					if (r.top() < top)
						top = r.top();
					if (r.bottom() > bottom)
						bottom = r.bottom();
				}

				if (bottom < VHEIGHT)
				{
					const int16_t ny = static_cast<int16_t>(base.y + dy);
					draws.push_back({ base.x, ny, base.flip, opc });
					const Rectangle<int16_t> r = screen_rect(base.x, ny, base.flip);
					if (r.top() < top)
						top = r.top();
					if (r.bottom() > bottom)
						bottom = r.bottom();
				}
			}
		}

		for (const Copy& c : draws)
			animation.draw(DrawArgument(Point<int16_t>(c.x, c.y), c.flip, c.opc), alpha);
	}

	void Background::draw(double viewx, double viewy, float alpha) const
	{
		// A 方案核心：绘制期实时按当前视口重算平铺参数，视口变了立刻适配，
		// 不再依赖构造期快照（进游戏切 game_* 档位、游戏内切档都不通知本类）。
		const_cast<Background*>(this)->refresh_viewport();

		double x;

		if (moveobj.hmobile())
		{
			x = moveobj.get_absolute_x(viewx, alpha);
		}
		else
		{
			double shift_x = rx * (WOFFSET - viewx) / 100 + WOFFSET;
			x = moveobj.get_absolute_x(shift_x, alpha);
		}

		double y;

		if (moveobj.vmobile())
		{
			y = moveobj.get_absolute_y(viewy, alpha);
		}
		else
		{
			double shift_y = ry * (HOFFSET - viewy) / 100 + HOFFSET;
			y = moveobj.get_absolute_y(shift_y, alpha);
		}

		if (htile > 1)
		{
			while (x > 0)
				x -= cx;

			while (x < -cx)
				x += cx;
		}

		if (vtile > 1)
		{
			while (y > 0)
				y -= cy;

			while (y < -cy)
				y += cy;
		}

		int16_t ix = static_cast<int16_t>(std::round(x));
		int16_t iy = static_cast<int16_t>(std::round(y));

		// type=0：高分下单张盖不住视口 → 半透明淡出延展（全局 backgrounds/foregrounds）。
		// 平铺/滚动类保持原 ceil 平铺，不做整层复制。
		if (type == Type::NORMAL)
		{
			draw_normal_with_fade(ix, iy, alpha);
			return;
		}

		// int16_t 在 2560 档 + cx=1 兜底时会溢出（cx*htile 远超 32767），
		// 平铺跨度改 int32_t，循环变量同样放宽，避免高分下少铺/死循环。
		int32_t tw = static_cast<int32_t>(cx) * htile;
		int32_t th = static_cast<int32_t>(cy) * vtile;

		for (int32_t tx = 0; tx < tw; tx += cx)
			for (int32_t ty = 0; ty < th; ty += cy)
				animation.draw(DrawArgument(Point<int16_t>(
					static_cast<int16_t>(ix + tx), static_cast<int16_t>(iy + ty)), flipped, opacity / 255), alpha);
	}

	void Background::update()
	{
		moveobj.move();
		animation.update();
	}

	MapBackgrounds::MapBackgrounds(nl::node src)
	{
		int16_t no = 0;
		nl::node back = src[std::to_string(no)];

		while (back.size() > 0)
		{
			bool front = back["front"].get_bool();

			if (front)
				foregrounds.push_back(back);
			else
				backgrounds.push_back(back);

			no++;
			back = src[std::to_string(no)];
		}

		black = src["0"]["bS"].get_string() == "";
		// 天气天空门禁：back/0 的 (bS,no)，对齐插件 FieldHasSky 的 ReadBackmostBase。
		// 必须做类型守卫：无 back 的地图/登录图上 src["0"] 是空节点，
		// 直接 operator int32_t() 会抛异常（曾导致客户端起窗阶段直接退进程）。
		{
			nl::node b0 = src["0"];
			if (b0)
			{
				nl::node bs_node = b0["bS"];
				if (bs_node.data_type() == nl::node::type::string)
					backmost_bs = bs_node.get_string();

				nl::node no_node = b0["no"];
				if (no_node.data_type() == nl::node::type::integer)
					backmost_no = no_node;
			}
		}
	}

	// black 是 bool 成员，默认构造必须初始化；否则换图重建 MapBackgrounds 时
	// 读到未定义值，drawbackgrounds 的铺底判断随机失效 → 露白清屏。
	MapBackgrounds::MapBackgrounds() : black(true) {}

	void MapBackgrounds::drawbackgrounds(double viewx, double viewy, float alpha) const
	{
		drawbackgrounds(viewx, viewy, alpha, 0, backgrounds.size());
	}

	// 天气扩展：区间绘制（纯新增）。begin==0 时保留原黑底铺底语义，
	// 后续区间调用不再重复铺底，避免把已画好的天空/山树盖掉。
	void MapBackgrounds::drawbackgrounds(double viewx, double viewy, float alpha, size_t begin, size_t end) const
	{
		// C 方案：begin==0 时先铺一层全屏底色。旧实现只在 black/空背景时铺纯黑，
		// 有天空背景的地图根本不铺底 —— 平铺舍入缝隙直接露出 GL clear 色（黑/白块）。
		// 现在所有地图都铺底：室内(no back base, black)保留深色，户外铺天空蓝，
		// 任何分辨率下残余 1px 缝隙都与天空相近而不可见。
		// 注意：这段只执行一次（begin==0），后续区间调用不再铺，避免盖掉天空/山树。
		if (begin == 0)
		{
			if (black)
				GraphicsGL::get().drawscreenfill(0.0f, 0.0f, 0.0f, 1.0f);
			else
				GraphicsGL::get().drawscreenfill(0.42f, 0.66f, 0.86f, 1.0f);
		}

		if (begin >= backgrounds.size())
			return;
		const size_t stop = end > backgrounds.size() ? backgrounds.size() : end;
		for (size_t i = begin; i < stop; ++i)
			backgrounds[i].draw(viewx, viewy, alpha);
	}

	void MapBackgrounds::drawforegrounds(double viewx, double viewy, float alpha) const
	{
		for (auto& foreground : foregrounds)
			foreground.draw(viewx, viewy, alpha);
	}

	void MapBackgrounds::update()
	{
		for (auto& background : backgrounds)
			background.update();

		for (auto& foreground : foregrounds)
			foreground.update();
	}

	void MapBackgrounds::on_viewport_changed()
	{
		// 显式刷新钩子（三处视口切换点调用）。Background 每次 draw 都会实时重算，
		// 这里再主动刷新一次，保证切换当帧即使尚未 draw 也持有正确的平铺参数。
		for (auto& background : backgrounds)
			background.refresh_viewport();

		for (auto& foreground : foregrounds)
			foreground.refresh_viewport();
	}
}