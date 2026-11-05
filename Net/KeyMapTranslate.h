//////////////////////////////////////////////////////////////////////////////////
//	KeyMapTranslate.h — 键位 action id 的收/发包边界翻译（open-issues §F 方案甲）
//
//	背景（docs/features/ui-v83-window-restore/notes/open-issues.md §F）：
//	s9/v83 服务端 KEYMAP 下发的 action 编号是 v83 段；本客户端 KeyAction.h 的
//	21+ 段沿用了较新版本编号，直接透传导致四条玩家可感知的键位故障：
//	  · `b`(22=v83 MONSTERBOOK) 误发 EnterCashShopPacket（被传进商城/换图）
//	  · "`"(23=v83 CASHSHOP) 只把聊天目标改成联盟频道，不开商城
//	  · ";"(27=v83 MEDAL) 误开 Boss 队成员列表（开错窗）
//	  · "5"(24=v83 TOALLIANCE) 落在枚举空位 → 死键
//
//	方案甲约束：KeyAction.h 枚举与 UIStateGame 的 28 个 case 一字不动，
//	只在此边界做双向翻译：
//	  收包 KeymapHandler::handle      → from_server（v83 编号 → 本枚举，落到已有 case 上）
//	  发包 ChangeKeyMapPacket         → to_server （本枚举 → v83 编号，保证服务端
//	                                    存档里仍是 v83 语义，重登不串号）
//
//	未列出的 id 双向原样透传；四处映射保持双射。按值翻译是安全的：MENU 型
//	action 走 0..27+ 编号段，FACE=50..54、FACESLOT=100..106、技能/道具绑定用
//	大数 id，均与本表的 22/23/24/27 无交集。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../IO/KeyAction.h"

namespace ms
{
	namespace KeyAction
	{
		inline Id from_server(int32_t action)
		{
			switch (action)
			{
				case 22: return MONSTERCOLLECTION;		// v83 MONSTERBOOK(22) → 本枚举 42（UIMonsterBook 入口）
				case 23: return CASHSHOP;				// v83 CASHSHOP(23) → 本枚举 22（EnterCashShopPacket，s9 已处理）
				case 24: return ALLIANCECHAT;			// v83 TOALLIANCE(24) → 本枚举 23（联盟聊天目标）
				case 27: return static_cast<Id>(28);	// v83 MEDAL(27) → 本包无勋章窗，压成无 case 的枚举空位，
														//   灭掉 BOSSPARTY(27)「; 开错窗」；勋章窗落地后改映射
				default: return static_cast<Id>(action);
			}
		}

		inline int32_t to_server(Id action)
		{
			switch (action)
			{
				case MONSTERCOLLECTION: return 22;
				case CASHSHOP: return 23;
				case ALLIANCECHAT: return 24;
				case static_cast<Id>(28): return 27;
				default: return static_cast<int32_t>(action);
			}
		}
	}
}
