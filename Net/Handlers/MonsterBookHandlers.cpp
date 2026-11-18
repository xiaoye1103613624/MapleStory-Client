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
			// 图鉴是 UIState 缓存的 TOGGLED 单例：pre_add 在实例已存在时只
			// toggle_active()、不重建也不重跑 ctor 的 load_cards()（UIStateGame.cpp:1030）。
			// 仅面板 active 时 reload 会让关窗期间拾取的卡灌不进单例缓存的 all_cards，
			// 重开图鉴仍是旧列表 —— 即「拾取后不显示」真因。实例存在即 reload：
			// load_cards() 只读模型重建列表，不弹窗、不改 active，零副作用。
			if (auto ui = UI::get().get_element<UIMonsterBook>())
			{
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
