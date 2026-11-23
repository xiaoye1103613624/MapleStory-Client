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
#include "FootholdTree.h"

#include <iostream>

namespace ms
{
	FootholdTree::FootholdTree(nl::node src)
	{
		int16_t leftw = 30000;
		int16_t rightw = -30000;
		int16_t botb = -30000;
		int16_t topb = 30000;

		for (auto basef : src)
		{
			uint8_t layer;

			try
			{
				layer = static_cast<uint8_t>(std::stoi(basef.name()));
			}
			catch (const std::exception& ex)
			{
				LOG(LOG_ERROR, __func__ << ": " << ex.what());
				continue;
			}

			for (auto midf : basef)
			{
				for (auto lastf : midf)
				{
					uint16_t id;

					try
					{
						id = static_cast<uint16_t>(std::stoi(lastf.name()));
					}
					catch (const std::exception& ex)
					{
						LOG(LOG_ERROR, __func__ << ": " << ex.what());
						continue;
					}

					const Foothold& foothold = footholds.emplace(
						std::piecewise_construct,
						std::forward_as_tuple(id),
						std::forward_as_tuple(lastf, id, layer)
					).first->second;

					if (foothold.l() < leftw)
						leftw = foothold.l();

					if (foothold.r() > rightw)
						rightw = foothold.r();

					if (foothold.b() > botb)
						botb = foothold.b();

					if (foothold.t() < topb)
						topb = foothold.t();

					if (foothold.is_wall())
						continue;

					int16_t start = foothold.l();
					int16_t end = foothold.r();

					// int16_t loop counters wrap past INT16_MAX and never terminate
					// when end == 32767 (or the span crosses the wrap). Use int32_t.
					for (int32_t i = start; i <= end; ++i)
						footholdsbyx.emplace(static_cast<int16_t>(i), id);
				}
			}
		}

		walls = { static_cast<int16_t>(leftw + 25), static_cast<int16_t>(rightw - 25) };
		borders = { static_cast<int16_t>(topb - 300), static_cast<int16_t>(botb + 100) };
	}

	FootholdTree::FootholdTree() {}

	void FootholdTree::install_floor(int16_t x1, int16_t x2, int16_t y)
	{
		footholds.clear();
		footholdsbyx.clear();

		if (x2 < x1)
		{
			const int16_t tmp = x1;
			x1 = x2;
			x2 = tmp;
		}

		constexpr uint16_t id = 1;
		footholds.emplace(
			id,
			Foothold(id, 0, x1, y, x2, y, 0, 0));

		for (int32_t i = x1; i <= x2; ++i)
			footholdsbyx.emplace(static_cast<int16_t>(i), id);

		walls = { static_cast<int16_t>(x1 + 25), static_cast<int16_t>(x2 - 25) };
		borders = { static_cast<int16_t>(y - 300), static_cast<int16_t>(y + 100) };
	}

	void FootholdTree::limit_horizontal(PhysicsObject& phobj) const
	{
		// 与 limit_movement 的水平段同源（MinMS FootholdTree.cpp:235-271），
		// 但不含垂直 limity(ground)：飞行 template 怪必须悬浮而非被钉在地面行走。
		if (!phobj.hmobile())
			return;

		double crnt_x = phobj.crnt_x();
		double next_x = phobj.next_x();
		bool left = phobj.hspeed < 0.0f;
		double wall = get_wall(phobj.fhid, left, phobj.next_y());
		bool collision = left
			? crnt_x >= wall && next_x <= wall
			: crnt_x <= wall && next_x >= wall;

		if (!collision && phobj.is_flag_set(PhysicsObject::Flag::TURNATEDGES))
		{
			wall = get_edge(phobj.fhid, left);
			collision = left
				? crnt_x >= wall && next_x <= wall
				: crnt_x <= wall && next_x >= wall;
			if (!collision)
				collision = left ? next_x < wall : next_x > wall;
		}

		if (collision)
		{
			phobj.limitx(wall);
			phobj.clear_flag(PhysicsObject::Flag::TURNATEDGES);
		}
	}

