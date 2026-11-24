//////////////////////////////////////////////////////////////////////////////////
// 天气地面层实现（批次3）。常量逐条锚定 bundle：
//   weatheraccum.cpp:55-77   ACCUM_UOL/%s/%d, ACCUM_MAX=70, RISE=90000, MELT=25000,
//                            PLACE_PER_S=14, SURFACE_DROP=5, SLOT_W=26, STACK_MAX=3
//   weatherpuddle.cpp:39/58  PUDDLE_UOL=%d/%d, TIERS=3, PLACE_PER_S, RISE/DRY
//   weathersplash.cpp:26/409 SPLASH_UOL=rain/%d, PER_SEC, LIFETIME
//   weathersound.cpp         Sound/Weather.img/{rain,wind,splash}, 跟随 SFX slider
// 原生端无 COM layer/z，地面高度用玩家脚下近似（真机校准 TODO 集中在本文件顶部）。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherGround.h"
#include "WeatherFx.h"

#include <cmath>
#include <fstream>
#include <cstdlib>
#include "../Gameplay/Physics/FootholdTree.h"

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Audio/Audio.h"
#include "../Configuration.h"
#include "../Constants.h"
#include "../Gameplay/Stage.h"
#include "../Util/Resource/ResourceService.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#include <nlnx/audio.hpp>
#endif

namespace ms
{
	namespace weather
	{
		// ==== 可调常量（bundle 锚定 + 原生近似集中处） ====
		static const int ACCUM_MAX = 70;        // 参考 weatheraccum.cpp:56
		static const int ACCUM_RISE_MS = 90000; // 参考 :57
		static const int ACCUM_MELT_MS = 25000; // 参考 :58
		static const float ACCUM_PLACE_PER_S = 14.0f; // 参考 :70
		static const int ACCUM_SURFACE_DROP = 5;      // 参考 :69

		static const int PUDDLE_MAX = 26;       // 参考 weatherpuddle.cpp:42
		static const int PUDDLE_TIERS = 3;      // bundle :39/%d/%d 三级
		static const int PUDDLE_RISE_MS = 75000;  // 参考 :43
		static const int PUDDLE_DRY_MS = 40000;   // 参考 :44
		static const float PUDDLE_PLACE_PER_S = 3.0f; // 参考 :62

		static const int SPLASH_MAX = 40;            // 参考 weathersplash.cpp:29
		static const float SPLASH_PER_SEC = 22.4f;   // 参考 :34
		static const int SPLASH_LIFETIME_MS = 520;    // 参考 :28 (5帧x80ms)

		// TODO(真机校准): 地面层 y 的近似口径。
		// bundle 通过 ReadPhysSpace/GetFHUnderneath 逐落点测量真实地面 z/y（weatheraccum.cpp:51-52）。
		// 原生端 Stage 只暴露 Player::get_position()（脚下世界坐标，见 MapObject.h:54），
		// 没有公开的"任意世界 x 的 foothold y"查询。此处统一用 player_world.y() + DROP
		// 作为地面线：平坦地图贴合，台阶/坡道会浮空或陷入。要精确需给 Physics/FootholdTree
		// 增加 get_y_below(x) 公开查询（Stage::respawn 已在用 physics.get_y_below，见 Stage.cpp:187），
		// 再由 Stage::draw 传入逐落点地面 y。此为本版最大观感风险，真机对照后调 kGroundDropPx。
		static const int16_t kGroundDropPx = 4;
		// 参考 weatheraccum.cpp:59-61/76-77
		static const int ACCUM_SLOT_W = 26;
		static const int ACCUM_STACK_MAX = 3;
		static const int ACCUM_STACK_LIFT = 2;
		static const int ACCUM_EDGE_PROBE = 14;
		static const int ACCUM_EDGE_STEP = 10;
		// 参考 weatherpuddle.cpp:60-61/78-79/63
		static const int PUDDLE_MIN_GAP = 56;
		static const int PUDDLE_SAME_FLOOR_DY = 24;
		static const int PUDDLE_PROBE_DX = 12;
		static const int PUDDLE_MAX_SLOPE = 3;
		static const int PUDDLE_MARGIN = 80;
		static const int SPLASH_MARGIN = 64;         // 参考 :35
		static const int SPLASH_SURFACE_DROP = 4;    // 参考 :45

		Ground& Ground::get()
		{
			static Ground instance;
			return instance;
		}

