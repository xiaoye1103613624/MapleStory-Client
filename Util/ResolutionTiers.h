//////////////////////////////////////////////////////////////////////////////////
//	ResolutionTiers — 多分辨率档位服务（头文件内实现）
//
//	档位表与顺序直接对齐已落地的插件实现
//	E:/project/BeiDou-ijl15/ezorsia/compat/rs/rs.cpp::rs_tier_dims（L49-63），
//	反查规则对齐 rs_tier_from_dims（L65-76）。全头文件 inline 实现是为了不新增
//	.cpp、不改 MapleStory.vcxproj（该工程文件有并发改动史，R28-B4 已明确回避）。
//
//	语义：window.width/height 是登录态基准（恒 1280×720）；进入游戏后按
//	window.game_tier / window.game_width / window.game_height 切换。档位合法时以
//	档位表为准（避免配置里宽高与档位不一致），否则用宽高；缺键时回落登录尺寸。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Constants.h"

#include <cstdint>

namespace ms
{
	namespace ResolutionTiers
	{
		struct Tier
		{
			int16_t width;
			int16_t height;
		};

		// 登录/选角/世界/频道视口：UI/Login.img/Common/frame 的原生尺寸，永不随档位变化。
		inline constexpr int16_t LOGIN_W = 1280;
		inline constexpr int16_t LOGIN_H = 720;

		inline constexpr int TIER_COUNT = 9;
		// 255 = 未显式选档（使用 window.game_width/game_height，兼容旧配置）。
		inline constexpr uint8_t TIER_NONE = 255;

		// 顺序与 rs_tier_dims 完全一致；tier 5 = 1280×720 恰好等于登录尺寸，
		// 不能因为「与登录同宽高」就误判成登录态（rs.cpp L1505-1509 的黑屏坑）。
		inline constexpr Tier TIERS[TIER_COUNT] =
		{
			{ 800, 600 },
			{ 1024, 768 },
			{ 1366, 768 },
			{ 1600, 900 },
			{ 1920, 1080 },
			{ 1280, 720 },
			{ 1440, 900 },
			{ 1680, 1050 },
			{ 2560, 1440 }
		};

		inline constexpr const char* LABELS[TIER_COUNT] =
		{
			"800 x 600", "1024 x 768", "1366 x 768", "1600 x 900", "1920 x 1080",
			"1280 x 720", "1440 x 900", "1680 x 1050", "2560 x 1440"
		};

		inline bool valid(int tier)
		{
			return tier >= 0 && tier < TIER_COUNT;
		}

		inline bool dims_from_tier(int tier, int16_t& out_width, int16_t& out_height)
		{
			if (!valid(tier))
				return false;

			out_width = TIERS[tier].width;
			out_height = TIERS[tier].height;
			return true;
		}

		// 精确匹配优先；无精确档位时按插件阈值反查最接近的一档（用于下拉显示，
		// 不用于决定实际视口 —— 实际视口只认档位表或显式 game_width/height）。
		inline int tier_from_dims(int16_t width, int16_t height)
		{
			for (int i = 0; i < TIER_COUNT; ++i)
			{
				if (TIERS[i].width == width && TIERS[i].height == height)
					return i;
			}

			if (width >= 2500 && height >= 1400) return 8;
			if (width >= 1900 && height >= 1000) return 4;
			if (width >= 1680 && height >= 1000) return 7;
			if (width >= 1600 && height >= 880) return 3;
			if (width >= 1440 && height >= 880) return 6;
			if (width >= 1360 && height >= 700) return 2;
			if (width >= 1280 && height >= 700) return 5;
			if (width >= 1000 && height >= 700) return 1;
			return 0;
		}

		inline const char* label(int tier)
		{
			return valid(tier) ? LABELS[tier] : "";
		}

		// 档位大于桌面时回落到「不超过桌面的最大可用档」；若连 800×600 都放不下返回 -1。
		// 不自动切全屏、不允许窗口越界（用户决策 D2）。
		inline int clamp_to_desktop(int tier, int16_t max_width, int16_t max_height)
		{
			int16_t width = 0;
			int16_t height = 0;

			if (dims_from_tier(tier, width, height)
				&& width <= max_width && height <= max_height)
				return tier;

			int best = -1;
			int64_t best_area = -1;

			for (int i = 0; i < TIER_COUNT; ++i)
			{
				const int16_t w = TIERS[i].width;
				const int16_t h = TIERS[i].height;

				if (w > max_width || h > max_height)
					continue;

				const int64_t area = static_cast<int64_t>(w) * h;

				if (area > best_area)
				{
					best_area = area;
					best = i;
				}
			}

			return best;
		}

		// 成对写入视口尺寸：只改一半会产生一帧的中间态（Window::check_events 每帧读 Constants）。
		inline void apply_viewsize(int16_t width, int16_t height)
		{
			Constants::Constants::get().set_viewsize(width, height);
		}

		// 从配置项解析进图视口：tier 合法时以档位表为准，否则用 fallback 宽高
		// （window.game_width/height，缺键时已由 Configuration::load 规整为登录尺寸）。
		// 超桌面时回落到「不超过桌面的最大可用档」，不自动切全屏（用户决策 D2）。
		// 返回实际使用的 tier；-1 表示连 800×600 都放不下，调用方应保持当前视口。
		inline int resolve_game_size(int tier, int16_t fallback_w, int16_t fallback_h,
			int16_t max_w, int16_t max_h, int16_t& out_w, int16_t& out_h)
		{
			if (valid(tier))
				dims_from_tier(tier, out_w, out_h);
			else
			{
				out_w = fallback_w;
				out_h = fallback_h;
			}

			if (out_w <= max_w && out_h <= max_h)
				return valid(tier) ? tier : tier_from_dims(out_w, out_h);

			const int requested = valid(tier) ? tier : tier_from_dims(out_w, out_h);
			const int clamped = clamp_to_desktop(requested, max_w, max_h);

			if (clamped < 0)
				return -1;

			dims_from_tier(clamped, out_w, out_h);
			return clamped;
		}
	}
}
