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
#include "../Graphics/GraphicsGL.h"
#include "../Character/SkillMacros.h"
#include "../Weather/WeatherFx.h"
#include "../Weather/WeatherState.h"
#include "../Weather/WeatherSkyTable.h"
#include "../Weather/WeatherBackLayers.h"
#include "../Weather/WeatherLamps.h"
#include "../Weather/WeatherGround.h"

#include "../IO/UI.h"

#include "../IO/UITypes/UIStatusBar.h"
#include "../IO/UITypes/UIChatBar.h"
#include "../IO/UITypes/UIChatEmoticon.h"
#include "../Net/Packets/AttackAndSkillPackets.h"
#include "../Net/Packets/GameplayPackets.h"
#include "../Net/Packets/MessagingPackets.h"
#include "../Net/Packets/PetPackets.h"
#include "../Net/Packets/PlayerPackets.h"
#include "../Net/Packets/SummonDragonPackets.h"
#include "../Net/Packets/MapInteractPackets.h"
#include "MapleMap/Reactor.h"
#include "../Constants.h"
#include "../Util/Misc.h"
#include "../Util/PerfLog.h"
#include "../Util/Resource/ResourceService.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

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
		// 2026-11-18 退出商城黑屏根治：旧实现只认 state==INACTIVE 才 load_map，
		// 而进商城由 SetCashShopHandler::transition 走 clear()->load(-1,0) 把 Stage
		// 留在 ACTIVE + CashShopPreview；退出商城的 exit_cashshop 若没先 clear()，
		// load(真实 mapid) 就落进 TRANSITION 分支变空操作 —— 世界仍是商城预览，
		// 游戏视角全黑、BGM 仍是预览曲（_audio.log 末行 GoPicnic 铁证）。
		// 不变量：只要请求的 mapid 与当前不一致，就必须真正换图；无论外部 clear()
		// 是否漏调。这样三个调用点（SetField/CashShop/exit_cashshop）同源安全。
		// 注：TRANSITION 是历史遗留枚举，全仓无任何赋值点，实际只有 INACTIVE/ACTIVE。
		// 注意 mapid 成员未在构造期初始化，必须用 || 短路：
		// INACTIVE 时直接 load_map，不读取可能未初始化的 this->mapid。
		// ACTIVE 时 mapid 必已被某次 load_map 赋值，比较才安全。
		if (state == State::INACTIVE || this->mapid != mapid)
			load_map(mapid);   // 抛异常 = 真失败（map data missing），交给上层记录

		// 地图数据一就绪就激活，不要等 respawn。
		// 旧实现把 state=ACTIVE 放在 respawn() 之后：respawn 里任一步
		// （get_portal_by_id / get_y_below / player.respawn / camera.set_view）
		// 抛异常都会让那行永不执行，Stage::draw 的 state!=ACTIVE 直接 return，
		// 整层背景/瓦片/角色全不画，每帧只剩 glClearColor(1,1,1,1) 的纯白 ——
		// 这就是「白屏 + 看不到角色 + 小地图不刷新」共用的上游故障。
		// respawn 的异常继续冒泡给 SetFieldHandlers（那边已落盘 ex.what()），
		// 但不再拖住 ACTIVE 状态：至少地图/瓦片可绘制，GL 也已由上层解锁。
		state = State::ACTIVE;

		// Angel OnGameStageChanged：聊天 type2（输入中）切回 type1，避免跨图残留输入带。
		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->end_chat_if_typing();

		respawn(portalid);
	}

	void Stage::loadplayer(const CharEntry& entry)
	{
		player = entry;
		playable = player;
		store_char_entry(entry);

		start = ContinuousTimer::get().start();

		CharStats stats = player.get_stats();
		levelBefore = stats.get_stat(MapleStat::Id::LEVEL);
		expBefore = stats.get_exp();
	}

	void Stage::store_char_entry(const CharEntry& entry)
	{
		char_entry = entry;
	}

	const CharEntry& Stage::get_char_entry() const
	{
		return char_entry;
	}

	bool Stage::has_char_entry(int32_t cid) const
	{
		return char_entry.id != 0 && char_entry.id == cid;
	}

	void Stage::on_viewport_changed()
	{
		// 视口切档主动刷新背景平铺（A+C 方案）。Background::draw 也每帧实时重算，
		// 这里让切换当帧立即正确，不依赖下一帧。天气天空层 draw_list 已按当前
		// Constants 实时计算 cover/平铺，无需重建素材（重建会丢天空门禁状态）。
		backgrounds.on_viewport_changed();
	}

	void Stage::clear(bool keep_weather)
	{
		state = State::INACTIVE;

		chars.clear();
		npcs.clear();
		mobs.clear();
		drops.clear();
		reactors.clear();
		field_markers.clear();
		guild_boss.clear();
		aggro_sent.clear();	// Wave 4：换图清仇恨节流表（oid 图内唯一，跨图必换）
		touch_atk_sent.clear();
		hot_accum_ms = 0;
		touching_reactor_oid = 0;

		// 天气系统：换图/退商城（keep_weather=true）保留 State/Fx/Lamps，避免
		// clear→0x373D apply 之间 ≥200ms 的"闪白天"窗口，也避免路灯 stagger 重置。
		// 服务端每次进图都补发 0x373D(snap=true) 权威覆盖；登录/退出彻底清空。
		if (!keep_weather)
		{
			weather::State::get().clear();
			weather::Fx::get().reset();
			weather::BackLayers::get().clear();
			weather::Lamps::get().clear();
			weather::Ground::get().clear();
		}
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
		// 天气门禁：nosky + skytable + 水下例外（对齐插件 FieldHasSky / IsUnderwaterMap）。
		// 不 gate 服务端 0x373D 状态——进图包在装图之后才到；门禁只决定本图能不能画天气。
		{
			bool allow_falling = false, allow_tint = false;
			weather::field_gate(mapid,
				backgrounds.get_backmost_bs().c_str(), backgrounds.get_backmost_no(),
				allow_falling, allow_tint);
			weather::State::get().set_field_gate(allow_falling, allow_tint);
			weather::BackLayers::get().on_map_loaded(mapid,
				backgrounds.get_backmost_bs(), backgrounds.get_backmost_no());
		}
		weather::Ground::get().on_map_loaded();
		physics = Physics(src["foothold"]);
		mapinfo = MapInfo(src, physics.get_fht().get_walls(), physics.get_fht().get_borders());
		portals = MapPortals(src["portal"], mapid);
		// 公会 PQ：地图根节点 healer/pulley（仅 990000900 等有）
		guild_boss = MapGuildBoss(src);
	}

	void Stage::respawn(int8_t portalid)
	{
		Music(mapinfo.get_bgm()).play();

		Point<int16_t> spawnpoint = portals.get_portal_by_id(portalid);
		Point<int16_t> startpos = physics.get_y_below(spawnpoint);
		player.respawn(startpos, mapinfo.is_underwater());
		camera.clear_tremble(); // 换图清屏复位震屏偏移（MinMS Stage.cpp:277）
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

		// 天气分层（对齐插件 MakeBack 的 z 序语义，weather.cpp:1916-1924）：
		//   1) 地图自有天空 back/0-1
		//   2) 注入的天空素材（渐变/月/星/云/沙暴/彩虹/雪远）
		//   3) 地图山树 back/2+ —— 永远盖在天气之上，树不会被天气吞掉
		// 三段都走 MapBackgrounds 的区间重载，原生逐层绘制算法一行未改。
		// 2026-11-23 用户拍板口径（最新）：**地图自带背景 back 全部压暗**（天空 back/0-1
		// 与山树 back/2+ 都压），注入的夜空/月、瓦片/物件、NPC/怪物、角色、UI 一律不压暗。
		const bool night_active = weather::Fx::get().night_level() > 0.01f
			&& weather::State::get().allow_tint();
		const Color fg_tint = night_active ? weather::Fx::get().scene_tint() : Color(1.0f, 1.0f, 1.0f, 1.0f);
		GraphicsGL& gl = GraphicsGL::get();

		const size_t sky_end = backgrounds.size() < 2 ? backgrounds.size() : 2;
		if (night_active) gl.push_tint(fg_tint);
		backgrounds.drawbackgrounds(viewx, viewy, alpha, 0, sky_end); // 天空 back/0-1：压暗
		if (night_active) gl.pop_tint();
		weather::BackLayers::get().draw_sky(viewx, viewy, alpha);     // 注入夜空/月/云/星/彩虹：不压
		if (night_active) gl.push_tint(fg_tint);
		backgrounds.drawbackgrounds(viewx, viewy, alpha, sky_end, backgrounds.size()); // 山树 back/2+：压暗
		if (night_active) gl.pop_tint();

		for (auto id : Layer::IDs)
		{
			// 瓦片/物件/反应器/NPC/怪物/角色/掉落/标记：全部不压暗。
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
		// Kaentake 动态聊天表情：头顶动画按角色屏幕位置绘制（viewpos 同 chars 口径）。
		UIChatEmoticon::draw_overhead(viewpos, alpha);
		portals.draw(viewpos, alpha);
		guild_boss.draw(viewpos, alpha);
		backgrounds.drawforegrounds(viewx, viewy, alpha);
		effect.draw();

		// 天气前景素材（雨/雾/雪近）：自绘，严格在角色之后、夜色蒙版之前；不动原生背景。
		weather::BackLayers::get().draw_front(viewx, viewy, alpha);
		// 自绘雨雪/花瓣/沙尘粒子（A 方案：GraphicsGL 直接画，真机可见）。
		weather::Fx::get().draw_particles(viewx, viewy);

		// 天气地面层（水坑/积雪/溅射）：角色之后、夜色光照之前。
		weather::Ground::get().draw(viewpos, player.get_position(), &physics.get_fht());
		// 夜色已改为 Stage::draw 开头的分层 push_tint（Fx::draw 保留空实现兼容旧调用）。
		// 2026-11-23：只保留月光光晕（玩家/NPC 光晕已删除），强度随夜色深入。
		weather::Fx::get().draw_lights();
		weather::Fx::get().draw_lightning();
		// 路灯暖色光晕（世界坐标按 viewx/viewy 换算），盖在场景之上、UI 之下。
		weather::Lamps::get().draw(viewx, viewy, alpha);

		// 2026-11-23 夜色只压场景、不压 UI：把场景批次（含夜色乘色/灯光/光晕）
		// 在此即时提交。修复前所有 quad 挤到帧末唯一 flush()，乘色在 UI 绘制后
		// 才作用于整屏 framebuffer，导致所有窗口/道具/文字一起变暗——与
		// Stage.cpp:266「只压场景不压 UI」的设计意图相悖。UI 在此之后由
		// MapleStory.cpp:236 绘制，在帧末 flush() 中提交，不受夜色影响。
		GraphicsGL::get().flush_scene();
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
					// 冒烟前置体检：把四项未验项各自缺什么一次打清楚
					// （AP 是否够、等级/职业能否用目标技能、背包有无书/袋、
					//   场上是否有 buff 源）。只读不改，perf 通道落盘。
					{
						const CharStats& cs = player.get_stats();
						const int32_t ap = cs.get_stat(MapleStat::Id::AP);
						const int32_t lv = cs.get_stat(MapleStat::Id::LEVEL);
						const int32_t job = cs.get_job().get_id();
						perf::mark("SMOKE", "probe ap=" + std::to_string(ap)
							+ " lv=" + std::to_string(lv)
							+ " job=" + std::to_string(job));
						// 手雷前置：Gunslinger.GRENADE(5201002) / NightWalker.POISON_BOMB(14111006)
						// 本端只有一个技能书容器，取等级 0 即"没这个技能"。
						perf::mark("SMOKE", "skill grenade=" + std::to_string(player.get_skills().get_level(5201002))
							+ " poisonbomb=" + std::to_string(player.get_skills().get_level(14111006)));
					}

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
		guild_boss.update();

		reactors.update(physics);
		npcs.update(physics);
		mobs.update(physics);
		chars.update(physics);
		UIChatEmoticon::update_overhead();
		drops.update(physics);
		field_markers.update(physics);
		player.update(physics);
		// W2：本角色召唤兽/龙 MOVE + SUMMON_ATTACK
		field_markers.report_owned_c2s(player.get_oid(), player.get_position(), mobs);

		portals.update(player.get_position());
		camera.update(player.get_position());

		if (!player.is_climbing() && !player.is_sitting() && !player.is_attacking())
		{
			if (player.is_key_down(KeyAction::Id::UP) && !player.is_key_down(KeyAction::Id::DOWN))
				check_ladders(true);

			// 碰触门（TOUCH / SCRIPTED_TOUCH）：走进即传送，无需按键。
			// 普通门（REGULAR/SCRIPTED/HIDDEN…）原生要求按上键，见下一个分支。
			check_portals(MapPortals::PortalScope::TOUCH_ONLY);

			// 其余传送门：按上键主动交互（083 依据：`CPortalList::FindPortal @0x712ab1` 按键通道命中框 = 水平 ±range / 垂直固定 ±50 band；旧注释「v95 CPortal::CheckKey」在 083 全库无此符号，属依据链污染，见 `tools/_ida_r93/portal_family.json`）
			if (player.is_key_down(KeyAction::Id::UP))
				check_portals(MapPortals::PortalScope::ALL);

			// W4：触碰式反应器 enter/leave
			check_touch_reactors();

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

		// HEAL_OVER_TIME：每 10s 上报一次自然回复（对账 s9 HealOvertimeHandler）。
		hot_accum_ms = static_cast<uint16_t>(hot_accum_ms + Constants::TIMESTEP);
		if (hot_accum_ms >= 10000)
		{
			hot_accum_ms = 0;
			try_heal_over_time();
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

			// TOUCH_MONSTER_ATTACK(47)：身体压力或能量条满(15000)时触怪造成伤害。
			// s9 TouchMonsterDamageHandler 仅此两条件结算；body=parseDamage。
			const bool body_pressure = player.has_buff(Buffstat::Id::BODY_PRESSURE);
			const bool energy_full = player.get_buff_value(Buffstat::Id::ENERGY_CHARGE) >= 15000;
			if (body_pressure || energy_full)
			{
				auto touch_it = touch_atk_sent.find(oid_id);
				if (touch_it == touch_atk_sent.end()
					|| now - touch_it->second >= std::chrono::milliseconds(500))
				{
					touch_atk_sent[oid_id] = now;
					Attack attack = player.prepare_attack(false);
					if (body_pressure)
					{
						const int32_t sid = player.get_buff_skillid(Buffstat::Id::BODY_PRESSURE);
						attack.skill = sid > 0 ? sid : 21101003; // Aran.BODY_PRESSURE 缺省
					}
					attack.hitcount = 1;
					attack.mobcount = 1;
					AttackResult result(attack);
					std::vector<int32_t> targets{ oid_id };
					mobs.send_attack(result, attack, targets, 1);
					result.attacker = player.get_oid();
					AttackPacket(result, OutPacket::Opcode::TOUCH_MONSTER_ATTACK).dispatch();
					LOG(LOG_DEBUG, "[Stage] TOUCH_MONSTER_ATTACK oid=" << oid_id
						<< " skill=" << attack.skill
						<< " energy=" << (int)player.get_buff_value(Buffstat::Id::ENERGY_CHARGE));
				}
			}

			if (MobAttack attack = mobs.create_attack(oid_id))
			{
				MobAttackResult result = player.damage(attack);
				TakeDamagePacket(result, TakeDamagePacket::From::TOUCH).dispatch();
			}
		}

		// 天气系统：固定逻辑步长推进本地时钟与夜色趋近。
		// TIMESTEP 本身就是毫秒（主循环用 TIMESTEP*1000 转微秒给 Timer）；
		// 这里绝不能再 *1000，否则本地时钟/粒子相位快约 1000 倍、包间昼夜狂闪。
		// 未收到 0x373D 时 Fx::update 内部零开销返回，不影响 stock 表现。
		weather::Fx::get().update(static_cast<int64_t>(Constants::TIMESTEP));
		// 天气前景素材层：每帧按天空/时钟驱动雨/雾/雪 alpha + 动画；不改原生背景逻辑。
		// flags 带 SNAP 时（进图包）直接到位。
		weather::BackLayers::get().update((weather::State::get().flags() & weather::FLAG_SNAP) != 0,
			static_cast<uint16_t>(Constants::TIMESTEP));
		// 天气路灯：按当前夜色强度推进光晕 fade；进图 SNAP 时直接到位不暖灯。
		weather::Lamps::get().update(weather::Fx::get().night_level(),
			(weather::State::get().flags() & weather::FLAG_SNAP) != 0);
		// 天气地面层：推进积雪/水坑/溅射动画 + 环境音状态桩。
		weather::Ground::get().update(static_cast<uint16_t>(Constants::TIMESTEP));
		weather::Ground::get().update_ambient();
	}

	void Stage::try_heal_over_time()
	{
		// 死亡或未进图不报。死亡态一律不回血，避免与服务端权威 HP 反复覆盖
		// （真机曾出现 110↔1110 跳变以及 autopot 触发后的异常高值）。
		if (player.get_char_state() == Char::State::DIED)
			return;

		const int32_t hp = player.get_stats().get_stat(MapleStat::Id::HP);
		if (hp == 0)
			return;

		const int32_t maxhp = player.get_stats().get_stat(MapleStat::Id::MAXHP);
		const int32_t mp = player.get_stats().get_stat(MapleStat::Id::MP);
		const int32_t maxmp = player.get_stats().get_stat(MapleStat::Id::MAXMP);
		const float rec = mapinfo.get_recovery();
		if (rec <= 0.0f)
			return;

		// 原生近似：每 tick ≈ max * 1% * recovery；钳到 s9 abHeal=77*rec*1.5。
		const int ab_heal = static_cast<int>(77.0f * rec * 1.5f + 0.5f);
		auto clamp_heal = [ab_heal](int want, int room) -> int16_t
		{
			if (want <= 0 || room <= 0)
				return 0;
			int v = std::min(want, room);
			v = std::min(v, std::max(ab_heal, 1));
			return static_cast<int16_t>(v);
		};

		const int want_hp = static_cast<int>(std::lround(static_cast<double>(maxhp) * 0.01 * rec));
		const int want_mp = static_cast<int>(std::lround(static_cast<double>(maxmp) * 0.01 * rec));
		const int16_t heal_hp = clamp_heal(want_hp, static_cast<int>(maxhp) - static_cast<int>(hp));
		int16_t heal_mp = clamp_heal(want_mp, static_cast<int>(maxmp) - static_cast<int>(mp));
		// s9：healMP != 0 && healMP < 1000 才加；钳到 999。
		if (heal_mp >= 1000)
			heal_mp = 999;

		if (heal_hp == 0 && heal_mp == 0)
			return;

		HealOvertimePacket(heal_hp, heal_mp).dispatch();
		LOG(LOG_DEBUG, "[Stage] HEAL_OVER_TIME hp=" << heal_hp << " mp=" << heal_mp
			<< " rec=" << rec << " abHeal=" << ab_heal);
		perf::mark("HOT", std::string("hp=") + std::to_string(heal_hp)
			+ " mp=" + std::to_string(heal_mp)
			+ " rec=" + std::to_string(rec));
	}

	void Stage::show_character_effect(int32_t cid, CharEffect::Id effect)
	{
		if (auto character = get_character(cid))
			character->show_effect_id(effect);
	}

	void Stage::check_portals(MapPortals::PortalScope scope)
	{
		if (player.is_attacking())
			return;

		Point<int16_t> playerpos = player.get_position();
		Portal::WarpInfo warpinfo = portals.find_warp_at(playerpos, scope);

		if (warpinfo.intramap)
		{
			// W4：同图内门上报 USE_INNER_PORTAL，本地仍瞬移保手感
			UseInnerPortalPacket(warpinfo.name).dispatch();
			Point<int16_t> spawnpoint = portals.get_portal_by_name(warpinfo.toname);
			Point<int16_t> startpos = physics.get_y_below(spawnpoint);

			player.respawn(startpos, mapinfo.is_underwater());
		}
		else if (warpinfo.valid)
		{
			if (warpinfo.scripted)
				ChangeMapSpecialPacket(warpinfo.name).dispatch();
			else
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

		if (!loot.first)
			return;

		// W1：有召唤宠物时优先走 PET_LOOT（服务端校验宠物/玩家距离与吸物装备）
		int32_t pet_uid = 0;
		for (uint8_t i = 0; i < 3; ++i)
		{
			const PetLook& pet = player.get_pet(i);
			if (pet.get_itemid() > 0 && pet.get_uniqueid() > 0)
			{
				pet_uid = pet.get_uniqueid();
				break;
			}
		}
		if (pet_uid > 0)
			PetLootPacket(pet_uid, loot.first).dispatch();
		else
			PickupItemPacket(loot.first, loot.second).dispatch();
	}

	void Stage::check_touch_reactors()
	{
		Point<int16_t> playerpos = player.get_position();
		int32_t found = 0;
		if (auto* list = reactors.get_reactors())
		{
			for (auto& kv : *list)
			{
				auto* r = dynamic_cast<Reactor*>(kv.second.get());
				if (!r || !r->is_active())
					continue;
				// 不可攻击的反应器视为触碰型（对账 TouchReactor 用途）
				if (r->is_hittable())
					continue;
				Rectangle<int16_t> range(
					playerpos + Point<int16_t>(-40, -80),
					playerpos + Point<int16_t>(40, 20));
				if (r->is_in_range(range))
				{
					found = r->get_oid();
					break;
				}
			}
		}
		if (found != touching_reactor_oid)
		{
			if (touching_reactor_oid > 0)
				TouchingReactorPacket(touching_reactor_oid, false).dispatch();
			if (found > 0)
				TouchingReactorPacket(found, true).dispatch();
			touching_reactor_oid = found;
		}
	}

	void Stage::send_key(KeyType::Id type, int32_t action, bool down)
	{
		if (state != State::ACTIVE || !playable)
			return;

		// 死亡态：吞掉所有按键，避免倒地后还能放技能/开窗。
		// 2026-11-25：改为原生 v83 行为——复活不由 JUMP 键触发，而是 HP 归零时
		// 由 ChangeStatsHandler 弹 UIDeathNotice（FOCUSED，先于本函数接住按键）；
		// 玩家点确定/取消或等 10 分钟超时，回调里发 ChangeMapPacket(died=true, 当前mapid)。
		// 这里只兜底吞输入：若弹窗因故未激活，死亡态也不该有任何操作。
		if (player.get_char_state() == Char::State::DIED)
			return;

		switch (type)
		{
			case KeyType::Id::ACTION:
				playable->send_action(KeyAction::actionbyid(action), down);
				break;
			case KeyType::Id::ITEM:
				// 2026-11-22：道具绑到快捷键后按下去没反应 —— Stage::send_key
				// 旧 switch 只有 ACTION/SKILL/MACRO/FACE，ITEM 被静默吞掉，
				// 快捷栏单击（activate_*_qs_slot）虽调到此处也永不下发 USE_ITEM(72)。
				// 与 MinMS UIStateGame→Stage::send_key(ITEM)→Player::use_item 同源：
				// 只在按下沿用一次，避免 UIStatusBar 的 true/false 成对调用消耗两个。
				if (down)
					player.use_item(action);
				break;
			case KeyType::Id::SKILL:
				// MinMS Stage::send_key：技能只在按下沿触发；松开沿再 use_move
				// 会让瞬移连放两次、二段跳二次空包（仍发 USE_SKILL(91)）。
				if (down)
					combat.use_move(action);
				break;
			case KeyType::Id::MACRO:
				// Angel v43[7]=1 + CMacroSysMan::Update：整行宏绑到键位后，按键
				// 把该行三个技能入队、按硬直逐个释放（H1，2026-11-16 补齐；
				// 旧实现同帧三次 use_move 被 can_attack 吞掉后两个）。
				// H2：勾选「显示个性技能名」(slot.shout) 时，原生 IsShoutMacro
				// 路径先把宏名喊到普通聊天，再释放技能。
				if (down)
				{
					const SkillMacroSlot& slot = SkillMacros::get().get_slot(static_cast<size_t>(action));

					if (slot.shout != 0 && !slot.name.empty())
						GeneralChatPacket(slot.name, false).dispatch();

					combat.use_macro(slot.skill1, slot.skill2, slot.skill3);
				}
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

		{
			Cursor::State guide_st = Cursor::State::IDLE;
			if (field_markers.try_click_guide(clicked, cursor_position, camera.position(), guide_st))
				return guide_st;
		}

		Cursor::State npc_st = npcs.send_cursor(clicked, cursor_position, camera.position());

		if (npc_st != Cursor::State::IDLE)
			return npc_st;

		// 其他玩家：悬停高亮、点击请求角色信息窗（此前完全没接，点玩家无反应 /
		// 点击被聊天栏兜底吃掉）。
		return chars.send_cursor(clicked, cursor_position, camera.position());
	}

	bool Stage::is_player(int32_t cid) const
	{
		return cid == player.get_oid();
	}

	const Physics& Stage::get_physics() const
	{
		return physics;
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

	MapGuildBoss& Stage::get_guild_boss()
	{
		return guild_boss;
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