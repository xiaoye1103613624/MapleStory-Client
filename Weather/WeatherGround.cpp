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

#include "WeatherState.h"
#include "WeatherConfig.h"
#include "../Audio/Audio.h"
#include "../Constants.h"
#include "../Gameplay/Stage.h"
#include "../Util/Resource/ResourceService.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace weather
	{
		// ==== 可调常量（bundle 锚定 + 原生近似集中处） ====
		static const int ACCUM_MAX = 40;        // bundle :56 =70；原生端降密度避免过多 draw call
		static const int ACCUM_RISE_MS = 90000; // bundle :57
		static const int ACCUM_MELT_MS = 25000; // bundle :58
		static const float ACCUM_PLACE_PER_S = 14.0f; // bundle :70
		static const int ACCUM_SURFACE_DROP = 5;      // bundle :69（沉降像素）

		static const int PUDDLE_MAX = 16;       // bundle :58 =26；原生端降密度
		static const int PUDDLE_TIERS = 3;      // bundle :39/%d/%d 三级
		static const int PUDDLE_RISE_MS = 90000;
		static const int PUDDLE_DRY_MS = 40000;
		static const float PUDDLE_PLACE_PER_S = 3.0f;

		static const int SPLASH_MAX = 32;
		static const float SPLASH_PER_SEC = 8.0f;
		static const int SPLASH_LIFETIME_MS = 900;

		// TODO(真机校准): 地面层 y 的近似口径。
		// bundle 通过 ReadPhysSpace/GetFHUnderneath 逐落点测量真实地面 z/y（weatheraccum.cpp:51-52）。
		// 原生端 Stage 只暴露 Player::get_position()（脚下世界坐标，见 MapObject.h:54），
		// 没有公开的"任意世界 x 的 foothold y"查询。此处统一用 player_world.y() + DROP
		// 作为地面线：平坦地图贴合，台阶/坡道会浮空或陷入。要精确需给 Physics/FootholdTree
		// 增加 get_y_below(x) 公开查询（Stage::respawn 已在用 physics.get_y_below，见 Stage.cpp:187），
		// 再由 Stage::draw 传入逐落点地面 y。此为本版最大观感风险，真机对照后调 kGroundDropPx。
		static const int16_t kGroundDropPx = 4;

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
			const nl::node effect_root = ms::res::ResourceService::get().root(ms::res::RootId::Effect);
			nl::node acc = effect_root["WeatherAccum.img"];
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

			nl::node pud = effect_root["WeatherPuddle.img"];
			for (int t = 0; t < PUDDLE_TIERS; ++t)
			{
				nl::node n = pud[std::to_string(t)]["0"];
				if (n)
					puddle_nodes_.emplace_back(n);
			}

			nl::node spl = effect_root["WeatherSplash.img"]["rain"];
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
				while (want-- > 0 && static_cast<int>(accum_patches_.size()) < target)
				{
					Patch p;
					size_t idx = next_rand() % accum_nodes_.size();
					p.anim = accum_nodes_[idx];
					// 槽位：以相机宽度近似视图，落点在视图内伪随机（bundle SLOT_W=26 网格近似）。
					const int vieww = Constants::Constants::get().get_viewwidth();
					p.wx = static_cast<int16_t>((next_rand() % static_cast<uint32_t>(vieww)) - vieww / 2);
					p.wy = static_cast<int16_t>(kGroundDropPx);
					accum_patches_.push_back(p);
				}
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
				while (want-- > 0 && static_cast<int>(puddles_.size()) < target)
				{
					Patch p;
					int tier = static_cast<int>(puddle_level_ * PUDDLE_TIERS);
					if (tier >= PUDDLE_TIERS) tier = PUDDLE_TIERS - 1;
					p.tier = static_cast<uint8_t>(tier);
					p.anim = puddle_nodes_[static_cast<size_t>(tier) % puddle_nodes_.size()];
					const int vieww = Constants::Constants::get().get_viewwidth();
					p.wx = static_cast<int16_t>((next_rand() % static_cast<uint32_t>(vieww)) - vieww / 2);
					p.wy = static_cast<int16_t>(kGroundDropPx);
					puddles_.push_back(p);
				}
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
			int want = static_cast<int>(splash_carry_);
			splash_carry_ -= static_cast<float>(want);
			while (want-- > 0 && static_cast<int>(splashes_.size()) < SPLASH_MAX)
			{
				const int vieww = Constants::Constants::get().get_viewwidth();
				int16_t wx = static_cast<int16_t>((next_rand() % static_cast<uint32_t>(vieww)) - vieww / 2);
				spawn_splash(wx, static_cast<int16_t>(kGroundDropPx));
			}
		}

		void Ground::update(uint16_t timestep)
		{
			if (!active_)
				return;
			const State& st = State::get();
			if (!st.has_state())
			{
				clear();
				return;
			}
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
			// 原生 Audio::Sound 只有一次性 play()（Audio.h:71-80），无 loop/stop 句柄，
			// bundle weathersound.cpp 的 rain/wind 持续 loop 无法等价实现。
			// 不臆造 API：本版仅记录当前天空供未来扩展，实际不发声（如实标注）。
			const_cast<Ground*>(this)->last_ambient_sky_ = loaded_sky_;
		}

		void Ground::draw(const DrawArgument& viewpos, const Point<int16_t>& player_world) const
		{
			if (!active_)
				return;
			// 世界 -> 屏幕：屏幕 x = wx - viewpos.x()，地面 y = 视口底部 - 视口高/2 + player_world 近似。
			// TODO(真机校准): 此处 y 仅用玩家脚下高度近似逐落点地面，台阶/坡道会偏移（见顶部说明）。
			const int viewh = Constants::Constants::get().get_viewheight();
			const int16_t ground_y = static_cast<int16_t>(player_world.y() + viewh / 2 - viewh / 2);
			const int base_x = viewpos.getpos().x();
			const int base_y = ground_y - viewpos.getpos().y();

			// 先堆积、再水坑、最后溅射（溅射在最上层，符合 bundle 的层序）。
			for (const Patch& p : accum_patches_)
				p.anim.draw(DrawArgument(static_cast<int16_t>(base_x + p.wx), static_cast<int16_t>(base_y + p.wy)), 1.0f);
			for (const Patch& p : puddles_)
				p.anim.draw(DrawArgument(static_cast<int16_t>(base_x + p.wx), static_cast<int16_t>(base_y + p.wy)), 1.0f);
			for (const Splash& s : splashes_)
				s.anim.draw(DrawArgument(static_cast<int16_t>(base_x + s.wx), static_cast<int16_t>(base_y + s.wy)), 1.0f);
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
