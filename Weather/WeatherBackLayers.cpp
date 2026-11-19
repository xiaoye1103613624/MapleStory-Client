//////////////////////////////////////////////////////////////////////////////////
// 天气背景注入层实现。层数值逐条锚定 bundle weatherfx.cpp:122-273。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherBackLayers.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Gameplay/Stage.h"

#include <cmath>

namespace ms
{
	namespace weather
	{
		BackLayers& BackLayers::get()
		{
			static BackLayers instance;
			return instance;
		}

		// 层索引（与 g_backSpecs 顺序一致）。
		enum Layer : size_t
		{
			L_SKYFILL = 0,   // 天空填充 nightDesert back/0
			L_GRADIENT,      // 夜空渐变 MapleMoonSky back/0
			L_MOON,          // 月 nightDesert back/3
			L_STAR0,         // 星 nightDesert ani/0
			L_STAR1,         // 星 nightDesert ani/1
			L_DUST0,         // 沙暴 nightDesert back/19 行1
			L_DUST1,         // 沙暴 行2
			L_RAINBOW,       // 彩虹 WeatherRainbow back/0
			L_CLOUD0,        // 云 nightDesert back/8 ×3
			L_CLOUD1,
			L_CLOUD2,
			L_SNOWFAR,       // 雪远 WeatherSnow back/0（背景）
			L_BACK_COUNT,
			// --- 前景 ---
			L_RAIN0 = 0,     // 雨 ancientForest back/28-31 ×4
			L_RAIN1,
			L_RAIN2,
			L_RAIN3,
			L_FOG0,          // 雾 WeatherFog back/0-3 ×4
			L_FOG1,
			L_FOG2,
			L_FOG3,
			L_SNOWNEAR,      // 雪近 WeatherSnow back/1（前景）
			L_FRONT_COUNT
		};

		// {bs, no, ani, x, y, rx, ry, type, cx, cy, a, front}
		static const Background::Params kBack[] = {
			// 天空填充（bundle :122，SKY_DY=-100 已折进 y=204）
			{ "nightDesert", 0, false, 162, 204, -10, -90, 3, 50, 50, 0, false },
			// 夜空渐变（bundle :138，MapleMoonSky back/0）
			{ "MapleMoonSky", 0, false, 162, -241, -10, -15, 1, 50, 0, 0, false },
			// 月 + 星（bundle :145-149）
			{ "nightDesert", 3, false, 346, -275, -5, -5, 0, 0, 0, 0, false },
			{ "nightDesert", 0, true, 0, 0, 0, 0, 3, 440, 360, 0, false },
			{ "nightDesert", 1, true, 150, -120, 0, 0, 3, 500, 400, 0, false },
			// 沙暴两行（bundle :171-174，y=90 使两行相接不重叠）
			{ "nightDesert", 19, false, 0, -260, -6, -12, 1, 0, 0, 0, false },
			{ "nightDesert", 19, false, 0, 90, -6, -12, 1, 0, 0, 0, false },
			// 彩虹（bundle :186）
			{ "WeatherRainbow", 0, false, 0, -150, -5, -5, 0, 0, 0, 0, false },
			// 云 ×3（bundle :196-200，rx 是滚动速率，负=右移）
			{ "nightDesert", 8, false, -454, -340, -2, -50, 4, 0, 0, 0, false },
			{ "nightDesert", 8, false, 120, -420, -3, -50, 4, 0, 0, 0, false },
			{ "nightDesert", 8, false, -700, -270, -4, -50, 4, 0, 0, 0, false },
			// 雪远（bundle :264，cx/cy 是 pitch 不是画布尺寸，禁止自动填充）
			{ "WeatherSnow", 0, true, -21, -212, -20, 5, 7, 1000, 300, 0, false },
		};

		static const Background::Params kFront[] = {
			// 雨 ×4（bundle :268-273，type 7 垂直下落）
			{ "ancientForest", 28, false, -330, -435, -100, 207, 7, 700, 400, 0, true },
			{ "ancientForest", 29, false, 136, -195, -100, 205, 7, 500, 500, 0, true },
			{ "ancientForest", 30, false, 165, -26, -100, 195, 7, 750, 450, 0, true },
			{ "ancientForest", 31, false, -623, -52, -100, 200, 7, 500, 1200, 0, true },
			// 雾 ×4（bundle :235-240，rx 绝不为 0，否则除零）
			{ "WeatherFog", 0, false, 0, -40, -22, -6, 4, 0, 0, 0, true },
			{ "WeatherFog", 1, false, 0, 90, -37, -4, 4, 0, 0, 0, true },
			{ "WeatherFog", 2, false, 0, -10, -61, -8, 4, 0, 0, 0, true },
			{ "WeatherFog", 3, false, 0, -150, -13, -3, 4, 0, 0, 0, true },
			// 雪近（bundle :265）
			{ "WeatherSnow", 1, true, -270, -249, 0, 10, 7, 900, 500, 0, true },
		};

