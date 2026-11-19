//////////////////////////////////////////////////////////////////////////////////
// 天空图鉴门禁（把插件的 weather_skytable.inc 原样移植为 C++ 表）。
// 依据：BeiDou-ijl15/ezorsia/weather/weather_skytable.inc（每行一个 backmost
// (bS,no)，代表"开放式天空"）+ weather.cpp:2073-2100 FieldHasSky 的判定顺序：
//   1) IsExcludedMap(mapid) 先短路
//   2) ReadBackmostBase() 取 back/0 的 bS + no
//   3) 命中本表 → 该图有天空，才允许注入天空/雨层
// 不在表内的图（室内、树洞、Kerning Square 等）一律不注入，避免天空层盖住室内背景。
// 与服务端 WeatherMapVisibility.java 的分类必须一致（插件同款双份约定）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <cstring>

namespace ms
{
	namespace weather
	{
		struct SkyBase { const char* bs; int32_t no; };

		// weather_skytable.inc 原样（59 行）；shineWood2/department 已在插件侧整键剔除。
		static const SkyBase kSkyBases[] = {
    { "sandBeach", 0 },
    { "toyCastle", 0 },
    { "timeTemple", 0 },
    { "grassySoil", 0 },
    { "minar", 0 },
    { "dragonValley", 0 },
    { "skyStation", 0 },
    { "midForest", 0 },
    { "mureung", 0 },
    { "dryRock", 0 },
    { "shineWood", 0 },
    { "ereb", 0 },
    { "snowyLightrock", 0 },
    { "backcho", 0 },
    { "desert", 0 },
    { "omegaSector", 0 },
    { "mushCatle", 0 },
    { "Rien", 0 },
    { "darkMountain", 0 },
    { "event", 20 },
    { "vicportTown", 0 },
    { "folkvillige", 0 },
    { "dryRock2", 0 },
    { "highLand_MY", 0 },
    { "ancientForest", 0 },
    { "folkvillige", 23 },
    { "WeddingG", 0 },
    { "snowyDarkrock", 0 },
    { "citySG", 0 },
    { "sunsetCity", 0 },
    { "Christmas", 2 },
    { "metroCity", 0 },
    { "nautilusPort", 0 },
    { "ThemeBack1", 0 },
    { "eurel", 0 },
    { "ShanghaiCN", 0 },
    { "globalJP", 0 },
    { "shineWood3", 0 },
    { "desert", 29 },
    { "nightDesert", 0 },
    { "boatquay1SG", 0 },
    { "globalJP3_ghost", 0 },
    { "grassySoil", 8 },
    { "halloween", 0 },
    { "minar", 31 },
    { "neoCity", 10 },
    { "CBDfieldSG", 0 },
    { "Rien", 18 },
    { "folkvillige", 16 },
    { "boatquay2SG", 0 },
    { "boatquay3SG", 0 },
    { "changiAirSG", 0 },
    { "desert", 1 },
    { "eurel", 27 },
    { "neoCity", 0 },
    { "nightDesert", 27 },
    { "thai", 0 },
    { "QQ206600672_Z1", 1 },
    { "QQ206600672_Z1", 2 },
		};

		inline bool is_sky_base(const char* bs, int32_t no)
		{
			if (bs == nullptr || bs[0] == '\0' || no < 0)
				return false;
			for (const SkyBase& k : kSkyBases)
				if (k.no == no && std::strcmp(k.bs, bs) == 0)
					return true;
			return false;
		}
	}
}
