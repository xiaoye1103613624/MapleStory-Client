//////////////////////////////////////////////////////////////////////////////////
// 天气渲染实现：昼夜曲线逐字移植自 bundle weather.cpp:363-431（与服务端
// WeatherService.nightLevel() 成对，两端必须一致）；叠加走 GraphicsGL::drawscreenfill。
//////////////////////////////////////////////////////////////////////////////////
// 2026-11-20 修复编译破损：WeatherFx.h 的 nl::node 声明需要 USE_NX + nlnx。
// 必须先包含 MapleStory.h（它 #define USE_NX 并带入 nlnx），否则 WeatherFx.h
// 首次被包含时 USE_NX 未定义/或宏重定义顺序错乱 → "nl 不是类或命名空间"。
#include "../MapleStory.h"

#include "WeatherFx.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Graphics/GraphicsGL.h"
#include "../Constants.h"

#include "../Graphics/Texture.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include <nlnx/wz.hpp>
#endif

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <iterator>

#include "../Util/Resource/ResourceService.h"

#include <cmath>

namespace ms
{
	namespace weather
	{

		// 2026-11-23 用户拍板：夜色时段改为黄昏 15:00→17:00 由浅变深、
		// 黎明 04:00→06:00 由深变浅，更像现实天黑节奏。smoothstep 曲线不变、
		// 最黑强度不变（仍由 tint ramp + boost 决定，见 :136-149）。
		// 三端不变量：必须与 WeatherBackLayers.cpp:62-63 及服务端
		// BeiDou-Server_s9/WeatherService.java:290-293 逐字一致；服务端需重启生效。
		static constexpr int DAWN_START = 4 * 60;
		static constexpr int DAWN_END = 6 * 60;
		static constexpr int DUSK_START = 15 * 60;
		static constexpr int DUSK_END = 17 * 60;

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

		// smoothstep(t)=t²(3-2t)：两端导数 0、中段陡，取代线性，使黄昏/黎明暗变化
		// 更接近真实太阳高度角。必须与服务端 WeatherService.nightLevel() 逐字同步
		// （端到端不变量：包里只发分钟，两端各自用同一函数算 night level）。
		static float smoothstep(float t)
		{
			const float c = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
			return c * c * (3.0f - 2.0f * c);
		}

		static float night_from_minute(float minute)
		{
			if (minute < DAWN_START || minute >= DUSK_END)
				return 1.0f;
			if (minute < DAWN_END)
			{
				const float t = (minute - DAWN_START) / static_cast<float>(DAWN_END - DAWN_START);
				return 1.0f - smoothstep(t);   // 夜→昼
			}
			if (minute < DUSK_START)
				return 0.0f;
			const float t = (minute - DUSK_START) / static_cast<float>(DUSK_END - DUSK_START);
			return smoothstep(t);             // 昼→夜
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
			if (h % 10u == 0u) return 0.0f;    // 静默窗（10%，原 20% 太稀疏）
			const uint32_t within = age_ms % window_ms;
			// 2026-11-23：闪电可见时长 0.9s→1.5s，让有雨天更容易看到。
			const uint32_t earliest = 500u, pulse = 1500u;
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
			// 2026-11-22 修复色相瞬跳：原 night_mul_* 用 target 直算、无平滑，
			// draw() 却只用平滑的 night_level 作强度 → 0x373D 到达瞬间色相瞬跳。
			// 改为：先平滑 tr/tg/tb（bundle weather.cpp:168-175 fade 追赶），
			// 再用平滑值算乘色系数。强度仍由 night_level_ 平滑驱动。
			const bool snap = (st.flags() & FLAG_SNAP) != 0;
			const float step = snap ? 1.0f : (prof.fade_step > 0.0f ? prof.fade_step : 0.015f);
			tint_r_ = step_toward(tint_r_, tr, step);
			tint_g_ = step_toward(tint_g_, tg, step);
			tint_b_ = step_toward(tint_b_, tb, step);
			night_level_ = step_toward(night_level_, target_night, step);

			const float smooth_r = tint_r_;
			const float smooth_g = tint_g_;
			const float smooth_b = tint_b_;
			night_mul_r_ = base_r * dim + (smooth_r - night_level_ * (base_r * dim));
			night_mul_g_ = base_g * dim + (smooth_g - night_level_ * (base_g * dim));
			night_mul_b_ = base_b * dim + (smooth_b - night_level_ * (base_b * dim));

			// 2026-11-23：闪电触发放宽到"任意有雨天"（RAIN 或 STORM）。
			// 原 STORM-only + rain>=0.70 与 STORM 实际 rain=0.38 不匹配 → 从不触发。
			const bool has_rain = (st.sky() == Sky::RAIN || st.sky() == Sky::STORM);
			if (has_rain && prof.rain >= 0.20f && st.sky_elapsed_ms() > 0)
				lightning_ = lightning_pulse(static_cast<uint32_t>(st.sky_elapsed_ms()), st.sky_token());
			else
				lightning_ = 0.0f;
		}

