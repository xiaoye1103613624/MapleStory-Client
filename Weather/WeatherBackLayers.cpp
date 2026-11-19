//////////////////////////////////////////////////////////////////////////////////
// 天气天空/前景素材层实现。数值逐条锚定 bundle weatherfx.cpp:122-273。
// 坐标/平铺/滚动契约照抄 083 Background::draw（MapBackgrounds.cpp:112-165）。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherBackLayers.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Constants.h"
#include "../Gameplay/Stage.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <cmath>
#include <string>
#include <fstream>

namespace ms
{
	namespace weather
	{
		// alpha 驱动源
		enum { K_NIGHT, K_DUST, K_RAINBOW, K_CLOUD, K_SNOW, K_RAIN, K_FOG };

		// {bank, no, ani, x, y, rx, ry, type, cx, cy, kind}
		struct Def { const char* bank; int no; bool ani; int x; int y; double rx; double ry; int type; int cx; int cy; int kind; };

		// 天空类（bundle front=0；画在地图背景之前，山树盖其上）
		static const Def kSkyDefs[] = {
			// 天空填充 nightDesert back/0（bundle :122，SKY_DY=-100 折进 y=204；H+V tile）
			{ "nightDesert", 0, false, 162, 204, -10, -90, 3, 50, 50, K_NIGHT },
			// 夜空渐变 MapleMoonSky back/0（bundle :138；H tile）
			{ "MapleMoonSky", 0, false, 162, -241, -10, -15, 1, 50, 0, K_NIGHT },
			// 月+星（bundle :145-149，随时钟）
			{ "nightDesert", 3, false, 346, -275, -5, -5, 0, 0, 0, K_NIGHT },
			{ "nightDesert", 0, true, 0, 0, 0, 0, 3, 440, 360, K_NIGHT },
			{ "nightDesert", 1, true, 150, -120, 0, 0, 3, 500, 400, K_NIGHT },
			// 沙暴两行（bundle :171-174，y=90 相接不重叠）
			{ "nightDesert", 19, false, 0, -260, -6, -12, 1, 0, 0, K_DUST },
			{ "nightDesert", 19, false, 0, 90, -6, -12, 1, 0, 0, K_DUST },
			// 彩虹（bundle :186）
			{ "WeatherRainbow", 0, false, 0, -150, -5, -5, 0, 0, 0, K_RAINBOW },
			// 云×3（bundle :196-200，type4=HMOVEA 横向滚动 rx/16 每帧）
			{ "nightDesert", 8, false, -454, -340, -2, -50, 4, 0, 0, K_CLOUD },
			{ "nightDesert", 8, false, 120, -420, -3, -50, 4, 0, 0, K_CLOUD },
			{ "nightDesert", 8, false, -700, -270, -4, -50, 4, 0, 0, K_CLOUD },
			// 雪远（bundle :264，type7=VMOVEB；cx/cy 是 pitch 大于画布，防壁纸感）
			{ "WeatherSnow", 0, true, -21, -212, -20, 5, 7, 1000, 300, K_SNOW },
		};

		// 前景类（bundle front=1；画在角色之后）
		static const Def kFrontDefs[] = {
			// 雨×4（bundle :268-273，type7=VMOVEB 垂直下落）
			{ "ancientForest", 28, false, -330, -435, -100, 207, 7, 700, 400, K_RAIN },
			{ "ancientForest", 29, false, 136, -195, -100, 205, 7, 500, 500, K_RAIN },
			{ "ancientForest", 30, false, 165, -26, -100, 195, 7, 750, 450, K_RAIN },
			{ "ancientForest", 31, false, -623, -52, -100, 200, 7, 500, 1200, K_RAIN },
			// 雾×4（bundle :235-240，type4=HMOVEA，rx 绝不为 0）
			{ "WeatherFog", 0, false, 0, -40, -22, -6, 4, 0, 0, K_FOG },
			{ "WeatherFog", 1, false, 0, 90, -37, -4, 4, 0, 0, K_FOG },
			{ "WeatherFog", 2, false, 0, -10, -61, -8, 4, 0, 0, K_FOG },
			{ "WeatherFog", 3, false, 0, -150, -13, -3, 4, 0, 0, K_FOG },
			// 雪近（bundle :265）
			{ "WeatherSnow", 1, true, -270, -249, 0, 10, 7, 900, 500, K_SNOW },
		};

		static float step_toward(float cur, float target, float step)
		{
			if (cur < target) return cur + step < target ? cur + step : target;
			if (cur > target) return cur - step > target ? cur - step : target;
			return cur;
		}

		static float night_level(int minute)
		{
			if (minute < 5 * 60 || minute >= 19 * 60) return 1.0f;
			if (minute < 7 * 60) return 1.0f - (minute - 5 * 60) / 120.0f;
			if (minute < 17 * 60) return 0.0f;
			return (minute - 17 * 60) / 120.0f;
		}

		BackLayers& BackLayers::get()
		{
			static BackLayers instance;
			return instance;
		}

