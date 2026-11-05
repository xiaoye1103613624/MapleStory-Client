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
		// 对齐 BeiDou IDA sub_74660A OnScriptMessage：先公共头，再按 msgtype 逐 case 解析。
		// 禁止先盲读 string（Say speaker&4 / Quiz / Avatar 等头字段不同）。
		recv.skip(1); // speakerType

		int32_t npcid = recv.read_int();
		const int8_t msgtype = recv.read_byte();
		const int8_t speaker = recv.read_byte();

		int16_t style = 0;
		std::string text;
		UINpcTalk::TalkInputParams input;
		bool show_ui = false;
		bool end_unhandled = false;

		auto drain_rest = [&recv]()
		{
			while (recv.available())
				recv.skip_byte();
		};

		switch (msgtype)
		{
		case 0: // Say — 7467AB：(speaker&4)? override npc；string；prev+next
			if ((speaker & 4) != 0 && recv.length() >= 4)
				npcid = recv.read_int();
			if (recv.length() >= 2)
				text = recv.read_string();
			if (recv.length() >= 2)
				style = recv.read_short();
			show_ui = true;
			break;

		case 1: // YesNo — 74692C(..., a7=0)
		case 12: // AcceptDecline — 74692C(..., a7=1)
			if (recv.length() >= 2)
				text = recv.read_string();
			show_ui = true;
			break;

		case 2: // GetText — 746A8B：talk + def + short minLen + short editParam
			if (recv.length() >= 2)
				text = recv.read_string();
			if (recv.length() >= 2)
				input.def_text = recv.read_string();
			if (recv.length() >= 2)
			{
				const int16_t min_len = recv.read_short();
				if (min_len > 0)
					input.text_min_len = min_len;
			}
			if (recv.length() >= 2)
			{
				const int16_t edit_param = recv.read_short();
				if (edit_param > 0)
					input.edit_maxlen = edit_param;
			}
			show_ui = true;
			break;

		case 3: // GetNumber — 746DFF：talk + def/min/max；9AC732 a5=0,a6=10
			if (recv.length() >= 2)
				text = recv.read_string();
			if (recv.length() >= 12)
			{
				input.def_number = recv.read_int();
				input.min_number = recv.read_int();
				input.max_number = recv.read_int();
				input.has_number_bounds = true;
			}
			input.number_min_digits = 0; // 线路径恒 0；tip 820 路径已接但默认不触发
			input.edit_maxlen = 10;
			if (recv.length() >= 4)
				recv.read_int(); // s9 getNPCTalkNum 尾 pad（客户端 IDA 不读）
			show_ui = true;
			break;

		case 4: // Simple — 746FAD
			if (recv.length() >= 2)
				text = recv.read_string();
			show_ui = true;
			break;

		case 5: // Quiz — A26B09：byte res；res==0 → 3×string + 3×int（本轮无 UI）
		{
			const int8_t res = recv.available() ? recv.read_byte() : static_cast<int8_t>(0);
			if (res == 0)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (recv.length() >= 2)
						recv.read_string();
				}
				for (int i = 0; i < 3; ++i)
				{
					if (recv.length() >= 4)
						recv.read_int();
				}
			}
			end_unhandled = true;
			break;
		}

		case 6: // SpeedQuiz — A26C66：byte res；res==0 → 5×int
		{
			const int8_t res = recv.available() ? recv.read_byte() : static_cast<int8_t>(0);
			if (res == 0)
			{
				for (int i = 0; i < 5; ++i)
				{
					if (recv.length() >= 4)
						recv.read_int();
				}
			}
			end_unhandled = true;
			break;
		}

		case 7: // AskAvatar — 74713D → UI dlgType=5（sub_9AC552 / sub_9A92C9）
		case 8: // MembershopAvatar — 74730B（同 UI；出站 lastMsg=8）
		{
			if (recv.length() >= 2)
				text = recv.read_string();
			const int n = recv.available() ? static_cast<unsigned char>(recv.read_byte()) : 0;
			input.style_ids.clear();
			input.style_ids.reserve(static_cast<size_t>(n));
			for (int i = 0; i < n && recv.length() >= 4; ++i)
				input.style_ids.push_back(recv.read_int());
			if (!input.style_ids.empty())
				show_ui = true;
			else
			{
				// 空样式：对齐服务端 sendStyle 空数组走 Ok+dispose；本端直接 -1 收口
				end_unhandled = true;
			}
			break;
		}

		case 9: // AskPet — 7474A2：string + byte n + n×byte
		{
			if (recv.length() >= 2)
				recv.read_string();
			const int n = recv.available() ? static_cast<unsigned char>(recv.read_byte()) : 0;
			for (int i = 0; i < n && recv.available(); ++i)
				recv.read_byte();
			end_unhandled = true;
			break;
		}

		case 10: // AskPetAll — 74775C：string + byte n + byte flag + n×byte
		{
			if (recv.length() >= 2)
				recv.read_string();
			const int n = recv.available() ? static_cast<unsigned char>(recv.read_byte()) : 0;
			if (recv.available())
				recv.read_byte();
			for (int i = 0; i < n && recv.available(); ++i)
				recv.read_byte();
			end_unhandled = true;
			break;
		}

		case 13: // AskBoxText — 746C46：同 GetText 形态（dlgType=8）
			if (recv.length() >= 2)
				recv.read_string();
			if (recv.length() >= 2)
				recv.read_string();
			if (recv.length() >= 2)
				recv.read_short();
			if (recv.length() >= 2)
				recv.read_short();
			end_unhandled = true;
			break;

		case 14: // Mirror — 76B5C8：int + string
			if (recv.length() >= 4)
				recv.read_int();
			if (recv.length() >= 2)
				recv.read_string();
			end_unhandled = true;
			break;

		default:
			end_unhandled = true;
			break;
		}

		drain_rest();

		if (end_unhandled)
		{
			// 未接 Quiz/Pet/Mirror 等 UI：读完包后安全收口，避免脚本挂死。
			LOG(LOG_WARN, "[NPC] ScriptMessage msgtype=" << static_cast<int>(msgtype)
				<< " reserved/no-UI; NpcTalkMore(-1)");
			NpcTalkMorePacket(msgtype, static_cast<int8_t>(-1)).dispatch();
			return;
		}

		if (!show_ui)
			return;

		const bool need_avatar = (msgtype == 7 || msgtype == 8);
		const bool has_shell = need_avatar
			? UiAssetResolve::usable(UiAssetResolve::window("UtilDlgEx_Avatar"))
			: UiAssetResolve::usable(UiAssetResolve::window("UtilDlgEx"));
		// 缺窗体资源时不要 emplace 隐形窗口，否则脚本会一直等客户端回复。
		if (!has_shell)
		{
			LOG(LOG_WARN, "[NPC] "
				<< (need_avatar ? "UtilDlgEx_Avatar" : "UtilDlgEx")
				<< " missing; end talk msgtype=" << static_cast<int>(msgtype));
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