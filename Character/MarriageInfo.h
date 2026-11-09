////////////////////////////////////////////////////////////////////////////////
//	结婚/订婚数据层
//
//	来源：s9 PacketCreator.addRingInfo 的 marriage 块（PacketCreator.java:6894-6913）
//	与 MARRIAGE_RESULT mode 11（WeddingPackets.OnMarriageResult(int,Character,boolean)）。
//	两处写序完全同构：
//	  short present(0/1)
//	  int   relationshipId        // s9 Character.getRelationshipId()
//	  int   groomId               // gender==0 ? self : partner
//	  int   brideId               // gender==0 ? partner : self
//	  short status                // 1=ENGAGED 3=MARRIED（WeddingPackets.MarriageStatus）
//	  int   ringItemId ×2
//	  fixedString(13) groomName
//	  fixedString(13) brideName
//
//	为什么要在登录解析里存下来：请柬邀请（RING_ACTION mode5）与送礼（WEDDING_ACTION
//	mode6）都要带 marriageId，本端原先 parse_ring3 只读不存，导致这两个动作拿不到
//	id、整条婚礼流程无从发起。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>

namespace ms
{
	struct MarriageInfo
	{
		// s9 WeddingPackets.MarriageStatus
		enum Status : int16_t
		{
			SINGLE = 0,
			ENGAGED = 1,
			RESERVED = 2,
			MARRIED = 3
		};

		bool present = false;
		int32_t relationship_id = 0;
		int32_t groom_id = 0;
		int32_t bride_id = 0;
		int16_t status = SINGLE;
		int32_t ring_item_id = 0;
		std::string groom_name;
		std::string bride_name;
		// 世界地图 loverPos：NOTIFY_MARRIED_PARTNER_MAP_TRANSFER(int mapid + int partner)。
		// 原生只在已开的世界地图/婚礼窗刷新，不为这个包开任何窗（R28-B2 同类要求）。
		int32_t partner_mapid = 0;

		// 配偶 id / 名：本人 cid 由 Player::get_oid() 提供（MapObject 侧），
		// 块内 groom/bride 之一即本人，另一侧就是配偶。
		int32_t partner_id(int32_t self_cid) const
		{
			return self_cid == groom_id ? bride_id : groom_id;
		}

		std::string partner_name(int32_t self_cid) const
		{
			return self_cid == groom_id ? bride_name : groom_name;
		}

		// 本人是否为新郎（s9 用 gender==0 判，块内 cid 已把结果写好，直接比）。
		bool is_groom(int32_t self_cid) const
		{
			return self_cid == groom_id;
		}

		bool is_married() const { return present && status == MARRIED; }
		bool is_engaged() const { return present && status == ENGAGED; }
	};
}