		static float step_toward(float cur, float target, float step)
		{
			if (cur < target) return cur + step < target ? cur + step : target;
			if (cur > target) return cur - step > target ? cur - step : target;
			return cur;
		}

		static float night_level(int minute)
		{
			// 与 WeatherFx.cpp 同曲线（DAWN 5-7, DUSK 17-19）。
			if (minute < 5 * 60 || minute >= 19 * 60) return 1.0f;
			if (minute < 7 * 60) return 1.0f - (minute - 5 * 60) / 120.0f;
			if (minute < 17 * 60) return 0.0f;
			return (minute - 17 * 60) / 120.0f;
		}

		void BackLayers::on_map_loaded()
		{
			const State& st = State::get();
			// 保守判定：D1 无 WZ bS/no 户外白名单，暂以"收到世界状态"作为户外近似。
			// 室内误注入的风险由后续 skytable 精确判定收敛（见 notes）。
			if (!st.has_state())
			{
				clear();
				return;
			}
			Stage::get().get_backgrounds().set_injected(
				std::vector<Background::Params>(kBack, kBack + L_BACK_COUNT),
				std::vector<Background::Params>(kFront, kFront + L_FRONT_COUNT));
			active_ = true;
		}

		void BackLayers::update(bool snap)
		{
			const State& st = State::get();
			// 惰性灌入：0x373D 世界状态包常在 Stage::load_map 之后才到（bundle 同款时序），
			// 收到 state 且当前未激活时在此补灌，避免进图早于包导致注入层永远缺失。
			if (!active_)
			{
				if (st.has_state())
					on_map_loaded();
				else
					return;
			}
			if (!st.has_state())
			{
				clear();
				return;
			}
			const Sky sky = st.sky();
			const Profile& prof = Config::get().profile(sky);
			const float night = night_level(st.minute_of_day());
			const float step = snap ? 1.0f : (prof.fade_step > 0.0f ? prof.fade_step : 0.015f);

			// 目标 alpha（0..1），bundle ApplyBackFade :1092-1118 口径。
			const float aSkyFill = night;                              // 天空填充随夜色
			const float aGradient = night;                             // 夜空渐变随夜色
			const float aNight = night;                                // 月/星随时钟
			const float aDust = (sky == Sky::SANDSTORM) ? 1.0f : 0.0f; // 沙暴
			const float aRainbow = (st.rainbow_secs_left() > 0) ? 0.75f : 0.0f;
			const float aCloud = prof.cloud;                           // 云随 profile
			const float aSnow = (sky == Sky::SNOW) ? 1.0f : 0.0f;
			const float aRain = prof.rain;                             // 雨随 profile
			const float aFog = (sky == Sky::BLIZZARD || sky == Sky::STORM) ? 1.0f : 0.0f;

			auto& bg = Stage::get().get_backgrounds();
			const float backTargets[] = {
				aSkyFill, aGradient, aNight, aNight, aNight, aDust, aDust,
				aRainbow, aCloud, aCloud, aCloud, aSnow
			};
			for (size_t i = 0; i < L_BACK_COUNT; ++i)
			{
				const float cur = snap ? 0.0f : bg.get_injected_opacity(false, i);
				bg.set_injected_opacity(false, i, step_toward(cur, backTargets[i], step));
			}

			const float frontTargets[] = {
				aRain, aRain, aRain, aRain, aFog, aFog, aFog, aFog, aSnow
			};
			for (size_t i = 0; i < L_FRONT_COUNT; ++i)
			{
				const float cur = snap ? 0.0f : bg.get_injected_opacity(true, i);
				bg.set_injected_opacity(true, i, step_toward(cur, frontTargets[i], step));
			}
		}

		void BackLayers::clear()
		{
			if (!active_)
				return;
			Stage::get().get_backgrounds().clear_injected();
			active_ = false;
		}
	}
}
