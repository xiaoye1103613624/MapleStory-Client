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

#include "../Physics/PhysicsObject.h"

#include "../../Graphics/Animation.h"

#include <iostream>
#include <string>
#include <cstddef>

namespace ms
{
	class Background
	{
	public:
		Background(nl::node src);

		void draw(double viewx, double viewy, float alpha) const;
		void update();
		// 视口切换钩子：由外层 MapBackgrounds 在三处切换点主动调用（cpp:294-300），
		// 故须 public。[编译修复 2026-11-21] 并行改动把声明留在 private 致 C2248。
		void refresh_viewport();

	private:
		enum Type
		{
			NORMAL,
			HTILED,
			VTILED,
			TILED,
			HMOVEA,
			VMOVEA,
			HMOVEB,
			VMOVEB
		};

		static Type typebyid(int32_t id)
		{
			if (id >= NORMAL && id <= VMOVEB)
				return static_cast<Type>(id);

			LOG(LOG_DEBUG, "Unknown Background::Type id: [" << id << "]");

			return NORMAL;
		}

		void settype(Type type);
		// refresh_viewport 已移至 public（外层 MapBackgrounds 需调用）。

		Type type = Type::NORMAL;
		int16_t VWIDTH;
		int16_t VHEIGHT;
		int16_t WOFFSET;
		int16_t HOFFSET;

		Animation animation;
		bool animated;
		int16_t cx;
		int16_t cy;
		double rx;
		double ry;
		int16_t htile;
		int16_t vtile;
		float opacity;
		bool flipped;

		MovingObject moveobj;
	};

	class MapBackgrounds
	{
	public:
		MapBackgrounds(nl::node src);
		MapBackgrounds();

		void drawbackgrounds(double viewx, double viewy, float alpha) const;
		void drawforegrounds(double viewx, double viewy, float alpha) const;
		void update();

		// ---- 多分辨率自适应（A+C 方案，2026-11-21）----
		// 背景平铺块数历史上在构造期按"当时"的 VWIDTH/VHEIGHT 缓存；而游戏视口是在
		// MapBackgrounds 构造之后才由 SetFieldHandlers::apply_game_viewsize() 切换的，
		// 之后游戏内切档也没有任何入口重算 —— 结果高分辨率下背景只按旧视口平铺，
		// 四边露底（黑）。修复分两层：
		//   1) Background::draw/MapBackgrounds::drawbackgrounds 绘制期实时从 Constants 读视口，
		//      平铺块数现算（A：动态平铺），不再依赖构造期快照；
		//   2) 兜底色由纯黑改为天空色（C），残余 1px 缝隙不可见。
		// on_viewport_changed 目前仅作显式刷新钩子（清掉任何视口相关缓存），供三处视口
		// 切换点调用；即使调用方漏调，绘制期实时计算也能正确适配，双保险。
		void on_viewport_changed();

		// ---- 天气扩展（纯新增，不改动上面任何既有方法的行为）----
		// 只画 backgrounds[begin,end) 区间：让调用方能把天气天空素材插在
		// 「地图自有天空层之后、山树层之前」，等价于插件在 DLL 侧做的 z 序重写
		// （BeiDou-ijl15 weather.cpp:1916-1924：front==0 的 z 带与 front!=0 的近相机带）。
		// 单参版本等价于 (0, backgrounds.size())，原生逐层绘制算法一行未改。
		void drawbackgrounds(double viewx, double viewy, float alpha, size_t begin, size_t end) const;

		// back/0 的 (bS,no)：天气天空门禁用（对齐插件 FieldHasSky 的 ReadBackmostBase，
		// weather.cpp:2073-2100）。bS 为空表示该图无背景基座（black）。
		const std::string& get_backmost_bs() const { return backmost_bs; }
		int32_t get_backmost_no() const { return backmost_no; }
		// 背景层数（供天气分层切片）
		size_t size() const { return backgrounds.size(); }

	private:
		std::vector<Background> backgrounds;
		std::vector<Background> foregrounds;
		bool black;
		std::string backmost_bs;
		int32_t backmost_no = -1;
	};
}