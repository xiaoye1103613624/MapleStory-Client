//////////////////////////////////////////////////////////////////////////////////
// 天气路灯实现。手工放置表逐条抄自 bundle lamps.cpp:581-661（mapId, x, y, lampId）。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherLamps.h"

#include "../Gameplay/Stage.h"
#include "../Graphics/GraphicsGL.h"
#include "../Graphics/Animation.h"
#include "../Constants.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace weather
	{
		Lamps& Lamps::get()
		{
			static Lamps instance;
			return instance;
		}

		// bundle MapleMoonLamp.img/glow/{large,medium,small}/0 光晕 sprite（替代矩形近似）。
		static Animation& glow_anim()
		{
			static Animation anim;
			static bool loaded = false;
			if (!loaded)
			{
				loaded = true;
#ifdef USE_NX
				nl::node obj = nl::nx::Map001["Obj"];
				if (!obj)
					obj = nl::nx::Map["Obj"];
				nl::node n = obj["MapleMoonLamp.img"]["glow"]["large"]["0"];
				if (n)
					anim = Animation(n);
#endif
			}
			return anim;
		}

		struct ManualLamp { int map_id; int x; int y; int lamp_id; };

		// bundle lamps.cpp:581-661 手工放置表（40 条）。
		static const ManualLamp kManualLamps[] = {
			// Henesys (lamp 6)
			{ 100000000,  1698,   334, 6 }, { 100000000,  2699,   334, 6 },
			{ 100000000,  3733,   454, 6 }, { 100000000,  4870,   454, 6 },
			{ 100000000,  6016,   454, 6 }, { 100000000,  5713,  -176, 6 },
			{ 100000000,   351,   274, 6 },
			// Lith Harbour (lamp 0)
			{ 104000000,   223,   287, 0 }, { 104000000,  1004,   347, 0 },
			{ 104000000,   569,   707, 0 }, { 104000000,  1734,   647, 0 },
			{ 104000000,  2821,   527, 0 }, { 104000000,  2582,  -223, 0 },
			{ 104000000,  1774,  -133, 0 }, { 104000000,  3991,  -193, 0 },
			{ 104000000,  4080,   437, 0 },
			// Perion (lamp 12, desert brazier)
			{ 102000000,   565,  1875, 12 }, { 102000000,  2422,  1935, 12 },
			{ 102000000,  2186,  1239, 12 }, { 102000000,  1272,   979, 12 },
			{ 102000000,   594,   599, 12 }, { 102000000,  2635,   660, 12 },
			{ 102000000,  2393,     1, 12 },
			// Ellinia (lamp 22, glowing bloom)
			{ 101000000,  -453,   241, 22 }, { 101000000,   215,   -40, 22 },
			{ 101000000,  1196,   219, 22 }, { 101000000,   753,  -418, 22 },
			{ 101000000,   126,  -965, 22 }, { 101000000,  -785,  -806, 22 },
			{ 101000000,  -682, -1454, 22 }, { 101000000,  -359, -2062, 22 },
			{ 101000000,  1047, -1900, 22 }, { 101000000,   403, -2527, 22 },
			{ 101000000,  -246, -3140, 22 }, { 101000000,    36, -4189, 22 },
			// Kerning City (lamp 8)
			{ 103000000,   895,   426, 8 }, { 103000000,  1854,  -204, 8 },
			{ 103000000,    -3,   306, 8 }, { 103000000, -1179,   426, 8 },
			{ 103000000, -1878,     6, 8 },
		};

		void Lamps::update(float night_level)
		{
			// 黄昏/夜晚渐亮，白天熄灭； photocell 迟滞由 night_level 曲线天然提供。
			const float target = night_level > 0.15f ? (night_level - 0.15f) / 0.85f : 0.0f;
			const float step = 0.02f;
			if (level_ < target) level_ = level_ + step < target ? level_ + step : target;
			else if (level_ > target) level_ = level_ - step > target ? level_ - step : target;
		}

		void Lamps::draw(double viewx, double viewy, float alpha) const
		{
			if (level_ <= 0.01f)
				return;
			const int map_id = Stage::get().get_mapid();
			const int16_t vw = Constants::Constants::get().get_viewwidth();
			const int16_t vh = Constants::Constants::get().get_viewheight();

			for (const auto& lamp : kManualLamps)
			{
				if (lamp.map_id != map_id)
					continue;
				// 世界→屏幕照 Char.cpp:44（viewx 是相机偏移量 = vw/2 - 玩家世界x）：
				// screen = world + viewx。此前"- viewx + vw/2"符号反了导致灯画到屏外。
				const double sx = static_cast<double>(lamp.x) + viewx;
				const double sy = static_cast<double>(lamp.y) + viewy;
				if (sx < -80 || sx > vw + 80 || sy < -80 || sy > vh + 80)
					continue;
				// 灯柱实体：暖色竖条（夜里可见的发光体），画在光晕之前。
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 3, static_cast<int16_t>(sy) - 78,
					6, 76, 0.55f, 0.42f, 0.22f, level_ * alpha);
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 8, static_cast<int16_t>(sy) - 84,
					16, 10, 1.0f, 0.88f, 0.55f, level_ * 0.9f * alpha);
				// bundle 用光晕 sprite（MapleMoonLamp.img/glow/large/0）；加载失败退矩形近似。
				Animation& glow = glow_anim();
				if (glow.get_dimensions().x() > 0)
				{
					// emitter 在灯柱上部：bundle lamptable 行 emitter dy≈-60..-110，取 -80 近似。
					glow.draw(DrawArgument(Point<int16_t>(
						static_cast<int16_t>(sx), static_cast<int16_t>(sy) - 80)),
						level_ * 0.9f * alpha);
				}
				else
				{
					const float a = level_ * 0.16f;
					GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 60, static_cast<int16_t>(sy) - 110, 120, 120,
						1.0f, 0.85f, 0.45f, a);
					GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 35, static_cast<int16_t>(sy) - 90, 70, 70,
						1.0f, 0.92f, 0.60f, a * 1.4f);
					GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 12, static_cast<int16_t>(sy) - 75, 24, 24,
						1.0f, 0.98f, 0.82f, a * 2.0f);
				}
			}
		}

		void Lamps::clear()
		{
			level_ = 0.0f;
		}
	}
}
