//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Mystic Door map object (真对象化：Map/Obj/door.img)
//
//	取证（_tools/mapobj_probe 2026-11-04，运行时部署 Data 直读，probe-out.txt 在案）：
//	  Map/Obj/door.img/darkWood: door{0:77x89, 1:112x148, 2:104x117, 3:78x100}
//	                            doorpull{0:46x60, 1:46x60, 2:14x39, 3:16x36}
//	协议（s9 DoorObject.sendSpawnData:110-121）：
//	  两端地图各收一条 SPAWN_DOOR(bool launched + int ownerid + pos)；镇侧另收
//	  SPAWN_PORTAL。交互上行 USE_DOOR(0x85) = int(ownerid) + byte（DoorHandler
//	  仅按 ownerid 匹配当前地图 DoorObject ⇒ byte 值服务端不消费）。
//	  归属/队伍裁决全在服务端（非 owner 非队友 → blockedMessage(6)）。
//	风格映射：原生按地图 doorPath 取 Obj 风格；包内无风格名、MapInfo 未解析
//	  doorPath ⇒ 统一 darkWood（部署在案的通用款），偏离已记报告 G6。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

#include "../../Graphics/Animation.h"

namespace ms
{
	class MapDoor : public FieldMarker
	{
	public:
		MapDoor(int32_t owner, Point<int16_t> pos, bool launched);

		int32_t get_owner() const;
		bool is_launched() const;
		// SPAWN_DOOR 重复下发（放门→收回→再放）只刷表现态，不重建精灵。
		void refresh(Point<int16_t> pos, bool launched);

		// 门框命中判定（draw 坐标系同源的贴图 bounds）。
		bool in_range(Point<int16_t> playerpos) const;

		// 命中 + 冷却外 ⇒ true 并起 1500ms 冷却（换图往返 >1s；被服务端
		// blocked 拒掉时也不会逐帧刷屏上行——原生靠换图成功消门，本端多一层
		// 冷却保险）。
		bool consume_trigger();

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics) override;

	private:
		int32_t owner;
		bool launched;
		int32_t send_cd = 0;

		Animation panel;	// darkWood/door/1（112x148 主门板）
		Animation handle;	// darkWood/doorpull/0（46x60 把手）
		bool has_panel = false;
		bool has_handle = false;
	};
}
