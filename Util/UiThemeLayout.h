//////////////////////////////////////////////////////////////////////////////////
//	UiThemeLayout — ThemePack 策略（Strategy）接口与实现注册表
//
//	设计动机（用户 2026-11-18 要求）：主题切换做成适配器/策略模式，
//	支持 083 / 095 / 192 多版本共存——同一功能在不同主题下 UI 相同/不同的
//	资源根、布局常量、能力集由各自策略封装；新增版本 = 加一个策略 + 注册一行。
//
//	职责边界：本文件只描述「布局家族 + 资源根 + 布局常量」；协议/玩法永远
//	停留当前 s9/083 协议栈，主题只是表现层。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "UiTheme.h"   // ThemeId / LayoutFamily

#include "../MapleStory.h"
#include "../Template/Point.h"

#ifdef USE_NX
#include <nlnx/node.hpp>
#endif

#include <cstdint>

namespace ms
{
	namespace UiThemeLayout
	{
		/// 布局常量：由各版本策略提供，避免 UIStatusBar 里散落 if(theme==...) 魔数。
		struct LayoutMetrics
		{
			Point<int16_t> hud_base;        // HUD 布局基准原点（192 IDA: CLayoutMan::Init 512,577）
			Point<int16_t> quickslot_origin;// 快捷格首格原点（192 IDA s_ptShortKeyPos: (7,15)）
			int16_t quickslot_pitch = 33;   // 快捷格横向 pitch（192 IDA 实测 33px）
			int16_t quickslot_rows = 2;     // 快捷格排数
		};

		/// 主题布局策略接口。所有方法默认 const/noexcept-free（资源节点查询可能写诊断日志）。
		class IThemeLayout
		{
		public:
			virtual ~IThemeLayout() = default;

			virtual UiTheme::ThemeId id() const = 0;
			virtual UiTheme::LayoutFamily family() const = 0;

			/// 该策略依赖的资源包是否齐备（决定 can_use / 自动回退）。
			virtual bool available() const = 0;

			/// 资源根解析——替代 UiAssetResolve 里散落的 if/else。
			virtual nl::node status_bar_root() const = 0;
			virtual nl::node window_root() const = 0;
			virtual nl::node window4_root() const = 0;
			virtual nl::node shop_root() const = 0;

			/// 布局常量。
			virtual LayoutMetrics metrics() const = 0;
		};

		/// 策略注册表：按 ThemeId 取策略；单例、线程安全（只读、一次性注册）。
		const IThemeLayout* get(UiTheme::ThemeId id);
		/// 当前生效主题（resolve 后）对应的策略，永不返回 nullptr（Auto 回退 classic）。
		const IThemeLayout* active();
	}
}
