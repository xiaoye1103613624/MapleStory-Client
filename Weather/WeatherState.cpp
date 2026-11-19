//////////////////////////////////////////////////////////////////////////////////
// 天气世界状态实现（单线程，无 atomics）
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherState.h"

namespace ms
{
	namespace weather
	{
		State& State::get()
		{
			static State instance;
			return instance;
		}

		void State::apply(int minute_of_day, int ms_per_game_minute, uint8_t sky_raw,
			uint8_t flags, int sky_elapsed_sec, int rainbow_secs,
			uint8_t tint_r, uint8_t tint_g, uint8_t tint_b,
			uint8_t palette_raw, int sky_elapsed_ms, uint32_t sky_token)
		{
			has_state_ = true;
			if (minute_of_day >= 0 && minute_of_day < 1440)
				minute_ = minute_of_day;
			if (ms_per_game_minute > 0)
				ms_per_game_minute_ = ms_per_game_minute;
			sky_ = sanitize_sky(sky_raw);
			palette_ = sanitize_palette(palette_raw);
			flags_ = flags;
			tint_r_ = tint_r;
			tint_g_ = tint_g;
			tint_b_ = tint_b;
			rainbow_secs_ = rainbow_secs > 0 ? rainbow_secs : 0;
			sky_elapsed_ms_ = sky_elapsed_ms > 0 ? sky_elapsed_ms : (sky_elapsed_sec > 0 ? sky_elapsed_sec * 1000 : 0);
			sky_token_ = sky_token;

			// snap（进图）时彩虹/闪电计时用精确值；本端不消费秒数，留字段供后续层。
			(void)sky_elapsed_sec;
		}

		void State::tick(int64_t dt_ms)
		{
			if (!has_state_)
				return;
			// 粒子/闪电相位必须始终推进。
			// FLAG_FROZEN（GM 用 !weather HH:MM 或 web override 冻结时钟时服务端会带）
			// 只应冻结"游戏时钟"，绝不能冻结天气动画——否则雨雪粒子相位恒定、
			// 整屏粒子静止不动（用户 2026-11-20 反馈"粒子是不是不是动态的"，实测复现）。
			// sky_elapsed_ms 语义 = 当前天空已持续时长，冻结时钟时它照样在走。
			if (sky_elapsed_ms_ >= 0)
				sky_elapsed_ms_ += static_cast<int>(dt_ms);
			if ((flags_ & FLAG_FROZEN) || ms_per_game_minute_ <= 0)
				return;
			// dt_ms 累加到分钟：以浮点累计避免小 dt 被整除丢弃。
			static double carry = 0.0;
			carry += static_cast<double>(dt_ms) / static_cast<double>(ms_per_game_minute_);
			const int whole = static_cast<int>(carry);
			if (whole != 0)
			{
				minute_ = (minute_ + whole) % 1440;
				carry -= whole;
			}
			if (rainbow_secs_ > 0)
				rainbow_secs_ -= static_cast<int>(dt_ms / 1000);
			if (rainbow_secs_ < 0)
				rainbow_secs_ = 0;
		}

		void State::clear()
		{
			*this = State{};
		}
	}
}
