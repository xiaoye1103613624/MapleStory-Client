//////////////////////////////////////////////////////////////////////////////////
// 天气表现配置层（D4：profiles/tint ramps 运行时可覆盖 + 静态兜底）
//
// 服务端只发 sky id / palette id / tint / 时钟（wire 见 WeatherTypes.h），
// 每种天空"长什么样"的 RGB/alpha/fade 全部在本层。默认表逐字移植自 bundle
// client/weather_profiles.inc 与 weather_palettes.inc，保证服务端不发 0x373D 时
// 渲染 stock（identity tint + alpha 0）。
//
// 行序即 wire format：static_assert 锁死枚举 id 与数组下标一致；中间插行会让
// 所有后续 sky/palette 静默错位，禁止重排，只能追加。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "WeatherTypes.h"

#include <cstdint>

namespace ms
{
	namespace weather
	{
		struct Profile
		{
			float cloud = 0.0f;        // 云层目标 alpha 0..1
			float rain = 0.0f;         // 雨层目标 alpha 0..1
			int native_item = 0;       // BlowWeather 美术指针 item id，0 表示不用原生粒子
			uint8_t r = 0x4A, g = 0x5A, b = 0x8C;  // 该天空淡向的夜色
			float boost = 0.0f;        // 与时刻无关的额外压暗
			float fade_step = 0.015f;  // 每帧趋近步长
			uint8_t sound = 0;         // 0 none / 1 rain / 2 wind（音效层预留）
		};

		// TintRamp — 避开 Windows/SDL 的 Palette/palette 宏与类型污染。
		struct TintRamp
		{
			uint8_t dusk_r = 0, dusk_g = 0, dusk_b = 0;
			uint8_t night_r = 0, night_g = 0, night_b = 0;
		};

		class Config
		{
		public:
			static Config& get();

			const Profile& profile(Sky sky) const;
			const TintRamp& tint_ramp(PaletteId id) const;

			void apply_profiles(uint32_t version, const Profile* rows, size_t count);
			void apply_tint_ramps(uint32_t version, const TintRamp* rows, size_t count);
			uint32_t profile_version() const { return profile_version_; }
			uint32_t tint_ramp_version() const { return tint_ramp_version_; }

			// 现实化强度档（0=轻/1=中/2=强），由 0x3741 下发。
			// 未收到包时用参考 sky 语义默认（rain/snow=中，storm/blizzard/sandstorm=强）。
			uint8_t intensity(Sky sky) const { return intensities_[static_cast<size_t>(sky)]; }
			void apply_intensities(uint32_t version, const uint8_t* rows, size_t count);
			uint32_t intensity_version() const { return intensity_version_; }

			void reset();

		private:
			Config();
			Profile profiles_[static_cast<size_t>(Sky::COUNT)];
			TintRamp tint_ramps_[static_cast<size_t>(PaletteId::COUNT)];
			// 参考语义默认：CLEAR/Rain/SNOW/OVERCAST/LEAVES/BLOSSOM=中(1)，
			// STORM/BLIZZARD/SANDSTORM=强(2)。
			uint8_t intensities_[static_cast<size_t>(Sky::COUNT)] = {1, 1, 1, 1, 2, 2, 1, 1, 2};
			uint32_t profile_version_ = 0;
			uint32_t tint_ramp_version_ = 0;
			uint32_t intensity_version_ = 0;
			bool profiles_overridden_ = false;
			bool tint_ramps_overridden_ = false;
		};
	}
}
