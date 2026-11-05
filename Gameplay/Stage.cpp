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
#include "Stage.h"

#include "../Configuration.h"
#include "../MapleStory.h"

#include "../IO/UI.h"

#include "../IO/UITypes/UIStatusBar.h"
#include "../Net/Packets/AttackAndSkillPackets.h"
#include "../Net/Packets/GameplayPackets.h"
#include "../Util/PerfLog.h"
#include "../Util/Resource/ResourceService.h"

#include <stdexcept>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	Stage::Stage() : combat(player, chars, mobs, reactors)
	{
		state = State::INACTIVE;
	}

	void Stage::init()
	{
		drops.init();
	}

	void Stage::load(int32_t mapid, int8_t portalid)
	{
		switch (state)
		{
			case State::INACTIVE:
				load_map(mapid);   // 抛异常 = 真失败（map data missing），交给上层记录
				break;
			case State::TRANSITION:
				break;
		}

		// 地图数据一就绪就激活，不要等 respawn。
		// 旧实现把 state=ACTIVE 放在 respawn() 之后：respawn 里任一步
		// （get_portal_by_id / get_y_below / player.respawn / camera.set_view）
		// 抛异常都会让那行永不执行，Stage::draw 的 state!=ACTIVE 直接 return，
		// 整层背景/瓦片/角色全不画，每帧只剩 glClearColor(1,1,1,1) 的纯白 ——
		// 这就是「白屏 + 看不到角色 + 小地图不刷新」共用的上游故障。
		// respawn 的异常继续冒泡给 SetFieldHandlers（那边已落盘 ex.what()），
		// 但不再拖住 ACTIVE 状态：至少地图/瓦片可绘制，GL 也已由上层解锁。
		state = State::ACTIVE;

		respawn(portalid);
	}

	void Stage::loadplayer(const CharEntry& entry)
	{
		player = entry;
		playable = player;

		start = ContinuousTimer::get().start();

		CharStats stats = player.get_stats();
		levelBefore = stats.get_stat(MapleStat::Id::LEVEL);
		expBefore = stats.get_exp();
	}

	void Stage::clear()
	{
		state = State::INACTIVE;

		chars.clear();
		npcs.clear();
		mobs.clear();
		drops.clear();
		reactors.clear();
		field_markers.clear();
		aggro_sent.clear();	// Wave 4：换图清仇恨节流表（oid 图内唯一，跨图必换）
	}

	void Stage::load_map(int32_t mapid)
	{
		Stage::mapid = mapid;

		std::string strid = string_format::extend_id(mapid, 9);
		std::string prefix = std::to_string(mapid / 100000000);

		// img：只装当前地图，避免 ensure_map_roots 扫全 Map/ 导致黑屏未响应。
		if (mapid != -1)
			ms::res::ResourceService::get().ensure_map(mapid);

		nl::node src = mapid == -1 ? nl::nx::UI["CashShopPreview.img"] : nl::nx::Map002["Map"]["Map" + prefix][strid + ".img"];

		// 空地图节点会造出无 foothold 的物理世界，表现为进图后无法移动；
		// 抛异常交给 SetFieldHandlers::transition 捕获并解锁 GL，避免黑屏软锁。
		if (!src || !src["foothold"])
		{
			LOG(LOG_ERROR, "[MAP] missing map node mapid=" << mapid
				<< " path=Map002/Map/Map" << prefix << "/" << strid << ".img");
			throw std::runtime_error("map data missing");
		}

		tilesobjs = MapTilesObjs(src);
		backgrounds = MapBackgrounds(src["back"]);
		physics = Physics(src["foothold"]);
		mapinfo = MapInfo(src, physics.get_fht().get_walls(), physics.get_fht().get_borders());
		portals = MapPortals(src["portal"], mapid);
	}

	void Stage::respawn(int8_t portalid)
	{
		Music(mapinfo.get_bgm()).play();

		Point<int16_t> spawnpoint = portals.get_portal_by_id(portalid);
		Point<int16_t> startpos = physics.get_y_below(spawnpoint);
		player.respawn(startpos, mapinfo.is_underwater());
		camera.set_position(startpos);
		camera.set_view(mapinfo.get_walls(), mapinfo.get_borders());
	}

	void Stage::draw(float alpha) const
	{
		if (state != State::ACTIVE)
			return;

		Point<int16_t> viewpos = camera.position(alpha);
		Point<double> viewrpos = camera.realposition(alpha);
		double viewx = viewrpos.x();
		double viewy = viewrpos.y();

		backgrounds.drawbackgrounds(viewx, viewy, alpha);

		for (auto id : Layer::IDs)
		{
			tilesobjs.draw(id, viewpos, alpha);
			reactors.draw(id, viewx, viewy, alpha);
			npcs.draw(id, viewx, viewy, alpha);
			mobs.draw(id, viewx, viewy, alpha);
			chars.draw(id, viewx, viewy, alpha);
			player.draw(id, viewx, viewy, alpha);
			drops.draw(id, viewx, viewy, alpha);
			field_markers.draw(id, viewx, viewy, alpha);
		}

		combat.draw(viewx, viewy, alpha);
		portals.draw(viewpos, alpha);
		backgrounds.drawforegrounds(viewx, viewy, alpha);
		effect.draw();
	}

	void Stage::update()
	{
		if (state != State::ACTIVE)
			return;

		// debug.gm_smoke：进图稳住 3s 后自动发一次刷怪包（仅 GM 冒烟用）。
		// 为什么需要它：本机输入注入全部受阻（SetForegroundWindow/AttachThreadInput
		// 夺焦失败、PostMessage WM_CHAR 不被 SDL 接收），UI 驱动的冒烟跑不起来；
		// 而"场上有怪"是验证触怪仇恨(0xBD)/手雷(0x6D)/一键AP(0x58) 的共同前置。
		// 三态机 + 每图只发一次；关掉开关则整段零开销（一次 bool 读取）。
		// 注意：这里**不**用 Configuration::get_admin() 做门 —— 服务端 admin 位是
		// (use_enforce_admin_account||canFly) && GMLevel>1（PacketCreator.getAuthSuccess），
		// 本机 DB 种子该开关 false 且账号非 IP 放行，admin 位会是 false，
		// 但服务端 AdminCommandHandler 自己的门是 chr.isGM()（GMLevel>0），
		// 两者不等价。冒烟开关本身就是显式 opt-in，够安全。
		if (smoke_state != 2)
		{
			if (smoke_state == 0)
			{
				if (Setting<GmSmokeEnabled>::get().load())
				{
					smoke_state = 1;
					smoke_armed_at = std::chrono::steady_clock::now();
					perf::mark("SMOKE", "armed map=" + std::to_string(mapid));
				}
				else
				{
					smoke_state = 2;	// 未启用：本进程不再检查
				}
			}
			else if (smoke_state == 1)
			{
				const auto waited = std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - smoke_armed_at).count();

				if (waited >= 3000)
				{
					const int32_t mob = Setting<GmSmokeMob>::get().load();
					int32_t qty = Setting<GmSmokeCount>::get().load();
					if (qty <= 0) qty = 1;
					if (qty > 50) qty = 50;

					if (mob > 0)
					{
						AdminSummonMobPacket(mob, qty).dispatch();
						perf::mark("SMOKE", "summon mob=" + std::to_string(mob)
							+ " x" + std::to_string(qty) + " map=" + std::to_string(mapid));
					}

					smoke_state = 2;
				}
			}
		}

		combat.update();
		backgrounds.update();
		effect.update();
		tilesobjs.update();

		reactors.update(physics);
		npcs.update(physics);
		mobs.update(physics);
		chars.update(physics);
		drops.update(physics);
		field_markers.update(physics);
		player.update(physics);

		portals.update(player.get_position());
		camera.update(player.get_position());

		if (!player.is_climbing() && !player.is_sitting() && !player.is_attacking())
		{
			if (player.is_key_down(KeyAction::Id::UP) && !player.is_key_down(KeyAction::Id::DOWN))
				check_ladders(true);

			// 可见/触摸传送门（REGULAR/TOUCH 等）：走进即传送，无需按键
			check_portals();

			// 隐藏/脚本隐藏传送门：需按上键主动交互
			if (player.is_key_down(KeyAction::Id::UP))
				check_portals(true);

			// G6 mystic door：触碰式（进门框即上行 USE_DOOR，归属裁决在服务端
			// DoorHandler；冷却防刷屏见 MapFieldMarkers::check_doors）
			field_markers.check_doors(player.get_position());

			if (player.is_key_down(KeyAction::Id::DOWN))
				check_ladders(false);

			if (player.is_key_down(KeyAction::Id::SIT))
				check_seats();

			if (player.is_key_down(KeyAction::Id::ATTACK))
				combat.use_move(0);

			if (player.is_key_down(KeyAction::Id::PICKUP))
				check_drops();
		}

		if (player.is_invincible())
			return;

		if (int32_t oid_id = mobs.find_colliding(player.get_phobj()))
		{
			// Wave 4：触怪上报自动仇恨（s9 AutoAggroHandler int oid）。放在
			// create_attack 之外 —— 无 bodyAttack 的怪服务端同样该转战。
			// 每 oid 1s 节流（碰撞每帧跑）。
			auto aggro_it = aggro_sent.find(oid_id);
			const auto now = std::chrono::steady_clock::now();

			if (aggro_it == aggro_sent.end() || now - aggro_it->second >= std::chrono::seconds(1))
			{
				aggro_sent[oid_id] = now;
				AutoAggroPacket(oid_id).dispatch();
			}

			if (MobAttack attack = mobs.create_attack(oid_id))
			{
				MobAttackResult result = player.damage(attack);
				TakeDamagePacket(result, TakeDamagePacket::From::TOUCH).dispatch();
			}
		}
	}

	void Stage::show_character_effect(int32_t cid, CharEffect::Id effect)
	{
		if (auto character = get_character(cid))
			character->show_effect_id(effect);
	}

	void Stage::check_portals(bool allow_hidden)
	{
		if (player.is_attacking())
			return;

		Point<int16_t> playerpos = player.get_position();
		Portal::WarpInfo warpinfo = portals.find_warp_at(playerpos, allow_hidden);

		if (warpinfo.intramap)
		{
			Point<int16_t> spawnpoint = portals.get_portal_by_name(warpinfo.toname);
			Point<int16_t> startpos = physics.get_y_below(spawnpoint);

			player.respawn(startpos, mapinfo.is_underwater());
		}
		else if (warpinfo.valid)
		{
			ChangeMapPacket(false, -1, warpinfo.name, false).dispatch();

			CharStats& stats = Stage::get().get_player().get_stats();

			stats.set_mapid(warpinfo.mapid);

			Sound(Sound::Name::PORTAL).play();
		}
	}

	void Stage::check_seats()
	{
		if (player.is_sitting() || player.is_attacking())
			return;

		Optional<const Seat> seat = mapinfo.findseat(player.get_position());
		player.set_seat(seat);
	}

	void Stage::check_ladders(bool up)
	{
		if (!player.can_climb() || player.is_climbing() || player.is_attacking())
			return;

		Optional<const Ladder> ladder = mapinfo.findladder(player.get_position(), up);
		player.set_ladder(ladder);
	}

	void Stage::check_drops()
	{
		Point<int16_t> playerpos = player.get_position();
		MapDrops::Loot loot = drops.find_loot_at(playerpos);

		if (loot.first)
			PickupItemPacket(loot.first, loot.second).dispatch();
	}

	void Stage::send_key(KeyType::Id type, int32_t action, bool down)
	{
		if (state != State::ACTIVE || !playable)
			return;

		switch (type)
		{
			case KeyType::Id::ACTION:
				playable->send_action(KeyAction::actionbyid(action), down);
				break;
			case KeyType::Id::SKILL:
				combat.use_move(action);
				break;
			case KeyType::Id::ITEM:
				player.use_item(action);
				break;
			case KeyType::Id::FACE:
				player.set_expression(action);
				break;
		}
	}

	Cursor::State Stage::send_cursor(bool clicked, Point<int16_t> cursor_position)
	{
		auto statusbar = UI::get().get_element<UIStatusBar>();

		if (statusbar && statusbar->is_menu_active())
		{
			if (clicked)
				statusbar->remove_menus();

			if (statusbar->is_in_range(cursor_position))
				return statusbar->send_cursor(clicked, cursor_position);
		}

		return npcs.send_cursor(clicked, cursor_position, camera.position());
	}

	bool Stage::is_player(int32_t cid) const
	{
		return cid == player.get_oid();
	}

	MapNpcs& Stage::get_npcs()
	{
		return npcs;
	}

	MapChars& Stage::get_chars()
	{
		return chars;
	}

	MapMobs& Stage::get_mobs()
	{
		return mobs;
	}

	MapReactors& Stage::get_reactors()
	{
		return reactors;
	}

	MapDrops& Stage::get_drops()
	{
		return drops;
	}

	MapFieldMarkers& Stage::get_field_markers()
	{
		return field_markers;
	}

	Player& Stage::get_player()
	{
		return player;
	}

	Combat& Stage::get_combat()
	{
		return combat;
	}

	Optional<Char> Stage::get_character(int32_t cid)
	{
		if (is_player(cid))
			return player;
		else
			return chars.get_char(cid);
	}

	int32_t Stage::find_character_id(const std::string& name) const
	{
		return chars.find_id_by_name(name);
	}

	int Stage::get_mapid()
	{
		return mapid;
	}

	void Stage::add_effect(std::string path)
	{
		effect = MapEffect(path);
	}

	void Stage::clear_effect()
	{
		effect = MapEffect();
	}

	Camera& Stage::get_camera()
	{
		return camera;
	}

	int64_t Stage::get_uptime()
	{
		return ContinuousTimer::get().stop(start);
	}

	uint16_t Stage::get_uplevel()
	{
		return levelBefore;
	}

	int64_t Stage::get_upexp()
	{
		return expBefore;
	}

	void Stage::transfer_player()
	{
		PlayerMapTransferPacket().dispatch();

		if (Configuration::get().get_admin())
			AdminEnterMapPacket(AdminEnterMapPacket::Operation::ALERT_ADMINS).dispatch();
	}
}