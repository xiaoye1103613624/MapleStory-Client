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
#include "MapNpcs.h"

#include "Npc.h"

#include "../../Net/Packets/NpcInteractionPackets.h"

namespace ms
{
	void MapNpcs::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		npcs.draw(layer, viewx, viewy, alpha);
	}

	void MapNpcs::update(const Physics& physics)
	{
		for (; !spawns.empty(); spawns.pop())
		{
			const NpcSpawn& spawn = spawns.front();

			int32_t oid = spawn.get_oid();

			// 同 oid 重新下发（脚本挪位、控制器 mode 0 传送）时必须吃新包的 position/fh：
			// 原来只 makeactive() 复用旧对象，新坐标被整包丢掉，NPC 就卡在老地方不动。
			if (npcs.contains(oid))
				npcs.remove(oid);

			npcs.add(spawn.instantiate(physics));
		}

		npcs.update(physics);
	}

	void MapNpcs::spawn(NpcSpawn&& spawn)
	{
		spawns.emplace(std::move(spawn));
	}

	void MapNpcs::remove(int32_t oid)
	{
		if (auto npc = npcs.get(oid))
			npc->deactivate();
	}

	void MapNpcs::clear()
	{
		npcs.clear();
	}

	MapObjects * MapNpcs::get_npcs()
	{
		return &npcs;
	}

	// SEND_TV 文字层广播：遍历场域 NPC，把 5 行消息灌进所有 MapleTV 模板
	// （原生 broadcastTV 是全世界广播，但 TV 大屏只存在于自由市场等特定图，
	// 本端按「当前图上的 TV NPC」落文字；removeTV 传空 vector 收回台标）。
	void MapNpcs::broadcast_tv_message(const std::vector<std::string>& lines)
	{
		for (auto& map_object : npcs)
		{
			Npc* npc = static_cast<Npc*>(map_object.second.get());

			if (npc && npc->is_maple_tv())
				npc->set_tv_message(lines);
		}
	}

	Cursor::State MapNpcs::send_cursor(bool pressed, Point<int16_t> position, Point<int16_t> viewpos)
	{
		for (auto& map_object : npcs)
		{
			Npc* npc = static_cast<Npc*>(map_object.second.get());

			if (npc && npc->is_active() && npc->inrange(position, viewpos))
			{
				if (pressed)
				{
					// TODO: Try finding dialog first
					// TalkToNPCPacket 构造内已顺带派发 NPC_ACTION 对话姿态。
					TalkToNPCPacket(npc->get_oid()).dispatch();

					return Cursor::State::IDLE;
				}
				else
				{
					return Cursor::State::CANCLICK;
				}
			}
		}

		return Cursor::State::IDLE;
	}
}