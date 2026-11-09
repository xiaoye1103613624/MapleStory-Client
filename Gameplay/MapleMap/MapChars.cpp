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
#include "MapChars.h"

#include "../../Net/Packets/PlayerInteractionPackets.h"

namespace ms
{
	Cursor::State MapChars::send_cursor(bool pressed, Point<int16_t> position, Point<int16_t> viewpos)
	{
		// 命中框：以角色脚底为锚，宽约 72、高约 110（站立角色大致体型）。
		// 与服务端 CHAR_INFO_REQUEST(97) 一致，点一下即开角色信息窗。
		for (const auto& kv : chars)
		{
			const MapObject* mo = kv.second.get();

			if (!mo || !mo->is_active())
				continue;

			Point<int16_t> absp = mo->get_position() + viewpos;
			Rectangle<int16_t> bounds(absp.x() - 36, absp.x() + 36, absp.y() - 110, absp.y());

			if (bounds.contains(position))
			{
				if (pressed)
				{
					CharInfoRequestPacket(kv.first).dispatch();
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

	void MapChars::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		chars.draw(layer, viewx, viewy, alpha);
	}

	void MapChars::update(const Physics& physics)
	{
		for (; !spawns.empty(); spawns.pop())
		{
			const CharSpawn& spawn = spawns.front();

			int32_t cid = spawn.get_cid();
			Optional<OtherChar> ochar = get_char(cid);

			if (ochar)
			{
				// TODO: Blank
			}
			else
			{
				chars.add(spawn.instantiate());
			}
		}

		chars.update(physics);
	}

	void MapChars::spawn(CharSpawn&& spawn)
	{
		spawns.emplace(std::move(spawn));
	}

	void MapChars::remove(int32_t cid)
	{
		chars.remove(cid);
	}

	void MapChars::clear()
	{
		chars.clear();
	}

	MapObjects * MapChars::get_chars()
	{
		return &chars;
	}

	const MapObjects * MapChars::get_chars() const
	{
		return &chars;
	}

	int32_t MapChars::find_id_by_name(const std::string& name) const
	{
		const MapObjects& objs = *get_chars();

		for (const auto& kv : objs)
		{
			if (const OtherChar* oc = dynamic_cast<const OtherChar*>(kv.second.get()))
				if (oc->get_name() == name)
					return kv.first;
		}

		return -1;
	}

	void MapChars::send_movement(int32_t cid, const std::vector<Movement>& movements)
	{
		if (Optional<OtherChar> otherchar = get_char(cid))
			otherchar->send_movement(movements);
	}

	void MapChars::update_look(int32_t cid, const LookEntry& look)
	{
		if (Optional<OtherChar> otherchar = get_char(cid))
			otherchar->update_look(look);
	}

	Optional<OtherChar> MapChars::get_char(int32_t cid)
	{
		return chars.get(cid);
	}
}