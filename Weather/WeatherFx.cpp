//////////////////////////////////////////////////////////////////////////////////
// 天气渲染实现：昼夜曲线逐字移植自 bundle weather.cpp:363-431（与服务端
// WeatherService.nightLevel() 成对，两端必须一致）；叠加走 GraphicsGL::drawscreenfill。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherFx.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Graphics/GraphicsGL.h"

#include <cmath>

namespace ms
{
	namespace weather
	{
		// 与 bundle weather.cpp:363-366 一致。
		static constexpr int DAWN_START = 5 * 60;
		static constexpr int DAWN_END = 7 * 60;
		static constexpr int DUSK_START = 17 * 60;
		static constexpr int DUSK_END = 19 * 60;

		// golden hour（bundle weather.cpp:377-383）：黎明偏粉、黄昏偏金。
		static constexpr float DAWN_R = 0xF2f, DAWN_G = 0x9Ef, DAWN_B = 0xB8f;
		static constexpr float DUSK_R = 0xFFf, DUSK_G = 0xA2f, DUSK_B = 0x48f;
		static constexpr float GOLDEN_STRENGTH = 0.85f;

		static float golden_factor(float minute, int start, int end)
		{
			if (minute < start || minute >= end)
				return 0.0f;
			const float t = (minute - static_cast<float>(start)) / static_cast<float>(end - start);
			return std::sin(t * 3.14159265f);
		}

		static float night_from_minute(float minute)
		{
			if (minute < DAWN_START || minute >= DUSK_END)
				return 1.0f;
			if (minute < DAWN_END)
				return 1.0f - (minute - DAWN_START) / static_cast<float>(DAWN_END - DAWN_START);
			if (minute < DUSK_START)
				return 0.0f;
			return (minute - DUSK_START) / static_cast<float>(DUSK_END - DUSK_START);
		}

		static float step_toward(float current, float target, float step)
		{
			if (current < target)
				return current + step < target ? current + step : target;
			if (current > target)
				return current - step > target ? current - step : target;
			return current;
		}

		Fx& Fx::get()
		{
			static Fx instance;
			return instance;
		}

		void Fx::update(int64_t dt_ms)
		{
			State& st = State::get();
			if (!st.has_state())
			{
				reset();
				return;
			}

			st.tick(dt_ms);

			const float minute = static_cast<float>(st.minute_of_day());
			const float target_night = night_from_minute(minute);

			// 区域夜色：先取该 region 的 night RGB，再按本 sky 的 boost 压暗。
			const TintRamp& ramp = Config::get().tint_ramp(st.palette_id());
			const Profile& prof = Config::get().profile(st.sky());
			// 通道归一到 0..1（字节夜色越暗值越小）；boost 越大越暗。
			const float base_r = static_cast<float>(ramp.night_r) / 255.0f;
			const float base_g = static_cast<float>(ramp.night_g) / 255.0f;
			const float base_b = static_cast<float>(ramp.night_b) / 255.0f;
			const float dim = 1.0f - prof.boost;

			float tr = target_night * (base_r * dim);
			float tg = target_night * (base_g * dim);
			float tb = target_night * (base_b * dim);

			// golden hour：与 bundle 同口径，boost 大时压掉暖色（暴风雨没有金色黄昏）。
			const float fd = golden_factor(minute, DAWN_START, DAWN_END);
			const float fs = golden_factor(minute, DUSK_START, DUSK_END);
			const float k = (fd > fs ? fd : fs);
			if (k > 0.0f)
			{
				float clear = 1.0f - prof.boost * 2.0f;
				if (clear < 0.0f) clear = 0.0f;
				const float warm = k * GOLDEN_STRENGTH * clear;
				const float wr = (fd > fs ? DAWN_R : DUSK_R) / 255.0f;
				const float wg = (fd > fs ? DAWN_G : DUSK_G) / 255.0f;
				const float wb = (fd > fs ? DAWN_B : DUSK_B) / 255.0f;
				tr += (wr - tr) * warm;
				tg += (wg - tg) * warm;
				tb += (wb - tb) * warm;
			}

			// snap（进图）直接到位；否则按 profile.fade_step 趋近。
			const bool snap = (st.flags() & FLAG_SNAP) != 0;
			const float step = snap ? 1.0f : (prof.fade_step > 0.0f ? prof.fade_step : 0.015f);
			tint_r_ = step_toward(tint_r_, tr, step);
			tint_g_ = step_toward(tint_g_, tg, step);
			tint_b_ = step_toward(tint_b_, tb, step);
			night_level_ = step_toward(night_level_, target_night, step);
		}

		void Fx::draw() const
		{
			const State& st = State::get();
			if (!st.has_state())
				return;

			// 全屏夜色叠加：目标色是 tint_r/g/b，透明度随夜色强度。
			// 白天（night≈0）叠加接近 0，观感 stock；深夜叠加深蓝半透明层。
			const float strength = night_level_;
			if (strength <= 0.001f)
				return;
			const float alpha = strength * 0.55f;  // 上限 0.55，避免把场景压成纯黑
			GraphicsGL::get().drawscreenfill(tint_r_, tint_g_, tint_b_, alpha);
		}

		void Fx::reset()
		{
			night_level_ = 0.0f;
			tint_r_ = tint_g_ = tint_b_ = 1.0f;
		}
	}
}
