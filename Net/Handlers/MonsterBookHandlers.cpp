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
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.	//
////////////////////////////////////////////////////////////////////////////////
#include "MonsterBookHandlers.h"

#include "../../Character/Player.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIMonsterBook.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"

#include <string>

namespace ms
{
	namespace {
		void refresh_book_if_open()
		{
			// 已打开时刷新；不因入站强制弹出图鉴窗
			if (auto ui = UI::get().get_element<UIMonsterBook>())
			{
				if (ui->is_active())
					ui->reload();
			}
		}

	}

	void MonsterBookCoverHandler::handle(InPacket& recv) const
	{
		int32_t cover = recv.read_int();
		while (recv.available())
			recv.skip_byte();
		Stage::get().get_player().get_monsterbook().set_cover(cover);
		refresh_book_if_open();
		// 封面变更已刷 UI，不 tip
		(void)cover;
	}

	void MonsterBookCardHandler::handle(InPacket& recv) const
	{
		recv.read_byte(); // full(0) / partial(1)
		int32_t cardid = recv.read_int();
		int32_t level = recv.read_int();
		while (recv.available())
			recv.skip_byte();

		// 与服务端 addMonsterBookInfo 一致：卡片键为 cardid % 10000
		Stage::get().get_player().get_monsterbook().add_card(
			static_cast<int16_t>(cardid % 10000),
			static_cast<int8_t>(level)
		);

		refresh_book_if_open();
		// 卡册已开才可见；不 tip 刷屏
		(void)cardid; (void)level;
	}
}
