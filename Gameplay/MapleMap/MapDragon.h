//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — Evan Dragon map object (真对象化：Mob/8840000.img)
//
//	取证（_tools/mapobj_probe + img_catalog 2026-11-04，运行时部署）：
//	  Mob/8840000.img: stand(6) move(18) hit(1) die1(90) attack1-5 skill1-7
//	  filpL(6)/filpR(6) summon(41)。部署仅 8840000（8840001..14 全部缺失——
//	  与原生同树，V17.7 Data/Mob 也只有这一只），等级 job(1001..1005) 一律
//	  映射 8840000，观感差异 = 参照端资源现状，非本端遗漏。
//	协议（s9）：
//	  SPAWN_DRAGON = int(owner cid) + short x,short 0,short y,short 0
//	                 + byte stance + byte 0 + short job（PacketCreator:7405-7415）
//	  MOVE_DRAGON  = int cid + pos(start) + movement list（与原生移动段同构）
//	  REMOVE_DRAGON= int chrId
//	  龙 oid 语义 = owner cid（Dragon.java:34 getObjectId()），原生键 cid^0x40000000。
//	  stance 奇偶 = 朝向（Mob::set_stance 同法：偶数 flip=朝左，Mob.cpp:189）。
//	  死亡段 die1(90帧) 不接：s9 removeDragon 仅在登出/换图/删号调用
//	  （MapleMap.java:2773 等），无伤害上行 ⇒ REMOVE ≠ 死亡，播 90 帧会把
//	  登出表现为死亡动画。转身段 filpL/filpR(6帧) 数据在案不接：无独立
//	  转身上行，与 Mob 现状一致（本端 Mob 同样未播转身段）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"

#include "../../Graphics/Animation.h"

#include <map>
#include <string>

namespace ms
{
	class MapDragon : public FieldMarker
	{
	public:
		MapDragon(int32_t oid, int32_t owner_cid, Point<int16_t> pos, int8_t stance, int16_t job);

		void draw(double viewx, double viewy, float alpha) const override;
		int8_t update(const Physics& physics) override;

		// MOVE_DRAGON：位置 + 朝向（状态字节奇偶决定 flip，同 Mob）。
		void apply_move(Point<int16_t> pos, uint8_t state_byte);
		int32_t get_owner() const { return owner_cid; }

	private:
		int32_t owner_cid;
		bool flip = false;

		std::map<std::string, Animation> animations;
		std::string state = "stand";
		int32_t move_ttl = 0;	// 位移余晖计时（move 段回 stand 用）
	};
}
