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

	// 天气世界状态同步（SendOpcode.WEATHER_SYNC = 0x373D，2026-11-18）
	// 服务端权威时钟/天空：落地到 weather::State；宽容解码兼容老 19B 包。
	class WeatherSyncHandler : public PacketHandler
	{
		void handle(InPacket& recv) const override;
	};

	// 路灯预览（SendOpcode.LAMP_PREVIEW = 0x373F）：GM !lamp 本地放置/清除。
	// D1 核心闭环暂只落地解析与状态缓存，实际灯渲染走 WeatherLamps（后续层）。
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
}