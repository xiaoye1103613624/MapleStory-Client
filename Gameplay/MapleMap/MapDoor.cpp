//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mystic Door map object
//////////////////////////////////////////////////////////////////////////////////
#include "MapDoor.h"

#include "../../Constants.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	MapDoor::MapDoor(int32_t o, Point<int16_t> pos, bool l)
		// 名牌清空：真门与原生一致（原生 mystic door 无头顶文字）。
		// owner oid 直用（s9 spawnDoor 的 ownerid 即门 id，RemoveDoor 同键）。
		: FieldMarker(o, Kind::DOOR, pos, std::string()),
		owner(o),
		launched(l)
	{
#ifdef USE_NX
		// 探针坐实：Data/Map/Obj/door.img/darkWood/door{0..3} + doorpull{0..3}
		// （_tools/mapobj_probe/probe-out.txt）。运行时 Map 命名空间把目录挂载为
		// 节点（Tile.cpp 的 nx::Map["Tile"] 同法），故首选 ["Obj"]["door.img"]；
		// 后两级为挂载形态变体兜底，全空 ⇒ has_panel=false 走基类文本回退。
		nl::node style = nl::nx::Map["Obj"]["door.img"]["darkWood"];

		if (!style)
			style = nl::nx::Map["Obj.img"]["door.img"]["darkWood"];
		if (!style)
			style = nl::nx::Map["door.img"]["darkWood"];

		nl::node p = style["door"][1];
		if (!p)
			p = style["door"][0];
		if (p)
		{
			panel = Animation(p);
			has_panel = true;
		}

		nl::node h = style["doorpull"][0];
		if (h)
		{
			handle = Animation(h);
			has_handle = true;
		}
#endif
	}

	int32_t MapDoor::get_owner() const
	{
		return owner;
	}

	bool MapDoor::is_launched() const
	{
		return launched;
	}

	void MapDoor::refresh(Point<int16_t> pos, bool l)
	{
		// 重复下发只刷表现态与落位（s9 launched 随状态变化重发整包）。
		set_position(pos);
		launched = l;
	}

	bool MapDoor::in_range(Point<int16_t> playerpos) const
	{
		if (!has_panel)
			return false;

		// 门框 hitbox：以贴图 bounds 为准（s9 侧只校验存在性与归属，
		// 触发几何在客户端——与静态 Portal::bounds 同法）。
		Rectangle<int16_t> bounds = panel.get_bounds();
		bounds.shift(get_position());

		return bounds.contains(playerpos);
	}

	bool MapDoor::consume_trigger()
	{
		// 冷却中 ⇒ 不重复上行（换图成功前的逐帧重踩、被服务端 blocked 后
		// 的连续贴门都收敛在这 1500ms 里）。
		if (send_cd > 0)
			return false;

		send_cd = 1500;
		return true;
	}

	int8_t MapDoor::update(const Physics& physics)
	{
		int8_t layer = FieldMarker::update(physics);

		if (send_cd > 0)
			send_cd -= Constants::TIMESTEP;

		return layer;
	}

	void MapDoor::draw(double viewx, double viewy, float alpha) const
	{
		if (!active)
			return;

		if (!has_panel)
		{
			// 资源缺失（非 darkWood 图族）⇒ 回退基类文本场标，不空画。
			FieldMarker::draw(viewx, viewy, alpha);
			return;
		}

		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);

		panel.draw(DrawArgument(absp), alpha);

		if (has_handle)
		{
			// launched 态把手左移（doorpull 相对门板；原生按 doorpull 帧段
			// 2/3 的窄条表现为"拉开"，这里以偏移近似，观感校准记报告 G6）。
			Point<int16_t> hpos = absp + (launched ? Point<int16_t>(-34, 6) : Point<int16_t>(0, 0));
			handle.draw(DrawArgument(hpos), alpha);
		}
	}
}
