////////////////////////////////////////////////////////////////////////////////
//	Wedding / Marriage 出站（对齐 s9 RingActionHandler / WeddingHandler /
//	WeddingTalkHandler 的 readByte/readString/readInt 精确次序）
//
//	协议事实源（2026-11-06 现场读源码，非推测）：
//	  s9 RecvOpcode.RING_ACTION = 0x89 = 137 → RingActionHandler
//	  s9 RecvOpcode.WEDDING_ACTION = 0x8A = 138 → WeddingHandler
//	  s9 RecvOpcode.WEDDING_TALK = WEDDING_TALK_MORE = 0x8B = 139 → WeddingTalkHandler
//	两条通道在 s9 PacketProcessor.java:250/308/309/310 均已注册，非死通道。
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ms
{
	// 婚礼相关道具号（s9 constants/id/ItemId.java:229-284 逐条抄录）。
	// 客户端只做范围判定，不做价格/掉落等业务，保持零重复定义。
	namespace WeddingItem
	{
		/// 订婚戒指盒（求婚消耗品，USE 栏 2240000-2240015）。
		/// s9 ENGAGEMENT_BOX_MIN = 2240000，ENGAGEMENT_BOX_MAX = 2240015。
		inline bool is_proposal_box(int32_t itemid)
		{
			return itemid >= 2240000 && itemid <= 2240015;
		}

		/// 待寄出的婚礼请柬（ETC 栏）。
		/// INVITATION_CHAPEL=4031377 / INVITATION_CATHEDRAL=4031395
		inline bool is_send_invitation(int32_t itemid)
		{
			return itemid == 4031377 || itemid == 4031395;
		}

		/// 已收到的请柬（ETC 栏，双击看新人）。
		/// RECEIVED_INVITATION_CHAPEL=4031406 / CATHEDRAL=4031407
		inline bool is_received_invitation(int32_t itemid)
		{
			return itemid == 4031406 || itemid == 4031407;
		}

		/// 订婚凭据（ETC 栏 4031357..4031364，s9 ItemId 空盒 + 对戒 8 项）。
		/// s9 breakMarriageRing 判据：type==ETC && itemId/10000==403 → breakEngagement。
		inline bool is_engagement_token(int32_t itemid)
		{
			return itemid >= 4031357 && itemid <= 4031364;
		}

		/// 结婚戒指（EQUIP 11128xx：1112803/1112806/1112807/1112809）。
		/// s9 RingActionHandler.breakMarriageRing 判据为 itemId / 10 == 111280。
		inline bool is_wedding_ring(int32_t itemid)
		{
			return itemid / 10 == 111280;
		}
	}

	// Opcode: RING_ACTION(137) — s9 RingActionHandler mode 0「Send Proposal」
	// 读包序：byte mode → readString(name) → readInt(itemid)
	class WeddingProposalPacket : public OutPacket
	{
	public:
		WeddingProposalPacket(const std::string& name, int32_t itemid)
			: OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(0);
			write_string(name);
			write_int(itemid);
		}
	};

	// Opcode: RING_ACTION(137) — mode 1「Cancel Proposal」：无后续字段
	class WeddingCancelProposalPacket : public OutPacket
	{
	public:
		WeddingCancelProposalPacket() : OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(1);
		}
	};

	// Opcode: RING_ACTION(137) — mode 2「Accept/Deny Proposal」
	// 读包序：byte mode → readByte(accepted>0) → readString(source名) → readInt(source cid)
	// 注意字段顺序与 FamilyAccept 不同：这里是 byte 在前、id 在最后。
	class WeddingAnswerPacket : public OutPacket
	{
	public:
		WeddingAnswerPacket(const std::string& proposer, int32_t proposer_id, bool accept)
			: OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(2);
			write_byte(accept ? 1 : 0);
			write_string(proposer);
			write_int(proposer_id);
		}
	};

	// Opcode: RING_ACTION(137) — mode 3「Break Engagement」：readInt(wItemId)
	// wItemId 既可以是婚礼戒指（11128xx → breakMarriage），
	// 也可以是订婚凭据 ETC 4031xxx（→ breakEngagement）。
	class WeddingBreakPacket : public OutPacket
	{
	public:
		explicit WeddingBreakPacket(int32_t w_item_id) : OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(3);
			write_int(w_item_id);
		}
	};

	// Opcode: RING_ACTION(137) — mode 5「Invite %s to Wedding」
	// 读包序：byte mode → readString(guest) → readInt(marriageId) → readByte(invitationSlot)
	// marriageId 来自 s9 Character.getRelationshipId()（登录 addRingInfo 下发）。
	class WeddingInvitePacket : public OutPacket
	{
	public:
		WeddingInvitePacket(const std::string& guest, int32_t marriage_id, int16_t invitation_slot)
			: OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(5);
			write_string(guest);
			write_int(marriage_id);
			write_byte(static_cast<int8_t>(invitation_slot));
		}
	};

	// Opcode: RING_ACTION(137) — mode 6「Open Wedding Invitation」
	// 读包序：byte mode → readInt(slot) → readInt(invitationid)
	// 两个字段都是 4 字节（slot 虽被 s9 收窄成 byte，但写侧是 int）。
	class WeddingOpenInvitationPacket : public OutPacket
	{
	public:
		WeddingOpenInvitationPacket(int32_t slot, int32_t invitation_id)
			: OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			write_byte(6);
			write_int(slot);
			write_int(invitation_id);
		}
	};

	// Opcode: RING_ACTION(137) — mode 9「Wedding Wishlist」
	// 读包序：byte mode → readShort(amount，服务端截到 10) → amount × readString()
	class WeddingWishlistPacket : public OutPacket
	{
	public:
		explicit WeddingWishlistPacket(const std::vector<std::string>& items)
			: OutPacket(OutPacket::Opcode::RING_ACTION)
		{
			int16_t count = static_cast<int16_t>(items.size());
			if (count > 10)
				count = 10;
			write_byte(9);
			write_short(count);
			for (int16_t i = 0; i < count; i++)
				write_string(items[i]);
		}
	};

	// Opcode: WEDDING_ACTION(138) — mode 6「gift item to spouse」
	// 读包序：byte mode → readShort(slot) → readInt(itemid) → readShort(quantity)
	class WeddingGiftSendPacket : public OutPacket
	{
	public:
		WeddingGiftSendPacket(int16_t slot, int32_t itemid, int16_t quantity)
			: OutPacket(OutPacket::Opcode::WEDDING_ACTION)
		{
			write_byte(6);
			write_short(slot);
			write_int(itemid);
			write_short(quantity);
		}
	};

	// Opcode: WEDDING_ACTION(138) — mode 7「take items from registry」
	// 读包序：byte mode → readByte(invType) → readByte(itemPos)
	class WeddingGiftTakePacket : public OutPacket
	{
	public:
		WeddingGiftTakePacket(int8_t inv_type, int8_t item_pos)
			: OutPacket(OutPacket::Opcode::WEDDING_ACTION)
		{
			write_byte(7);
			write_byte(inv_type);
			write_byte(item_pos);
		}
	};

	// Opcode: WEDDING_ACTION(138) — mode 8「out of Wedding Registry」：解锁回原位
	class WeddingRegistryExitPacket : public OutPacket
	{
	public:
		WeddingRegistryExitPacket() : OutPacket(OutPacket::Opcode::WEDDING_ACTION)
		{
			write_byte(8);
		}
	};

	// Opcode: WEDDING_TALK(139) — s9 WeddingTalkHandler：byte action
	// action==1 且非新人 → OnWeddingProgress(false,0,0,2)（宾客祝福）
	// 其余 → OnWeddingProgress(true,0,0,3)（仪式结束效果）
	// 备注：s9 WEDDING_TALK_MORE 与 WEDDING_TALK 同为 0x8B，读序仅 1 个 byte，
	// 继续对话直接复用 action 值即可，故本端不再单独发 _MORE 通道。
	class WeddingTalkPacket : public OutPacket
	{
	public:
		explicit WeddingTalkPacket(uint8_t action) : OutPacket(OutPacket::Opcode::WEDDING_TALK)
		{
			write_byte(static_cast<int8_t>(action));
		}
	};
}