		uint32_t Ground::next_rand() const
		{
			// xorshift32，种子只依赖本地累计时间，不引入线程/全局随机状态。
			static uint32_t s = 0x9E3779B9u;
			s ^= s << 13;
			s ^= s >> 17;
			s ^= s << 5;
			return s;
		}

		void Ground::on_map_loaded()
		{
			// Effect 根经 ResourceService 门面取（root() 会按当前后端同步解析，未装载返回空 node）。
			// 不直接依赖 nl::nx 全局是否已绑定，兼容 img/nx 两种后端。
			(void)ms::res::ResourceService::get().root(ms::res::RootId::Effect);
			clear();
			ensure_nodes();
			active_ = !accum_nodes_.empty() || !puddle_nodes_.empty() || !splash_nodes_.empty();
			loaded_sky_ = State::get().has_state() ? State::get().sky() : Sky::CLEAR;
		}

		void Ground::ensure_nodes()
		{
#ifdef USE_NX
			// bundle 路径锚定：
			//   weatheraccum.cpp:55  Effect/WeatherAccum.img/%s/%d  -> snow/leaf/petal
			//   weatherpuddle.cpp:39 Effect/WeatherPuddle.img/%d/%d -> 0/1/2 tier
			//   weathersplash.cpp:26 Effect/WeatherSplash.img/rain/%d
			// 注意：不能再用 ResService::root(RootId::Effect)——实测该根只绑 17 项、
			// WeatherAccum/Puddle/Splash/Particles 都不在其中，取到的 node 全空，
			// 导致积雪/水坑/溅射三件套静默失效。改为按文件独立加载。
			nl::node acc = weather::effect_img("WeatherAccum.img");
			const char* kinds[] = { "snow", "leaf", "petal" };
			for (const char* kind : kinds)
			{
				for (int i = 0; i < 2; ++i)
				{
					nl::node n = acc[kind][std::to_string(i)];
					if (n)
						accum_nodes_.emplace_back(n);
				}
			}

			nl::node pud = weather::effect_img("WeatherPuddle.img");
			for (int t = 0; t < PUDDLE_TIERS; ++t)
			{
				nl::node n = pud[std::to_string(t)]["0"];
				if (n)
					puddle_nodes_.emplace_back(n);
			}

			nl::node spl = weather::effect_img("WeatherSplash.img")["rain"];
			for (int i = 0; i < 3; ++i)
			{
				nl::node n = spl[std::to_string(i)];
				if (n)
					splash_nodes_.emplace_back(n);
			}
#endif
		}

		static bool is_accum_sky(Sky sky)
		{
			return sky == Sky::SNOW || sky == Sky::BLIZZARD
				|| sky == Sky::LEAVES || sky == Sky::BLOSSOM;
		}

		static bool is_wet_sky(Sky sky)
		{
			return sky == Sky::RAIN || sky == Sky::STORM;
		}

		void Ground::step_accum(uint16_t timestep, Sky sky)
		{
			const bool growing = is_accum_sky(sky);
			if (growing && accum_kind_ != sky && accum_level_ <= 0.0f)
				accum_kind_ = sky; // 换材质时旧堆积已化尽，直接切换

			if (growing && (accum_kind_ == sky || accum_level_ <= 0.0f))
			{
				accum_level_ += static_cast<float>(timestep) / ACCUM_RISE_MS;
				if (accum_level_ > 1.0f) accum_level_ = 1.0f;
			}
			else
			{
				accum_level_ -= static_cast<float>(timestep) / ACCUM_MELT_MS;
				if (accum_level_ < 0.0f) accum_level_ = 0.0f;
			}

			const int target = static_cast<int>(accum_level_ * ACCUM_MAX);
			if (!accum_nodes_.empty() && static_cast<int>(accum_patches_.size()) < target)
			{
				accum_carry_ += ACCUM_PLACE_PER_S * (static_cast<float>(timestep) / 1000.0f);
				int want = static_cast<int>(accum_carry_);
				accum_carry_ -= static_cast<float>(want);
				// 只累计待放置数：真实落点要读 foothold 表面，统一在 draw()（有 fht）里做。
				// 参考 weatheraccum.cpp:27-28 "Placement is by SLOT, not by random x"。
				accum_pending_ += want;
			}
			else if (static_cast<int>(accum_patches_.size()) > target)
			{
				accum_patches_.resize(target); // 化雪从最新落点开始退
			}
			for (Patch& p : accum_patches_)
				p.anim.update(timestep);
		}

