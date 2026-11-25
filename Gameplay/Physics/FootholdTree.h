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

#include "Foothold.h"
#include "PhysicsObject.h"

#include <unordered_map>

namespace ms
{
	// 落点解析结果：坐标 + 实际落脚的 foothold id。
	// 必须一起返回——只回写坐标不回写 fh 时，Npc 仍持有 spawn 包里的旧 fh，
	// 首帧 update_fh 会按旧 fh 重算 ground 并把 onground 判 false，继续坠落。
	struct SpawnPoint
	{
		Point<int16_t> position;
		uint16_t fhid = 0;
	};

	// The collection of platforms in a maple map
	// Used for collision-detection
	class FootholdTree
	{
	public:
		FootholdTree(nl::node source);
		FootholdTree();

		// 商城预览沙盒：清空并装一条水平地板（MinMS install_floor）。
		void install_floor(int16_t x1, int16_t x2, int16_t y);

		// Takes an accelerated PhysicsObject and limits its movement based on the platforms in this tree
		void limit_movement(PhysicsObject& touse) const;
		// 飞行怪专用：只做水平墙体钳制，不做垂直 foothold 吸附（MinMS FootholdTree.h:43）。
		void limit_horizontal(PhysicsObject& touse) const;
		// Updates a PhysicsObject's fhid based on it's position
		void update_fh(PhysicsObject& touse) const;
		// Determine the point on the ground below the specified position
		int16_t get_y_below(Point<int16_t> position) const;
		// 三端瞬移对齐：取指定 x 上方（ground<=position.y）最近的 foothold y。
		// 无平台时返回 borders.first()。对称于 get_y_below。
		int16_t get_y_above(Point<int16_t> position) const;
		// 把 life 放到它自己声明的那块 foothold 上（fh 来自服务端 spawn 包）。
		// 不能复用 get_y_below：包里 y 槽传的是 wz 的 cy，实测比平台线低 5~17px，
		// 按坐标反查要求 ground >= y，会把正下方那块平台判成"不够低"而选到下一层，
		// NPC 于是"跑到路下面"；再找不到就退到 borders 之外，表现为 NPC 不见了。
		// 返回解析后的实际 fhid，供 Npc/Spawn 回写 phobj.fhid，避免首帧再坠落。
		SpawnPoint resolve_spawn_point(Point<int16_t> position, uint16_t fhid) const;
		// spawn 包声明的 fh 是否在本图存在且可用（非竖墙）——A 重锚门禁用：
		// 存在则保留该包（新图包），不存在即旧图残留，按 x 重锚或丢弃。
		bool has_foothold(uint16_t fhid) const;
		// 该平台是否水平覆盖 x（重锚后校验，避免把点落到平台端点外）
		bool covers_x(uint16_t fhid, double fx) const;
		// Returns the leftmost and rightmost platform positions of the map
		Range<int16_t> get_walls() const;
		// Returns the topmost and bottommost platform positions of the map
		Range<int16_t> get_borders() const;

		// 遍历全部非墙 foothold（天气堆积按平台采样用）。
		template <typename Fn>
		void for_each_floor(Fn&& fn) const
		{
			for (const auto& kv : footholds)
			{
				if (!kv.second.is_wall())
					fn(kv.second);
			}
		}

	private:
		uint16_t get_fhid_below(double fx, double fy) const;
		// 取 x 覆盖范围内 ground<=fy 的最高 foothold id（0=无，瞬移上方落点用）。
		uint16_t get_fhid_above(double fx, double fy) const;
		// 按 x 就近选平台（不依赖包里的 y/cy）：spawn 包 y 槽是 wz cy，比平台线低 5~17px，
		// get_fhid_below 的 ground>=y 判据会因此跳过正确平台选中下一层。
		// 新图重锚时 x 在平台外返回 0，交由调用方丢弃该包。
		uint16_t get_fhid_at_x(double fx) const;
		double get_wall(uint16_t fhid, bool left, double fy) const;
		double get_edge(uint16_t fhid, bool left) const;
		const Foothold& get_fh(uint16_t fhid) const;

		std::unordered_map<uint16_t, Foothold> footholds;
		std::unordered_multimap<int16_t, uint16_t> footholdsbyx;

		Foothold nullfh;
		Range<int16_t> walls;
		Range<int16_t> borders;
	};
}