//////////////////////////////////////////////////////////////////////////////////
// 天气渲染实现：昼夜曲线逐字移植自 bundle weather.cpp:363-431（与服务端
// WeatherService.nightLevel() 成对，两端必须一致）；叠加走 GraphicsGL::drawscreenfill。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherFx.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Graphics/GraphicsGL.h"
#include "../Constants.h"

#include "../Graphics/Texture.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <map>
#include <string>
#include <vector>

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

		// 风暴闪电：bundle weatherfx.cpp:1042-1069 的同步脉冲简化版。
		// 用服务端下发的 skyToken + skyElapsedMs 保证全客户端同步；统一 cadence，
		// 不再逐 palette 27 种 pattern（差异对观感影响小，注释记录简化）。
		static uint32_t lightning_hash(uint32_t x)
		{
			x ^= x >> 16; x *= 0x7FEB352Du;
			x ^= x >> 15; x *= 0x846CA68Bu;
			x ^= x >> 16;
			return x;
		}

		static float lightning_pulse(uint32_t age_ms, uint32_t seed)
		{
			const uint32_t window_ms = 12000u; // 12s 一个窗口
			const uint32_t window = age_ms / window_ms;
			const uint32_t h = lightning_hash(seed ^ (window * 0x9E3779B9u));
			if (h % 5u == 0u) return 0.0f;     // 静默窗
			const uint32_t within = age_ms % window_ms;
			const uint32_t earliest = 700u, pulse = 900u;
			if (within < earliest || within >= earliest + pulse) return 0.0f;
			const float t = static_cast<float>(within - earliest) / static_cast<float>(pulse);
			// 双闪 envelope：起-落-再起-落（bundle LightningEnvelope 的简化）
			const float env = std::sin(t * 3.14159265f) * (0.6f + 0.4f * ((h >> 8) & 7u) / 7.0f);
			return env > 0.0f ? env : 0.0f;
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

			// 风暴闪电（bundle :1062-1069）：仅 STORM 且雨强足够时按 token/age 同步脉冲。
			if (st.sky() == Sky::STORM && prof.rain >= 0.70f && st.sky_elapsed_ms() > 0)
				lightning_ = lightning_pulse(static_cast<uint32_t>(st.sky_elapsed_ms()), st.sky_token());
			else
				lightning_ = 0.0f;
		}

		void Fx::draw() const
		{
			const State& st = State::get();
			if (!st.has_state())
				return;

			// 全屏夜色叠加：叠加色是"盖上去的暗色"(tint_*)，透明度随夜色强度。
			// 真机 2026-11-18 灰白蒙版根因：初值 tint=白(1.0) 且 fade_step 慢，
			// 白天/过渡期用白色叠加 alpha>0 → 灰白。修法=初值改暗(Fx.h) + 双重硬门。
			const float strength = night_level_;
			if (strength <= 0.03f)   // 白天完全不画（原 0.001 太小，微小夜色也叠一层）
				return;
			// 保险：任一通道异常偏白(>0.75)说明状态污染，本帧跳过绝不画白蒙版。
			if (tint_r_ > 0.75f || tint_g_ > 0.75f || tint_b_ > 0.75f)
				return;
			const float alpha = strength * 0.55f;  // 上限 0.55，避免把场景压成纯黑
			GraphicsGL::get().drawscreenfill(tint_r_, tint_g_, tint_b_, alpha);
		}

		// 天气粒子：使用 live 真实美术 Effect/WeatherParticles.img
		// （snowFall / blizzardSnow / leafMix / blossomMix / sandGrit；节点与画布尺寸经 _wz 实测：
		//  5x9 / 4x9 / 19x23 / 13x13 / 6x5）。参考实现 weatherfx.cpp:23-43 用"原生粒子 +
		//  密度区分天气"；雨丝无对应美术，仍用自绘细线。
		static const std::vector<Texture>& particle_frames(const char* name)
		{
			static std::map<std::string, std::vector<Texture>> cache;
			auto it = cache.find(name);
			if (it != cache.end())
				return it->second;

			std::vector<Texture> frames;
#ifdef USE_NX
			nl::node root = nl::nx::Effect["WeatherParticles.img"];
			if (!root)
				root = nl::nx::Effect["WeatherParticles"];
			nl::node node = root[name];
			for (auto sub : node)
			{
				if (sub.data_type() != nl::node::type::bitmap)
					continue;
				Texture t(sub);
				if (t.is_valid())
					frames.push_back(t);
			}
#endif
			return cache.emplace(name, std::move(frames)).first->second;
		}

		// 各天气的粒子参数：groups 用真实美术；雨/风暴用自绘雨丝（无对应美术）。
		// 速度按参考实现"雪慢/暴雪快/沙暴密"的区分，并把雨速从 700 降到 380 px/s
		// （用户反馈"雨天水滴很快"）。
		void Fx::draw_particles(double viewx, double viewy) const
		{
			const State& st = State::get();
			if (!st.has_state())
				return;

			const Sky sky = st.sky();
			const Profile& prof = Config::get().profile(sky);

			const bool rain = (sky == Sky::RAIN || sky == Sky::STORM);
			if (!rain)
			{
				const char* group = nullptr;
				int count = 0;
				int speed = 0;       // px/s
				float drift = 0.0f;
				switch (sky)
				{
				case Sky::SNOW:      group = "snowFall";     count = 120; speed = 130; break;
				case Sky::BLIZZARD:  group = "blizzardSnow"; count = 260; speed = 300; drift = 60.0f; break;
				case Sky::LEAVES:    group = "leafMix";      count = 64;  speed = 140; drift = 45.0f; break;
				case Sky::BLOSSOM:   group = "blossomMix";   count = 64;  speed = 110; drift = 35.0f; break;
				case Sky::SANDSTORM: group = "sandGrit";     count = 220; speed = 320; drift = 120.0f; break;
				default: break;
				}
				if (group == nullptr)
					return;

				const std::vector<Texture>& frames = particle_frames(group);
				if (frames.empty())
					return;

				const int16_t vw = Constants::Constants::get().get_viewwidth();
				const int16_t vh = Constants::Constants::get().get_viewheight();
				const uint32_t phase = static_cast<uint32_t>(st.sky_elapsed_ms());

				for (int i = 0; i < count; ++i)
				{
					const uint32_t h = lightning_hash(static_cast<uint32_t>(i) * 2246822519u + 0x9E3779B9u);
					const uint32_t h2 = lightning_hash(h ^ 0x85EBCA6Bu);
					const uint32_t h3 = lightning_hash(h2 ^ 0xC2B2AE35u);

					const Texture& tex = frames[h % frames.size()];
					const int sw = tex.width();
					const int sh = tex.height();
					const int span = vh + sh * 2 + 40;
					const int fall = static_cast<int>((phase * static_cast<uint32_t>(speed) / 1000u + h2 % span) % span) - sh - 20;

					const float sway = drift > 0.0f
						? std::sin((phase + h3 % 2000) / 700.0f) * drift
						: 0.0f;
					const int x = static_cast<int>((h3 % static_cast<uint32_t>(vw)) + sway);
					const int px = x < 0 ? x + vw : (x >= vw ? x - vw : x);

					tex.draw(DrawArgument(Point<int16_t>(static_cast<int16_t>(px), static_cast<int16_t>(fall)),
						0.55f + 0.35f * ((h2 >> 8 & 7) / 7.0f)));
				}
				return;
			}

			// 雨/风暴：自绘细长雨丝。速度 380（风暴 470）px/s，比之前 700 慢约一半。
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			const uint32_t phase = static_cast<uint32_t>(st.sky_elapsed_ms());
			const int speed = (sky == Sky::STORM) ? 470 : 380;
			const int count = (sky == Sky::STORM) ? 96 : 72;
			const float base_alpha = 0.22f + 0.34f * prof.rain;

			for (int i = 0; i < count; ++i)
			{
				const uint32_t h = lightning_hash(static_cast<uint32_t>(i) * 2654435761u);
				const uint32_t h2 = lightning_hash(h ^ 0x5bf03635u);
				const int sx = static_cast<int>(h % static_cast<uint32_t>(vw));
				const int span = vh + 32;
				const int sy = static_cast<int>((phase * static_cast<uint32_t>(speed) / 1000u + h2 % span) % span) - 16;
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx), static_cast<int16_t>(sy),
					1, 12, 0.72f, 0.84f, 1.0f, base_alpha);
			}
		}

		// 闪电独立绘制：即使白天(bundle 要求 rain>=0.7 才闪，通常夜色>0)，也必须画在 tint 之后。
		void Fx::draw_lightning() const
		{
			if (lightning_ <= 0.01f)
				return;
			// 白蓝闪光，alpha 随脉冲；上限 0.55 避免瞬间全白刺眼。
			GraphicsGL::get().drawscreenfill(0.85f, 0.90f, 1.0f, lightning_ * 0.55f);
		}

		void Fx::reset()
		{
			night_level_ = 0.0f;
			lightning_ = 0.0f;
			// 与 Fx.h 初值一致：暗色，不是白。
			tint_r_ = 0x4A / 255.0f;
			tint_g_ = 0x5A / 255.0f;
			tint_b_ = 0x8C / 255.0f;
		}
	}
}
