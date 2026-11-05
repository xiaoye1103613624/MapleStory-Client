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

#include "../../Graphics/Animation.h"

namespace ms
{
	class Portal
	{
	public:
		enum Type
		{
			SPAWN,
			INVISIBLE,
			REGULAR,
			TOUCH,
			TYPE4,
			TYPE5,
			WARP,
			SCRIPTED,
			SCRIPTED_INVISIBLE,
			SCRIPTED_TOUCH,
			HIDDEN,
			SCRIPTED_HIDDEN,
			SPRING1,
			SPRING2,
			TYPE14
		};

		static Type typebyid(int32_t id)
		{
			return static_cast<Type>(id);
		}

		// 隐藏门 / 脚本隐藏门：ph|psh 的 portalStart→Continue→Exit 三态。
		enum class HidePhase : uint8_t
		{
			NONE,		// 未触碰、不绘制
			START,		// 刚进入：播 portalStart 一次
			CONTINUE,	// 停留：循环 portalContinue
			EXIT		// 刚离开：播 portalExit 一次后回 NONE
		};

		struct WarpInfo
		{
			int32_t mapid;
			std::string toname;
			std::string name;
			bool intramap;
			bool valid;
			bool scripted;	// W4：脚本门走 CHANGE_MAP_SPECIAL

			WarpInfo(int32_t m, bool i, std::string tn, std::string n, bool sc = false)
				: mapid(m), intramap(i), toname(tn), name(n), scripted(sc)
			{
				valid = mapid < 999999999;
			}

			WarpInfo() : WarpInfo(999999999, false, {}, {}) {}
		};

		// regular_anim：显式门共享动画指针（由 MapPortals 统一推进帧）。
		// hide_set：隐藏门三态模板（可为空；非隐藏类型忽略）。
		struct HideAnims
		{
			const Animation* start = nullptr;
			const Animation* cont = nullptr;
			const Animation* exit = nullptr;
		};

		Portal(const Animation* regular_anim, HideAnims hide, Type type, std::string name, bool intramap, Point<int16_t> position, int32_t tomap, std::string toname);
		Portal();

		void update(Point<int16_t> playerpos);
		void draw(Point<int16_t> viewpos, float alpha) const;

		std::string get_name() const;
		Type get_type() const;
		Point<int16_t> get_position() const;
		Rectangle<int16_t> bounds() const;

		WarpInfo getwarpinfo() const;

		static bool is_hide_family(Type t)
		{
			return t == HIDDEN || t == SCRIPTED_HIDDEN;
		}

	private:
		void set_phase(HidePhase next, const Animation* src);

		const Animation* regular = nullptr;	// REGULAR / TOUCH 共享
		HideAnims hide_src;
		Animation owned;					// 隐藏门当前相位的本地副本
		HidePhase phase = HidePhase::NONE;
		bool touched = false;
		Type type = SPAWN;
		std::string name;
		Point<int16_t> position;
		WarpInfo warpinfo;
	};
}
