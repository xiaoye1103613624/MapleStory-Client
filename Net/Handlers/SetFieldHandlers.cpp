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
#include "SetFieldHandlers.h"

#include "Helpers/CharacterParser.h"
#include "Helpers/LoginParser.h"

#include "../Packets/GameplayPackets.h"

#include "../../Audio/Audio.h"
#include "../../Configuration.h"

#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/Window.h"

#include "../../IO/UITypes/UICharSelect.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"

#include "../../Util/Misc.h"
#include "../../Util/Resource/ResourceService.h"

#include <string>

namespace ms
{
	static void enter_game_log(const std::string& line)
	{
		LOG(LOG_DEBUG, line);
	}

	void SetFieldHandler::transition(int32_t mapid, uint8_t portalid, bool enter_game) const
	{
		float fadestep = 0.025f;

		Window::get().fadeout(
			fadestep,
			[mapid, portalid, enter_game]()
			{
				// Always unlock after the fade even if map/HUD load throws —
				// otherwise GraphicsGL stays locked (black screen) while BGM
				// already started in Stage::respawn, and the main thread can
				// appear "Not Responding".
				enter_game_log(std::string("[SetField] fade done map=") + std::to_string(mapid)
					+ " portal=" + std::to_string(portalid)
					+ " enter_game=" + (enter_game ? "1" : "0"));

				// 首次进游戏也清空 atlas：登录期 MapLogin 大背景已占满 atlas，
				// 装图中途 clearinternal 会打乱已上传图块 → 黑屏+BGM。
				GraphicsGL::get().unload_atlas();
				enter_game_log(enter_game
					? "[SetField] unload_atlas (enter_game)"
					: "[SetField] unload_atlas (map transfer)");

				// img：补齐 Mob/Npc 等进图根；单张地图在 Stage::load → ensure_map。
				try
				{
					ms::res::ResourceService::get().ensure_map_roots();
					enter_game_log("[SetField] ensure_map_roots ok");
					Window::get().poll_events();
				}
				catch (const std::exception& ex)
				{
					enter_game_log(std::string("[SetField] ensure_map_roots FAILED: ") + ex.what());
				}

				try
				{
					Stage::get().load(mapid, portalid);
					enter_game_log("[SetField] Stage::load ok");
					Window::get().poll_events();
				}
				catch (const std::exception& ex)
				{
					enter_game_log(std::string("[SetField] Stage::load FAILED: ") + ex.what());
				}

				// Pump messages only — do NOT call check_events() here.
				// check_events may call initwindow → GraphicsGL::reinit, and the
				// old reinit wiped the atlas after Stage::load had already
				// uploaded tiles (BGM on, still locked → black + Not Responding).
				Window::get().poll_events();

				UI::get().enable();
				Timer::get().start();
				GraphicsGL::get().unlock();
				enter_game_log("[SetField] GraphicsGL unlocked");

				// Switch to GAME UI after the map is loaded (and GL unlocked).
				// Constructing StatusBar3/UIWindow2 HUD while locked used to
				// freeze the client on v83 packs that only ship StatusBar /
				// UIWindow.
				if (enter_game)
				{
					try
					{
						UI::get().change_state(UI::State::GAME);
						enter_game_log("[SetField] UI State::GAME ok");
					}
					catch (const std::exception& ex)
					{
						enter_game_log(std::string("[SetField] UI State::GAME FAILED: ") + ex.what());
					}

					Window::get().poll_events();
				}

				try
				{
					Stage::get().transfer_player();
					enter_game_log("[SetField] transfer_player ok");
				}
				catch (const std::exception& ex)
				{
					enter_game_log(std::string("[SetField] transfer_player FAILED: ") + ex.what());
				}

				Window::get().poll_events();
				enter_game_log(std::string("[SetField] fadeprocedure done locked=")
					+ (GraphicsGL::get().is_locked() ? "1" : "0"));
			});

		GraphicsGL::get().lock();
		Stage::get().clear();
		Timer::get().start();
	}

	void SetFieldHandler::handle(InPacket& recv) const
	{
		Constants::Constants::get().set_viewwidth(Setting<Width>::get().load());
		Constants::Constants::get().set_viewheight(Setting<Height>::get().load());

		int32_t channel = recv.read_int();

		// 服务端每次 SET_FIELD 的首个 int 就是**当前 0 基频道号**
		// （BeiDou-Server_s9 util/PacketCreator.java:995 getCharInfo / :1076 getWarpToMap
		//  都写 `getClient().getChannel() - 1`）。换频后客户端是重连到新频道服再收
		// SET_FIELD，这里不存的话 Configuration::channelid 会一直停在登录时选的那条，
		// UIChannel 就把「当前频道」画错、同频道保护也随之失效（服务端对同频道换频
		// 是直接 disconnect 的）。
		Configuration::get().set_channelid(static_cast<uint8_t>(channel));

		int8_t mode1 = recv.read_byte();
		int8_t mode2 = recv.read_byte();

		if (mode1 == 0 && mode2 == 0)
			change_map(recv, channel);
		else
			set_field(recv);
	}

	void SetFieldHandler::change_map(InPacket& recv, int32_t) const
	{
		recv.skip(3);

		int32_t mapid = recv.read_int();
		int8_t portalid = recv.read_byte();
		while (recv.available())
			recv.skip_byte();
		// 换图过密，不 tip
		transition(mapid, portalid, false);
	}

	void SetFieldHandler::set_field(InPacket& recv) const
	{
		recv.skip(23);

		int32_t cid = recv.read_int();
		auto charselect = UI::get().get_element<UICharSelect>();

		if (!charselect)
		{
			enter_game_log("[SetField] set_field aborted: UICharSelect missing");
			LOG(LOG_ERROR, "[SetField] enter-game without UICharSelect; reconnect required");
			return;
		}

		const CharEntry& playerentry = charselect->get_character(cid);

		if (playerentry.id != cid)
		{
			enter_game_log(std::string("[SetField] set_field aborted: cid mismatch want=")
				+ std::to_string(cid) + " got=" + std::to_string(playerentry.id));
			LOG(LOG_ERROR, "[SetField] character id mismatch cid=" << cid);
			return;
		}

		Stage::get().loadplayer(playerentry);

		LoginParser::parse_stats(recv);

		Player& player = Stage::get().get_player();

		recv.read_byte(); // 'buddycap'

		if (recv.read_bool())
			recv.read_string(); // 'linkedname'

		CharacterParser::parse_inventory(recv, player.get_inventory());
		CharacterParser::parse_skillbook(recv, player.get_skills());
		CharacterParser::parse_cooldowns(recv, player);
		CharacterParser::parse_questlog(recv, player.get_quests());
		CharacterParser::parse_minigame(recv);
		CharacterParser::parse_ring1(recv);
		CharacterParser::parse_ring2(recv);
		CharacterParser::parse_ring3(recv);
		CharacterParser::parse_teleportrock(recv, player.get_teleportrock());
		CharacterParser::parse_monsterbook(recv, player.get_monsterbook());
		CharacterParser::parse_nyinfo(recv);
		CharacterParser::parse_areainfo(recv);
		while (recv.available())
			recv.skip_byte();

		player.recalc_stats(true);

		PlayerUpdatePacket().dispatch();

		uint8_t portalid = player.get_stats().get_portal();
		int32_t mapid = player.get_stats().get_mapid();

		// 进图淡入已显，不 tip
		(void)mapid;

		transition(mapid, portalid, true);

		Sound(Sound::Name::GAMESTART).play();
	}
}