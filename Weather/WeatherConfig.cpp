//////////////////////////////////////////////////////////////////////////////////
// 天气表现配置层实现：默认表逐字移植自 bundle weather_profiles.inc / weather_palettes.inc。
// 表行序 = wire format，static_assert 锁死枚举计数与表长度一致。
//////////////////////////////////////////////////////////////////////////////////
#include "WeatherConfig.h"

#include <cstring>

namespace ms
{
	namespace weather
	{
		static const Profile kDefaultProfiles[] = {
			{ 0.00f, 0.00f,       0, 0x4A, 0x5A, 0x8C, 0.00f, 0.015f, 0 }, // CLEAR
			{ 0.85f, 1.00f,       0, 0x42, 0x50, 0x78, 0.18f, 0.015f, 1 }, // RAIN
			{ 0.70f, 0.00f,       0, 0x60, 0x6A, 0x92, 0.10f, 0.015f, 0 }, // SNOW
			{ 1.00f, 0.00f,       0, 0x58, 0x5E, 0x74, 0.22f, 0.012f, 2 }, // OVERCAST
			{ 1.00f, 1.00f,       0, 0x30, 0x38, 0x58, 0.38f, 0.008f, 1 }, // STORM
			{ 1.00f, 0.00f, 5120997, 0x6E, 0x76, 0x9C, 0.28f, 0.010f, 2 }, // BLIZZARD
			{ 0.35f, 0.00f, 5120998, 0x54, 0x54, 0x82, 0.04f, 0.015f, 2 }, // LEAVES
			{ 0.30f, 0.00f, 5120999, 0x50, 0x5C, 0x8E, 0.02f, 0.015f, 2 }, // BLOSSOM
			{ 0.00f, 0.00f, 5120996, 0x9E, 0x74, 0x46, 0.34f, 0.009f, 2 }, // SANDSTORM
		};

		static const TintRamp kDefaultTintRamps[] = {
			{ 0xB8,0xBC,0xD2, 0x41,0x50,0x8E }, // EL_NATH
			{ 0xB6,0xB8,0xD5, 0x48,0x52,0x94 }, // RIEN
			{ 0xB8,0x7E,0xAD, 0x60,0x54,0x92 }, // MUSHROOM_SHRINE
			{ 0x6B,0x91,0x83, 0x3D,0x66,0x6C }, // ELLINIA
			{ 0xA2,0x77,0x76, 0x63,0x55,0x82 }, // PERION
			{ 0xA0,0x64,0x7A, 0x6E,0x4A,0x7E }, // KERNING_CITY
			{ 0xAA,0x67,0x76, 0x74,0x49,0x7A }, // SHOWA
			{ 0x94,0x80,0xB8, 0x6A,0x50,0x90 }, // ORBIS
			{ 0xA2,0x78,0x96, 0x6B,0x4E,0x84 }, // MU_LUNG
			{ 0xC1,0x86,0x70, 0x6E,0x50,0x78 }, // ARIANT
			{ 0x5C,0x6A,0x7F, 0x2E,0x38,0x57 }, // SLEEPYWOOD
			{ 0x5A,0x75,0x96, 0x35,0x41,0x65 }, // AQUA_ROAD
			{ 0x5F,0x88,0x78, 0x3A,0x5E,0x58 }, // LEAFRE
			{ 0x82,0x70,0xB1, 0x57,0x4C,0x90 }, // LUDIBRIUM
			{ 0x68,0x99,0x9A, 0x40,0x64,0x6E }, // FLORINA
			{ 0x9B,0x7C,0xB7, 0x5E,0x54,0x90 }, // AMORIA
			{ 0x6B,0x91,0xA4, 0x44,0x60,0x7E }, // LITH_HARBOUR
			{ 0x75,0x7B,0xA5, 0x46,0x5A,0x85 }, // MAGATIA
			{ 0x6B,0x85,0xA4, 0x46,0x58,0x7F }, // NAUTILUS
			{ 0x80,0x8A,0xA8, 0x4C,0x5C,0x88 }, // HENESYS
			{ 0x80,0x86,0xB2, 0x4E,0x5A,0x96 }, // EREVE
			{ 0x7A,0x6F,0xAB, 0x55,0x4A,0x8C }, // TEMPLE_OF_TIME
			{ 0x5C,0x86,0x7B, 0x3C,0x5E,0x5C }, // ELLIN_FOREST
			{ 0xA0,0x64,0x7A, 0x6E,0x4A,0x7E }, // NEW_LEAF_CITY
			{ 0x6C,0x8A,0xA1, 0x44,0x5C,0x7E }, // FORMOSA
			{ 0x7A,0x70,0xAB, 0x4A,0x54,0x90 }, // ZIPANGU
			{ 0x7C,0x82,0xA6, 0x4A,0x5A,0x8C }, // DEFAULT
		};

