//////////////////////////////////////////////////////////////////////////////////
// Weather world state (single-threaded, no atomics)
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
			rainbow_ms_carry_ = 0.0;
			sky_elapsed_ms_ = sky_elapsed_ms > 0 ? sky_elapsed_ms : (sky_elapsed_sec > 0 ? sky_elapsed_sec * 1000 : 0);
			sky_token_ = sky_token;

			// legacy skyElapsedSec is skipped by the handler; keep signature field unused.
			(void)sky_elapsed_sec;
		}

		void State::apply_elements(uint8_t version, int game_date, const uint8_t* rows, size_t count)
		{
			// 2026-11-23 V2 P0：0x3742。版本号回退则忽略（防旧包覆盖新配置）。
			if (version < element_version_)
				return;
			element_version_ = version;
			if (game_date > 0)
				game_date_ = game_date;
			for (size_t i = 0; i < EL_COUNT; i++)
				elements_[i] = (rows && i < count) ? rows[i] : 0;
		}

		void State::set_field_gate(bool allow_falling, bool allow_tint)
		{
			allow_falling_ = allow_falling;
			allow_tint_ = allow_tint;
		}

		void State::tick(int64_t dt_ms)
		{
			if (!has_state_)
				return;

			// Particle/lightning phase always advances.
			// FLAG_FROZEN freezes only the game clock, not weather animation.
			if (sky_elapsed_ms_ >= 0)
				sky_elapsed_ms_ += static_cast<int>(dt_ms);

			if (!((flags_ & FLAG_FROZEN) || ms_per_game_minute_ <= 0))
			{
				// Accumulate fractional game-minutes so small dt is not truncated away.
				static double carry = 0.0;
				carry += static_cast<double>(dt_ms) / static_cast<double>(ms_per_game_minute_);
				const int whole = static_cast<int>(carry);
				if (whole != 0)
				{
					minute_ = (minute_ + whole) % 1440;
					carry -= whole;
				}
			}

			// Rainbow remaining: accumulate ms then convert to seconds (8ms frames OK).
			if (rainbow_secs_ > 0 && dt_ms > 0)
			{
				rainbow_ms_carry_ += static_cast<double>(dt_ms);
				const int whole_sec = static_cast<int>(rainbow_ms_carry_ / 1000.0);
				if (whole_sec > 0)
				{
					rainbow_secs_ -= whole_sec;
					rainbow_ms_carry_ -= static_cast<double>(whole_sec) * 1000.0;
					if (rainbow_secs_ < 0)
						rainbow_secs_ = 0;
				}
			}
		}

		void State::clear()
		{
			*this = State{};
		}
	}
}
