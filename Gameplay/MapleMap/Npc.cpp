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
#include "Npc.h"

#include "../../Constants.h"
#include "../../Data/QuestData.h"
#include "../../Character/QuestLog.h"
#include "../../Gameplay/Stage.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/Misc.h"

#include <codecvt>
#include <vector>
#include <unordered_set>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	namespace
	{
		// IDA CMovePath::IsTimeForFlush：walk 默认 ≥1000ms 才 Flush
		constexpr int16_t kNpcPathFlushMs = 1000;
		// 控制器本地水平力（比 Mob 弱；仅本端授控可动 NPC）
		constexpr double kNpcWalkForce = 0.08;

		// NPC 头顶任务标记：可接任务集全局缓存（1 秒刷新一次），
		// 避免每个 NPC 每 500ms 都全扫 Check.img（数百 NPC × 上千任务）。
		struct QuestMarkCache
		{
			std::vector<int16_t> available;
			std::unordered_set<int32_t> available_npcs;
			uint32_t tick = 0;
		};
		QuestMarkCache quest_mark_cache;
		constexpr uint32_t kQuestMarkRefreshTicks = 60; // ~1s @ 60fps
	}

	Npc::Npc(int32_t id, int32_t o, bool fl, uint16_t f, bool cnt, Point<int16_t> position,
		int16_t rmin, int16_t rmax) : MapObject(o)
	{
		// 与 Mob 对齐用 extend_id：原来的 7 - strid.size() 遇到 8 位 id 会无符号下溢，
		// insert(0, 巨值, '0') 直接抛 length_error，把整个 spawn 打崩。
		std::string strid = string_format::extend_id(id, 7) + ".img";

		nl::node src = nl::nx::Npc[strid];
		nl::node strsrc = nl::nx::String["Npc.img"][std::to_string(id)];

		std::string link = src["info"]["link"];

		if (link.size() > 0)
		{
			link.append(".img");
			src = nl::nx::Npc[link];
		}

		nl::node info = src["info"];

		// MapleTV 大屏（原生 bMapleTV 模板，如自由市场 9250025 / 岛内枫叶电视）：
		// 立绘里的两块深色屏幕本身不烘焙内容，画面由 CNpc::SetMapleTVMessage
		// 用 UI/MapleTV.img 贴图叠出来。本客户端此前完全没读这棵 img，正是
		// 「自由市场 MapleTV 没有画面显示」的根因。
		// 偏移是 WZ short：msgX/msgY = 上屏(240x90)左上角，adX/adY = 下屏
		// (240x180)左上角，均相对 NPC 锚点；少数 .nx 把负 short 读成 0xFFxx
		// 正值（9250025 的 msgX=65304 实为 -232），统一按 int16_t 解释。
		maple_tv = info["MapleTV"].get_bool();

		if (maple_tv)
		{
			tv_msg_pos = Point<int16_t>(
				static_cast<int16_t>(info["MapleTVmsgX"].get_integer()),
				static_cast<int16_t>(info["MapleTVmsgY"].get_integer())
			);
			tv_ad_pos = Point<int16_t>(
				static_cast<int16_t>(info["MapleTVadX"].get_integer()),
				static_cast<int16_t>(info["MapleTVadY"].get_integer())
			);

			nl::node tvroot = nl::nx::UI["MapleTV.img"];
			nl::node tv_media_src = tvroot["TVmedia"]["0"];
			tv_basic = Texture(tvroot["TVbasic"]["0"]);       // 上屏默认台标
			tv_chat = Texture(tvroot["TVchat"]["0"]);         // 上屏文字底板（SEND_TV 时盖台标）

			// R6：节点缺失时静默降级，只保留 NPC 立绘。先判节点再构造 Animation，
			// 否则空 Animation 的 get_origin()/get_dimensions() 会越界取 frames[0]。
			if (tv_media_src && tv_media_src.size() > 0)
				tv_media = Animation(tv_media_src);            // 下屏轮播广告

			maple_tv = tv_basic.is_valid() && tv_media_src && tv_media_src.size() > 0;
		}

		hidename = info["hideName"].get_bool();
		mouseonly = info["talkMouseOnly"].get_bool();
		scripted = info["script"].size() > 0 || info["shop"].get_bool();
		// 可动 NPC：info.move 或存在 move 动画（对齐原生 m_pTemplate->bMove）
		movable = info["move"].get_bool();

		for (auto npcnode : src)
		{
			std::string state = npcnode.name();

			if (state != "info")
			{
				animations[state] = npcnode;
				states.push_back(state);
				if (state == "move")
					movable = true;
			}

			for (auto speaknode : npcnode["speak"])
				lines[state].push_back(strsrc[speaknode.get_string()]);
		}

		name = strsrc["name"].get_string();
		func = strsrc["func"].get_string();

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(func);

		for (size_t i = 0; i < wide.size(); i++)
		{
			wchar_t c = wide[i];

			// Korean
			if (c >= 0x1100 && c <= 0x11FF)
			{
				func = "";
				break;
			}
		}

		namelabel = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::YELLOW, Text::Background::NAMETAG, name);
		funclabel = Text(Text::Font::A13B, Text::Alignment::CENTER, Color::Name::YELLOW, Text::Background::NAMETAG, func);

		npcid = id;
		flip = !fl;
		control = cnt;
		stance = "stand";
		rx_min = rmin;
		rx_max = rmax;
		path_accum_ms = 0;
		path_start_x = position.x();
		path_start_y = position.y();

		phobj.fhid = f;
		set_position(position);

		// 对齐 Angel CNpc::GetDCRange@0x6d3fde + CVecCtrlNpc 的活动范围约束：
		// movable NPC 无条件置 TURNATEDGES，使平台边缘/台阶处由 FootholdTree::limit_movement
		// 转向（Mob.cpp:175 同款）。没有范围（rx_min==rx_max）时靠这条兜住"走下台阶"。
		if (movable)
			phobj.set_flag(PhysicsObject::Flag::TURNATEDGES);

		if (control && movable)
			LOG(LOG_DEBUG, "Npc control oid=" << oid << " id=" << npcid << " movable=1");
	}

	void Npc::draw(double viewx, double viewy, float alpha) const
	{
		Point<int16_t> absp = phobj.get_absolute(viewx, viewy, alpha);

		if (animations.count(stance))
			animations.at(stance).draw(DrawArgument(absp, flip), alpha);

		// 原生 TV 模板都是不翻转的场域静态对象（自由市场 9250025 的 life.f=0，
		// 服务端 spawnNPC 写 getF()!=1 → 本端 flip=false）。若将来遇到 f=1 的
		// 镜像模板，偏移需按镜像重算，这里宁可只留立绘也不画错位置。
		if (maple_tv && !flip)
		{
			// 两块屏幕的位图都是 bottom-left origin（TVbasic 0,89 / TVmedia 0,180），
			// 而 WZ 偏移给的是屏幕左上角，所以绘制锚点要补上当前帧 origin。
			// 后画：压住立绘里那两块烘焙的深色屏底，不被木框/齿轮盖住。
			// SEND_TV 文字层在场时，上屏用 TVchat（同 240x90、同 origin）替换
			// 默认台标，再逐行画 5 行消息（原生 SetMapleTVMessage 语义）。
			const bool tv_text_on = !tv_lines.empty() && tv_chat.is_valid();
			const Texture& upper = tv_text_on ? tv_chat : tv_basic;

			upper.draw(DrawArgument(absp + tv_msg_pos + upper.get_origin()));
			tv_media.draw(DrawArgument(absp + tv_ad_pos + tv_media.get_origin()), alpha);

			if (tv_text_on)
			{
				// 240x90 屏内 5 行：行距 14、首行墨迹约 y8，A11M 墨迹顶 = draw_y+10
				// ⇒ 首行 draw_y = -2，逐行 +14（原生 TV 文字观感校准值）。
				int16_t ty = static_cast<int16_t>(-2);

				for (const std::string& line : tv_lines)
				{
					if (line.empty())
						continue;

					Text tl(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, line, 230);
					tl.draw(absp + tv_msg_pos + Point<int16_t>(120, ty));
					ty += 14;
				}
			}
		}

		if (!hidename)
		{
			// If ever changing code for namelabel confirm placements with map 10000
			namelabel.draw(absp + Point<int16_t>(0, -4));
			funclabel.draw(absp + Point<int16_t>(0, 18));
		}

		// NPC 头顶任务标记（MinMS QuestNpcMarkers 语义子集）：
		// 可接任务集 1 秒全局刷新一次；本 NPC 状态每 500ms 重算。
		if (++quest_mark_cache.tick >= kQuestMarkRefreshTicks)
		{
			quest_mark_cache.tick = 0;
			Player& pl = Stage::get().get_player();
			quest_mark_cache.available = QuestData::list_available(
				static_cast<int16_t>(pl.get_level()),
				pl.get_stats().get_job().get_id(),
				pl.get_quests());
			quest_mark_cache.available_npcs.clear();
			for (int16_t qid : quest_mark_cache.available)
			{
				const QuestData& qd = QuestData::get(qid);
				if (qd)
					quest_mark_cache.available_npcs.insert(static_cast<int32_t>(qd.get_npc()));
			}
		}
		if (++quest_mark_tick >= 30)
		{
			quest_mark_tick = 0;
			const QuestLog& log = Stage::get().get_player().get_quests();
			bool inprogress = false;
			// 1) 进行中：已接且本 NPC 是任务发布者。
			for (const auto& q : log.get_started())
			{
				const QuestData& qd = QuestData::get(static_cast<int32_t>(q.first));
				if (qd && qd.get_npc() == npcid)
				{
					inprogress = true;
					break;
				}
			}
			// 2) 可接：本 NPC 在全局可接发布者集合中。
			bool available = quest_mark_cache.available_npcs.count(npcid) > 0;
			quest_mark_kind = inprogress ? 2 : (available ? 1 : 0);
		}
		if (quest_mark_kind > 0)
		{
			// QuestIcon/<kind>/0 是标记动画（D3 orange-wz 实测节点在位）。
			nl::node qnode = nl::nx::UI["UIWindow.img"]["QuestIcon"][std::to_string(quest_mark_kind)]["0"];
			if (qnode)
			{
				Animation mark(qnode);
				mark.draw(DrawArgument(absp + Point<int16_t>(0, -70)), alpha);
			}
		}
	}

	int8_t Npc::update(const Physics& physics)
	{
		if (!active)
			return phobj.fhlayer;

		// MapleTV 下屏广告按 WZ 帧 delay（150/250ms）轮播；循环动画的 update()
		// 不返回 true，这里不需要据返回值切下一组。
		if (maple_tv)
			tv_media.update();

		if (control && movable)
		{
			if (stance == "move")
				phobj.hforce = flip ? -kNpcWalkForce : kNpcWalkForce;
			else
				phobj.hforce = 0.0;

			physics.move_object(phobj);

			// Angel GetDCRange@0x6d3fde 等价：活动范围夹取。越界则钉回边界 + 翻向 + 回 stand，
			// 立即上行一次新坐标，避免服务端仍按旧方向插值。
			clamp_to_move_range();
			if (just_bounced)
			{
				if (phobj.fhid > 0)
					try_flush_movepath();   // 立即上行新坐标，避免服务端按旧方向插值
				just_bounced = false;
			}

			path_accum_ms = static_cast<int16_t>(path_accum_ms + Constants::TIMESTEP);
			if (path_accum_ms >= kNpcPathFlushMs && phobj.fhid > 0)
				try_flush_movepath();

			bool aniend = false;
			if (animations.count(stance))
				aniend = animations.at(stance).update();

			if (aniend)
				next_control_move();
		}
		else
		{
			physics.move_object(phobj);
			// 非授控 NPC 同样受活动范围约束（包坐标越界/物理漂移兜底）。
			clamp_to_move_range();

			if (animations.count(stance))
			{
				bool aniend = animations.at(stance).update();

				if (aniend && states.size() > 0)
				{
					size_t next_stance = random.next_int(states.size());
					std::string new_stance = states[next_stance];
					set_stance(new_stance);
				}
			}
		}

		return phobj.fhlayer;
	}

	void Npc::clamp_to_move_range()
	{
		// 范围无效（包未下发 rx0/rx1，两值相等）时不夹取，仅靠 TURNATEDGES 在台阶边缘转向。
		if (rx_min == rx_max)
			return;

		const double x = phobj.crnt_x();
		if (x > rx_max)
		{
			phobj.limitx(rx_max);   // 钉回右界并清 hspeed
			phobj.hforce = 0.0;
			flip = true;            // 触右界翻向左
			set_stance("stand");
			just_bounced = true;
		}
		else if (x < rx_min)
		{
			phobj.limitx(rx_min);   // 钉回左界并清 hspeed
			phobj.hforce = 0.0;
			flip = false;           // 触左界翻向右
			set_stance("stand");
			just_bounced = true;
		}
	}

	void Npc::try_flush_movepath()
	{
		// 最小可发送：一节 absolute attr=0（Encode case 0），尾空 keyPad+rect=9。
		// 字段序按 s9 NPC_ACTION 契约，禁止混入 MovementPacket；083 Encode @0x68a563 仅作对照。
		const int16_t x = phobj.get_x();
		const int16_t y = phobj.get_y();
		const int16_t vx = static_cast<int16_t>(phobj.hspeed);
		const int16_t vy = static_cast<int16_t>(phobj.vspeed);
		const int16_t fh = static_cast<int16_t>(phobj.fhid);

		MovePathElem elem;
		elem.nAttr = 0;
		elem.x = x;
		elem.y = y;
		elem.vx = vx;
		elem.vy = vy;
		elem.fh = fh;
		elem.xOffset = 0;
		elem.yOffset = 0;
		// bMoveAction：原生 MoveAction 字节；本端用左右 + move/stand 粗编码
		elem.bMoveAction = static_cast<uint8_t>((flip ? 1 : 0) | ((stance == "move") ? 2 : 0));
		elem.tElapse = path_accum_ms;

		std::vector<MovePathElem> elems;
		elems.push_back(elem);

		// 本端移动段头按 s9 契约带安全坐标/速度 x/y/vx/vy（083 Encode @0x68a563 头仅 x/y，仅作对照），
		// 不是段起点。nAction/nChatIdx：纯移动常用 -1 / 0（与对话姿态对称）。
		NpcActionMovePacket(oid, -1, 0, x, y, vx, vy, elems).dispatch();
		LOG(LOG_DEBUG, "NpcActionMove oid=" << oid
			<< " x=" << x << " y=" << y << " fh=" << fh
			<< " elapse=" << path_accum_ms);

		path_start_x = x;
		path_start_y = y;
		path_accum_ms = 0;
	}

	void Npc::next_control_move()
	{
		if (!movable)
			return;

		if (stance == "stand" || stance == "alert")
		{
			if (animations.count("move"))
			{
				flip = random.next_bool();
				set_stance("move");
			}
			else
			{
				set_stance("stand");
			}
		}
		else if (stance == "move")
		{
			switch (random.next_int(3))
			{
			case 0:
				set_stance("stand");
				break;
			case 1:
				flip = false;
				set_stance("move");
				break;
			default:
				flip = true;
				set_stance("move");
				break;
			}
		}
		else
		{
			set_stance(animations.count("stand") ? "stand" : stance);
		}
	}

	void Npc::set_stance(const std::string& st)
	{
		if (stance != st)
		{
			stance = st;

			auto iter = animations.find(stance);

			if (iter == animations.end())
				return;

			iter->second.reset();
		}
	}

	void Npc::set_control(bool value)
	{
		control = value;
		path_accum_ms = 0;
		path_start_x = phobj.get_x();
		path_start_y = phobj.get_y();
		if (control && movable)
			LOG(LOG_DEBUG, "Npc set_control oid=" << oid << " on");
	}

	bool Npc::has_control() const
	{
		return control;
	}

	bool Npc::isscripted() const
	{
		return scripted;
	}

	bool Npc::inrange(Point<int16_t> cursorpos, Point<int16_t> viewpos) const
	{
		if (!active)
			return false;

		Point<int16_t> absp = get_position() + viewpos;

		Point<int16_t> dim =
			animations.count(stance) ?
			animations.at(stance).get_dimensions() :
			Point<int16_t>();

		return Rectangle<int16_t>(
			absp.x() - dim.x() / 2,
			absp.x() + dim.x() / 2,
			absp.y() - dim.y(),
			absp.y()
			).contains(cursorpos);
	}

	std::string Npc::get_name()
	{
		return name;
	}

	std::string Npc::get_func()
	{
		return func;
	}

	// 原生 CNpc::SetMapleTVMessage：SEND_TV 的 5 行文字灌进场域 TV NPC 的上屏；
	// 空 vector（removeTV）收回默认台标。非 MapleTV 模板的 NPC 忽略。
	void Npc::set_tv_message(const std::vector<std::string>& lines)
	{
		if (!maple_tv)
			return;

		tv_lines = lines;
	}
}
