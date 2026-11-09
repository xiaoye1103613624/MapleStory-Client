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
#include "../../Util/ResolutionTiers.h"

#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"
#include "../../IO/Window.h"

#include "../../IO/UITypes/UICharSelect.h"
#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../Graphics/Color.h"

#include "../../Util/Misc.h"
#include "../../Util/PerfLog.h"
#include "../../Util/Resource/ResourceService.h"

#include <chrono>
#include <fstream>
#include <string>

namespace ms
{
	static void enter_game_log(const std::string& line)
	{
		LOG(LOG_DEBUG, line);

		// Release 下 LOG 被定义成 void(0)（MapleStory.h:77），所有 [SetField]
		// 行都不落盘，白屏时拿不到 Stage::load 是否抛异常的实锤。这里额外写
		// exe 同目录 setfield_trace.log，绕过日志级别，供进图问题取证。
		static std::ofstream trace("setfield_trace.log", std::ios::app);
		if (trace.is_open())
		{
			trace << line << std::endl;
			trace.flush();
		}
	}

	// 选角进游戏后按 window.game_* 设置视口；登录/选角/世界/频道恒 1280×720。
	// 只写 Constants：本函数在 Window::fadeout 的回调里，禁止调 check_events()
	// （回调内 initwindow → GraphicsGL::reinit 会清掉刚上传的图块导致黑屏）。
	// 实际建窗由下一帧 MapleStory::update() 的 Window::check_events 完成。
	static void apply_game_viewsize()
	{
		const int tier = static_cast<int>(Setting<GameTier>::get().load());
		const int16_t fallback_w = static_cast<int16_t>(Setting<GameWidth>::get().load());
		const int16_t fallback_h = static_cast<int16_t>(Setting<GameHeight>::get().load());
		const int16_t max_w = Configuration::get().get_max_width();
		const int16_t max_h = Configuration::get().get_max_height();

		int16_t width = 0;
		int16_t height = 0;
		const int chosen = ResolutionTiers::resolve_game_size(
			tier, fallback_w, fallback_h, max_w, max_h, width, height);

		if (chosen < 0)
		{
			LOG(LOG_WARN, "[MR] game tier " << tier << " exceeds desktop "
				<< max_w << "x" << max_h << " and no tier fits; keeping viewport");
			return;
		}

		// 仅当「显式选了某一档且该档超桌面」才算回落；TIER_NONE(255) 走 fallback 不算。
		if (chosen != tier && ResolutionTiers::valid(tier))
		{
			LOG(LOG_WARN, "[MR] game tier " << tier << " exceeds desktop; fall back to tier " << chosen);
			// 同步内存值，避免设置窗下拉显示与实际视口不一致（不在此处写盘）。
			Setting<GameTier>::get().save(static_cast<uint8_t>(chosen));
			Setting<GameWidth>::get().save(static_cast<uint16_t>(width));
			Setting<GameHeight>::get().save(static_cast<uint16_t>(height));
		}

		ResolutionTiers::apply_viewsize(width, height);
		LOG(LOG_INFO, "[MR] enter game viewport " << width << "x" << height << " tier=" << chosen);
	}

	void SetFieldHandler::transition(int32_t mapid, uint8_t portalid, bool enter_game) const
	{
		// 淡出+淡入各 1/step 个 TIMESTEP(8ms) 拍：0.025→40 拍=320ms/侧，
		// 0.04→25 拍=200ms/侧（Wave P 丝滑批，原生无此长黑窗）。
		// 保留黑场是为遮住装图卡顿，不是留 640ms 仪式感。
		float fadestep = 0.04f;

		Window::get().fadeout(
			fadestep,
			[mapid, portalid, enter_game]()
			{
				// Wave 0.1 仪器：淡出回调整体跑在一帧里，它的耗时 = 用户感知的
				// "换图黑一下"。分段 mark 到 boot_perf.log（debug.perf_log 开关）。
				const auto seg_t0 = std::chrono::steady_clock::now();
				// 捕获 [&]：body 要读外层 lambda 捕获的 mapid —— 显式只捕
				// seg_t0 时外层按值捕获名在嵌套 lambda 里不可见（C3493 根因）。
				auto seg = [&](const char* label)
				{
					const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::steady_clock::now() - seg_t0).count();
					perf::mark("SETFIELD", std::string(label) + " map=" + std::to_string(mapid)
						+ " cum_ms=" + std::to_string(ms));
				};

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
				seg("after_unload_atlas");

				// img：补齐 Mob/Npc 等进图根；单张地图在 Stage::load → ensure_map。
				try
				{
					ms::res::ResourceService::get().ensure_map_roots();
					enter_game_log("[SetField] ensure_map_roots ok");
					Window::get().poll_events();
					seg("after_ensure_map_roots");
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
					seg("after_stage_load");
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
				// after_stage_load→after_unlock 差值 = HUD 构建 + 视口切换成本；
				// 首帧真正的贴图上传发生在解锁后的第一个 draw（不在本回调内）。
				seg("after_unlock");

				// 所有 SET_FIELD（首次进游戏 / 换图 / 传送 / 退出商城）都按 window.game_*
				// 恢复游戏视口：商城由 SetCashShopHandler::transition 单独设 1024×768，
				// 若只在 enter_game 分支恢复，退出商城后视口会一直停在 1024×768。
				// 只写 Constants；实际建窗等下一帧主循环 check_events 完成（本回调内禁止
				// 调 check_events：reinit 会清掉刚上传的图块导致黑屏）。
				apply_game_viewsize();

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
					seg("total");
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
		// 旧实现每次 SET_FIELD 都把 window.width/height 回灌 Constants —— 那是登录基准，
		// 换图/传送时会把游戏内已切换的分辨率覆盖掉。游戏态视口统一由
		// transition() 按 window.game_* 恢复（含 enter_game=false 的换图/退出商城），
		// 登录态由 UI::change_state(State::LOGIN) 恢复；此处不再回灌。
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

		// SET_FIELD 携带服务端刚从 DB 装载并修正过的权威角色状态；CHARLIST 的
		// entry 只服务于选角页，其当前 HP/MP 在部分服务端版本是新手默认值。
		// 旧实现把 parse_stats 的返回值丢弃，Player 一直沿用选角页数据，
		// 于是进图首帧出现 50/3622 这类假血条，直到后续 STAT_CHANGED 才纠正。
		// 这里把进图包 stats 应用回 Player，保证「下线多少、上线就是多少」。
		StatsEntry fieldstats = LoginParser::parse_stats(recv);

		Player& player = Stage::get().get_player();

		static const MapleStat::Id synced_stats[] = {
			MapleStat::Id::LEVEL, MapleStat::Id::JOB,
			MapleStat::Id::STR, MapleStat::Id::DEX, MapleStat::Id::INT, MapleStat::Id::LUK,
			MapleStat::Id::HP, MapleStat::Id::MAXHP,
			MapleStat::Id::MP, MapleStat::Id::MAXMP,
			MapleStat::Id::AP, MapleStat::Id::SP, MapleStat::Id::FAME
		};

		auto& playerstats = player.get_stats();

		for (MapleStat::Id id : synced_stats)
			playerstats.set_stat(id, fieldstats.stats[id]);

		playerstats.set_exp(fieldstats.exp);

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
		CharacterParser::parse_ring3(recv, player.get_marriage());
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