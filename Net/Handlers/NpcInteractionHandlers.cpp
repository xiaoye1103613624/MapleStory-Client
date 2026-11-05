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
#include "NpcInteractionHandlers.h"

#include "../../IO/UI.h"
#include "../../Gameplay/Stage.h"

#include "../../IO/UITypes/UINpcTalk.h"
#include "../../IO/UITypes/UIShop.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../Graphics/Color.h"

#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"

#include "../../MapleStory.h"

#include <string>

namespace ms
{
	void NpcDialogueHandler::handle(InPacket& recv) const
	{
		recv.skip(1);

		int32_t npcid = recv.read_int();
		int8_t msgtype = recv.read_byte(); // 0 Say, 1 YesNo, 2 GetText, 3 GetNumber, 4 Simple, 12 AcceptDecline
		int8_t speaker = recv.read_byte();
		std::string text = recv.read_string();

		int16_t style = 0;
		UINpcTalk::TalkInputParams input;

		if (msgtype == 0 && recv.length() >= 2)
			style = recv.read_short();
		// BeiDou PacketCreator.getNPCTalkText：def string + int(常 0)
		// getNPCTalkNum：def/min/max int + int(常 0)。校验依据 IDA sub_9AB50A / sub_9AC732。
		// 注意：InPacket::available() 是 bool；剩余字节数须用 length()。
		else if (msgtype == 2)
		{
			if (recv.length() >= 2)
				input.def_text = recv.read_string();
			if (recv.length() >= 4)
			{
				const int32_t tail = recv.read_int();
				if (tail > 0)
					input.text_min_len = tail;
			}
		}
		else if (msgtype == 3)
		{
			if (recv.length() >= 12)
			{
				input.def_number = recv.read_int();
				input.min_number = recv.read_int();
				input.max_number = recv.read_int();
				input.has_number_bounds = true;
				input.edit_maxlen = 10; // IDA AskNumber 助手 sub_9AC732 a6→this+0xCC 常 10
			}
			if (recv.length() >= 4)
				recv.read_int(); // trailing padding（服务端写 0）
		}
		while (recv.available())
			recv.skip_byte();

		// 缺 UtilDlgEx 时不要 emplace 隐形窗口，否则脚本会一直等客户端回复。
		if (!UiAssetResolve::usable(UiAssetResolve::window("UtilDlgEx")))
		{
			LOG(LOG_WARN, "[NPC] UtilDlgEx missing; end talk msgtype=" << static_cast<int>(msgtype));
			NpcTalkMorePacket(msgtype, static_cast<int8_t>(-1)).dispatch();
			return;
		}

		UI::get().emplace<UINpcTalk>();
		UI::get().enable();

		if (auto npctalk = UI::get().get_element<UINpcTalk>())
			npctalk->change_text(npcid, msgtype, style, speaker, text, input);
		// NpcTalk 已显，不重复 tip
	}

	void OpenNpcShopHandler::handle(InPacket& recv) const
	{
		int32_t npcid = recv.read_int();
		auto oshop = UI::get().get_element<UIShop>();

		if (!oshop)
		{
			Player& player = Stage::get().get_player();
			oshop = UI::get().emplace<UIShop>(player.get_look(), player.get_inventory());
		}
		if (!oshop)
		{
			while (recv.available())
				recv.skip_byte();
			return;
		}
		else if (!oshop->is_active())
			oshop->makeactive();

		UIShop& shop = *oshop;

		shop.reset(npcid);

		int16_t size = recv.read_short();

		for (int16_t i = 0; i < size; i++)
		{
			int32_t itemid = recv.read_int();
			int32_t price = recv.read_int();
			int32_t pitch = recv.read_int();
			int32_t time = recv.read_int();

			recv.skip(4);

			bool norecharge = recv.read_short() == 1;

			if (norecharge)
			{
				int16_t buyable = recv.read_short();

				shop.add_item(itemid, price, pitch, time, buyable);
			}
			else
			{
				recv.skip(4);

				int16_t rechargeprice = recv.read_short();
				int16_t slotmax = recv.read_short();

				shop.add_rechargable(itemid, price, pitch, time, rechargeprice, slotmax);
			}
		}
		while (recv.available())
			recv.skip_byte();
		// Shop UI 已刷，不 tip
		(void)size;
	}

	void ConfirmShopTransactionHandler::handle(InPacket& recv) const
	{
		// v83 / BeiDou: first byte is the result mode (0 = ok for common paths).
		if (recv.available() <= 0)
			return;

		const int8_t mode = recv.read_byte();
		if (mode != 0)
			LOG(LOG_INFO, "[SHOP] transaction result mode=" << static_cast<int>(mode));
		while (recv.available())
			recv.skip_byte();

		// Shop ensure：结果自显，不 tip
		auto shop = UI::get().get_element<UIShop>();
		if (!shop)
		{
			auto& player = Stage::get().get_player();
			shop = UI::get().emplace<UIShop>(player.get_look(), player.get_inventory());
		}
		else if (!shop->is_active())
			shop->makeactive();
		if (shop)
			return;
		std::string tip = mode == 0 ? "Shop ok" : ("Shop err " + std::to_string(mode));
		auto mt = mode == 0 ? UIChatBar::MessageType::YELLOW : UIChatBar::MessageType::RED;
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(tip.c_str(), mt);
	}
}