////////////////////////////////////////////////////////////////////////////////
//	公会 PQ healer / pulley 物件态机
////////////////////////////////////////////////////////////////////////////////
#include "MapGuildBoss.h"

#include "../../Constants.h"
#include "../../Util/Misc.h"

#include <cstdlib>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// 原生 RelMove 由 Gr2D 插值；本端用时长近似（ms），按 |ΔY| 与 rise/fall 估速。
		constexpr int16_t k_min_move_ms = 120;
		constexpr int16_t k_max_move_ms = 800;
	}

	MapGuildBoss::MapGuildBoss() = default;

	nl::node MapGuildBoss::resolve_obj(const std::string& wz_path)
	{
#ifdef USE_NX
		if (wz_path.empty())
			return {};

		// 地图属性常见写法：Map/Obj/guild.img/syarenian/boss/0
		std::string rel = wz_path;
		if (rel.rfind("Map/", 0) == 0)
			rel = rel.substr(4);

		nl::node n = nl::nx::Map.resolve(rel);
		if (n)
			return n;

		// 回退：拆 Obj/<img>/l0/l1/l2
		if (rel.rfind("Obj/", 0) == 0)
			rel = rel.substr(4);
		const size_t slash = rel.find('/');
		if (slash == std::string::npos)
			return {};
		const std::string img = rel.substr(0, slash);
		const std::string rest = rel.substr(slash + 1);
		n = nl::nx::Map["Obj"][img].resolve(rest);
		if (n)
			return n;
		return nl::nx::Map["Obj"][img + (img.find(".img") == std::string::npos ? ".img" : "")].resolve(rest);
#else
		(void)wz_path;
		return {};
#endif
	}

	MapGuildBoss::MapGuildBoss(nl::node map_src)
	{
#ifdef USE_NX
		nl::node healer = map_src["healer"];
		if (healer)
		{
			healer_x = static_cast<int16_t>(static_cast<int32_t>(healer["x"]));
			healer_y_min = static_cast<int16_t>(static_cast<int32_t>(healer["yMin"]));
			if (healer["rise"])
				healer_rise = static_cast<int16_t>(static_cast<int32_t>(healer["rise"]));
			if (healer["fall"])
				healer_fall = static_cast<int16_t>(static_cast<int32_t>(healer["fall"]));

			const std::string path = healer["healer"].get_string();
			nl::node anim_src = resolve_obj(path);
			if (anim_src)
			{
				healer_anim = Animation(anim_src);
				healer_y_now = healer_y_min;
				healer_y_from = healer_y_min;
				healer_y_to = healer_y_min;
				healer_loaded = true;
				LOG(LOG_INFO, "[GuildBoss] map healer path=" << path
					<< " x=" << healer_x << " yMin=" << healer_y_min
					<< " rise=" << healer_rise << " fall=" << healer_fall);
			}
			else
			{
				LOG(LOG_WARN, "[GuildBoss] healer anim missing path=" << path);
			}
		}

		nl::node pulley = map_src["pulley"];
		if (pulley)
		{
			pulley_x = static_cast<int16_t>(static_cast<int32_t>(pulley["x"]));
			pulley_y = static_cast<int16_t>(static_cast<int32_t>(pulley["y"]));
			const std::string path = pulley["pulley"].get_string();
			nl::node anim_src = resolve_obj(path);
			if (anim_src)
			{
				pulley_anim = Animation(anim_src);
				pulley_loaded = true;
				pulley_state_byte = 0;
				LOG(LOG_INFO, "[GuildBoss] map pulley path=" << path
					<< " x=" << pulley_x << " y=" << pulley_y);
			}
			else
			{
				LOG(LOG_WARN, "[GuildBoss] pulley anim missing path=" << path);
			}
		}
#else
		(void)map_src;
#endif
	}

	void MapGuildBoss::clear()
	{
		healer_loaded = false;
		pulley_loaded = false;
		healer_move_ms = 0;
		healer_move_dur = 0;
		pulley_state_byte = 0;
		healer_anim = Animation();
		pulley_anim = Animation();
	}

	void MapGuildBoss::set_healer_y(int16_t y)
	{
		if (!healer_loaded)
		{
			LOG(LOG_DEBUG, "[GuildBoss] healer move ignored (no map object) y=" << y);
			return;
		}

		const int16_t old_y = healer_y_now;
		healer_y_from = old_y;
		healer_y_to = y;
		healer_y_now = old_y;

		const int32_t dy = static_cast<int32_t>(y) - static_cast<int32_t>(old_y);
		const int16_t speed = dy >= 0
			? (healer_fall > 0 ? healer_fall : 10)
			: (healer_rise > 0 ? healer_rise : 1);
		int32_t dur = speed > 0
			? (std::abs(dy) * 16) / speed
			: k_min_move_ms;
		if (dur < k_min_move_ms)
			dur = k_min_move_ms;
		if (dur > k_max_move_ms)
			dur = k_max_move_ms;
		healer_move_dur = static_cast<int16_t>(dur);
		healer_move_ms = 0;

		LOG(LOG_INFO, "[GuildBoss] healer move " << old_y << "->" << y
			<< " dur_ms=" << healer_move_dur);
	}

	void MapGuildBoss::set_pulley_state(uint8_t state)
	{
		pulley_state_byte = state;
		LOG(LOG_INFO, "[GuildBoss] pulley state=" << (int)state
			<< (pulley_loaded ? "" : " (no map object)"));
	}

	void MapGuildBoss::update()
	{
		if (healer_loaded)
		{
			healer_anim.update();
			if (healer_move_dur > 0 && healer_move_ms < healer_move_dur)
			{
				healer_move_ms = static_cast<int16_t>(
					healer_move_ms + Constants::TIMESTEP);
				if (healer_move_ms >= healer_move_dur)
				{
					healer_move_ms = healer_move_dur;
					healer_y_now = healer_y_to;
				}
				else
				{
					const float t = static_cast<float>(healer_move_ms)
						/ static_cast<float>(healer_move_dur);
					healer_y_now = static_cast<int16_t>(
						healer_y_from + static_cast<int16_t>(
							(healer_y_to - healer_y_from) * t));
				}
			}
		}
		if (pulley_loaded)
			pulley_anim.update();
	}

	void MapGuildBoss::draw(Point<int16_t> viewpos, float alpha) const
	{
		(void)alpha;
		if (healer_loaded)
		{
			const Point<int16_t> pos(healer_x, healer_y_now);
			healer_anim.draw(DrawArgument(pos + viewpos), 1.0f);
		}
		if (pulley_loaded)
		{
			// state 仅门控交互（原生 sub_558B45）；绘制始终播循环帧。
			const Point<int16_t> pos(pulley_x, pulley_y);
			pulley_anim.draw(DrawArgument(pos + viewpos), 1.0f);
			(void)pulley_state_byte;
		}
	}
}
