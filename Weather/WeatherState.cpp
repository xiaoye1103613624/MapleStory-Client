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
			if (!has_state_ || (flags_ & FLAG_FROZEN) || ms_per_game_minute_ <= 0)
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
			if (sky_elapsed_ms_ >= 0)
				sky_elapsed_ms_ += static_cast<int>(dt_ms);
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
