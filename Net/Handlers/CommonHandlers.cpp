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
#include "../../Character/CombatPanelStats.h"
#include "../../Util/PerfLog.h"

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

	void SetItemFinalDamageHandler::handle(InPacket& recv) const
	{
		// 布局对齐 BeiDou PacketCreator.setItemFinalDamageBonus / ijl15 SetItemBridge
		CombatPanelStats& s = CombatPanelStats::get();
		s.final_damage_percent = recv.read_short();
		s.damage_skin_id = recv.read_int();
		s.dam_r = recv.read_short();
		s.boss_dam_r = recv.read_short();
		s.normal_dam_r = recv.read_short();
		s.ignore_pdr = recv.read_short();
		s.ignore_mdr = recv.read_short();
		s.crit_rate = recv.read_short();
		s.crit_dam = recv.read_short();
		s.pad_r = recv.read_short();
		s.mad_r = recv.read_short();

		const uint16_t fd_count = static_cast<uint16_t>(recv.read_short());
		for (uint16_t i = 0; i < fd_count; i++)
			recv.skip_short();

		s.item_drop_prop = recv.read_short();
		s.meso_drop_prop = recv.read_short();
		s.damage_reduce = recv.read_short();

		if (recv.available() >= 6)
		{
			s.asr_r = recv.read_short();
			s.buff_time_r = recv.read_short();
			s.stance_prop = recv.read_short();
		}

		while (recv.available())
			recv.skip_byte();

		s.synced = true;
		s.notify_ui();
		LOG(LOG_DEBUG, "CombatPanel 0x175 synced damR=" << s.dam_r
			<< " fdR=" << s.final_damage_percent
			<< " bdR=" << s.boss_dam_r);
	}
}