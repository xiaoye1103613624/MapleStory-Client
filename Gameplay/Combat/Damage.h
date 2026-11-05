////////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace ms
{
	// 伤害数值与封包契约的单一定义处。
	//
	// 背景：原生 BeiDou.exe 的 CalcDamage 把单段伤害硬钳在 199999.0，
	// 随后 __ftol 转 int（IDA 证据见 docs/features/damage-calc-cap-parity-2026-11/notes/r001）。
	// 本轮按用户决议把上限提到 2^53：这是 double 能精确表示的最大整数
	// （与 Kaentake/ijl15 damagelong 的 kExactIntLimit 一致）。
	//
	// 12 字节/行契约（客户端与 s9 必须逐字节一致）：
	//   int32 vanillaLine  未溢出写真值，溢出写 INT32_MAX 哨兵；暴击置 0x80000000 位
	//   int32 realLo       真实非负 int64 的低 32 位（小端）
	//   int32 realHi       真实非负 int64 的高 32 位（小端）
	// 暴击符号位**只**承载在 vanillaLine，真实值字段永远非负。
	namespace damage
	{
		constexpr int64_t EXACT_INT_CAP = 9007199254740992LL;		// 2^53
		constexpr double EXACT_INT_CAP_D = 9007199254740992.0;
		constexpr int32_t CRIT_FLAG = std::numeric_limits<int32_t>::min();

		using DamageLine = std::pair<int64_t, bool>;
		using DamageLines = std::vector<DamageLine>;

		// 服务端 0x3730 同步的破功值；-1 = 未同步。用 inline 函数的局部静态
		// 保证全程序单一实例，避免 Mob.cpp 反向依赖 IO/Window.h（会引入 SDL/GL）。
		inline int64_t& server_limit_break()
		{
			static int64_t value = -1;
			return value;
		}

		inline void set_server_limit_break(int64_t value)
		{
			server_limit_break() = value;
		}

		// 生效上限 = min(配置 cap, 服务端破功值, 2^53)；破功值未下发时忽略该项。
		inline int64_t effective_cap(int64_t configured)
		{
			int64_t cap = configured > 0 ? std::min<int64_t>(configured, EXACT_INT_CAP)
				: EXACT_INT_CAP;
			int64_t synced = server_limit_break();

			if (synced > 0)
				cap = std::min<int64_t>(cap, synced);

			return cap;
		}

		// 唯一饱和函数：double 公式的结果先夹到 [0, cap]，再取整为 int64。
		inline int64_t clamp_damage(double raw, int64_t cap)
		{
			int64_t limit = cap > 0 ? std::min<int64_t>(cap, EXACT_INT_CAP) : EXACT_INT_CAP;

			if (std::isnan(raw))
				return 0;

			if (raw <= 0.0)
				return 0;

			if (raw >= EXACT_INT_CAP_D || std::isinf(raw))
				return limit;

			return std::min<int64_t>(static_cast<int64_t>(raw), limit);
		}

		// 12 字节模式的 int32 兼容字段。
		inline int32_t vanilla_line(int64_t value, bool critical)
		{
			int64_t safe = std::max<int64_t>(0, std::min<int64_t>(value, EXACT_INT_CAP));
			int32_t base = safe > std::numeric_limits<int32_t>::max()
				? std::numeric_limits<int32_t>::max()
				: static_cast<int32_t>(safe);

			if (critical)
				base |= CRIT_FLAG;

			return base;
		}

		inline bool vanilla_is_critical(int32_t vanilla)
		{
			return vanilla < 0;
		}

		// legacy 4 字节/行：写包前必须压回 int32，避免溢出变成负数被误判成暴击。
		inline int32_t legacy_line(int64_t value)
		{
			int64_t safe = std::max<int64_t>(0, std::min<int64_t>(value, EXACT_INT_CAP));

			return static_cast<int32_t>(
				std::min<int64_t>(safe, std::numeric_limits<int32_t>::max()));
		}
	}
}