	void FootholdTree::limit_movement(PhysicsObject& phobj) const
	{
		if (phobj.hmobile())
		{
			double crnt_x = phobj.crnt_x();
			double next_x = phobj.next_x();

			bool left = phobj.hspeed < 0.0f;
			double wall = get_wall(phobj.fhid, left, phobj.next_y());
			bool collision = left ? crnt_x >= wall && next_x <= wall : crnt_x <= wall && next_x >= wall;

			if (!collision && phobj.is_flag_set(PhysicsObject::Flag::TURNATEDGES))
			{
				wall = get_edge(phobj.fhid, left);
				collision = left ? crnt_x >= wall && next_x <= wall : crnt_x <= wall && next_x >= wall;
			}

			if (collision)
			{
				phobj.limitx(wall);
				phobj.clear_flag(PhysicsObject::Flag::TURNATEDGES);
			}
		}

		if (phobj.vmobile())
		{
			double crnt_y = phobj.crnt_y();
			double next_y = phobj.next_y();

			auto ground = Range<double>(
				get_fh(phobj.fhid).ground_below(phobj.crnt_x()),
				get_fh(phobj.fhid).ground_below(phobj.next_x())
				);

			bool collision = crnt_y <= ground.first() && next_y >= ground.second();

			if (collision)
			{
				phobj.limity(ground.second());

				limit_movement(phobj);
			}
			else
			{
				if (next_y < borders.first())
					phobj.limity(borders.first());
				else if (next_y > borders.second())
					phobj.limity(borders.second());
			}
		}
	}

	void FootholdTree::update_fh(PhysicsObject& phobj) const
	{
		if (phobj.type == PhysicsObject::Type::FIXATED && phobj.fhid > 0)
			return;

		const Foothold& curfh = get_fh(phobj.fhid);
		bool checkslope = false;

		double x = phobj.crnt_x();
		double y = phobj.crnt_y();

		if (phobj.onground)
		{
			if (std::floor(x) > curfh.r())
				phobj.fhid = curfh.next();
			else if (std::ceil(x) < curfh.l())
				phobj.fhid = curfh.prev();

			if (phobj.fhid == 0)
				phobj.fhid = get_fhid_below(x, y);
			else
				checkslope = true;
		}
		else
		{
			phobj.fhid = get_fhid_below(x, y);

			if (phobj.fhid == 0)
				return;
		}

		const Foothold& nextfh = get_fh(phobj.fhid);
		phobj.fhslope = nextfh.slope();
		phobj.fhforce = nextfh.force();

		double ground = nextfh.ground_below(x);

		if (phobj.vspeed == 0.0 && checkslope)
		{
			double vdelta = abs(phobj.fhslope);

			if (phobj.fhslope < 0.0)
				vdelta *= (ground - y);
			else if (phobj.fhslope > 0.0)
				vdelta *= (y - ground);

			if (curfh.slope() != 0.0 || nextfh.slope() != 0.0)
			{
				if (phobj.hspeed > 0.0 && vdelta <= phobj.hspeed)
					phobj.y = ground;
				else if (phobj.hspeed < 0.0 && vdelta >= phobj.hspeed)
					phobj.y = ground;
			}
		}

		phobj.onground = phobj.y == ground;

		if (phobj.enablejd || phobj.is_flag_set(PhysicsObject::Flag::CHECKBELOW))
		{
			// C12 forbidFallDown：当前落脚点禁止下跳时直接关 enablejd。
			// 证据：原生 foothold 加载 sub_A43E7B 读 4417→bool；sdlMS/SoloMapling
			// 在 prone+jump 路径上 `if (!forbidFallDown)` 才穿板。本端下跳已统一
			// 门控于 enablejd（PlayerStates Walk/Prone），此处并入即可闭合。
			if (nextfh.forbidFallDown())
			{
				phobj.enablejd = false;
			}
			else
			{
				uint16_t belowid = get_fhid_below(x, nextfh.ground_below(x) + 1.0);

				if (belowid > 0)
				{
					double nextground = get_fh(belowid).ground_below(x);
					phobj.enablejd = (nextground - ground) < 600.0;
					phobj.groundbelow = ground + 1.0;
				}
				else
				{
					phobj.enablejd = false;
				}
			}

			phobj.clear_flag(PhysicsObject::Flag::CHECKBELOW);
		}

		if (phobj.fhlayer == 0 || phobj.onground)
			phobj.fhlayer = nextfh.layer();

		if (phobj.fhid == 0)
		{
			phobj.fhid = curfh.id();
			phobj.limitx(curfh.x1());
		}
	}

