//////////////////////////////////////////////////////////////////////////////////
// 天气渲染层（D1 核心闭环）：世界时钟驱动的昼夜压暗 + 区域夜色。
//
// 与 bundle DLL 的差异（如实标注）：bundle 通过 hook 对 tiles/objs/backs/npc 逐层做
// multiply tint；原生端 Sprite 没有统一 multiply 通道，本版用 GraphicsGL::drawscreenfill
// 做全屏半透明夜色叠加（darken）。观感接近但不是逐像素乘法，UI 不被压暗（在 UI 绘制前
// 调用）。若要严格还原 multiply，需要给 GraphicsGL 加 tint uniform——列为后续增强。
//
// 天空注入（云/月/雨雪 sheet）、地面堆积、路灯 sprite 依赖 P6 WZ 资源，接口在
// WeatherReserved，本版不启用。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms
{
	namespace weather
	{
		class Fx
		{
		public:
			static Fx& get();

			// 由 Stage::update 每帧调用：推进本地时钟、趋近目标夜色。
			void update(int64_t dt_ms);

			// 由 Stage::draw 在场景绘制完成后、UI 之前调用：叠加夜色。
			// active 为 false（非游戏态或未收到 0x373D）时零绘制。
			void draw() const;
			// 风暴闪电全屏脉冲，须在 draw() 之后调用（盖在夜色之上）。
			void draw_lightning() const;
			float night_level() const { return night_level_; }

			void reset();  // 换线/断线

		private:
			Fx() = default;
			float night_level_ = 0.0f;   // 0 白天 1 深夜
			// 叠加色是"要给场景盖上的暗色"，初值必须是区域默认夜色而非白(1.0)——
			// 白色叠加在 alpha>0 时会形成灰白蒙版（真机 2026-11-18 已复现），从根上消除白路径。
			float tint_r_ = 0x4A / 255.0f, tint_g_ = 0x5A / 255.0f, tint_b_ = 0x8C / 255.0f;
			float lightning_ = 0.0f;     // 风暴闪电全屏脉冲 0..1（bundle WeatherFx::TickLightning）
		};
	}
}
