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
#include "CommonHandlers.h"

#include "../Packets/CommonPackets.h"

#include "../../IO/UI.h"
#include "../../IO/UITypes/UIAdmin.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/Window.h"
#include "../../Graphics/Color.h"
#include "../../Gameplay/Combat/Damage.h"

#include <string>

namespace ms
{
	void PingHandler::handle(InPacket&) const
	{
		PongPacket().dispatch();
	}

	void LimitBreakSyncHandler::handle(InPacket& recv) const
	{
		int64_t lb = recv.read_long();
		while (recv.available())
			recv.skip_byte();
		Window::get().set_limit_break(lb);
		// 破功值不再只用于标题：同步给伤害 clamp（min(配置 cap, 破功值, 2^53)）。
		// 负值/0 表示服务端未配置上限，effective_cap 会忽略该项。
		damage::set_server_limit_break(lb);
		std::string tip = "LimitBreak " + std::to_string(lb);
		if (auto ui = UI::get().get_element<UIAdmin>())
		{
			if (ui->is_active())
			{
				ui->set_message(tip);
				return;
			}
		}
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(tip.c_str(), UIChatBar::MessageType::WHITE);
	}
}