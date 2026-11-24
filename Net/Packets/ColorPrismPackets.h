//////////////////////////////////////////////////////////////////////////////////
// ColorPrismPackets — 七彩棱镜 C→S 发包（中文包协议，Opcode 0x11D）。
//
// D1 切换（2026-11-21）：旧巴西服包（WEAPON_TINT_ACTION 0x372E，
// ColorPrismApply/Restore/Look/Skill 六类）已随服务端一起废弃，全部移除。
// 现在逐字对齐服务端 ColoringPrismHandler（指南 §3.4）：
//   byte action
//     0 REQUEST — 无 body，服务端回 DYE_LIST(0x184 resp=1)。
//     1 CONFIRM — byte invType(有符号,-1=已穿戴), short slot, int itemId,
//                 int hueBits, int satBits, int lightBits（float 位原样写）。
//     2 CLEAR   — byte invType, short slot, int itemId。
// slot 为已穿戴槽时是 wire 负数（EquipSlot::Id 绝对值取负），invType 必须发 -1。
// 浮点位用 memcpy(float→int32_t) 还原服务端 Float.floatToIntBits，禁止强转丢位。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <cstdint>
#include <cstring>

namespace ms
{
	// action 常量（与服务端 ColoringPrismHandler / 客户端 handler 一致）
	static constexpr uint8_t CP_ACTION_REQUEST = 0;
	static constexpr uint8_t CP_ACTION_CONFIRM = 1;
	static constexpr uint8_t CP_ACTION_CLEAR = 2;

	/// 已穿戴槽的 invType 固定为有符号 -1（EQUIPPED）。服务端以 signed readByte 读。
	static constexpr int8_t CP_INVTYPE_EQUIPPED = -1;

	/// float → IEEE754 bit pattern（等价 Java Float.floatToIntBits）。
	inline int32_t cp_float_bits(float value)
	{
		int32_t raw = 0;
		std::memcpy(&raw, &value, sizeof(raw));
		return raw;
	}

	/// action=0：请求本人染色列表（无 body）。
	class ColorPrismRequestPacket : public OutPacket
	{
	public:
		ColorPrismRequestPacket() : OutPacket(OutPacket::Opcode::COLORING_PRISM_ACTION)
		{
			write_byte(CP_ACTION_REQUEST);
		}
	};

	/// action=1：确认染色。hue∈[-180,180]、sat/light∈[-1,1]，按位写入。
	class ColorPrismConfirmPacket : public OutPacket
	{
	public:
		ColorPrismConfirmPacket(int8_t inv_type, int16_t slot, int32_t item_id,
			float hue, float sat, float light)
			: OutPacket(OutPacket::Opcode::COLORING_PRISM_ACTION)
		{
			write_byte(CP_ACTION_CONFIRM);
			write_byte(inv_type);   // 有符号：-1 = 已穿戴
			write_short(slot);      // wire 槽号（已穿戴为负）
			write_int(item_id);
			write_int(cp_float_bits(hue));   // 服务端按 intBits 收，客户端 float 位原样写
			write_int(cp_float_bits(sat));
			write_int(cp_float_bits(light));
		}
	};

	/// action=2：清除某件装备的染色（还原原型）。
	class ColorPrismClearPacket : public OutPacket
	{
	public:
		ColorPrismClearPacket(int8_t inv_type, int16_t slot, int32_t item_id)
			: OutPacket(OutPacket::Opcode::COLORING_PRISM_ACTION)
		{
			write_byte(CP_ACTION_CLEAR);
			write_byte(inv_type);
			write_short(slot);
			write_int(item_id);
		}
	};
}