		void Ground::step_puddle(uint16_t timestep, Sky sky)
		{
			const bool wet = is_wet_sky(sky);
			if (wet)
			{
				puddle_level_ += static_cast<float>(timestep) / PUDDLE_RISE_MS;
				if (puddle_level_ > 1.0f) puddle_level_ = 1.0f;
			}
			else
			{
				puddle_level_ -= static_cast<float>(timestep) / PUDDLE_DRY_MS;
				if (puddle_level_ < 0.0f) puddle_level_ = 0.0f;
			}

			const int target = static_cast<int>(puddle_level_ * PUDDLE_MAX);
			if (!puddle_nodes_.empty() && static_cast<int>(puddles_.size()) < target)
			{
				puddle_carry_ += PUDDLE_PLACE_PER_S * (static_cast<float>(timestep) / 1000.0f);
				int want = static_cast<int>(puddle_carry_);
				puddle_carry_ -= static_cast<float>(want);
				puddle_pending_ += want;   // 落点同样在 draw 里按 foothold + 坡度/间距规则求
			}
			else if (static_cast<int>(puddles_.size()) > target)
			{
				puddles_.resize(target);
			}
			// 越下越宽：随等级把已有水坑 promote 到更高 tier（bundle :710-745）。
			const int want_tier = static_cast<int>(puddle_level_ * PUDDLE_TIERS);
			for (Patch& p : puddles_)
			{
				if (p.tier < want_tier && puddle_nodes_.size() > static_cast<size_t>(want_tier))
				{
					p.tier = static_cast<uint8_t>(want_tier);
					p.anim = puddle_nodes_[static_cast<size_t>(want_tier)];
				}
				p.anim.update(timestep);
			}
		}

		void Ground::spawn_splash(int16_t wx, int16_t wy)
		{
			if (splash_nodes_.empty()) return;
			Splash s;
			s.anim = splash_nodes_[next_rand() % splash_nodes_.size()];
			s.wx = wx;
			s.wy = wy;
			s.life_ms = SPLASH_LIFETIME_MS;
			splashes_.push_back(s);
		}

		void Ground::step_splash(uint16_t timestep, Sky sky)
		{
			for (auto it = splashes_.begin(); it != splashes_.end();)
			{
				it->life_ms -= timestep;
				it->anim.update(timestep);
				if (it->life_ms <= 0)
					it = splashes_.erase(it);
				else
					++it;
			}
			const float intensity = is_wet_sky(sky) ? Config::get().profile(sky).rain : 0.0f;
			if (intensity <= 0.0f || static_cast<int>(splashes_.size()) >= SPLASH_MAX)
				return;
			splash_carry_ += (SPLASH_PER_SEC * intensity) * (static_cast<float>(timestep) / 1000.0f);
			const int want = static_cast<int>(splash_carry_);
			splash_carry_ -= static_cast<float>(want);
			// 参考实现（weathersplash.cpp:38 注释）：生成与落点校正都在"渲染帧"里做，
			// 不在逻辑 tick 里。这里只累计待生成数，真正落点由 draw() 用 foothold 求。
			splash_pending_ += want;
		}

		void Ground::update(uint16_t timestep)
		{
			if (!active_)
				return;
			const State& st = State::get();
			// 室内/水下不积雪不积水（对齐 g_bFallingSkyField）。不 clear()：
			// active_ 表示资源已装好，换图时 on_map_loaded 会重建。
			if (!st.has_state() || !st.allow_falling())
				return;
			const Sky sky = st.sky();
			loaded_sky_ = sky;
			elapsed_ms_ += timestep;
			step_accum(timestep, sky);
			step_puddle(timestep, sky);
			step_splash(timestep, sky);
			update_ambient();
		}

