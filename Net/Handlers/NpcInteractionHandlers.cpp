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

#include "../../IO/UITypes/UINpcTalk.h"
#include "../../IO/UITypes/UIShop.h"

#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/UiAssetResolve.h"

#include "../../MapleStory.h"

namespace ms
{
	void NpcDialogueHandler::handle(InPacket& recv) const
	{
		recv.skip(1);

		int32_t npcid = recv.read_int();
		int8_t msgtype = recv.read_byte(); // 0 Say, 1 YesNo, 4 Simple, 12 AcceptDecline
		int8_t speaker = recv.read_byte();
		std::string text = recv.read_string();

		int16_t style = 0;

		if (msgtype == 0 && recv.length() > 0)
			style = recv.read_short();

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
			npctalk->change_text(npcid, msgtype, style, speaker, text);
	}

	void OpenNpcShopHandler::handle(InPacket& recv) const
	{
		int32_t npcid = recv.read_int();
		auto oshop = UI::get().get_element<UIShop>();

		if (!oshop)
			return;

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
	}

	void ConfirmShopTransactionHandler::handle(InPacket& recv) const
	{
		// v83 / BeiDou: first byte is the result mode (0 = ok for common paths).
		if (recv.length() <= 0)
			return;

		const int8_t mode = recv.read_byte();
		if (mode != 0)
			LOG(LOG_INFO, "[SHOP] transaction result mode=" << static_cast<int>(mode));

		// Remaining bytes (if any) are version-specific; skip safely.
		if (recv.length() > 0)
			recv.skip(recv.length());
	}
}