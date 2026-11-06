//////////////////////////////////////////////////////////////////////////////////
//	CMovePath::Encode 移植（Maplestory95.i64 @ 0x666e20）
//	证据：docs/features/change-quickslot-persist/evidence/cmovepath-encode-0x666e20.decompiled.txt
//	禁止复用 MovementPacket::writemovement（玩家 Move 形 ≠ NPC path）。
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <vector>
#include <algorithm>
#include <cstdint>

namespace ms
{
	// 与原生 CMovePath::ELEM 对齐的可编码节（字段名跟伪代码）。
	struct MovePathElem
	{
		uint8_t nAttr = 0;
		int16_t x = 0;
		int16_t y = 0;
		int16_t vx = 0;
		int16_t vy = 0;
		int16_t fh = 0;
		int16_t fhFallStart = 0;
		int16_t xOffset = 0;
		int16_t yOffset = 0;
		uint8_t bMoveAction = 0;
		int16_t tElapse = 0;
		uint8_t bStat = 0;
	};

	// OutPacket 子类可调用的 Encode 写体（不含 opcode / oid / action / chatIdx）。
	// 尾：keyPadCount==0 → 1 字节；rect 8 字节；合计 9，对齐 s9 length-9。
	// 本端不写 ClientOptMan GetOpt(2) 的 usRandCnt 附加（v83/s9 默认关）。
	class MovePathPacket : public OutPacket
	{
	public:
		explicit MovePathPacket(Opcode opc) : OutPacket(opc) {}

	protected:
		void encode_movepath(int16_t x, int16_t y, int16_t vx, int16_t vy,
			const std::vector<MovePathElem>& elems)
		{
			write_short(x);
			write_short(y);
			write_short(vx);
			write_short(vy);

			int16_t left = x;
			int16_t top = y;
			int16_t right = x;
			int16_t bottom = y;

			write_byte(static_cast<int8_t>(elems.size() > 255 ? 255 : elems.size()));

			for (const MovePathElem& e : elems)
			{
				write_byte(static_cast<int8_t>(e.nAttr));

				const uint8_t a = e.nAttr;
				bool wrote_common_tail = true;

				switch (a)
				{
				case 0:
				case 5:
				case 0xC:
				case 0xE:
				case 0x23:
				case 0x24:
					write_short(e.x);
					write_short(e.y);
					write_short(e.vx);
					write_short(e.vy);
					write_short(e.fh);
					if (a == 0xC)
						write_short(e.fhFallStart);
					write_short(e.xOffset);
					write_short(e.yOffset);
					break;
				case 1:
				case 2:
				case 0xD:
				case 0x10:
				case 0x12:
				case 0x1F:
				case 0x20:
				case 0x21:
				case 0x22:
					write_short(e.vx);
					write_short(e.vy);
					break;
				case 3:
				case 4:
				case 6:
				case 7:
				case 8:
				case 0xA:
					write_short(e.x);
					write_short(e.y);
					write_short(e.fh);
					break;
				case 9:
					write_byte(static_cast<int8_t>(e.bStat));
					wrote_common_tail = false;
					break;
				case 0xB:
					write_short(e.vx);
					write_short(e.vy);
					write_short(e.fhFallStart);
					break;
				case 0x11:
					write_short(e.x);
					write_short(e.y);
					write_short(e.vx);
					write_short(e.vy);
					break;
				default:
					// 未知 attr：不发明字段；仅写公共尾（与伪代码 default→LABEL_15 一致）
					break;
				}

				if (wrote_common_tail)
				{
					left = (std::min)(left, e.x);
					top = (std::min)(top, e.y);
					right = (std::max)(right, e.x);
					bottom = (std::max)(bottom, e.y);

					write_byte(static_cast<int8_t>(e.bMoveAction));
					write_short(e.tElapse);
				}
			}

			// keyPadCount == 0
			write_byte(0);
			write_short(left);
			write_short(top);
			write_short(right);
			write_short(bottom);
		}
	};
}
