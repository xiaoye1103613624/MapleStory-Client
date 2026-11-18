//////////////////////////////////////////////////////////////////////////////////
// 天气系统 wire 类型定义（原生源码移植自 bundle client/weather.h）
//
// 服务端权威时钟与天空，客户端只负责渲染。0x373D 是重同步包，不是 tick：
// 进图一包 + 天空变化时一包，两包之间客户端自行推进本地时钟。
//
// WIRE（opcode 之后 26 字节，逐字段对齐服务端 WeatherPackets.java:82-109）：
//   short  minuteOfDay       0..1439
//   int    msPerGameMinute   每游戏分钟真实毫秒
//   byte   sky               SKY_*
//   byte   flags             bit0=snap bit1=frozen bit2=baresky
//   int    skyElapsedSec     legacy 占位（本端不消费但必须跳过，删掉后面全偏移）
//   byte   tintR / tintG / tintB
//   short  rainbowSecsLeft
//   byte   palette           Palette（客户端调色板 selector）
//   int    skyElapsedMs      精确毫秒（闪电同步预留）
//   int    skyToken          每次天空的随机种子（预留）
// 未知 id 一律降级到 CLEAR / DEFAULT，保证服务端不发 0x373D 时渲染 stock。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace ms
{
	namespace weather
	{
		// 编号即 wire format，由服务端 WeatherProfile.java 拥有；不可重排，只能追加。
		enum class Sky : uint8_t
		{
			CLEAR = 0,
			RAIN = 1,
			SNOW = 2,
			OVERCAST = 3,
			STORM = 4,
			BLIZZARD = 5,
			LEAVES = 6,
			BLOSSOM = 7,
			SANDSTORM = 8,
			COUNT = 9
		};

		// 区域调色板 id，顺序是服务端 WeatherPalette.java 的 wire 契约。
		// Named PaletteId (not Palette) to avoid clashing with WeatherConfig::Palette
		// row struct and with Windows/SDL palette macros/types.
		enum class PaletteId : uint8_t
		{
			EL_NATH = 0,
			RIEN,
			MUSHROOM_SHRINE,
			ELLINIA,
			PERION,
			KERNING_CITY,
			SHOWA,
			ORBIS,
			MU_LUNG,
			ARIANT,
			SLEEPYWOOD,
			AQUA_ROAD,
			LEAFRE,
			LUDIBRIUM,
			FLORINA,
			AMORIA,
			LITH_HARBOUR,
			MAGATIA,
			NAUTILUS,
			HENESYS,
			EREVE,
			TEMPLE_OF_TIME,
			ELLIN_FOREST,
			NEW_LEAF_CITY,
			FORMOSA,
			ZIPANGU,
			DEFAULT,
			COUNT
		};

		enum Flags : uint8_t
		{
			FLAG_SNAP = 0x01,     // 直接跳到目标，不做淡入淡出（进图用）
			FLAG_FROZEN = 0x02,   // GM 冻结时钟：客户端停止推进本地副本，避免抖动
			FLAG_BARESKY = 0x04   // 测试：隐藏地图自身天空，只留注入层
		};

		inline Sky sanitize_sky(uint8_t raw)
		{
			return raw < static_cast<uint8_t>(Sky::COUNT) ? static_cast<Sky>(raw) : Sky::CLEAR;
		}

		inline PaletteId sanitize_palette(uint8_t raw)
		{
			return raw < static_cast<uint8_t>(PaletteId::COUNT)
				? static_cast<PaletteId>(raw)
				: PaletteId::DEFAULT;
		}
	}
}
