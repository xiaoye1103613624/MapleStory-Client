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
#include "../Graphics/GraphicsGL.h"

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
		enum { K_NIGHT, K_DUST, K_RAINBOW, K_CLOUD, K_SNOW, K_RAIN, K_FOG, K_CELESTIAL, K_AURORA, K_TORNADO };

		// {bank, no, ani, x, y, rx, ry, type, cx, cy, kind, cover, cond_cold, cond_band}
		// cover=true：单层拉伸铺满视口（仅夜空渐变）；其余素材 false 走动态平铺/滚动。
		// cond_cold/cond_band=-1 不限；天体层按 State flags 匹配（V2 P2）。
		struct Def { const char* bank; int no; bool ani; int x; int y; double rx; double ry; int type; int cx; int cy; int kind; bool cover; int8_t cond_cold; int8_t cond_band; float night_min; float night_max; };

		// 天空类（bundle front=0；画在地图背景之前，山树盖其上）。
		// 2026-11-23 A 档：按 bundle weatherfx.cpp:644-655 补齐 星/彩虹/云/沙暴。
		static const Def kSkyDefs[] = {
			// 夜空渐变 MapleMoonSky back/0（bundle :123-138；cover 铺满）
			{ "MapleMoonSky", 0, false, 162, -241, -10, -15, 1, 50, 0, K_NIGHT, true, -1, -1, -1.0f, -1.0f },
			// 2026-11-23 V2 P2 天体（决策1按季节切换）：替代原 nightDesert 月，避免重复。
			// 太阳（白天 band=2）：blackheavensky_sunset back/25 332x332
			{ "blackheavensky_sunset", 25, false, 346, -200, -3, -3, 0, 0, 0, K_CELESTIAL, false, -1, 2, -1.0f, -1.0f },
			// 暖光柱（黄昏 band=3 且夜色浅 night<0.6）：blackmagician_base2 ani/0/0 506x608
			{ "blackmagician_base2", 0, true, 300, -260, -3, -3, 0, 0, 0, K_CELESTIAL, false, -1, 3, -1.0f, 0.6f },
			// 满月（bundle 原 nightDesert back/3，浅夜 0.6<=night<0.85，四季通用）
			{ "nightDesert", 3, false, 346, -275, -5, -5, 0, 0, 0, K_CELESTIAL, false, -1, -1, 0.6f, 0.85f },
			// 残月（冷季 深夜 night>=0.85）：blackheavensky_night back/29 219x216
			{ "blackheavensky_night", 29, false, 346, -230, -4, -4, 0, 0, 0, K_CELESTIAL, false, 1, -1, 0.85f, -1.0f },
			// 魔法月（暖季 深夜 night>=0.85）：blackmagician_base3 back/34 860x532
			{ "blackmagician_base3", 34, false, 346, -300, -4, -4, 0, 0, 0, K_CELESTIAL, false, 0, -1, 0.85f, -1.0f },
			// 2026-11-23 V2 P4 缺素材兜底：极光（bm3_1 back/6 青光带，染色放大）
			{ "bm3_1", 6, false, 0, -260, -1, 0, 4, 0, 0, K_AURORA, false, -1, -1, -1.0f, -1.0f },
			// 龙卷（bm1_3 ani/5 旋转气柱，取首帧）：element EL_TORNADO 驱动
			{ "bm1_3", 5, true, 300, -120, 0, 0, 0, 0, 0, K_TORNADO, false, -1, -1, -1.0f, -1.0f },
			// 星空 ani/0、ani/1（bundle g_aNight[1][2]，H+V 平铺 type3，取首帧无 twinkle）
			{ "nightDesert", 0, true, 0, 0, 0, 0, 3, 440, 360, K_NIGHT, false, -1, -1, -1.0f, -1.0f },
			{ "nightDesert", 1, true, 150, -120, 0, 0, 3, 500, 400, K_NIGHT, false, -1, -1, -1.0f, -1.0f },
			// 彩虹 WeatherRainbow back/0（bundle g_rainbow，雨后，画在云之前）
			{ "WeatherRainbow", 0, false, 0, -150, -5, -5, 0, 0, 0, K_RAINBOW, false, -1, -1, -1.0f, -1.0f },
			// 云 nightDesert back/8 ×3（bundle g_aClouds，type4 横向滚动，rx=-2/-3/-4）
			{ "nightDesert", 8, false, -454, -340, -2, -50, 4, 0, 0, K_CLOUD, false, -1, -1, -1.0f, -1.0f },
			{ "nightDesert", 8, false, 120, -420, -3, -50, 4, 0, 0, K_CLOUD, false, -1, -1, -1.0f, -1.0f },
			{ "nightDesert", 8, false, -700, -270, -4, -50, 4, 0, 0, K_CLOUD, false, -1, -1, -1.0f, -1.0f },
			// 沙暴天空 nightDesert back/19 ×2 行相接（bundle g_aDust，type1 H tile）
			{ "nightDesert", 19, false, 0, -260, -6, -12, 1, 0, 0, K_DUST, false, -1, -1, -1.0f, -1.0f },
			{ "nightDesert", 19, false, 0, 90, -6, -12, 1, 0, 0, K_DUST, false, -1, -1, -1.0f, -1.0f },
		};

		// 前景类（bundle front=1；画在角色之后）
		// 前景 WZ sheet 层全部停用：ancientForest 雨 / WeatherFog / WeatherSnow 的画布是
		// 半透明整幅 sheet，插件靠 DLL 特殊混合才成雨丝雪点；原生直画是可见矩形色块。
		// 雨/雪/花瓣/沙尘统一走 WeatherFx::draw_particles 自绘（真机已验证可见）。
		static const Def kFrontDefs[] = {
			// 占位（bank 为空 → build() 直接跳过），仅为避免零长数组。
			{ "", 0, false, 0, 0, 0, 0, 0, 0, 0, K_RAIN, false, -1, -1, -1.0f, -1.0f },
		};

		static float step_toward(float cur, float target, float step)
		{
			if (cur < target) return cur + step < target ? cur + step : target;
			if (cur > target) return cur - step > target ? cur - step : target;
			return cur;
		}

		// 与 WeatherFx::night_from_minute（WeatherFx.cpp:68-81）逐字同口径的 smoothstep
		// 曲线。2026-11-21 统一：此前本文件用线性版，与全屏夜色蒙版强度不同步，
		// 黄昏/黎明月亮层与画面暗度错拍。两端都走 smoothstep 后一致。
		static float night_level(int minute)
		{
			// 2026-11-23 时段调整（与 WeatherFx.cpp:41-44 及服务端 WeatherService.java:290-293
			// 三端逐字一致）：黄昏 15:00→17:00 渐暗，黎明 04:00→06:00 渐亮。
			constexpr int DAWN_START = 4 * 60, DAWN_END = 6 * 60;
			constexpr int DUSK_START = 15 * 60, DUSK_END = 17 * 60;
			const auto smooth = [](float t) -> float {
				const float c = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
				return c * c * (3.0f - 2.0f * c);
			};
			if (minute < DAWN_START || minute >= DUSK_END) return 1.0f;
			if (minute < DAWN_END)
				return 1.0f - smooth(static_cast<float>(minute - DAWN_START) / (DAWN_END - DAWN_START));
			if (minute < DUSK_START) return 0.0f;
			return smooth(static_cast<float>(minute - DUSK_START) / (DUSK_END - DUSK_START));
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

			// 平铺参数改由 draw_list 每帧按当前视口实时计算，build 期不再缓存视口快照。
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
				l.cond_cold = d.cond_cold;
				l.cond_band = d.cond_band;
				l.night_min = d.night_min;
				l.night_max = d.night_max;

				// 平铺间距照 083 settype（MapBackgrounds.cpp:64-109）
				const Point<int16_t> dim = l.texture.get_dimensions();
				// bundle weatherfx.cpp:118-120：cx/cy 是注入时从美术读取的精确画布尺寸。
				// 雪/雨层的表值(1000/300 等)是 pitch 不是尺寸，必须用真实 dim 才正确平铺。
				l.cx = static_cast<int16_t>(dim.x() > 0 ? dim.x() : 1);
				l.cy = static_cast<int16_t>(dim.y() > 0 ? dim.y() : 1);
				// 天空渐变层（front==0）：C 方案 cover —— 单层拉伸铺满视口，
				// 无论分辨率/宽高比都不露底。细节层（云/月/前景）仍走动态平铺。
				// 2026-11-23：只有夜空渐变 cover 拉伸；云/星/彩虹/沙暴/月走动态平铺，
			// 否则 800px 云拉伸到 1920 会严重变形。由 Def.cover 逐层指定。
			l.cover = d.cover;
				// htile/vtile 不再在 build 期按旧视口算死 —— draw_list 会按当前
				// Constants 实时重算（A 方案），这里只置 1 去掉构造期缓存语义。
				l.htile = 1;
				l.vtile = 1;
				out.push_back(std::move(l));
			}
