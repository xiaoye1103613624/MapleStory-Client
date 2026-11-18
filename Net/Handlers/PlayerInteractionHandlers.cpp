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
#include "PlayerInteractionHandlers.h"

#include "../../IO/UI.h"

#include "../../IO/UITypes/UICharInfo.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"

#include <string>

namespace ms
{
	void CharInfoHandler::handle(InPacket& recv) const
	{
		// 最小合法 CHAR_INFO：cid(4)+lv(2)+job(2)+fame(2)+married(1)+2*string_len(2)+reserved(1)=14
		if (recv.available() < 14)
		{
			while (recv.available()) recv.skip_byte();
			return;
		}
		int32_t character_id = recv.read_int();
		uint16_t character_level = recv.read_short();
		int16_t character_job_id = recv.read_short();
		int16_t character_fame = recv.read_short();
		bool married = recv.read_byte() != 0;

		std::string guild_name = recv.read_string();
		std::string alliance_name = recv.read_string();

		recv.skip_byte();

		CharInfoData data;
		data.married = married;

		// 宠物（s9 PacketCreator.charInfo :2886-2900）：以 byte 0 结束；
		// 每只 itemid/name/level/tameness(亲密度)/fullness(饱清感)/short(0)/petEquipItemid
		int8_t pet_present = recv.read_byte();
		while (pet_present != 0 && data.pets.size() < 3 && recv.available() >= 4)
		{
			CharInfoData::Pet pet;
			pet.itemid = recv.read_int();
			pet.name = recv.available() ? recv.read_string() : "";
			pet.level = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
			pet.tameness = recv.available() ? static_cast<uint16_t>(recv.read_short()) : 0;
			pet.fullness = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
			if (recv.available() >= 2) recv.skip_short(); // 固定 0
			pet.equipItemid = recv.available() >= 4 ? recv.read_int() : 0;
			data.pets.push_back(std::move(pet));
			pet_present = recv.available() ? recv.read_byte() : 0;
		}

		// 坐骑（:2902-2911）：byte id != 0 时跟 level/exp/tiredness
		int8_t mount = recv.available() ? recv.read_byte() : 0;
		if (mount != 0 && recv.available() >= 12)
		{
			data.mount.id = mount;
			data.mount.level = recv.read_int();
			data.mount.exp = recv.read_int();
			data.mount.tiredness = recv.read_int();
		}

		// 心愿单（:2912-2915）：uint8_t 避免 >=128 变负导致整段丢弃
		uint8_t wishlist_size = recv.available() ? static_cast<uint8_t>(recv.read_byte()) : 0;
		for (uint8_t sn = 0; sn < wishlist_size && recv.available() >= 4; sn++)
			data.wishlist.push_back(recv.read_int());

		// 怪物图鉴（:2917-2922）
		data.book.level = recv.available() >= 4 ? recv.read_int() : 0;
		data.book.normal = recv.available() >= 4 ? recv.read_int() : 0;
		data.book.special = recv.available() >= 4 ? recv.read_int() : 0;
		int32_t book_cards = recv.available() >= 4 ? recv.read_int() : 0; // total
		data.book.total = book_cards;
		data.book.cover = recv.available() >= 4 ? recv.read_int() : 0;

		// 勋章（:2923-2941）：UICharInfo 只渲染 12 行，读满 12 即止（避免截断存储）
		data.medalId = recv.available() >= 4 ? recv.read_int() : 0;
		int16_t medal_quests_size = recv.available() >= 2 ? recv.read_short() : 0;
		for (int16_t s = 0; s < medal_quests_size && s < 12 && recv.available() >= 2; s++)
			data.medalQuests.push_back(recv.read_short());

		// 2026-11-18：他人装备列表（服务端 charInfo 尾部 addCharEquips）：
		// (byte bodypart, int itemid)* 直到 bodypart==0xFF 终止；bodypart 单位数上限 ~100，防畸形包。
		if (recv.available() >= 1)
		{
			uint8_t bodypart = static_cast<uint8_t>(recv.read_byte());
			int guard = 0;
			while (bodypart != 0xFF && guard++ < 128 && recv.available() >= 4)
			{
				CharInfoData::Equip eq;
				eq.bodypart = bodypart;
				eq.itemid = recv.read_int();
				data.equips.push_back(eq);
				if (!recv.available()) break;
				bodypart = static_cast<uint8_t>(recv.read_byte());
			}
		}
		while (recv.available())
			recv.skip_byte();

		// Update / ensure the character information window
		auto charinfo = UI::get().get_element<UICharInfo>();
		// 不同目标的 CHAR_INFO 必须换新窗：复用旧实例会让 target_cid/按钮禁用态停留在上一目标，
		// 且 target_character 指向旧角色（离图后 update_stats 裸解引用崩溃）。仅同 cid 才复用。
		if (charinfo && charinfo->get_target_cid() != character_id)
		{
			charinfo->deactivate();
			charinfo = nullptr;
		}
		if (!charinfo)
			charinfo = UI::get().emplace<UICharInfo>(character_id);
		else if (!charinfo->is_active())
			charinfo->makeactive();
		if (charinfo)
		{
			charinfo->update_stats(character_id, character_job_id, character_level, character_fame, guild_name, alliance_name);
			int pet_count = static_cast<int>(data.pets.size());
			int wish_count = static_cast<int>(data.wishlist.size());
			charinfo->update_extras(pet_count, wish_count, book_cards, data.medalId, married);
			charinfo->update_full(data);
		}
	}
}