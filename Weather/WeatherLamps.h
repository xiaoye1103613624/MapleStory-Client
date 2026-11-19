//////////////////////////////////////////////////////////////////////////////////
// 天气路灯（D1 核心版）：夜色降临点亮城镇路灯的暖色光晕。
//
// bundle lamps.cpp 用 74 种 sprite + emitter 精确偏移逐灯画光晕/灯丝；本版取
// 40 条手工放置坐标（lamps.cpp:581-661），在世界坐标画暖色光晕圆（GraphicsGL
// 多层矩形近似径向渐变），随 night_level 淡入。灯 sprite 精确 emitter 对齐、
// 地图自身 obj 匹配点亮留后续（见 TODO）。核心观感"天黑灯亮"已具备。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ms
{
	namespace weather
	{
		class Lamps
		{
		public:
			static Lamps& get();

			// 由 Stage::update 每帧调用：按夜色强度推进光晕 fade。
			void update(float night_level);

			// 由 Stage::draw 在场景之后调用：画光晕（世界坐标已换算为屏幕坐标）。
			void draw(double viewx, double viewy, float alpha) const;

			void clear();

		private:
			Lamps() = default;
			float level_ = 0.0f;   // 当前光晕强度（趋近 night_level）
		};
	}
}