#endif
		}

		void BackLayers::on_map_loaded()
		{
			on_map_loaded(-1, std::string(), -1);
		}

		void BackLayers::on_map_loaded(int32_t map_id, const std::string& bs, int32_t no)
		{
			clear();
			// 天空门禁：对齐插件 FieldHasSky（weather.cpp:859-885）——先 nosky 再 skytable。
			// 室内复用城镇天空 bank 的图必须排除，否则会往武器店/图书馆灌雨层。
			if (!field_has_sky(map_id, bs.c_str(), no))
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
				// K_FOG：原天气 + 2026-11-23 V2 0x3742 雾强度（EL_FOG/3）取大。
				(((sky == Sky::BLIZZARD || sky == Sky::STORM) ? 1.0f : 0.0f) > (st.element(Element::EL_FOG) / 3.0f)
					? ((sky == Sky::BLIZZARD || sky == Sky::STORM) ? 1.0f : 0.0f)
					: (st.element(Element::EL_FOG) / 3.0f)),
				1.0f,                                                    // K_CELESTIAL（条件在 drive 判定）
				st.element(Element::EL_AURORA) / 3.0f,                   // K_AURORA
				st.element(Element::EL_TORNADO) / 3.0f                   // K_TORNADO
			};
			// 2026-11-23 V2 P2：服务端 flags 的时段档/冷热季（决策1按季节切换天体）。
			const uint8_t band = time_band(st.flags());
			const bool cold = season_cold(st.flags());

			auto drive = [&](std::vector<Layer>& list)
			{
				for (Layer& l : list)
				{
					float target = targets[l.kind];
					// 天体层：条件匹配才显示（cond=-1 不限）。
					if (l.kind == K_CELESTIAL)
					{
						const bool cold_ok = (l.cond_cold < 0) || (l.cond_cold != 0) == cold;
						const bool band_ok = (l.cond_band < 0) || (l.cond_band == static_cast<int8_t>(band));
						const bool nmin_ok = l.night_min < 0.0f || night >= l.night_min;
						const bool nmax_ok = l.night_max < 0.0f || night <= l.night_max;
						target = (cold_ok && band_ok && nmin_ok && nmax_ok) ? 1.0f : 0.0f;
					}
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

		void BackLayers::draw_list(const std::vector<Layer>& list, double viewx, double viewy, float alpha) const
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
				const bool is_moon_layer = (l.kind == 0 && l.type == 0);
				const bool hmove = (l.type == 4 || l.type == 6 || l.type == 7);
				const bool vmove = (l.type == 5 || l.type == 6 || l.type == 7);
				if (is_moon_layer)
				{
					// 月亮固定屏幕坐标：不参与视差。走视差时相机每帧微动会让圆盘
					// 取整挪 1px，高对比月面读作"一直在闪"（真机 2026-11-20 坐实：
					// 月盘核心区两帧差异 96.35%）。参考 weatherfx.cpp:1101-1103 亦为
					// "整晚停在作者坐标"。
					x = l.base_x + woff;
					y = l.base_y + hoff;
				}
				else if (hmove)
					x = l.base_x + l.acc_x + viewx;
				else
					x = l.base_x + l.rx * (woff - viewx) / 100.0 + woff;
				if (is_moon_layer)
					;   // y 已定
				else if (vmove)
					y = l.base_y + l.acc_y + viewy;
				else
					y = l.base_y + l.ry * (hoff - viewy) / 100.0 + hoff;

				// 月亮（kind==K_NIGHT 且 type==0）：只用 nightDesert back/3 的真实月盘，不 cover。
				const bool is_moon = is_moon_layer;

				if (l.cover && !is_moon)
				{
					// C 方案：天空渐变层按当前视口 cover 拉伸铺满，任何分辨率/宽高比都不露底。
					// 渐变本身无像素网格，拉伸无损观感。
					const int16_t texw = l.texture.width();
					const int16_t texh = l.texture.height();
					if (texw > 0 && texh > 0)
					{
						const double scale = std::max(static_cast<double>(vw) / texw,
						                               static_cast<double>(vh) / texh);
						const int16_t dw = static_cast<int16_t>(std::ceil(texw * scale));
						const int16_t dh = static_cast<int16_t>(std::ceil(texh * scale));
						const Point<int16_t> origin = l.texture.get_origin();
						const int16_t dx = static_cast<int16_t>((vw - dw) / 2 + origin.x());
						const int16_t dy = static_cast<int16_t>((vh - dh) / 2 + origin.y());
						// [编译消歧义 2026-11-21] 原三参 (Point,Point,float) 无对应重载；
						// 语义=拉伸到 dw×dh + opacity，改用完整七参构造（等价 DrawArgument(position, stretch) 再带 alpha）。
						l.texture.draw(DrawArgument(Point<int16_t>(dx, dy), Point<int16_t>(dx, dy),
						                            Point<int16_t>(dw, dh), 1.0f, 1.0f,
						                            l.current, 0.0f));  // 不乘帧插值 alpha（闪烁根治 2026-11-21）
					}
					continue;
				}

				// 动态平铺块数：每帧按当前视口 ceil 计算，视口切档后下一帧即铺满（A 方案）。
				auto ceil_div = [](int32_t total, int32_t unit) -> int {
					return unit > 0 ? static_cast<int>((total + unit - 1) / unit) : 1;
				};
				int htile = 1, vtile = 1;
				switch (l.type)
				{
				case 1: case 4: // HTILED / HMOVEA
					htile = ceil_div(vw, l.cx) + 3;
					break;
				case 2: case 5: // VTILED / VMOVEA
					vtile = ceil_div(vh, l.cy) + 3;
					break;
				case 3: case 6: case 7: // TILED / HMOVEB / VMOVEB
					htile = ceil_div(vw, l.cx) + 3;
					vtile = ceil_div(vh, l.cy) + 3;
					break;
				}
				htile = htile > 64 ? 64 : (htile < 1 ? 1 : htile);
				vtile = vtile > 64 ? 64 : (vtile < 1 ? 1 : vtile);

				// 平铺归位照 083 :138-154
				if (htile > 1)
				{
					while (x > 0) x -= l.cx;
					while (x < -l.cx) x += l.cx;
				}
				if (vtile > 1)
				{
					while (y > 0) y -= l.cy;
					while (y < -l.cy) y += l.cy;
				}

				const int16_t ix = static_cast<int16_t>(std::round(x));
				const int16_t iy = static_cast<int16_t>(std::round(y));
				const int tw = l.cx * htile;
				const int th = l.cy * vtile;
			// Texture::draw 只认 DrawArgument 的 color.alpha（DrawArgument.h:45）。
			for (int tx = 0; tx < tw; tx += l.cx)
				for (int ty = 0; ty < th; ty += l.cy)
					l.texture.draw(DrawArgument(Point<int16_t>(
						static_cast<int16_t>(ix + tx), static_cast<int16_t>(iy + ty)),
						l.current));  // 不乘帧插值 alpha（闪烁根治 2026-11-21）
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
