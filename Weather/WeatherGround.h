//////////////////////////////////////////////////////////////////////////////////
// 天气地面层（批次3）：雨天水坑 / 雨滴溅射 / 雪叶花瓣堆积 + 打滑桩 + 环境音
//
// 从 bundle client/{weatheraccum,weatherpuddle,weathersplash,weathersound}.cpp 移植，
// 数值常量逐条锚定对应源码行号（见 .cpp 注释）。原生端没有 COM layer/z，改用
// Graphics/Animation 从 nl::node 构造、按世界坐标 - 相机 = 屏幕坐标绘制。
//
// 与 bundle 的差异（如实标注）：
//   - 地面高度无 foothold 测量（ReadPhysSpace/GetFHUnderneath 是 DLL 内部地址），
//     本版用"玩家脚下 + 下沉"近似，真机校准点集中在 kGroundDropPx（见 .cpp TODO）。
//   - 堆积位置是本地伪随机槽位，不做跨玩家同步（bundle 亦然，只有 sky/age 是权威）。
//   - 打滑（weathermove 改 Physics 摩擦常量）高风险共享逻辑，本版**不改 Physics**，
//     仅留 snow_slip_factor() 接口桩 + 注释（见 .cpp apply_slip_stub）。
//   - 环境音：原生 Audio::Sound 只有一次性 play()，无 loop/stop 句柄（Audio.h:71-80），
//     bundle 的 rain/wind 持续 loop 无法等价实现。本版不臆造 API，环境音留接口桩，
//     真 loop 需扩展 Audio（单独评审）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "WeatherTypes.h"

#include "../Graphics/Animation.h"
#include "../Graphics/DrawArgument.h"
#include "../Audio/Audio.h"
#include "../Template/Point.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ms
{
	class FootholdTree;

	namespace weather
	{
		class Ground
		{
		public:
			static Ground& get();

			// 进图时调用：确保 Effect 根已解析（lazy 根），按当前 sky 预加载 Animation。
			void on_map_loaded();
			// 每帧（Stage::update 尾部）推进积累/水坑/溅射状态与动画。
			void update(uint16_t timestep);
			// 每帧（Stage::draw，角色之后、前景层）绘制地面层。
			// fht 用于溅射落点：逐列求 foothold 表面（对齐 weathersplash.cpp:238），
			// 传 nullptr 时只画已有溅射、不新生成。
			void draw(const DrawArgument& viewpos, const Point<int16_t>& player_world,
				const FootholdTree* fht);
			// 换线/断线：释放所有 Animation（node 是 lazy 根引用，跨图需重解析）。
			void clear();

			// 打滑接口桩：返回雪地打滑系数（0=不打滑，1=最滑）。
			// 本版仅暴露数值供未来 weathermove 驱动 Physics 摩擦常量，不实际改动物理。
			float snow_slip_factor() const;

			// 环境音：按 sky 切换 rain/wind 循环，跟随 SFX 音量。由 update 内部调用。
			void update_ambient() const;

		private:
			Ground() = default;

			// 单个地面贴片（堆积/水坑共用：世界坐标 + 动画）。
			struct Patch
			{
				Animation anim;
				int16_t wx = 0;     // 世界坐标（不再相对玩家）
				int16_t wy = 0;     // foothold 表面 y - SURFACE_DROP
				uint8_t tier = 0;   // 水坑宽窄级别 0..2；堆积恒 0
				int16_t slot = -1;  // 积雪槽位（-1=非槽位物件）
			};

			// 短命溅射（雨滴撞地），带剩余寿命。
			struct Splash
			{
				Animation anim;
				int16_t wx = 0;
				int16_t wy = 0;
				int64_t life_ms = 0;
			};

			bool active_ = false;      // 资源是否加载成功（Effect 根可用且至少一类 Animation 非空）
			Sky loaded_sky_ = Sky::CLEAR; // 当前堆积/水坑已按哪种 sky 生成

			// 状态（bundle 口径：rise 90s / melt 25s，水坑同曲线，见 .cpp）。
			float accum_level_ = 0.0f;   // 雪/叶/花瓣 0..1
			float puddle_level_ = 0.0f;  // 水坑 0..1
			float splash_carry_ = 0.0f;  // 溅射生成速率的帧间小数累加
			int splash_pending_ = 0;     // 待生成数（draw 里按 foothold 落点实际生成）
			int accum_pending_ = 0;      // 积雪待放置数（draw 里按槽位+边缘探针放置）
			int puddle_pending_ = 0;     // 水坑待放置数（draw 里按坡度+间距规则放置）
			float accum_carry_ = 0.0f;
			float puddle_carry_ = 0.0f;
			int64_t elapsed_ms_ = 0;     // 本地累计，仅用于伪随机相位，不参与 wire 权威

			std::vector<Animation> accum_nodes_;   // 按 kind 解析的源动画（snow/leaf/petal）
			std::vector<Animation> puddle_nodes_;  // 水坑三级 0/1/2
			std::vector<Animation> splash_nodes_;  // 溅射 rain/%d

			std::vector<Patch> accum_patches_;
			std::vector<Patch> puddles_;
			std::vector<Splash> splashes_;

			Sky accum_kind_ = Sky::CLEAR;  // 当前堆积的材质（决定用 snow/leaf/petal）

			void ensure_nodes();
			void step_accum(uint16_t timestep, Sky sky);
			void step_puddle(uint16_t timestep, Sky sky);
			void step_splash(uint16_t timestep, Sky sky);
			void spawn_splash(int16_t wx, int16_t wy);

			// 确定性伪随机（同 sky 同帧序列可复现，便于真机对照；不引入 <random> 状态）。
			uint32_t next_rand() const;

			// 2026-11-22：环境音循环句柄。按 sky 切 rain/wind，切天空/清场时 stop。
			AmbientSound ambient_;
			Sky last_ambient_sky_ = Sky::CLEAR;
		};
	}
}
