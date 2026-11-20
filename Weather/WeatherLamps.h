//////////////////////////////////////////////////////////////////////////////////
// 天气路灯（D1 核心版）：夜色降临点亮城镇路灯的暖色光晕。
//
// bundle lamps.cpp 用 74 种 sprite + emitter 精确偏移逐灯画光晕/灯丝；本版取
// 40 条手工放置坐标（lamps.cpp:581-661），在世界坐标画暖色光晕圆（GraphicsGL
// 多层矩形近似径向渐变），随 night_level 淡入。灯 sprite 精确 emitter 对齐、
// 地图自身 obj 匹配点亮留后续（见 TODO）。核心观感"天黑灯亮"已具备。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms
{
	namespace weather
	{
		class Lamps
		{
		public:
			static Lamps& get();

			// 由 Stage::update 每帧调用：按夜色强度推进光晕 fade。
			void update(float night_level);

			// 由 Stage::draw 在场景之后调用：画光晕（世界坐标已换算为屏幕坐标）。
			void draw(double viewx, double viewy, float alpha) const;

			void clear();

		private:
			Lamps() = default;

			// 参考 lamps.cpp:415-450 的光敏三态：
			//   ON=0.35 触发 / OFF=0.28 释放（OFF 必须 >0，否则白天常亮）；
			//   暮色渐变 DIM_FROM=0.14..DIM_MAX=0.34 带下限，白天不漏光。
			static constexpr float kOnLevel = 0.35f;
			static constexpr float kOffLevel = 0.28f;
			static constexpr float kDimFrom = 0.14f;
			static constexpr float kDimMax = 0.34f;

			bool on_ = false;         // 光敏开关（带迟滞）
			float level_ = 0.0f;      // 每盏灯共用的点亮强度（0暗..1全亮）
			uint32_t tick_ms_ = 0;    // 累计时间，用于 warmup / stagger

			// 灯芯呼吸：约 5 秒一个周期（2500ms 升 / 2500ms 降）。
			static constexpr uint32_t kBreathePeriodMs = 5000;
			uint32_t breathe_ms_ = 0;
			float breathe_ = 1.0f;
		};
	}
}
