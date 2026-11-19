//////////////////////////////////////////////////////////////////////////////////
// 天气路灯实现。手工放置表逐条抄自 bundle lamps.cpp:581-661（mapId, x, y, lampId）。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherLamps.h"

#include "../Gameplay/Stage.h"
#include "../Graphics/GraphicsGL.h"
#include "../Constants.h"

namespace ms
{
	namespace weather
	{
		Lamps& Lamps::get()
		{
			static Lamps instance;
			return instance;
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
				// 世界坐标 → 屏幕坐标（与 Background 视差口径一致：视口中心为原点）。
				const double sx = static_cast<double>(lamp.x) - viewx + vw / 2.0;
				const double sy = static_cast<double>(lamp.y) - viewy + vh / 2.0;
				if (sx < -80 || sx > vw + 80 || sy < -80 || sy > vh + 80)
					continue;
				// 暖色光晕：三层由大到小的半透明矩形近似径向渐变（bundle 用光晕 sprite）。
				const float a = level_ * 0.16f;
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 60, static_cast<int16_t>(sy) - 110, 120, 120,
					1.0f, 0.85f, 0.45f, a);
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 35, static_cast<int16_t>(sy) - 90, 70, 70,
					1.0f, 0.92f, 0.60f, a * 1.4f);
				GraphicsGL::get().drawrectangle(static_cast<int16_t>(sx) - 12, static_cast<int16_t>(sy) - 75, 24, 24,
					1.0f, 0.98f, 0.82f, a * 2.0f);
			}
		}

		void Lamps::clear()
		{
			level_ = 0.0f;
		}
	}
}