		static_assert(sizeof(kDefaultProfiles) / sizeof(kDefaultProfiles[0]) == static_cast<size_t>(Sky::COUNT),
			"weather profile table row count must equal Sky::COUNT (wire format)");
		static_assert(sizeof(kDefaultTintRamps) / sizeof(kDefaultTintRamps[0]) == static_cast<size_t>(PaletteId::COUNT),
			"weather tint ramp table row count must equal PaletteId::COUNT (wire format)");

		Config& Config::get()
		{
			static Config instance;
			return instance;
		}

		Config::Config()
		{
			std::memcpy(profiles_, kDefaultProfiles, sizeof(kDefaultProfiles));
			std::memcpy(tint_ramps_, kDefaultTintRamps, sizeof(kDefaultTintRamps));
		}

		const Profile& Config::profile(Sky sky) const
		{
			const size_t idx = static_cast<size_t>(sky);
			return idx < static_cast<size_t>(Sky::COUNT) ? profiles_[idx] : profiles_[0];
		}

		const TintRamp& Config::tint_ramp(PaletteId id) const
		{
			const size_t idx = static_cast<size_t>(id);
			return idx < static_cast<size_t>(PaletteId::COUNT)
				? tint_ramps_[idx]
				: tint_ramps_[static_cast<size_t>(PaletteId::DEFAULT)];
		}

		void Config::apply_profiles(uint32_t version, const Profile* rows, size_t count)
		{
			if (!rows || version == 0)
				return;
			for (size_t i = 0; i < count && i < static_cast<size_t>(Sky::COUNT); ++i)
				profiles_[i] = rows[i];
			profiles_overridden_ = true;
			profile_version_ = version;
		}

		void Config::apply_tint_ramps(uint32_t version, const TintRamp* rows, size_t count)
		{
			if (!rows || version == 0)
				return;
			for (size_t i = 0; i < count && i < static_cast<size_t>(PaletteId::COUNT); ++i)
				tint_ramps_[i] = rows[i];
			tint_ramps_overridden_ = true;
			tint_ramp_version_ = version;
		}

		void Config::apply_intensities(uint32_t version, const uint8_t* rows, size_t count)
		{
			if (!rows || version == 0)
				return;
			for (size_t i = 0; i < count && i < static_cast<size_t>(Sky::COUNT); ++i)
				intensities_[i] = rows[i] > 2 ? 1 : rows[i];
			intensity_version_ = version;
		}

		void Config::reset()
		{
			std::memcpy(profiles_, kDefaultProfiles, sizeof(kDefaultProfiles));
			std::memcpy(tint_ramps_, kDefaultTintRamps, sizeof(kDefaultTintRamps));
			profiles_overridden_ = false;
			tint_ramps_overridden_ = false;
			profile_version_ = 0;
			tint_ramp_version_ = 0;
			// 强度恢复参考语义默认（不因断线/重连停留在上一次服务端的 DB 配置）
			{
				const uint8_t def[static_cast<size_t>(Sky::COUNT)] = {1, 1, 1, 1, 2, 2, 1, 1, 2};
				std::memcpy(intensities_, def, sizeof(def));
			}
			intensity_version_ = 0;
		}
	}
}