	const Foothold& FootholdTree::get_fh(uint16_t fhid) const
	{
		auto iter = footholds.find(fhid);

		if (iter == footholds.end())
			return nullfh;

		return iter->second;
	}

	double FootholdTree::get_wall(uint16_t curid, bool left, double fy) const
	{
		auto shorty = static_cast<int16_t>(fy);
		Range<int16_t> vertical(shorty - 50, shorty - 1);
		const Foothold& cur = get_fh(curid);

		if (left)
		{
			const Foothold& prev = get_fh(cur.prev());

			if (prev.is_blocking(vertical))
				return cur.l();

			const Foothold& prev_prev = get_fh(prev.prev());

			if (prev_prev.is_blocking(vertical))
				return prev.l();

			return walls.first();
		}
		else
		{
			const Foothold& next = get_fh(cur.next());

			if (next.is_blocking(vertical))
				return cur.r();

			const Foothold& next_next = get_fh(next.next());

			if (next_next.is_blocking(vertical))
				return next.r();

			return walls.second();
		}
	}

	double FootholdTree::get_edge(uint16_t curid, bool left) const
	{
		// 2026-11-22：按 Angel 原生 `CVecCtrlMob::CollisionDetectWalk/Float` 的
		// foothold 邻接语义重写。旧实现「连续 ≥3 段就返回地图墙」是自造规则：
		// 长平台上的怪会越过平台末端一直走到地图边界，再从边缘掉到下层平台。
		// 原生按真实邻板可达性在链末端翻向，不存在「第 3 段」阈值。
		// 这里沿 prev/next 链走到真正的末端（邻板不存在或 blocking），返回该板末端；
		// 64 段上限防地图数据成环导致死循环。
		//
		// 注意：get_edge 的返回值是「钳位墙」，命中后 limit_movement 会 limitx 并翻向，
		// 故应返回末端板的外侧边界（向左链最左端的 l()，向右链最右端的 r()）。
		uint16_t walk = curid;
		uint16_t last = curid;
		constexpr int MAX_FH_CHAIN = 64;

		if (left)
		{
			for (int i = 0; i < MAX_FH_CHAIN; i++)
			{
				const Foothold& cur = get_fh(walk);
				const uint16_t previd = cur.prev();
				if (!previd)
					return cur.l(); // 链到头：当前板即最左端
				const Foothold& prev = get_fh(previd);
				if (prev.is_blocking(cur.vertical()))
					return cur.l(); // 邻板不可走：钳在当前板左缘（原生翻向点）
				last = previd;
				walk = previd;
			}
			return get_fh(last).l(); // 链过长保护：取已走过的最左板
		}
		else
		{
			for (int i = 0; i < MAX_FH_CHAIN; i++)
			{
				const Foothold& cur = get_fh(walk);
				const uint16_t nextid = cur.next();
				if (!nextid)
					return cur.r(); // 链到头：当前板即最右端
				const Foothold& next = get_fh(nextid);
				if (next.is_blocking(cur.vertical()))
					return cur.r(); // 邻板不可走：钳在当前板右缘（原生翻向点）
				last = nextid;
				walk = nextid;
			}
			return get_fh(last).r(); // 链过长保护：取已走过的最右板
		}
	}

