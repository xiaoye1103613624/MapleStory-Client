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

namespace ms
{
	class Background
	{
	public:
		Background(nl::node src);

		// 天气注入层：显式参数构造（数值来自 bundle weatherfx.cpp 的 BACKENTRY，
		// 不是地图 WZ 的 back 节点）。复用与本类相同的 settype/draw/update 平铺与视差逻辑。
		struct Params
		{
			std::string bs;   // back sheet 名（如 "nightDesert" / "WeatherSnow"）
			int32_t no = 0;   // sheet 的 back/ani 子节点号
			bool ani = false;
			int32_t x = 0, y = 0;
			double rx = 0.0, ry = 0.0;
			int32_t type = 0;  // 0=NORMAL..7=VMOVEB，与本类 Type 同编码
			int32_t cx = 0, cy = 0;
			float a = 0.0f;    // 初始 alpha（0..255），后续由 set_opacity 逐帧驱动
			bool front = false;
		};
		Background(const Params& params);

		void draw(double viewx, double viewy, float alpha) const;
		void update();

		// 天气 fade：逐帧覆盖透明度（0..1），保留构造时的初值以便复位。
		void set_opacity(float ratio01);
		float get_opacity_ratio() const { return opacity / 255.0f; }

	private:
		void init_common();  // 构造共用的视口尺寸初始化
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

		// 天气注入层（bundle weatherfx.cpp 的背景层）。由 WeatherBackLayers 在进图时
		// 灌入、每帧驱动 alpha；front=false 画在 drawbackgrounds、true 画在 drawforegrounds。
		void set_injected(const std::vector<Background::Params>& back, const std::vector<Background::Params>& front);
		void clear_injected();
		size_t injected_count(bool foreground) const;
		void set_injected_opacity(bool foreground, size_t index, float ratio01);
		float get_injected_opacity(bool foreground, size_t index) const;

	private:
		std::vector<Background> backgrounds;
		std::vector<Background> foregrounds;
		std::vector<Background> injected_backgrounds;
		std::vector<Background> injected_foregrounds;
		bool black;
	};
}