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
#include "LoginHandlers.h"

#include "Helpers/LoginParser.h"

#include "../Packets/LoginPackets.h"

#include "../../IO/UI.h"

#include "../../IO/UITypes/UICharSelect.h"
#include "../../IO/UITypes/UIGender.h"
#include "../../IO/UITypes/UILoginNotice.h"
#include "../../IO/UITypes/UILoginWait.h"
#include "../../IO/UITypes/UIRaceSelect.h"
#include "../../IO/UITypes/UITermsOfService.h"
#include "../../IO/UITypes/UIWorldSelect.h"

#include "../../Configuration.h"
#include "../../IO/Window.h"
#include "../../Util/Resource/ResourceService.h"

namespace ms
{
	void LoginResultHandler::handle(InPacket& recv) const
	{
		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
		{
			// 失败路径才立刻拆掉 Wait；成功路径要等 ensure_game_roots 结束再拆，
			// 否则装 Character 时无加载框，像“登录卡死半分钟”。
			UI::get().remove(UIElement::Type::LOGINNOTICE);
			UI::get().remove(UIElement::Type::TOS);
			UI::get().remove(UIElement::Type::GENDER);

			std::function<void()> okhandler = loginwait->get_handler();

			// The packet should contain a 'reason' integer which can signify various things
			if (int32_t reason = recv.read_int())
			{
				UI::get().remove(UIElement::Type::LOGINWAIT);

				// Login unsuccessful
				// The LoginNotice displayed will contain the specific information
				switch (reason)
				{
					case 2:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::BLOCKED_ID, okhandler);
						break;
					}
					case 5:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
						break;
					}
					case 7:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ALREADY_LOGGED_IN, okhandler);
						break;
					}
					case 13:
					{
						UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNABLE_TO_LOGIN_WITH_IP, okhandler);
						break;
					}
					case 23:
					{
						UI::get().emplace<UITermsOfService>(okhandler);
						break;
					}
					default:
					{
						// Other reasons
						if (reason > 0)
						{
							auto reasonbyte = static_cast<int8_t>(reason - 1);

							UI::get().emplace<UILoginNotice>(reasonbyte, okhandler);
						}

						break;
					}
				}
			}
			else
			{
				// Login successful
				// The packet contains information on the account, so we initialize the account with it.
				Account account = LoginParser::parse_account(recv);

				Configuration::get().set_admin(account.admin);

				// img：先装选角根并重建 BodyDrawInfo；期间保留 Wait 并泵消息。
				ms::res::ResourceService::get().ensure_game_roots();
				Window::get().poll_events();

				UI::get().remove(UIElement::Type::LOGINWAIT);

				if (account.female == 10)
				{
					UI::get().emplace<UIGender>(okhandler);
				}
				else
				{
					// Prefer a non-empty server account name; never wipe a typed ID
					// that UILogin already persisted when "save account" is on.
					if (Setting<SaveLogin>::get().load() && !account.name.empty())
					{
						Setting<DefaultAccount>::get().save(account.name);
						Configuration::get().save();
					}

					// Request the list of worlds and channels online
					ServerRequestPacket().dispatch();
				}
			}
		}
	}

	void ServerStatusHandler::handle(InPacket& recv) const
	{
		// Possible values for status:
		// 0 - Normal
		// 1 - Highly populated
		// 2 - Full
		//
		// 服务端依据：ServerStatusRequestHandler → World.getWorldCapacityStatus()，
		// 世界不存在时回 2（PacketCreator.getServerStatus 只写一个 short）。
		// 客户端语义：1 提示人口过多（UILoginNotice::POPULATION_TOO_HIGH）仍可进入，
		// 2 额外阻止进入该世界；交由 UIWorldSelect 统一处理。
		const int16_t status = recv.read_short();

		if (auto worldselect = UI::get().get_element<UIWorldSelect>())
			worldselect->set_server_status(status);
	}

	void SelectCharacterHandler::handle(InPacket& recv) const
	{
		std::function<void()> okhandler = []() {};

		// The packet should contain a 'reason' integer which can signify various things
		if (int16_t reason = recv.read_short())
		{
			// 选角失败：复位请求门闩，允许玩家重新选择
			if (auto charselect = UI::get().get_element<UICharSelect>())
				charselect->clear_pending();

			// Select character unsuccessful
			// The LoginNotice displayed will contain the specific information
			switch (reason)
			{
				case 2:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::BLOCKED_ID, okhandler);
					break;
				}
				case 5:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
					break;
				}
				case 7:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::ALREADY_LOGGED_IN, okhandler);
					break;
				}
				case 13:
				{
					UI::get().emplace<UILoginNotice>(UILoginNotice::Message::UNABLE_TO_LOGIN_WITH_IP, okhandler);
					break;
				}
				case 23:
				{
					UI::get().emplace<UITermsOfService>(okhandler);
					break;
				}
				default:
				{
					// Other reasons
					if (reason > 0)
					{
						auto reasonbyte = static_cast<int8_t>(reason - 1);

						UI::get().emplace<UILoginNotice>(reasonbyte, okhandler);
					}

					break;
				}
			}
		}
	}

	void ServerlistHandler::handle(InPacket& recv) const
	{
		auto worldselect = UI::get().get_element<UIWorldSelect>();

		if (!worldselect)
			worldselect = UI::get().emplace<UIWorldSelect>();

		// Parse all worlds
		while (recv.available())
		{
			World world = LoginParser::parse_world(recv);

			if (world.id != -1)
			{
				worldselect->add_world(world);
			}
			else
			{
				// Remove previous UIs
				UI::get().remove(UIElement::Type::LOGIN);

				// Add the world selection screen to the UI
				worldselect->draw_world();

				// End of packet
				return;
			}
		}
	}

	void CharlistHandler::handle(InPacket& recv) const
	{
		// 旧实现要求「存在且活动的 UILoginWait」才处理，否则整包角色列表被静默丢弃。
		// 但 Wait 取消后 UIStateLogin 会在下一帧把它移除（见门闩看护），
		// 重连/补发场景回包到达时 Wait 已不在 → 选角页永远出不来。
		// 这里只保留 Wait 的清理语义，不再把它当必要条件。
		auto loginwait = UI::get().get_element<UILoginWait>();

		{
			uint8_t channel_id = recv.read_byte();

			// Parse all characters
			std::vector<CharEntry> characters;
			int8_t charcount = recv.read_byte();

			for (uint8_t i = 0; i < charcount; ++i)
				characters.emplace_back(LoginParser::parse_charentry(recv));

			int8_t pic = recv.read_byte();
			int32_t slots = recv.read_int();

			// 选角 CharLook 构造依赖 BodyDrawInfo；登录成功虽已 ensure，
			// 频道包到达前若 Character 未绑全会得到空锚点 → 头身脚叠成白影。
			// 在拆 Wait / 建 UICharSelect 前再保一次（已绑则几乎无开销）。
			ms::res::ResourceService::get().ensure_game_roots();
			Window::get().poll_events();

			// Remove previous UIs
			UI::get().remove(UIElement::Type::LOGINNOTICE);
			UI::get().remove(UIElement::Type::LOGINWAIT);

			// Remove the world selection screen
			if (auto worldselect = UI::get().get_element<UIWorldSelect>())
				worldselect->remove_selected();

			// Add the character selection screen
			UI::get().emplace<UICharSelect>(characters, charcount, slots, pic);
		}
	}

	void ServerIPHandler::handle(InPacket& recv) const
	{
		recv.skip_byte();

		LoginParser::parse_login(recv);

		int32_t cid = recv.read_int();
		PlayerLoginPacket(cid).dispatch();
	}

	void CharnameResponseHandler::handle(InPacket& recv) const
	{
		// Read the name and if it is already in use
		std::string name = recv.read_string();
		bool used = recv.read_bool();

		// Notify the character creation screen
		if (auto raceselect = UI::get().get_element<UIRaceSelect>())
			raceselect->send_naming_result(used);
	}

	void AddNewCharEntryHandler::handle(InPacket& recv) const
	{
		// 首字节为结果码：服务端 PacketCreator.addNewCharEntry 恒写 0 后接角色条目；
		// 创建失败走的是 DELETE_CHAR_RESPONSE(cid=0, state=9)（CreateCharHandler 的
		// 非法名/非法外观分支），不会带条目。旧实现无条件解析，失败包会把空条目
		// 插进选角页（表现为凭空多出一个空白角色）。
		const uint8_t result = recv.read_byte();

		if (result != 0 || !recv.available())
			return;

		// Parse info on the new character
		CharEntry character = LoginParser::parse_charentry(recv);

		// Read the updated character selection
		if (auto charselect = UI::get().get_element<UICharSelect>())
			charselect->add_character(std::move(character));
	}

	void DeleteCharResponseHandler::handle(InPacket& recv) const
	{
		// Read the character id and if deletion was successful (PIC was correct)
		int32_t cid = recv.read_int();
		uint8_t state = recv.read_byte();

		// 删除回包到达：无论成功/失败都复位请求门闩
		if (auto charselect = UI::get().get_element<UICharSelect>())
			charselect->clear_pending();

		// Extract information from the state byte
		if (state)
		{
			UILoginNotice::Message message;

			switch (state)
			{
				case 10:
					message = UILoginNotice::Message::BIRTHDAY_INCORRECT;
					break;
				case 20:
					message = UILoginNotice::Message::INCORRECT_PIC;
					break;
				default:
					message = UILoginNotice::Message::UNKNOWN_ERROR;
			}

			UI::get().emplace<UILoginNotice>(message);
		}
		else
		{
			if (auto charselect = UI::get().get_element<UICharSelect>())
				charselect->remove_character(cid);
		}
	}
}