	void Fx::draw() const
	{
		const State& st = State::get();
		// 室内无夜色；水下允许 tint（allow_tint && !allow_falling）。
		if (!st.has_state() || !st.allow_tint())
			return;

		// 2026-11-21 夜间逻辑根治：原生 bundle 是「逐层 multiply 乘色」——把场景 RGB
		// 按夜色调色板逐像素压暗（weather.cpp:908-920 Argb / 939-941 SceneryColor），
		// 2026-11-23 精确复刻：原生 weather.cpp:2043-2052 ApplySceneryNow 只对
		// tiles/objs/backs（全强度）与 NPC（0.45）逐层乘色，玩家角色/UI 不压暗。
		// 不再全屏 multiply（那会把角色和前景一起压黑，与原生相反）。
		// 分层 tint 由 Stage::draw 通过 GraphicsGL::push_tint(scene_tint()) 实现。
	}

	Color Fx::scene_tint() const
	{
		const float strength = night_level_;
		const float r = 1.0f + (night_mul_r_ - 1.0f) * strength;
		const float g = 1.0f + (night_mul_g_ - 1.0f) * strength;
		const float b = 1.0f + (night_mul_b_ - 1.0f) * strength;
		return Color(r, g, b, 1.0f);
	}


	// 2026-11-23 用户拍板：删除玩家/NPC 光晕（不压暗后太亮），只保留月光光晕。
	// 光晕强度随夜色深度（night_level）从淡到亮、天亮时归零，与夜景深入同步。
	void Fx::draw_lights() const
	{
		const State& st = State::get();
		if (!st.has_state() || !st.allow_tint())
			return;
		const float night = night_level_;
		if (night <= 0.01f)
			return;

		const int16_t vw = Constants::Constants::get().get_viewwidth();
		const int16_t vh = Constants::Constants::get().get_viewheight();
		GraphicsGL& g = GraphicsGL::get();

		// 月光光晕：月盘屏幕坐标 (346+vw/2, -275+vh/2)（与 WeatherBackLayers kSkyDefs 同源）。
		// 强度 ∝ night：黄昏淡入、深夜最亮、黎明淡出，与夜景深入同步。
		{
			const int16_t mx = static_cast<int16_t>(346 + vw / 2);
			const int16_t my = static_cast<int16_t>(-275 + vh / 2);
			const int16_t mr = static_cast<int16_t>(300 * night);
			g.draw_glow(mx, my, mr, 0.86f, 0.90f, 1.0f, 0.42f * night);
		}
	}

		// 天气粒子：使用 live 真实美术 Effect/WeatherParticles.img
		// （snowFall / blizzardSnow / leafMix / blossomMix / sandGrit；节点与画布尺寸经 _wz 实测：
		//  5x9 / 4x9 / 19x23 / 13x13 / 6x5）。参考实现 weatherfx.cpp:23-43 用"原生粒子 +
		//  密度区分天气"；雨丝无对应美术，仍用自绘细线。
		// Effect 根只绑了 17 项（实测 rootChildren=17、首项 BasicEff.img），
		// WeatherParticles.img 不在其中 → 必须单独加载这一张（用户要求"用客户端资源"）。
		// 与 ImgSource::load_standard 同后端：nl::img_file::open_single + root()。
		nl::node effect_img(const char* filename)
		{
			static std::map<std::string, nl::node> cache;
			static std::map<std::string, std::unique_ptr<nl::img_file>> files;

			auto it = cache.find(filename);
			if (it != cache.end())
				return it->second;

			nl::node root;
			const std::string& base = res::ResourceService::get().base();
			std::string path = base + "/Effect/" + filename;
			std::ifstream in(path, std::ios::binary);
			if (in)
			{
				std::vector<char> buf((std::istreambuf_iterator<char>(in)),
					std::istreambuf_iterator<char>());
				try
				{
					auto f = std::make_unique<nl::img_file>();
					f->open_single(std::move(buf));
					root = f->root();
					files.emplace(filename, std::move(f));
				}
				catch (...)
				{
					root = nl::node();
				}
			}
			cache.emplace(filename, root);
			return root;
			}

