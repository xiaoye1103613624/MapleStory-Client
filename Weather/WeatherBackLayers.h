//////////////////////////////////////////////////////////////////////////////////
// 天气背景注入层（bundle weatherfx.cpp 完整背景层移植）
//
// 把 bundle 的 8 组 back 层（夜空渐变/月/星/沙暴/彩虹/云/雨/雾/雪）作为额外
// Background 注入到 MapBackgrounds。数值逐条来自 weatherfx.cpp:122-273 的
// BACKENTRY 表，type 编码与原生 Background::Type 同源（MapBackgrounds.h:37-47）。
//
// 与 bundle 的差异（如实）：bundle 有逐像素 multiply tint（ApplyBackFade），
// 原生 Animation::draw 只有 alpha 通道，故本版用 alpha fade 近似，观察层叠观感需真机校准。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Gameplay/MapleMap/MapBackgrounds.h"

#include <cstddef>

namespace ms
{
	namespace weather
	{
		class BackLayers
		{
		public:
			static BackLayers& get();

			// 进图时按当前地图是否户外灌入注入层；非户外清空。
			void on_map_loaded();

			// 每帧按 State.sky()/profile/时钟算各层目标 alpha 并 fade；由 Stage::update 调用。
			void update(bool snap);

			void clear();

		private:
			BackLayers() = default;
			bool active_ = false;
		};
	}
}