	uint16_t FootholdTree::get_fhid_below(double fx, double fy) const
	{
		uint16_t ret = 0;
		double comp = borders.second();

		int16_t x = static_cast<int16_t>(fx);
		auto range = footholdsbyx.equal_range(x);

		for (auto iter = range.first; iter != range.second; ++iter)
		{
			const Foothold& fh = footholds.at(iter->second);
			double ycomp = fh.ground_below(fx);

			if (comp >= ycomp && ycomp >= fy)
			{
				comp = ycomp;
				ret = fh.id();
			}
		}

		return ret;
	}

	uint16_t FootholdTree::get_fhid_at_x(double fx) const
	{
		// 按 x 取最靠上（y 最小）的水平平台，完全不依赖包里 y/cy。
		// 与 get_fhid_below 的区别：后者要求 ground>=y，NPC 的 cy 比平台低 5~17px
		// 会被跳过选到下一层；这里只看 x 覆盖，取最上层落脚点。
		uint16_t ret = 0;
		double top = borders.second();

		int16_t x = static_cast<int16_t>(fx);
		auto range = footholdsbyx.equal_range(x);

		for (auto iter = range.first; iter != range.second; ++iter)
		{
			const Foothold& fh = footholds.at(iter->second);
			if (fh.is_wall())
				continue;

			double y = fh.ground_below(fx);
			if (top >= y)
			{
				top = y;
				ret = fh.id();
			}
		}

		return ret;
	}

	bool FootholdTree::has_foothold(uint16_t fhid) const
	{
		auto iter = footholds.find(fhid);
		return iter != footholds.end() && !iter->second.is_wall();
	}

	bool FootholdTree::covers_x(uint16_t fhid, double fx) const
	{
		auto iter = footholds.find(fhid);
		return iter != footholds.end() && iter->second.hcontains(static_cast<int16_t>(fx));
	}

	int16_t FootholdTree::get_y_below(Point<int16_t> position) const
	{
		if (uint16_t fhid = get_fhid_below(position.x(), position.y()))
		{
			const Foothold& fh = get_fh(fhid);

			return static_cast<int16_t>(fh.ground_below(position.x()));
		}
		else
		{
			return borders.second();
		}
	}

	SpawnPoint FootholdTree::resolve_spawn_point(Point<int16_t> position, uint16_t fhid) const
	{
		auto iter = footholds.find(fhid);

		// 正常路径：包里的 fh 在本图存在、非墙、且水平覆盖包的 x。
		// 保留服务端声明的 fh 与坐标——新图包走这条，不受重锚影响。
		if (iter != footholds.end() && !iter->second.is_wall()
			&& iter->second.hcontains(position.x()))
		{
			return { Point<int16_t>(position.x(),
						static_cast<int16_t>(iter->second.ground_below(position.x()))), fhid };
		}

		// 回退路径：fh 指向旧图 / 竖墙 / x 落在平台外（切图残留包或 fh 失效）。
		// 不再用 get_y_below（cy 偏差会选到下一层），改按 x 就近取最上层平台，
		// 并回写实际 fhid。x 外无平台（ret==0）时给 borders.second()，但 fhid 保持 0，
		// 让上层据此丢弃该包，而不是让 NPC 掉进地图边界。
		uint16_t resolved = get_fhid_at_x(position.x());
		if (resolved != 0)
		{
			const Foothold& fh = get_fh(resolved);
			return { Point<int16_t>(position.x(),
						static_cast<int16_t>(fh.ground_below(position.x()))), resolved };
		}

		return { Point<int16_t>(position.x(), borders.second()), 0 };
	}

	Range<int16_t> FootholdTree::get_walls() const
	{
		return walls;
	}
	Range<int16_t> FootholdTree::get_borders() const
	{
		return borders;
	}
}