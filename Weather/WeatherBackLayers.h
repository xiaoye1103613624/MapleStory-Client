//////////////////////////////////////////////////////////////////////////////////
// 天气前景素材层（独立自绘，不碰 083 原生 MapBackgrounds）
//
// 2026-11-19 用户纠错：昼夜/天气不得修改 083 原生地图背景渲染逻辑（魔法密林背景树/光线
// 曾被注入到 backgrounds 末尾的天空层盖住）。本版按用户要求还原 MapBackgrounds 到 083
// 原状，天气只用两种方式：①WeatherFx 全屏蒙版表现昼夜；②本类自绘"前景素材"（雨/雾/雪），
// 严格画在角色之后，不改原生背景向量。天空素材（渐变/月/星/沙暴/云）不再注入——它们的
// 层级必须在树后，原生单通道无法在 drawbackgrounds 内部插层，故统一由蒙版表达。
//
// 服务端权威不变：服务端 0x373D 广播天空/时钟（切换），客户端只渲染。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/Animation.h"
#include "../Graphics/DrawArgument.h"

#include <cstddef>
#include <vector>

namespace ms
{
	namespace weather
	{
		class BackLayers
		{
		public:
			static BackLayers& get();

			// 进图时从资源根解析前景素材动画（雨/雾/雪）；无资源则本层静默。
			void on_map_loaded();

			// 每帧按 State 天空/时钟推进素材 alpha 与动画；Stage::update 调用。
			void update(bool snap, uint16_t timestep);

			// 自绘前景素材（角色之后、夜色蒙版之前）；Stage::draw 调用。
			// viewpos=camera.position(alpha)，viewx/viewy=camera.realposition(alpha)。
			void draw(const DrawArgument& viewpos, double viewx, double viewy, float alpha) const;

			void clear();

		private:
			BackLayers() = default;

			struct Layer
			{
				Animation anim;
				int x = 0, y = 0;     // 世界坐标（视差基准）
				double rx = 0, ry = 0;
				float target = 0.0f;  // 目标 alpha（由天空驱动）
				float current = 0.0f; // 当前 alpha（fade）
				bool rain = false, fog = false, snow = false;
			};

			std::vector<Layer> layers_;
			bool active_ = false;
		};
	}
}
