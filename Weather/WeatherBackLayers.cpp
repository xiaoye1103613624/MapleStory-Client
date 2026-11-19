//////////////////////////////////////////////////////////////////////////////////
// 天气天空/前景素材层实现。数值逐条锚定 bundle weatherfx.cpp:122-273。
// 坐标/平铺/滚动契约照抄 083 Background::draw（MapBackgrounds.cpp:112-165）。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherBackLayers.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "WeatherSkyTable.h"
#include "../Constants.h"
#include "../Gameplay/Stage.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <cmath>
#include <string>

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
			// 天空渐变填充 nightDesert back/0（bundle :122，SKY_DY=-100 折进 y=204；H+V tile）
			{ "nightDesert", 0, false, 162, 204, -10, -90, 3, 50, 50, K_NIGHT },
			// 夜空渐变 MapleMoonSky back/0（bundle :138；H tile）
			{ "MapleMoonSky", 0, false, 162, -241, -10, -15, 1, 50, 0, K_NIGHT },
			// 月（静态）。插件另有 nightDesert ani/0-1 星星闪烁 + 云/沙暴/彩虹/雨雾雪 sheet，
			// 这些在 DLL 侧依赖加色/特殊混合；原生 Sprite 直画会变成淡色矩形色块
			// （真机 2026-11-19 实测），故只保留纯渐变与静态月，其余改由自绘粒子实现。
			{ "nightDesert", 3, false, 346, -275, -5, -5, 0, 0, 0, K_NIGHT },
		};

		// 前景类（bundle front=1；画在角色之后）
		// 前景 WZ sheet 层全部停用：ancientForest 雨 / WeatherFog / WeatherSnow 的画布是
		// 半透明整幅 sheet，插件靠 DLL 特殊混合才成雨丝雪点；原生直画是可见矩形色块。
		// 雨/雪/花瓣/沙尘统一走 WeatherFx::draw_particles 自绘（真机已验证可见）。
		static const Def kFrontDefs[] = {
			// 占位（bank 为空 → build() 直接跳过），仅为避免零长数组。
			{ "", 0, false, 0, 0, 0, 0, 0, 0, 0, K_RAIN },
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
				if (d.bank == nullptr || d.bank[0] == '\0')
					continue;   // 占位行
				nl::node sheet = backsrc[std::string(d.bank) + ".img"];
				if (!sheet)
					sheet = backsrc[d.bank];
				nl::node src = sheet[d.ani ? "ani" : "back"][d.no];
				if (!src)
					continue;

				// ani 节点是多帧属性（{"0":canvas,"1":canvas,...}），不是单张 bitmap。
				// 直接 Texture(src) 会得到 0x0 无效纹理 → cx 回退 1 → htile/vtile 爆炸
				// （真机实测 1283x723 ≈ 92 万次绘制，触发 0xC0000005）。取首帧。
				if (src.data_type() != nl::node::type::bitmap)
				{
					nl::node first = src["0"];
					if (first.data_type() == nl::node::type::bitmap)
						src = first;
					else
						continue;
				}

				Layer l;
				l.texture = Texture(src);
				if (!l.texture.is_valid())
					continue;   // 保险：无效纹理绝不入层（否则 draw 段直接 AV）
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
				// 封顶：单层最多 64x64 块，防止异常尺寸导致百万级绘制循环
				if (l.htile > 64) l.htile = 64;
				if (l.vtile > 64) l.vtile = 64;
				if (l.htile < 1) l.htile = 1;
				if (l.vtile < 1) l.vtile = 1;
				out.push_back(std::move(l));
			}
#endif
		}

		void BackLayers::on_map_loaded()
		{
			on_map_loaded(std::string(), -1);
		}

		void BackLayers::on_map_loaded(const std::string& bs, int32_t no)
		{
			clear();
			// 天空门禁：对齐插件 FieldHasSky（weather.cpp:2073-2100）。
			// 不在天空表内的图（室内/树洞/Kerning Square）不注入任何素材，避免盖住原生背景。
			if (!is_sky_base(bs.c_str(), no))
				return;
			build(sky_, false);
			build(front_, true);
			active_ = !sky_.empty() || !front_.empty();
		}

		void BackLayers::update(bool snap, uint16_t timestep)
		{
			if (!active_)
				return;   // 装图时已按天空门禁判定；未通过（室内图）就不需要每帧重建
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

			int _li = 0;
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