		void Ground::update_ambient() const
		{
			// 2026-11-22：环境音 loop 落地（原为桩）。bundle weathersound.cpp 锚定
			// Sound/Weather.img/{rain,wind,splash}：雨/风暴播 rain，雪/叶/花/沙暴播 wind，
			// 晴天/阴天停。跟随 SFX slider（AmbientSound 用 per-channel 音量）。
			// 只在 sky 变化时切换，避免每帧重建 stream。资源节点由 sound_img 独立加载。
			Ground* self = const_cast<Ground*>(this);

			if (loaded_sky_ == last_ambient_sky_)
				return;

			self->last_ambient_sky_ = loaded_sky_;

#ifdef USE_NX
			static const nl::node weather_img = weather::sound_img("Weather.img");

			const char* node = nullptr;
			switch (loaded_sky_)
			{
			case Sky::RAIN:
			case Sky::STORM:
				node = "rain";
				break;
			case Sky::SNOW:
			case Sky::BLIZZARD:
			case Sky::LEAVES:
			case Sky::BLOSSOM:
			case Sky::SANDSTORM:
				node = "wind";
				break;
			default:
				break;
			}

			if (!node || !weather_img)
			{
				self->ambient_.stop();
				return;
			}

			nl::audio src = weather_img[node];
			uint8_t sfx = Setting<SFXVolume>::get().load();
			float volume = static_cast<float>(sfx) / 100.0f;
			self->ambient_.play(src, volume);
#else
			self->ambient_.stop();
#endif
		}

