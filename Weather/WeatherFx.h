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

			void reset();  // 换线/断线

		private:
			Fx() = default;
			float night_level_ = 0.0f;   // 0 白天 1 深夜
			float tint_r_ = 1.0f, tint_g_ = 1.0f, tint_b_ = 1.0f;  // 当前趋近的夜色倍率
		};
	}
}