		void BackLayers::build(std::vector<Layer>& out, bool front)
		{
#ifdef USE_NX
			nl::node backsrc = nl::nx::Map001["Back"];
			if (!backsrc)
				backsrc = nl::nx::Map["Back"];
			if (!backsrc)
				return;

			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			const Def* defs = front ? kFrontDefs : kSkyDefs;
			const size_t count = front ? sizeof(kFrontDefs) / sizeof(Def) : sizeof(kSkyDefs) / sizeof(Def);

			for (size_t i = 0; i < count; ++i)
			{
				const Def& d = defs[i];
				nl::node sheet = backsrc[std::string(d.bank) + ".img"];
				if (!sheet)
					sheet = backsrc[d.bank];
				nl::node src = sheet[d.ani ? "ani" : "back"][d.no];
				if (!src)
					continue;

				Layer l;
				l.texture = Texture(src);
				l.base_x = static_cast<int16_t>(d.x);
				l.base_y = static_cast<int16_t>(d.y);
				l.rx = d.rx; l.ry = d.ry;
				l.type = d.type;
				l.kind = d.kind;

				// 平铺参数照 083 settype（MapBackgrounds.cpp:64-109）
				const Point<int16_t> dim = l.texture.get_dimensions();
				// bundle weatherfx.cpp:118-120：cx/cy 是注入时从美术读取的精确画布尺寸。
				// 雪/雨层的表值(1000/300 等)是 pitch 不是尺寸，必须用真实 dim 才正确平铺。
				l.cx = static_cast<int16_t>(dim.x() > 0 ? dim.x() : 1);
				l.cy = static_cast<int16_t>(dim.y() > 0 ? dim.y() : 1);
				switch (d.type)
				{
				case 1: case 4: // HTILED / HMOVEA
					l.htile = vw / l.cx + 3;
					break;
				case 2: case 5: // VTILED / VMOVEA
					l.vtile = vh / l.cy + 3;
					break;
				case 3: case 6: case 7: // TILED / HMOVEB / VMOVEB
					l.htile = vw / l.cx + 3;
					l.vtile = vh / l.cy + 3;
					break;
				}
				out.push_back(std::move(l));
			}
#endif
		}

		void BackLayers::on_map_loaded()
		{
			clear();
			if (!State::get().has_state())
				return;
			build(sky_, false);
			build(front_, true);
			active_ = !sky_.empty() || !front_.empty();
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
			const float night = night_level(st.minute_of_day());
			const float step = snap ? 1.0f : (prof.fade_step > 0.0f ? prof.fade_step : 0.015f);

			const float targets[] = {
				night,                                                   // K_NIGHT
				sky == Sky::SANDSTORM ? 1.0f : 0.0f,                     // K_DUST
				st.rainbow_secs_left() > 0 ? 0.75f : 0.0f,               // K_RAINBOW
				prof.cloud,                                              // K_CLOUD
				sky == Sky::SNOW ? 1.0f : 0.0f,                          // K_SNOW
				prof.rain,                                               // K_RAIN
				(sky == Sky::BLIZZARD || sky == Sky::STORM) ? 1.0f : 0.0f // K_FOG
			};

			auto drive = [&](std::vector<Layer>& list)
			{
				for (Layer& l : list)
				{
					const float target = targets[l.kind];
					l.current = snap ? target : step_toward(l.current, target, step);
					// HMOVE/VMOVE 滚动照 083（hspeed=rx/16 每逻辑帧）
					if (l.type == 4 || l.type == 6) l.acc_x += l.rx / 16.0;
					if (l.type == 5 || l.type == 6) l.acc_y += l.ry / 16.0;
					if (l.type == 7) { l.acc_x += l.rx / 16.0; l.acc_y += l.ry / 16.0; }
				}
			};
			drive(sky_);
			drive(front_);
		}

		void BackLayers::draw_list(const std::vector<Layer>& list, double viewx, double viewy, float alpha)
		{
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();
			const double woff = vw / 2.0;
			const double hoff = vh / 2.0;

			for (const Layer& l : list)
			{
				if (l.current <= 0.004f)
					continue;

				// 坐标照 083 Background::draw:112-136：
				// HMOVE 层用累计位移 + viewx（viewx 本身是屏幕偏移，Char.cpp:44 同口径）；
				// 静态层用视差 shift 公式。
				double x, y;
				const bool hmove = (l.type == 4 || l.type == 6 || l.type == 7);
				const bool vmove = (l.type == 5 || l.type == 6 || l.type == 7);
				if (hmove)
					x = l.base_x + l.acc_x + viewx;
				else
					x = l.base_x + l.rx * (woff - viewx) / 100.0 + woff;
				if (vmove)
					y = l.base_y + l.acc_y + viewy;
				else
					y = l.base_y + l.ry * (hoff - viewy) / 100.0 + hoff;

				// 平铺归位照 083 :138-154
				if (l.htile > 1)
				{
					while (x > 0) x -= l.cx;
					while (x < -l.cx) x += l.cx;
				}
				if (l.vtile > 1)
				{
					while (y > 0) y -= l.cy;
					while (y < -l.cy) y += l.cy;
				}

				const int16_t ix = static_cast<int16_t>(std::round(x));
				const int16_t iy = static_cast<int16_t>(std::round(y));
				const int tw = l.cx * l.htile;
				const int th = l.cy * l.vtile;
			// Texture::draw 只认 DrawArgument 的 color.alpha（DrawArgument.h:45）。
			for (int tx = 0; tx < tw; tx += l.cx)
				for (int ty = 0; ty < th; ty += l.cy)
					l.texture.draw(DrawArgument(Point<int16_t>(
						static_cast<int16_t>(ix + tx), static_cast<int16_t>(iy + ty)),
						l.current * alpha));
			}
		}

		void BackLayers::draw_sky(double viewx, double viewy, float alpha) const
		{
			if (active_)
				draw_list(sky_, viewx, viewy, alpha);
		}

		void BackLayers::draw_front(double viewx, double viewy, float alpha) const
		{
			if (active_)
				draw_list(front_, viewx, viewy, alpha);
		}

		void BackLayers::clear()
		{
			sky_.clear();
			front_.clear();
			active_ = false;
		}
	}
}
