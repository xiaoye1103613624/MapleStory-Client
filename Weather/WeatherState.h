//////////////////////////////////////////////////////////////////////////////////
// 天气世界状态单例（原生源码客户端，单线程主循环）
//
// 服务端权威：0x373D 是重同步包，落地到这里；两包之间客户端按 msPerGameMinute
// 推进本地时钟副本。与 DLL 版不同，原生端是单线程（MapleStory 主循环），不需要
// atomics/影子态/g_bNetDirty 三件套。
//
// 服务端不发 0x373D 时：has_state()==false，渲染层走 stock（identity tint、alpha 0）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "WeatherTypes.h"

#include <cstdint>


namespace ms
{
	namespace weather
	{
		class State
		{
		public:
			static State& get();

			// 由 WeatherSyncHandler 调用，逐字段落地 0x373D。minute/sky/tint/palette
			// 做非法值降级；snap 置位时重置淡入进度。
			void apply(int minute_of_day, int ms_per_game_minute, uint8_t sky_raw,
				uint8_t flags, int sky_elapsed_sec, int rainbow_secs,
				uint8_t tint_r, uint8_t tint_g, uint8_t tint_b,
				uint8_t palette_raw, int sky_elapsed_ms, uint32_t sky_token);

			// 主循环每帧调用：未冻结时按 dt 推进本地分钟副本（纯客户端，服务端发下一包会重同步）。
			void tick(int64_t dt_ms);

			void clear();  // 换线/断线回 stock

			bool has_state() const { return has_state_; }
			int minute_of_day() const { return minute_; }
			Sky sky() const { return sky_; }
			PaletteId palette_id() const { return palette_; }
			uint8_t flags() const { return flags_; }
			uint8_t tint_r() const { return tint_r_; }
			uint8_t tint_g() const { return tint_g_; }
			uint8_t tint_b() const { return tint_b_; }
			int rainbow_secs_left() const { return rainbow_secs_; }
			int sky_elapsed_ms() const { return sky_elapsed_ms_; }
			uint32_t sky_token() const { return sky_token_; }
			int ms_per_game_minute() const { return ms_per_game_minute_; }

		private:
			State() = default;
			bool has_state_ = false;
			int minute_ = 720;              // 正午（bundle weather.cpp:150-154 的理由）
			int ms_per_game_minute_ = 10000; // 4 现实小时一天 = 10000ms/游戏分钟
			Sky sky_ = Sky::CLEAR;
			PaletteId palette_ = PaletteId::DEFAULT;
			uint8_t flags_ = 0;
			uint8_t tint_r_ = 0x4A, tint_g_ = 0x5A, tint_b_ = 0x8C;
			int rainbow_secs_ = 0;
			int sky_elapsed_ms_ = 0;
			uint32_t sky_token_ = 0;
		};
	}
}

