////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero Public License as published by			//
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

#include "../../Character/Inventory/InventoryType.h"

namespace ms
{
	// 仓库交互包：Opcode STORAGE(309)，与 s9 StorageHandler 字段序对齐。
	// 客户端发出用 SendOpcode.STORAGE（同值 309），服务端 RecvOpcode.STORAGE 接收。
	class StoragePacket : public OutPacket
	{
	public:
		StoragePacket(uint8_t mode) : OutPacket(OutPacket::Opcode::STORAGE)
		{
			write_byte(mode);
		}
	};

	// 取出：mode 4 -> byte type, byte slot（slot 为 0 基）
	class StorageTakePacket : public StoragePacket
	{
	public:
		StorageTakePacket(InventoryType::Id type, int16_t slot) : StoragePacket(4)
		{
			write_byte(static_cast<int8_t>(type));
			write_byte(static_cast<int8_t>(slot));
		}
	};

	// 存入：mode 5 -> short slot, int itemId, short quantity（slot 为 1 基玩家背包槽）
	class StorageStorePacket : public StoragePacket
	{
	public:
		StorageStorePacket(int16_t slot, int32_t itemId, int16_t quantity) : StoragePacket(5)
		{
			write_short(slot);
			write_int(itemId);
			write_short(quantity);
		}
	};

	// 整理：mode 6
	class StorageArrangePacket : public StoragePacket
	{
	public:
		StorageArrangePacket() : StoragePacket(6) {}
	};

	// 金币：mode 7 -> int meso（正数=取出，负数=存入）
	class StorageMesoPacket : public StoragePacket
	{
	public:
		StorageMesoPacket(int32_t meso) : StoragePacket(7)
		{
			write_int(meso);
		}
	};

	// 关闭：mode 8
	class StorageClosePacket : public StoragePacket
	{
	public:
		StorageClosePacket() : StoragePacket(8) {}
	};
}
