//////////////////////////////////////////////////////////////////////////////////
// 天气前景素材层实现（独立自绘，不改 MapBackgrounds / 083 背景逻辑）。
// 素材数值锚定 bundle weatherfx.cpp:235-273（雨/雾/雪），仅取 front=1 的层。
//////////////////////////////////////////////////////////////////////////////////
#include "../MapleStory.h"  // defines USE_NX (for nl::nx in on_map_loaded)
#include "WeatherBackLayers.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Constants.h"
#include "../Gameplay/Stage.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

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

		// 前景素材定义：{bank, no, ani, x, y, rx, ry, kind}
		// 只保留原本 front=1 的层：雨(ancientForest back/28-31)、雾(WeatherFog 0-3)、雪近(WeatherSnow/1)。
		struct Def { const char* bank; int no; bool ani; int x; int y; double rx; double ry; int kind; };
		enum { K_RAIN, K_FOG, K_SNOW };
		static const Def kDefs[] = {
			{ "ancientForest", 28, false, -330, -435, -100, 207, K_RAIN },
			{ "ancientForest", 29, false,  136, -195, -100, 205, K_RAIN },
			{ "ancientForest", 30, false,  165,  -26, -100, 195, K_RAIN },
			{ "ancientForest", 31, false, -623,  -52, -100, 200, K_RAIN },
			{ "WeatherFog",     0, false,    0,  -40,  -22,  -6, K_FOG  },
			{ "WeatherFog",     1, false,    0,   90,  -37,  -4, K_FOG  },
			{ "WeatherFog",     2, false,    0,  -10,  -61,  -8, K_FOG  },
			{ "WeatherFog",     3, false,    0, -150,  -13,  -3, K_FOG  },
			{ "WeatherSnow",    1, true,  -270, -249,    0,  10, K_SNOW },
		};

		static float step_toward(float cur, float target, float step)
		{
			if (cur < target) return cur + step < target ? cur + step : target;
			if (cur > target) return cur - step > target ? cur - step : target;
			return cur;
		}

		void BackLayers::on_map_loaded()
		{
			clear();
			const State& st = State::get();
			if (!st.has_state())
				return;  // 服务端未发言 → stock，不画素材

			nl::node backsrc = nl::nx::Map001["Back"];
			if (!backsrc)
				backsrc = nl::nx::Map["Back"];
			if (!backsrc)
				return;

			layers_.reserve(sizeof(kDefs) / sizeof(kDefs[0]));
			for (const Def& d : kDefs)
			{
				nl::node sheet = backsrc[std::string(d.bank) + ".img"];
				if (!sheet)
					sheet = backsrc[d.bank];
				nl::node src = sheet[d.ani ? "ani" : "back"][d.no];
				if (!src)
					continue;
				Layer layer;
				layer.anim = Animation(src);
				layer.x = d.x; layer.y = d.y;
				layer.rx = d.rx; layer.ry = d.ry;
				layer.rain = (d.kind == K_RAIN);
				layer.fog = (d.kind == K_FOG);
				layer.snow = (d.kind == K_SNOW);
				layers_.push_back(std::move(layer));
			}
			active_ = !layers_.empty();
		}

		void BackLayers::update(bool snap, uint16_t timestep)
		{
			if (!active_)
			{
				if (State::get().has_state())
					on_map_loaded();
				else
					return;
			}
			const State& st = State::get();
			if (!st.has_state())
			{
				clear();
				return;
			}
			const Sky sky = st.sky();
			const Profile& prof = Config::get().profile(sky);
			const float step = snap ? 1.0f : (prof.fade_step > 0.0f ? prof.fade_step : 0.015f);
			const float aRain = prof.rain;
			const float aFog = (sky == Sky::BLIZZARD || sky == Sky::STORM) ? 1.0f : 0.0f;
			const float aSnow = (sky == Sky::SNOW) ? 1.0f : 0.0f;

			for (Layer& layer : layers_)
			{
				const float target = layer.rain ? aRain : (layer.fog ? aFog : aSnow);
				layer.current = (snap ? 0.0f : step_toward(layer.current, target, step));
				if (snap)
					layer.current = target;
				layer.anim.update(timestep);
			}
		}

		void BackLayers::draw(const DrawArgument& viewpos, double viewx, double viewy, float alpha) const
		{
			if (!active_)
				return;
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			const double woffset = vw / 2.0;
			const double hoffset = vh / 2.0;

			for (const Layer& layer : layers_)
			{
				if (layer.current <= 0.001f)
					continue;
				// 与 Background::draw 同款视差（MapBackgrounds.cpp:122-135）：
				// 非 hmobile 层用 rx 把世界 x 平移到视口中心；这里素材按世界坐标直接定位。
				const double sx = static_cast<double>(layer.x) + woffset - viewx * (1.0 + layer.rx / 100.0);
				const double sy = static_cast<double>(layer.y) + hoffset - viewy * (1.0 + layer.ry / 100.0);
				layer.anim.draw(DrawArgument(Point<int16_t>(static_cast<int16_t>(sx), static_cast<int16_t>(sy))), layer.current * alpha);
			}
		}

		void BackLayers::clear()
		{
			layers_.clear();
			active_ = false;
		}
	}
}
