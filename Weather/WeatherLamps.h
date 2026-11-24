//////////////////////////////////////////////////////////////////////////////////
// 天气路灯（D1 核心版）：夜色降临点亮城镇路灯的暖色光晕。
//
// bundle lamps.cpp 用 74 种 sprite + emitter 精确偏移逐灯画光晕/灯丝；本版取
// 40 条手工放置坐标（lamps.cpp:581-661），在世界坐标画暖色光晕圆（GraphicsGL
// 多层矩形近似径向渐变），随 night_level 淡入。
// 0x373F GM !lamp 预览：place/clear 写入本端 preview 槽，不进手工表、不写库。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <vector>

namespace ms
{
	namespace weather
	{
		class Lamps
		{
		public:
			static Lamps& get();

			// 由 Stage::update 每帧调用：按夜色强度推进光晕 fade。
			// snap=true（进图/0x373D SNAP）时直接到位，不再暖灯/错峰（bundle StepSwitch）。
			void update(float night_level, bool snap = false);

			// 由 Stage::draw 在场景之后调用：画光晕（世界坐标已换算为屏幕坐标）。
			void draw(double viewx, double viewy, float alpha) const;

			void clear();

			// 0x373F：mode 0=place 一盏预览灯；mode 1=清除本图全部预览。
			void apply_preview(uint8_t mode, uint8_t lamp_id, int32_t map_id, int32_t x, int32_t y);

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
			// 2026-11-23：当前夜色深度（0..1），光晕强度随它从淡到亮再到暗。
			float night_ = 0.0f;
			static constexpr int16_t kLampLiftY = -20; // 路灯整体上移，光源贴近灯泡高度

			// 参考 lamps.cpp:212-224 明确：路灯自身不应有周期性明暗动画
			// （art 自带的 ramp 是故障，参考用 LampKeepFrame/LampStaticFrame 停掉）。
			// 点亮后恒定常亮，只有"点亮那一刻"按灯序一次性错峰。
			static constexpr uint32_t kStaggerMs = 2600;       // 参考 LAMP_STAGGER_MS
			static constexpr uint32_t kStaggerPhaseMs = 977;   // bundle lamps.cpp:2008 (seq*977)%2600
			static constexpr uint32_t kTimestepMs = 8;         // Constants.h:29 TIMESTEP=8ms（原 +16 是 2 倍速）
			static constexpr int kWarmupFrames = 150;          // 1200ms/8ms（原 75 帧实为 600ms）
			static constexpr int kCoolFrames = 31;             // bundle 降 250ms/8ms
			uint32_t lit_since_ms_ = 0;   // 光敏触发时刻，用于一次性错峰计时

			struct PreviewLamp
			{
				int32_t map_id = 0;
				int32_t x = 0;
				int32_t y = 0;
				uint8_t lamp_id = 0;
			};
			std::vector<PreviewLamp> previews_;
		};
	}
}