		void Ground::draw(const DrawArgument& viewpos, const Point<int16_t>& player_world,
			const FootholdTree* fht)
		{
			if (!active_ || !State::get().allow_falling())
				return;

			const int16_t vw = Constants::Constants::get().get_viewwidth();
			// 世界坐标 = 屏幕坐标 - viewpos（screen = world + viewpos）。
			const int world_left = -viewpos.getpos().x();
			const int world_top = -viewpos.getpos().y();
			const int base_x = viewpos.getpos().x();
			const int base_y = viewpos.getpos().y();
			(void)player_world;   // 落点不再依赖角色 y（用户 2026-11-20 指出）

			// 溅射生成：逐列在 foothold 上找表面（对齐 weathersplash.cpp:238
			// GetFootholdUnderneath(x, camY-halfH, &ySurface)），而不是画在玩家脚下。
			// 世界坐标 = 屏幕坐标 - viewpos（screen = world + viewpos）。
			if (fht != nullptr)
			{
				// 积雪：26px 槽位 + ±14 边缘探针（Δy≤10）+ 每槽最多 3 层、每层抬 2px。
				// 参考 weatheraccum.cpp:59-61/76-77/292-295/345-351。
				while (accum_pending_ > 0 && !accum_nodes_.empty()
					&& static_cast<int>(accum_patches_.size()) < ACCUM_MAX)
				{
					--accum_pending_;
					const int slots = vw / ACCUM_SLOT_W + 2;
					const int slot = world_left / ACCUM_SLOT_W + static_cast<int>(next_rand() % static_cast<uint32_t>(slots));
					const int wx = slot * ACCUM_SLOT_W + static_cast<int>(next_rand() % ACCUM_SLOT_W);
					const int16_t ysurf = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx), static_cast<int16_t>(world_top)));
					// 边缘探针：平台边缘外 Δy 会突然变大，落在那里会悬空。
					const int16_t yl = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx - ACCUM_EDGE_PROBE), static_cast<int16_t>(world_top)));
					const int16_t yr = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx + ACCUM_EDGE_PROBE), static_cast<int16_t>(world_top)));
					if (std::abs(static_cast<int>(yl) - ysurf) > ACCUM_EDGE_STEP
						|| std::abs(static_cast<int>(yr) - ysurf) > ACCUM_EDGE_STEP)
						continue;

					// 同槽叠层：已在该槽的层数决定抬升
					int depth = 0;
					for (const Patch& q : accum_patches_)
						if (q.slot == slot) ++depth;
					if (depth >= ACCUM_STACK_MAX)
						continue;

					Patch p;
					p.anim = accum_nodes_[next_rand() % accum_nodes_.size()];
					p.wx = static_cast<int16_t>(wx);
					p.wy = static_cast<int16_t>(ysurf - ACCUM_SURFACE_DROP - depth * ACCUM_STACK_LIFT);
					p.slot = static_cast<int16_t>(slot);
					accum_patches_.push_back(p);
				}

				// 水坑：间距 ≥56、同层 dy<24、左右 ±12 坡度 ≤3px。
				// 参考 weatherpuddle.cpp:60-61/78-79/290-324。
				while (puddle_pending_ > 0 && !puddle_nodes_.empty()
					&& static_cast<int>(puddles_.size()) < PUDDLE_MAX)
				{
					--puddle_pending_;
					const int wx = world_left - PUDDLE_MARGIN
						+ static_cast<int>(next_rand() % static_cast<uint32_t>(vw + PUDDLE_MARGIN * 2));
					const int16_t ysurf = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx), static_cast<int16_t>(world_top)));
					// 坡度探针
					const int16_t yl = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx - PUDDLE_PROBE_DX), static_cast<int16_t>(world_top)));
					const int16_t yr = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx + PUDDLE_PROBE_DX), static_cast<int16_t>(world_top)));
					if (std::abs(static_cast<int>(yl) - ysurf) > PUDDLE_MAX_SLOPE
						|| std::abs(static_cast<int>(yr) - ysurf) > PUDDLE_MAX_SLOPE)
						continue;
					// 间距/同层检查
					bool tooClose = false;
					for (const Patch& q : puddles_)
					{
						if (std::abs(static_cast<int>(q.wx) - wx) < PUDDLE_MIN_GAP
							&& std::abs(static_cast<int>(q.wy) - ysurf) < PUDDLE_SAME_FLOOR_DY)
						{
							tooClose = true;
							break;
						}
					}
					if (tooClose)
						continue;

					Patch p;
					int tier = static_cast<int>(puddle_level_ * PUDDLE_TIERS);
					if (tier >= PUDDLE_TIERS) tier = PUDDLE_TIERS - 1;
					if (tier < 0) tier = 0;
					p.tier = static_cast<uint8_t>(tier);
					p.anim = puddle_nodes_[static_cast<size_t>(tier)];
					p.wx = static_cast<int16_t>(wx);
					p.wy = static_cast<int16_t>(ysurf);
					puddles_.push_back(p);
				}
			}

			if (splash_pending_ > 0 && fht != nullptr && !splash_nodes_.empty())
			{
				const int world_left = -viewpos.getpos().x();
				const int world_top = -viewpos.getpos().y();
				while (splash_pending_ > 0 && static_cast<int>(splashes_.size()) < SPLASH_MAX)
				{
					--splash_pending_;
					const int span = vw + SPLASH_MARGIN * 2;
					const int wx = world_left - SPLASH_MARGIN + static_cast<int>(next_rand() % static_cast<uint32_t>(span));
					const int16_t surface = fht->get_y_below(
						Point<int16_t>(static_cast<int16_t>(wx), static_cast<int16_t>(world_top)));
					spawn_splash(static_cast<int16_t>(wx),
						static_cast<int16_t>(surface - SPLASH_SURFACE_DROP));
				}
			}

			// 先堆积、再水坑、最后溅射（溅射在最上层，符合参考的层序）。
			// 世界坐标 → 屏幕（不再加玩家偏移：这是用户指出的"按角色 y 轴"问题的根因）。
			for (const Patch& p : accum_patches_)
				p.anim.draw(DrawArgument(Point<int16_t>(static_cast<int16_t>(p.wx + base_x),
					static_cast<int16_t>(p.wy + base_y))), 1.0f);
			for (const Patch& p : puddles_)
				p.anim.draw(DrawArgument(Point<int16_t>(static_cast<int16_t>(p.wx + base_x),
					static_cast<int16_t>(p.wy + base_y))), 1.0f);
			// 溅射是"世界坐标"落点，不能加玩家偏移（此前错误地用了 base_y）。
			for (const Splash& sp : splashes_)
				sp.anim.draw(DrawArgument(Point<int16_t>(static_cast<int16_t>(sp.wx + viewpos.getpos().x()),
					static_cast<int16_t>(sp.wy + viewpos.getpos().y()))), 1.0f);
		}

		float Ground::snow_slip_factor() const
		{
			// weathermove.cpp:194 通过 WeatherAccum_SnowLevel() 改 Physics 摩擦常量。
			// 本版只暴露数值：雪堆积越厚越滑。未来接入需改 Gameplay/Physics/Physics.h 的
			// walk/friction 常量，属高风险共享逻辑，须单独评审，避免破坏移动手感。
			if (accum_kind_ != Sky::SNOW && accum_kind_ != Sky::BLIZZARD)
				return 0.0f;
			return accum_level_;
		}

		void Ground::clear()
		{
			ambient_.stop(); // 换线/断线：停掉天气环境音循环
			last_ambient_sky_ = Sky::CLEAR;
			accum_patches_.clear();
			puddles_.clear();
			splashes_.clear();
			accum_nodes_.clear();
			puddle_nodes_.clear();
			splash_nodes_.clear();
			accum_level_ = 0.0f;
			puddle_level_ = 0.0f;
			splash_carry_ = 0.0f;
			accum_carry_ = 0.0f;
			puddle_carry_ = 0.0f;
			active_ = false;
			accum_kind_ = Sky::CLEAR;
		}
	}
}
