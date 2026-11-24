//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../PacketHandler.h"

namespace ms
{
	class PingHandler : public PacketHandler
	{
		void handle(InPacket&) const override;
	};

	// 破功值（伤害上限）同步。
	// 对应服务端 SendOpcode.LIMIT_BREAK_SYNC(0x3730)，负载只有一个 int64。
	// 数值只用于窗口标题显示，收到后交给 Window 保存。
	class LimitBreakSyncHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 套装/战斗面板同步（SendOpcode.SET_ITEM_FINAL_DAMAGE = 0x175）
	// 仅刷新属性窗显示，不改客户端伤害公式。
	class SetItemFinalDamageHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 套装展示文案/映射下发（SendOpcode.SET_ITEM_SKILL_BONUS = 0x176，2026-11-18）
	// 服务端权威：填 SetItemData 文案缓存 + item→set 映射，SetTooltip 优先取此。
	class SetItemSkillBonusHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 技能窗 (+N)（SendOpcode.SET_SKILL_BONUS = 0x17A）
	class SetSkillBonusHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 技改方案覆盖包（SendOpcode.SKILL_STAT_OVERRIDE = 0x3745）
	// wire: short skillCount + { int skillId, byte levelCount,
	//       每级 { byte level, byte fieldCount, 每字段 { byte keyIdx, int value } } }
	// 空表（skillCount=0）= 清覆盖，回退 Skill.nx。
	class SkillStatOverrideHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 装备成长 companion tip（SendOpcode.EQUIP_GROWTH_TIP = 0x17B）
	class EquipGrowthTipHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 天气世界状态同步（SendOpcode.WEATHER_SYNC = 0x373D，2026-11-18）
	// 服务端权威时钟/天空：落地到 weather::State；宽容解码兼容老 19B 包。
	class WeatherSyncHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 路灯预览（SendOpcode.LAMP_PREVIEW = 0x373F）：GM !lamp 本地放置/清除。
	// 落地到 weather::Lamps::apply_preview（白天也可见，方便量坐标）。
	class LampPreviewHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 天气强度档（SendOpcode.WEATHER_INTENSITY = 0x3741）：
	// wire = short version | byte[9] intensities(0轻/1中/2强)，落地到 weather::Config。
	class WeatherIntensityHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 2026-11-23 V2：气象现象强度/游戏日期（SendOpcode.WEATHER_ELEMENT = 0x3742）。
	// wire = short version | short gameDate(MMdd) | byte[16] elements(0..3)，落地 weather::State。
	class WeatherElementHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 侧栏菜单同步（SendOpcode.SIDE_MENU_SYNC = 0x3743）：
	// wire = int revision | maple-string UTF-8 JSON（side_menu schema）。
	// 落盘 side_menu.json 并热重载 UISideIconBar；revision 不新则忽略。
	class SideMenuSyncHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 伤害皮肤 picker 下行（S→C 0x3746..0x3749），落 DamageSkinState，由 picker 读取。
	class DamageSkinCatalogHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
	class DamageSkinInventoryHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
	class DamageSkinResultHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
	class DamageSkinBroadcastHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 美容院下行（S→C 0x174）：byte respType(0 OPEN / 1 DATA) + 造型槽数据。
	class BeautyResultHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};
}