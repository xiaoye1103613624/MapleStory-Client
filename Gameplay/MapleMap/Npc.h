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
#pragma once

#include "MapObject.h"

#include "../../Graphics/Animation.h"
#include "../../Graphics/Text.h"
#include "../../Util/Randomizer.h"

namespace ms
{
	// Represents a NPC on the current map
	// Implements the 'MapObject' interface to be used in a 'MapObjects' template
	class Npc : public MapObject
	{
	public:
		// Constructs an NPC by combining data from game files with data sent by the server
		// rx_min/rx_max：服务端下发的活动左右边界（已归一化），0/相等表示不限范围。
		Npc(int32_t npcid, int32_t oid, bool mirrored, uint16_t fhid, bool control, Point<int16_t> position,
			int16_t rx_min = 0, int16_t rx_max = 0);

		// Draws the current animation and name/function tags
		void draw(double viewx, double viewy, float alpha) const override;
		// Updates the current animation and physics
		int8_t update(const Physics& physics) override;

		// Changes stance and resets animation
		void set_stance(const std::string& stance);

		// SPAWN_NPC_C 授控：本端负责攒/发 NPC_ACTION 移动 path
		void set_control(bool value);
		bool has_control() const;

		// Check whether this is a server-sided NPC
		bool isscripted() const;
		// Check if the NPC is in range of the cursor
		bool inrange(Point<int16_t> cursorpos, Point<int16_t> viewpos) const;

		// Returns the NPC name
		std::string get_name();
		// Returns the NPC's function description or title
		std::string get_func();
		// Template NPC id (not map oid).
		int32_t get_npcid() const { return npcid; }

		// MapleTV 大屏消息（原生 CNpc::SetMapleTVMessage）：SEND_TV 广播的 5 行
		// 文字落在上屏（TVchat 帧盖住默认台标 TVbasic）；空 vector = 收回台标。
		void set_tv_message(const std::vector<std::string>& lines);
		bool is_maple_tv() const { return maple_tv; }

	private:
		// 控制器：≥1000ms 攒一节 absolute(attr0) 后 Flush→NPC_ACTION（对齐 IsTimeForFlush）
		void try_flush_movepath();
		void next_control_move();
		// Angel GetDCRange@0x6d3fde 等价：把 x 夹回 [rx_min,rx_max] 活动范围；
		// 越界时钉回边界、翻向、切回 stand，并置 just_bounced 请求立即上行。
		void clamp_to_move_range();

		std::map<std::string, Animation> animations;
		std::map<std::string, std::vector<std::string>> lines;
		std::vector<std::string> states;
		std::string name;
		std::string func;
		bool hidename;
		bool scripted;
		bool mouseonly;

		// NPC 头顶任务标记（MinMS QuestNpcMarkers 的可靠子集）：
		// 0=无 1=可接 2=进行中；"可交"需 Check.img 进度解析，暂不判定。
		mutable int8_t quest_mark_kind = 0;
		mutable int32_t quest_mark_tick = 0;

		int32_t npcid;
		bool flip;
		std::string stance;
		bool control;
		bool movable;           // wz 有 move 动画 / info.move
		int16_t rx_min;         // 活动左界（归一化；等于 rx_max 表示不限）
		int16_t rx_max;         // 活动右界
		bool just_bounced = false; // 本帧触界翻向，请求立即上行一次
		int16_t path_accum_ms;  // 距上次上行的累计 ms
		int16_t path_start_x;
		int16_t path_start_y;

		Randomizer random;
		Text namelabel;
		Text funclabel;

		// MapleTV 大屏：原生 CNpc::SetMapleTVMessage 对 info/MapleTV=1 的 NPC 模板
		// （自由市场 9250025 等）在立绘之上叠两块屏幕：上屏 240x90 台标、下屏
		// 240x180 轮播广告。WZ 的 MapleTVmsgX/Y 与 MapleTVadX/Y 是两块屏幕相对
		// NPC 锚点 absp 的左上角（short，部分 .nx 会读成 0x0000FFxx，按 int16 解释）。
		// 证据：Data/.nxcache/Npc.nx 9250025.img/info、UI.nx MapleTV.img/*、
		// 原生解密串 UI/MapleTV.img/TVbasic/0 与 UI/MapleTV.img/TVmedia/%d。
		bool maple_tv = false;
		Point<int16_t> tv_msg_pos;
		Point<int16_t> tv_ad_pos;
		Texture tv_basic;
		Animation tv_media;
		// SEND_TV 文字层：非空时上屏画 TVchat（240x90，origin 0,89）+ 逐行文字，
		// 盖掉默认台标；收到 removeTV 清空回 TVbasic。
		std::vector<std::string> tv_lines;
		Texture tv_chat;
	};
}