			// 2026-11-23 闪电：Direction3.img/effect/skill/thunder/0/0..14（15 帧 delay=70ms）。
		const std::vector<Texture>& thunder_frames()
		{
			static std::vector<Texture> frames;
			static bool tried = false;
			if (!tried)
			{
				tried = true;
				nl::node root = effect_img("Direction3.img");
				nl::node seq = root["effect"]["skill"]["thunder"]["0"];
				if (seq)
				{
					for (int i = 0; i < 32; i++)
					{
						nl::node f = seq[std::to_string(i)];
						if (!f) break;
						if (f.data_type() == nl::node::type::bitmap)
							frames.emplace_back(f);
					}
				}
			}
			return frames;
		}

		nl::node sound_img(const char* filename)
			{
			static std::map<std::string, nl::node> cache;
			static std::map<std::string, std::unique_ptr<nl::img_file>> files;

			auto it = cache.find(filename);
			if (it != cache.end())
				return it->second;

			nl::node root;
			const std::string& base = res::ResourceService::get().base();
			std::string path = base + "/Sound/" + filename;
			std::ifstream in(path, std::ios::binary);
			if (in)
			{
				std::vector<char> buf((std::istreambuf_iterator<char>(in)),
					std::istreambuf_iterator<char>());
				try
				{
					auto f = std::make_unique<nl::img_file>();
					f->open_single(std::move(buf));
					root = f->root();
					files.emplace(filename, std::move(f));
				}
				catch (...)
				{
					root = nl::node();
				}
			}
			cache.emplace(filename, root);
			return root;
			}

			static const std::vector<Texture>& particle_frames(const char* name)
		{
			static std::map<std::string, std::vector<Texture>> cache;
			auto it = cache.find(name);
			if (it != cache.end())
				return it->second;

			std::vector<Texture> frames;
#ifdef USE_NX
			nl::node root = effect_img("WeatherParticles.img");
			if (!root)
			{
				// 退路：若 Effect 根将来包含该 img，则直接用已绑定的根。
				nl::node eff = nl::nx::Effect["WeatherParticles.img"];
				if (!eff)
					eff = nl::nx::Effect["WeatherParticles"];
				root = eff;
			}
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
		// 现实化强度（0轻/1中/2强，0x3741 下发）：密度、速度、大小、alpha、风偏、
		// 雨丝长度/斜度同时变化——真实天气的强弱是多维联动，不是只改一个数。
		void Fx::draw_particles(double viewx, double viewy) const
		{
			const State& st = State::get();
			// 仅开放天空落雨雪；水下/室内绝不落粒子（对齐 g_bFallingSkyField=bSky）。
			if (!st.has_state() || !st.allow_falling())
				return;

			const Sky sky = st.sky();
			const Profile& prof = Config::get().profile(sky);
			// 强度档 → 乘数。轻档稀疏慢小、强档密集快大且风偏明显。
			const uint8_t lvl = Config::get().intensity(sky);
			const float densityMul = (lvl == 0) ? 0.35f : (lvl == 2 ? 1.60f : 1.0f);
			const float speedMul   = (lvl == 0) ? 0.70f : (lvl == 2 ? 1.35f : 1.0f);
			const float scaleMul   = (lvl == 0) ? 0.85f : (lvl == 2 ? 1.15f : 1.0f);
			const float alphaMul   = (lvl == 0) ? 0.70f : (lvl == 2 ? 1.0f : 0.90f);
			const float driftMul   = (lvl == 0) ? 0.55f : (lvl == 2 ? 1.40f : 1.0f);

			const bool rain = (sky == Sky::RAIN || sky == Sky::STORM);
			if (!rain)
			{
				const char* group = nullptr;
				int count = 0;
				int speed = 0;       // px/s
				float drift = 0.0f;
				float scale = 2.0f;   // 真实美术很小(5x9/6x5)，按 2x 放大才看清
				switch (sky)
				{
				case Sky::SNOW:      group = "snowFall";     count = static_cast<int>(220 * (0.55f + 0.45f * prof.cloud)); speed = 130; scale = 2.2f; break;
				case Sky::BLIZZARD:  group = "blizzardSnow"; count = 420; speed = 300; drift = 60.0f; scale = 2.4f; break;
				case Sky::LEAVES:    group = "leafMix";      count = 120; speed = 140; drift = 45.0f; scale = 1.6f; break;
				case Sky::BLOSSOM:   group = "blossomMix";   count = 140; speed = 110; drift = 35.0f; scale = 1.6f; break;
				case Sky::SANDSTORM: group = "sandGrit";     count = 600; speed = 320; drift = 120.0f; scale = 2.2f; break;
				default: break;
				}
				if (group == nullptr)
					return;

				// 强度档统一作用于密度/速度/尺寸/风偏（现实感来自多维联动）。
				count = static_cast<int>(count * densityMul);
				speed = static_cast<int>(speed * speedMul);
				scale *= scaleMul;
				drift *= driftMul;

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
						scale, scale, (0.80f + 0.20f * ((h2 >> 8 & 7) / 7.0f)) * alphaMul));
				}
				return;
			}

