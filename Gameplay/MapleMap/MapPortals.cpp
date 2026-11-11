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
#include "MapPortals.h"

#include "../../Constants.h"

#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	MapPortals::MapPortals(nl::node src, int32_t mapid)
	{
		Portal::HideAnims ph{ &ph_start, &ph_continue, &ph_exit };
		Portal::HideAnims psh{ &psh_start, &psh_continue, &psh_exit };

		for (auto sub : src)
		{
			int8_t portal_id = string_conversion::or_default<int8_t>(sub.name(), -1);

			if (portal_id < 0)
				continue;

			Portal::Type type = Portal::typebyid(sub["pt"]);
			std::string name = sub["pn"];
			std::string target_name = sub["tn"];
			int32_t target_id = sub["tm"];
			Point<int16_t> position = { sub["x"], sub["y"] };

			bool intramap = target_id == mapid;
			const Animation* shared = nullptr;
			Portal::HideAnims hide{};

			if (Portal::is_hide_family(type))
			{
				// SCRIPTED_HIDDEN → psh；普通 HIDDEN → ph
				hide = (type == Portal::SCRIPTED_HIDDEN) ? psh : ph;
			}
			else if (type == Portal::REGULAR || type == Portal::TOUCH
				|| type == Portal::SCRIPTED || type == Portal::SCRIPTED_TOUCH)
			{
				shared = &regular_anim;
			}

			portals_by_id.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(portal_id),
				std::forward_as_tuple(shared, hide, type, name, intramap, position, target_id, target_name)
			);

			portal_ids_by_name.emplace(name, portal_id);
		}

		cooldown = WARPCD;
	}

	MapPortals::MapPortals()
	{
		cooldown = WARPCD;
	}

	void MapPortals::update(Point<int16_t> playerpos)
	{
		regular_anim.update(Constants::TIMESTEP);

		for (auto& iter : portals_by_id)
		{
			Portal& portal = iter.second;
			switch (portal.get_type())
			{
			case Portal::HIDDEN:
			case Portal::SCRIPTED_HIDDEN:
			case Portal::TOUCH:
			case Portal::SCRIPTED_TOUCH:
				portal.update(playerpos);
				break;
			default:
				break;
			}
		}

		if (cooldown > 0)
			cooldown--;
	}

	void MapPortals::draw(Point<int16_t> viewpos, float inter) const
	{
		for (auto& ptit : portals_by_id)
			ptit.second.draw(viewpos, inter);
	}

	Point<int16_t> MapPortals::get_portal_by_id(uint8_t portal_id) const
	{
		auto iter = portals_by_id.find(portal_id);

		if (iter != portals_by_id.end())
		{
			constexpr Point<int16_t> ABOVE(0, 30);

			return iter->second.get_position() - ABOVE;
		}
		else
		{
			return {};
		}
	}

	Point<int16_t> MapPortals::get_portal_by_name(const std::string& portal_name) const
	{
		auto iter = portal_ids_by_name.find(portal_name);

		if (iter != portal_ids_by_name.end())
			return get_portal_by_id(iter->second);
		else
			return {};
	}

	Portal::WarpInfo MapPortals::find_warp_at(Point<int16_t> playerpos, PortalScope scope)
	{
		if (cooldown > 0)
			return {};

		for (auto& iter : portals_by_id)
		{
			const Portal& portal = iter.second;
			Portal::Type t = portal.get_type();

			// 原生语义（083 `CPortalList::FindPortal @0x712ab1` 按键通道 / `CPortalList::FindPortal_Collision @0x712b61` 碰触通道，由 `CUserLocal::CheckPortal_Collision @0x94dac6` 按 portal type 分派；旧注释「v95 ...」依据链污染已纠偏）：
			//  * 自动触发通道（无按键）只放行 TOUCH / SCRIPTED_TOUCH（碰触门）；
			//  * REGULAR / SCRIPTED / INVISIBLE / WARP / HIDDEN 等一律要按上键；
			//  * SPRING1/2 弹跳门走各自的 spring 通道，不在此处。
			if (scope == PortalScope::TOUCH_ONLY
				&& t != Portal::TOUCH && t != Portal::SCRIPTED_TOUCH)
				continue;

			if (portal.bounds().contains(playerpos))
			{
				Portal::WarpInfo info = portal.getwarpinfo();
				if (info.valid)
				{
					cooldown = WARPCD;
					return info;
				}
			}
		}

		return {};
	}

	void MapPortals::init()
	{
		nl::node src = nl::nx::Map["MapHelper.img"]["portal"]["game"];

		// 隐藏门三态（波4）：ph/default/{portalStart,portalContinue,portalExit}
		nl::node ph = src["ph"]["default"];
		ph_start = ph["portalStart"];
		ph_continue = ph["portalContinue"];
		ph_exit = ph["portalExit"];
		// 缺任一态时回退 continue，避免空 Animation 不画。
		if (!ph["portalStart"])
			ph_start = ph_continue;
		if (!ph["portalExit"])
			ph_exit = ph_continue;

		// 脚本隐藏门：psh/default 同形；无 default 时试 psh/1
		nl::node psh = src["psh"]["default"];
		if (!psh)
			psh = src["psh"]["1"];
		psh_start = psh["portalStart"];
		psh_continue = psh["portalContinue"];
		psh_exit = psh["portalExit"];
		if (!psh["portalStart"])
			psh_start = psh_continue;
		if (!psh["portalExit"])
			psh_exit = psh_continue;
		// psh 全空时回退到 ph，保证脚本隐藏门至少有 continue。
		if (!psh["portalContinue"] && ph["portalContinue"])
		{
			psh_start = ph_start;
			psh_continue = ph_continue;
			psh_exit = ph_exit;
		}

		// REGULAR 门动画：本数据 pv 直接下挂 0..7；带 default 的版本优先 default。
		nl::node pv = src["pv"];
		nl::node pv_default = pv["default"];
		regular_anim = pv_default ? Animation(pv_default) : Animation(pv);
	}

	Animation MapPortals::regular_anim;
	Animation MapPortals::ph_start;
	Animation MapPortals::ph_continue;
	Animation MapPortals::ph_exit;
	Animation MapPortals::psh_start;
	Animation MapPortals::psh_continue;
	Animation MapPortals::psh_exit;
}
