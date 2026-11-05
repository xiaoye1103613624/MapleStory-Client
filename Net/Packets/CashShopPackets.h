////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../OutPacket.h"

#include <array>
#include <cstddef>
#include <string>

namespace ms
{
	// 现金商城购买包：Opcode CASHSHOP_OPERATION(229)（已修正：原误抄 Send 侧 325，见 OutPacket.h 注释），与 s9 CashOperationHandler action 0x03 对齐。
	// 服务端读取顺序：byte action(0x03) -> byte(占位 0) -> int useNX -> int sn。
	class CashShopBuyPacket : public OutPacket
	{
	public:
		CashShopBuyPacket(int32_t useNX, int32_t sn) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x03);
			write_byte(0);
			write_int(useNX);
			write_int(sn);
		}
	};

	// 愿望清单：action 0x05，固定写 10 个 int SN（不足补 0）。对照 s9 modifyWishList。
	class CashShopWishPacket : public OutPacket
	{
	public:
		explicit CashShopWishPacket(const std::array<int32_t, 10>& sns)
			: OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x05);
			for (std::size_t i = 0; i < sns.size(); ++i)
				write_int(sns[i]);
		}

		explicit CashShopWishPacket(int32_t sn) : CashShopWishPacket(std::array<int32_t, 10>{ sn }) {}
	};

	// 礼品：action 0x04 — birthday, sn, name, msg（s9）
	class CashShopGiftPacket : public OutPacket
	{
	public:
		CashShopGiftPacket(int32_t birthday, int32_t sn, const std::string& name, const std::string& msg)
			: OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x04);
			write_int(birthday);
			write_int(sn);
			write_string(name);
			write_string(msg);
		}
	};

	// 从现金栏取出：action 0x0D — cashId
	class CashShopTakePacket : public OutPacket
	{
	public:
		explicit CashShopTakePacket(int32_t cashId) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x0D);
			write_int(cashId);
		}
	};

	// 放入现金栏：action 0x0E — cashId, skip4, invType
	class CashShopPutPacket : public OutPacket
	{
	public:
		CashShopPutPacket(int32_t cashId, int8_t invType) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x0E);
			write_int(cashId);
			skip(4);
			write_byte(invType);
		}
	};

	// 礼包购买：action 0x1E — 与 buy 同体（byte0 + useNX + sn）
	class CashShopPackagePacket : public OutPacket
	{
	public:
		CashShopPackagePacket(int32_t useNX, int32_t sn) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x1E);
			write_byte(0);
			write_int(useNX);
			write_int(sn);
		}
	};

	// 背包扩容：0x06 mode0 — skip1, cash, mode0, invType
	class CashShopInvSlotPacket : public OutPacket
	{
	public:
		CashShopInvSlotPacket(int32_t cash, int8_t invType) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x06);
			write_byte(0);
			write_int(cash);
			write_byte(0);
			write_byte(invType);
		}
	};

	// 背包扩容：0x06 mode1 — skip1, cash, mode1, sn
	class CashShopInvSlotSnPacket : public OutPacket
	{
	public:
		CashShopInvSlotSnPacket(int32_t cash, int32_t sn) : OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x06);
			write_byte(0);
			write_int(cash);
			write_byte(1);
			write_int(sn);
		}
	};

	// 仓库扩容：0x07 — skip1, cash, mode
	class CashShopStorageSlotPacket : public OutPacket
	{
	public:
		explicit CashShopStorageSlotPacket(int32_t cash)
			: OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x07);
			write_byte(0);
			write_int(cash);
			write_byte(0);
		}

		CashShopStorageSlotPacket(int32_t cash, int32_t sn)
			: OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x07);
			write_byte(0);
			write_int(cash);
			write_byte(1);
			write_int(sn);
		}
	};

	// 角色栏扩容：0x08 — skip1, cash, sn
	class CashShopCharSlotPacket : public OutPacket
	{
	public:
		CashShopCharSlotPacket(int32_t cash, int32_t sn)
			: OutPacket(OutPacket::Opcode::CASHSHOP_OPERATION)
		{
			write_byte(0x08);
			write_byte(0);
			write_int(cash);
			write_int(sn);
		}
	};

	// 刷新点券余额：s9 RecvOpcode.CHECK_CASH(0xE4) → TouchingCashShopHandler → QUERY_CASH_RESULT
	class CheckCashPacket : public OutPacket
	{
	public:
		CheckCashPacket() : OutPacket(OutPacket::Opcode::CHECK_CASH) {}
	};
}