			// 雨/风暴：自绘细长雨丝。速度 380（风暴 470）px/s，比之前 700 慢约一半。
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			const uint32_t phase = static_cast<uint32_t>(st.sky_elapsed_ms());
			// 强度分级：参考实现 weather_profiles.inc 的 rain 是连续 0..1（无三级枚举），
			// 这里同样用 profile.rain 连续缩放密度与速度，而不是造假的三档。
			// 小雨(rain≈0.5)：约 55 根、速度 300；大雨(rain=1.0)：约 110~140 根、380/470。
			const float ri = prof.rain < 0.0f ? 0.0f : (prof.rain > 1.0f ? 1.0f : prof.rain);
			const int speed = static_cast<int>((sky == Sky::STORM ? 470.0f : 380.0f) * (0.78f + 0.22f * ri) * speedMul);
			const int count = static_cast<int>(((sky == Sky::STORM ? 140.0f : 110.0f) * (0.5f + 0.5f * ri)) * densityMul);
			const float base_alpha = (0.40f + 0.35f * ri) * alphaMul;
			// 现实感：雨丝长度随强度增加（小雨短滴、大雨长线），强档加斜度（风驱）。
			const int drop_h = static_cast<int>(14 * scaleMul) + lvl * 4;
			const int drop_w = (lvl == 2) ? 3 : 2;
			const int slant = (lvl == 2) ? 6 : (lvl == 1 ? 2 : 0);

			for (int i = 0; i < count; ++i)
			{
				const uint32_t h = lightning_hash(static_cast<uint32_t>(i) * 2654435761u);
				const uint32_t h2 = lightning_hash(h ^ 0x5bf03635u);
				const int sx = static_cast<int>(h % static_cast<uint32_t>(vw));
				const int span = vh + 32;
				const int sy = static_cast<int>((phase * static_cast<uint32_t>(speed) / 1000u + h2 % span) % span) - 16;
				// 斜雨：用两层错位小矩形近似（强档向右倾），不做旋转避免开销。
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx), static_cast<int16_t>(sy),
					static_cast<int16_t>(drop_w), static_cast<int16_t>(drop_h),
					0.74f, 0.86f, 1.0f, base_alpha);
				if (slant > 0)
					GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx + slant), static_cast<int16_t>(sy + drop_h / 2),
						static_cast<int16_t>(drop_w), static_cast<int16_t>(drop_h / 2),
						0.74f, 0.86f, 1.0f, base_alpha * 0.6f);
			}
		}

		// 闪电独立绘制：即使白天(bundle 要求 rain>=0.7 才闪，通常夜色>0)，也必须画在 tint 之后。
		void Fx::draw_lightning() const
		{
			const State& st = State::get();
			if (!st.allow_falling() || lightning_ <= 0.01f)
				return;

			// 2026-11-23：叠加 Direction3.img/effect/skill/thunder 逐帧（delay=70ms），
			// 画在天空中上部；按 lightning_ 脉冲强度淡入淡出。
			const std::vector<Texture>& frames = thunder_frames();
			if (!frames.empty())
			{
				const int16_t vw = Constants::Constants::get().get_viewwidth();
				const uint32_t phase = static_cast<uint32_t>(st.sky_elapsed_ms());
				const size_t fi = (phase / 70u) % frames.size();
				const Texture& tex = frames[fi];
				// 放大 2 倍铺在天空中上部；alpha 随脉冲。
				const float scale = 2.0f;
				const int16_t w = static_cast<int16_t>(tex.width() * scale);
				const int16_t h = static_cast<int16_t>(tex.height() * scale);
				const int16_t x = static_cast<int16_t>(vw / 2 - w / 2);
				const int16_t y = 40;
				tex.draw(DrawArgument(Point<int16_t>(x, y), Point<int16_t>(0, 0),
					Point<int16_t>(w, h), 1.0f, 1.0f, lightning_, 0.0f));
			}

			// 保留白蓝全屏脉冲（bundle 夜色释放语义）：alpha 随脉冲，上限 0.55。
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
