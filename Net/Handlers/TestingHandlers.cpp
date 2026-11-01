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
#include "TestingHandlers.h"

#include "../../Audio/Audio.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"

#include "../../IO/UITypes/UILoginNotice.h"
#include "../../IO/UITypes/UIBossHP.h"
#include "../../IO/UITypes/UIAdmin.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"
#include "../../Util/Misc.h"

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		void field_tip(const std::string& text)
		{
			if (auto chatbar = UI::get().get_element<UIChatBar>())
			{
				chatbar->show_message(text.c_str(), UIChatBar::MessageType::YELLOW);
				return;
			}
		}
	}

	void CheckSpwResultHandler::handle(InPacket& recv) const
	{
		int reason = recv.read_byte();
		while (recv.available())
			recv.skip_byte();

		if (reason == 0)
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::INCORRECT_PIC);
		else
		{
			LOG(LOG_NETWORK, "[CheckSpwResultHandler]: Unknown reason: [" << reason << "]");
			std::string tip = "PIC check reason=" + std::to_string(reason);
			if (auto ad = UI::get().get_element<UIAdmin>())
			{
				if (ad->is_active())
					ad->set_message(tip);
			}
			else
				field_tip(tip);
		}

		UI::get().enable();
	}

	void FieldEffectHandler::handle(InPacket& recv) const
	{
		int mode = recv.read_byte();

		// 1 = trembleEffect(type, delay)
		if (mode == 1)
		{
			int8_t type = recv.available() ? recv.read_byte() : 0;
			int32_t delay = recv.available() >= 4 ? recv.read_int() : 0;
			while (recv.available())
				recv.skip_byte();
			LOG(LOG_DEBUG, "FieldEffect tremble type=" << (int)type << " delay=" << delay);
			Stage::get().get_camera().tremble(type, delay);
			// 镜头震动已显，不 tip
			return;
		}

		// 3 = screen / map effect path
		if (mode == 3)
		{
			std::string path = recv.read_string();
			while (recv.available())
				recv.skip_byte();
			Stage::get().add_effect(path);
			// MapFX 已播，不 tip
			return;
		}

		// 4 = mapSound / playSound
		if (mode == 4)
		{
			std::string path = recv.read_string();
			while (recv.available())
				recv.skip_byte();
			LOG(LOG_DEBUG, "FieldEffect sound=" << path);
#ifdef USE_NX
			nl::node node = nl::nx::Sound.resolve(path);
			if (!node)
				node = nl::nx::Sound002.resolve(path);
			if (node)
				Sound(node).play();
			else
#endif
				if (auto ad = UI::get().get_element<UIAdmin>())
				{
					if (ad->is_active())
						ad->set_message("SFX " + path);
				}
				// 缺 SFX：仅 Admin 可见，不扰普通玩家
			return;
		}

		// 5 = showBossHP (s9 PacketCreator.showBossHP)
		if (mode == 5)
		{
			if (recv.available() < 14)
			{
				while (recv.available())
					recv.skip_byte();
				return;
			}
			int32_t oid = recv.read_int();
			int32_t cur = recv.read_int();
			int32_t max = recv.read_int();
			uint8_t tag = static_cast<uint8_t>(recv.read_byte());
			uint8_t tagbg = static_cast<uint8_t>(recv.read_byte());
			while (recv.available())
				recv.skip_byte();
			auto ui = UI::get().get_element<UIBossHP>();
			if (!ui)
				ui = UI::get().emplace<UIBossHP>();
			if (ui)
				ui->set_hp(oid, cur, max, tag, tagbg);
			// BossHP UI 已显，不 tip
			(void)tag;
			(void)tagbg;
			return;
		}

		// 6 = musicChange
		if (mode == 6)
		{
			std::string path = recv.read_string();
			while (recv.available())
				recv.skip_byte();
			LOG(LOG_DEBUG, "FieldEffect music=" << path);
			Music(path).play();
			// 音乐已切，不 tip
			return;
		}

		// 0 / other string modes (dojo etc.)
		if (recv.available() > 0)
		{
			std::string path = recv.read_string();
			while (recv.available())
				recv.skip_byte();
			LOG(LOG_DEBUG, "FieldEffect mode=" << mode << " path=" << path);
			if (mode == 0 || mode == 2)
				Stage::get().add_effect(path);
			else
				if (auto ad = UI::get().get_element<UIAdmin>())
				{
					if (ad->is_active())
						ad->set_message("FieldFX " + std::to_string(mode) + " " + path);
				}
			return;
		}

		while (recv.available())
			recv.skip_byte();
		LOG(LOG_NETWORK, "[FieldEffectHandler]: Unknown value: [" << mode << "]");
	}
}