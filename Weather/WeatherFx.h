//////////////////////////////////////////////////////////////////////////////////
// 天气渲染层（D1 核心闭环）：世界时钟驱动的昼夜压暗 + 区域夜色。
//
// 与 bundle DLL 的差异（如实标注）：bundle 通过 hook 对 tiles/objs/backs/npc 逐层做
// multiply tint；原生端 Sprite 没有统一 multiply 通道，本版用 GraphicsGL::drawscreenfill
// 做全屏半透明夜色叠加（darken）。观感接近但不是逐像素乘法，UI 不被压暗（在 UI 绘制前
// 调用）。若要严格还原 multiply，需要给 GraphicsGL 加 tint uniform——列为后续增强。
//
// 天空注入（云/月/雨雪 sheet）、地面堆积、路灯 sprite 依赖 P6 WZ 资源，接口在
// WeatherReserved，本版不启用。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include "../Graphics/Color.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace weather
	{
		// 单独加载 Data/Effect 下的某个 img（Effect 根只绑了 17 项，Weather* 系列
		// 都不在其中；实测 rootChildren=17/首项 BasicEff.img）。返回空 node 表示失败。
		// 结果按文件名缓存，进程内只解析一次。
#ifdef USE_NX
		nl::node effect_img(const char* filename);
		// 2026-11-22：单独加载 Data/Sound 下的某个 img（环境音 Weather.img）。
		// 与 effect_img 同款 open_single，进程内按文件名缓存。
		nl::node sound_img(const char* filename);
#endif

		class Fx
		{
		public:
			static Fx& get();

			// 由 Stage::update 每帧调用：推进本地时钟、趋近目标夜色。
			void update(int64_t dt_ms);

			// 由 Stage::draw 在场景绘制完成后、UI 之前调用：叠加夜色。
			// active 为 false（非游戏态或未收到 0x373D）时零绘制。
			void draw() const;
			// 风暴闪电全屏脉冲，须在 draw() 之后调用（盖在夜色之上）。
			void draw_lightning() const;
			// 自绘天气粒子（雨丝/雪花）：GraphicsGL 直接画，不依赖 WZ canvas。
			// 在角色之后、夜色蒙版之前调用；密度/alpha 由 sky 与 profile 驱动。
			void draw_particles(double viewx, double viewy) const;
			// 2026-11-23：只保留月光光晕（玩家/NPC 光晕已按用户要求删除）。
			// 强度随 night_level 从淡到亮再到暗，与夜景深入同步。
			void draw_lights() const;
			float night_level() const { return night_level_; }
			// 2026-11-23 分层夜色：返回当前场景乘色 RGB（alpha 恒 1），供 Stage 分层 push_tint。
			// factor = lerp(1, night_mul_*, night_level_)，与旧 drawscreenmultiply 同口径。
			Color scene_tint() const;

			void reset();  // 换线/断线

		private:
			Fx() = default;
			float night_level_ = 0.0f;   // 0 白天 1 深夜
			// 叠加色是"要给场景盖上的暗色"，初值必须是区域默认夜色而非白(1.0)——
			// 白色叠加在 alpha>0 时会形成灰白蒙版（真机 2026-11-18 已复现），从根上消除白路径。
			float tint_r_ = 0x4A / 255.0f, tint_g_ = 0x5A / 255.0f, tint_b_ = 0x8C / 255.0f;
			// 2026-11-21 逐层 multiply 的夜色乘色系数（含 golden hour 色温），与 night_level_ 分开：
			// draw() 用 lerp(1, night_mul_*, night_level_) 压暗场景，天亮时因子为 1 不改画面。
			float night_mul_r_ = 0x4A / 255.0f, night_mul_g_ = 0x5A / 255.0f, night_mul_b_ = 0x8C / 255.0f;
			float lightning_ = 0.0f;     // 风暴闪电全屏脉冲 0..1（bundle WeatherFx::TickLightning）
		};
	}
}
