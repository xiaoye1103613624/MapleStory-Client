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
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "../../Util/Misc.h"

#include <codecvt>
#include <vector>

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
	}

	Npc::Npc(int32_t id, int32_t o, bool fl, uint16_t f, bool cnt, Point<int16_t> position) : MapObject(o)
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
		path_accum_ms = 0;
		path_start_x = position.x();
		path_start_y = position.y();

		phobj.fhid = f;
		set_position(position);

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

	void Npc::try_flush_movepath()
	{
		// 最小可发送：一节 absolute attr=0（Encode case 0），尾空 keyPad+rect=9。
		// 字段序仅来自 IDA CMovePath::Encode @ 0x666e20，禁止混入 MovementPacket。
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

		// Encode 头是 Flush 时刻的安全坐标/速度（IDA Encode 读 m_x/m_y/m_vx/m_vy），
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
