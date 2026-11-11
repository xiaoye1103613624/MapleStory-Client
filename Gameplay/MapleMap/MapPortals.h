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

#include "Portal.h"

#include <unordered_map>

namespace ms
{
	// Collection of portals on a map
	// Draws and updates portals
	// Also contains methods for using portals and obtaining spawn points
	class MapPortals
	{
	public:
		static void init();

		MapPortals(nl::node source, int32_t mapid);
		MapPortals();

		void update(Point<int16_t> playerpos);
		void draw(Point<int16_t> viewpos, float inter) const;

		// 传送门触发范围（083 依据：按键通道 = `CPortalList::FindPortal @0x712ab1`，碰触通道 = `CPortalList::FindPortal_Collision @0x712b61`，入口 = `CUserLocal::CheckPortal_Collision @0x94dac6`；旧注释写「v95 CPortal::CheckKey / ProcessPortal」属依据链污染 —— 083 全库 `*CPortal@@*` / `*CPortal::*` 符号命中为 0，见 `tools/_ida_r93/portal_family.json`）：
		//   TOUCH_ONLY —— 行走经过即触发，仅 TOUCH / SCRIPTED_TOUCH 类（弹跳/碰触门）。
		//   ALL        —— 按上键主动交互，放行全部可见/隐藏门。
		enum class PortalScope : uint8_t
		{
			TOUCH_ONLY,
			ALL
		};

		Portal::WarpInfo find_warp_at(Point<int16_t> playerpos, PortalScope scope = PortalScope::TOUCH_ONLY);

		Point<int16_t> get_portal_by_id(uint8_t id) const;
		Point<int16_t> get_portal_by_name(const std::string& name) const;

	private:
		// 显式门共享动画（MapHelper portal/game/pv）
		static Animation regular_anim;
		// 隐藏门三态（ph/default）
		static Animation ph_start;
		static Animation ph_continue;
		static Animation ph_exit;
		// 脚本隐藏门三态（psh/default；样式 1..4 本版统一用 default）
		static Animation psh_start;
		static Animation psh_continue;
		static Animation psh_exit;

		std::unordered_map<uint8_t, Portal> portals_by_id;
		std::unordered_map<std::string, uint8_t> portal_ids_by_name;

		static const int16_t WARPCD = 48;
		int16_t cooldown;
	};
}
