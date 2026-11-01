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

#include "../PacketHandler.h"

namespace ms
{
	// Handler for entering the Cash Shop
	class SetCashShopHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;

	private:
		void transition() const;
	};

	// s9 PacketCreator.showCash → SendOpcode.QUERY_CASH_RESULT(324)
	// 读序：int NX_CREDIT / int MAPLE_POINT / int NX_PREPAID
	class QueryCashResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 SendOpcode.CASHSHOP_OPERATION(325) 回包：wishlist / bought / message 等
	class CashShopOperationHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
		// 供同文件 gachapon 等复用
		static void skip_cash_item_info(InPacket& recv);
	};

	// s9 CASHSHOP_CASH_ITEM_GACHAPON_RESULT(333)：0xE4 fail / 0xE5 success
	class CashItemGachaponResultHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 sendNameTransferCheck → CASHSHOP_CHECK_NAME_CHANGE(328)
	class CashShopCheckNameChangeHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 sendNameTransferRules → CASHSHOP_CHECK_NAME_CHANGE_POSSIBLE_RESULT(329)
	class CashShopNameChangePossibleHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};

	// s9 sendWorldTransferRules → CASHSHOP_CHECK_TRANSFER_WORLD_POSSIBLE_RESULT(331)
	class CashShopTransferWorldPossibleHandler : public PacketHandler
	{
	public:
		void handle(InPacket& recv) const override;
	};
}