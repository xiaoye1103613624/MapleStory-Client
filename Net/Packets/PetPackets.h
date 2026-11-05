////////////////////////////////////////////////////////////////////////////////
//	W1 宠物族 c2s — 对账 s9 RecvOpcode + *Pet*Handler / SpawnPetHandler
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "MovementPacket.h"

#include <string>
#include <vector>

namespace ms
{
	// Opcode: SPAWN_PET(98 / 0x62)
	// SpawnPetHandler: readInt ts, readByte slot, readByte, readByte lead
	class SpawnPetPacket : public OutPacket
	{
	public:
		SpawnPetPacket(int8_t cash_slot, bool lead)
			: OutPacket(OutPacket::Opcode::SPAWN_PET)
		{
			write_time();
			write_byte(cash_slot);
			write_byte(0);
			write_byte(lead ? 1 : 0);
		}
	};

	// Opcode: PET_FOOD(76 / 0x4C)
	// PetFoodHandler: readInt ts, readShort pos, readInt itemId
	class PetFoodPacket : public OutPacket
	{
	public:
		PetFoodPacket(int16_t use_slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::PET_FOOD)
		{
			write_time();
			write_short(use_slot);
			write_int(itemid);
		}
	};

	// Opcode: USE_MOUNT_FOOD(77 / 0x4D)
	// UseMountFoodHandler: skip(4), readShort pos, readInt itemid
	class UseMountFoodPacket : public OutPacket
	{
	public:
		UseMountFoodPacket(int16_t use_slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::USE_MOUNT_FOOD)
		{
			write_int(0);
			write_short(use_slot);
			write_int(itemid);
		}
	};

	// Opcode: MOVE_PET(167 / 0xA7)
	// MovePetHandler: readInt petId, readLong, parseMovement
	class MovePetPacket : public MovementPacket
	{
	public:
		MovePetPacket(int32_t pet_unique_id, const Movement& movement)
			: MovementPacket(OutPacket::Opcode::MOVE_PET)
		{
			write_int(pet_unique_id);
			write_long(0);
			write_byte(1);
			writemovement(movement);
		}
	};

	// Opcode: PET_CHAT(168 / 0xA8)
	// PetChatHandler: petId, int, byte, act(byte), string
	class PetChatPacket : public OutPacket
	{
	public:
		PetChatPacket(int32_t pet_unique_id, int8_t act, const std::string& text)
			: OutPacket(OutPacket::Opcode::PET_CHAT)
		{
			write_int(pet_unique_id);
			write_int(0);
			write_byte(0);
			write_byte(act);
			write_string(text);
		}
	};

	// Opcode: PET_COMMAND(169 / 0xA9)
	// PetCommandHandler: petId, int, byte, command(byte)
	class PetCommandPacket : public OutPacket
	{
	public:
		PetCommandPacket(int32_t pet_unique_id, int8_t command)
			: OutPacket(OutPacket::Opcode::PET_COMMAND)
		{
			write_int(pet_unique_id);
			write_int(0);
			write_byte(0);
			write_byte(command);
		}
	};

	// Opcode: PET_LOOT(170 / 0xAA)
	// PetLootHandler: petId, skip(13), oid
	class PetLootPacket : public OutPacket
	{
	public:
		PetLootPacket(int32_t pet_unique_id, int32_t oid)
			: OutPacket(OutPacket::Opcode::PET_LOOT)
		{
			write_int(pet_unique_id);
			for (int i = 0; i < 13; ++i)
				write_byte(0);
			write_int(oid);
		}
	};

	// Opcode: PET_AUTO_POT(171 / 0xAB)
	// PetAutoPotHandler: byte, long, int, short slot, int itemId
	class PetAutoPotPacket : public OutPacket
	{
	public:
		PetAutoPotPacket(int16_t use_slot, int32_t itemid)
			: OutPacket(OutPacket::Opcode::PET_AUTO_POT)
		{
			write_byte(0);
			write_long(0);
			write_int(0);
			write_short(use_slot);
			write_int(itemid);
		}
	};

	// Opcode: PET_EXCLUDE_ITEMS(172 / 0xAC)
	// PetExcludeItemsHandler: petId, skip(4), byte count, int itemId × n
	class PetExcludeItemsPacket : public OutPacket
	{
	public:
		PetExcludeItemsPacket(int32_t pet_unique_id, const std::vector<int32_t>& item_ids)
			: OutPacket(OutPacket::Opcode::PET_EXCLUDE_ITEMS)
		{
			write_int(pet_unique_id);
			write_int(0);
			int8_t n = static_cast<int8_t>(item_ids.size() > 127 ? 127 : item_ids.size());
			write_byte(n);
			for (int8_t i = 0; i < n; ++i)
				write_int(item_ids[static_cast<size_t>(i)]);
		}
	};
}
