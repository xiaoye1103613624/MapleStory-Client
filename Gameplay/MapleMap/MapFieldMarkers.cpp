//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — MapFieldMarkers
//////////////////////////////////////////////////////////////////////////////////
#include "MapFieldMarkers.h"
#include "MapSummon.h"
#include "MapMist.h"
#include "MapDoor.h"
#include "MapDragon.h"

// 注意：不 include Stage.h（Stage.h 已 include 本头，成环）。
// tip 通道与 MessagingHandlers 同法（UIQuestAlarm 优先、UIChatBar 兜底）。
#include "../../IO/UI.h"
#include "../../IO/UITypes/UIMiscShells.h"
#include "../../IO/UITypes/UIChatBar.h"
#include "../../Net/Packets/PlayerPackets.h"
#include "../../Net/Packets/SummonDragonPackets.h"
#include "../../Net/Packets/NpcInteractionPackets.h"
#include "MapMobs.h"
#include "Mob.h"
#include "../../Gameplay/Movement.h"
#include "../../Constants.h"

#include <memory>
#include <vector>
#include <cmath>

namespace ms
{
	// 门提示去重：只在首次踩门打一次（防逐帧刷屏；Release 下 LOG 为 no-op，
	// 主要是 tip 不糊屏）。clear()（换图）时重置。
	namespace
	{
		bool door_tip_shown = false;

		void door_tip(const std::string& tip)
		{
			if (door_tip_shown)
				return;
			door_tip_shown = true;

			auto alarm = UI::get().get_element<UIQuestAlarm>();
			if (!alarm) alarm = UI::get().emplace<UIQuestAlarm>();
			else if (!alarm->is_active()) alarm->makeactive();
			if (alarm)
			{
				alarm->set_tip(tip);
				return;
			}
			if (auto chatbar = UI::get().get_element<UIChatBar>())
				chatbar->show_message(tip.c_str(), UIChatBar::MessageType::YELLOW);
		}
	}

	void MapFieldMarkers::draw(Layer::Id layer, double viewx, double viewy, float alpha) const
	{
		markers.draw(layer, viewx, viewy, alpha);
	}

	void MapFieldMarkers::update(const Physics& physics)
	{
		markers.update(physics);
	}

	void MapFieldMarkers::spawn(int32_t oid, FieldMarker::Kind kind, Point<int16_t> pos, const std::string& label, int32_t skill_id, int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			// Keep world position on refresh; only update label (merchant desc etc.)
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label(label);
			existing->makeactive();
			(void)pos;
			(void)kind;
			(void)skill_id;
			(void)lifetime_ms;
			return;
		}

		markers.add(std::make_unique<FieldMarker>(oid, kind, pos, label, skill_id, lifetime_ms));
	}

	void MapFieldMarkers::spawn_summon(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
		int8_t skill_level, int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
			{
				sum->set_label("S" + std::to_string(skill_id)
					+ (skill_level > 0 ? (" Lv" + std::to_string(skill_level)) : ""));
				existing->set_position(pos);
			}
			else if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
			{
				fm->set_label("S" + std::to_string(skill_id));
			}
			existing->makeactive();
			return;
		}
		markers.add(std::make_unique<MapSummon>(oid, owner_cid, skill_id, pos, skill_level, lifetime_ms));
	}

	void MapFieldMarkers::spawn_mist(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
		int32_t lifetime_ms)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label("Mist " + std::to_string(skill_id));
			existing->set_position(pos);
			existing->makeactive();
			(void)owner_cid;
			(void)lifetime_ms;
			return;
		}
		markers.add(std::make_unique<MapMist>(oid, owner_cid, skill_id, pos, lifetime_ms));
	}

	void MapFieldMarkers::spawn_door(int32_t owner, Point<int16_t> pos, bool launched)
	{
		if (auto existing = markers.get(owner))
		{
			// 同 oid 可能是文本场标（旧版）或 MapDoor：MapDoor 刷新表现态，
			// 其它种类重建为真门（服务端重复下发的语义就是整包刷新）。
			if (auto* door = dynamic_cast<MapDoor*>(existing.get()))
			{
				door->refresh(pos, launched);
				existing->makeactive();
				return;
			}
			markers.remove(owner);
		}

		markers.add(std::make_unique<MapDoor>(owner, pos, launched));
	}

	void MapFieldMarkers::spawn_dragon(int32_t oid, int32_t owner_cid, Point<int16_t> pos,
		int8_t stance, int16_t job)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* drag = dynamic_cast<MapDragon*>(existing.get()))
			{
				drag->set_position(pos);
				existing->makeactive();
				return;
			}
			markers.remove(oid);
		}

		markers.add(std::make_unique<MapDragon>(oid, owner_cid, pos, stance, job));
	}

	void MapFieldMarkers::dragon_move(int32_t oid, Point<int16_t> pos, uint8_t state_byte)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* drag = dynamic_cast<MapDragon*>(existing.get()))
			{
				drag->apply_move(pos, state_byte);
				return;
			}
		}

		// 未挂真对象（文本场标兜底形态或换图竞态）⇒ 至少把位置挪对。
		move(oid, pos);
	}

	void MapFieldMarkers::check_doors(Point<int16_t> playerpos)
	{
		// 上行 USE_DOOR(ownerid)（s9 DoorHandler 语义，PlayerPackets.h 注释在案）。
		// ownerid=0 在 s9 里是"取消自己的门"，绝不作为踩门上行；匹配不到门时
		// 服务端回 blockedMessage(6)+enableActions——归属/队伍裁决全在服务端。
		for (auto it = markers.begin(); it != markers.end(); ++it)
		{
			if (!it->second || !it->second->is_active())
				continue;

			if (auto* door = dynamic_cast<MapDoor*>(it->second.get()))
			{
				// in_range 先判命中，consume_trigger 再判冷却（1500ms）：
				// 换图成功前的逐帧重踩、被服务端 blocked 后的连续贴门，
				// 都收敛在冷却里，不逐帧刷上行。
				if (door->in_range(playerpos) && door->consume_trigger())
				{
					UseDoorPacket(door->get_owner()).dispatch();

					// 调试期观测：提示一次即可（上线级留 LOG_DEBUG）。
					door_tip("Mystic Door: used");
					LOG(LOG_DEBUG, "use_door owner=" << door->get_owner());
					return;
				}
			}
		}
	}

	void MapFieldMarkers::remove_by_kind(FieldMarker::Kind kind)
	{
		std::vector<int32_t> doomed;
		for (auto it = markers.begin(); it != markers.end(); ++it)
		{
			if (!it->second)
				continue;

			if (auto* fm = dynamic_cast<FieldMarker*>(it->second.get()))
			{
				if (fm->get_kind() == kind)
					doomed.push_back(it->first);
			}
		}

		for (int32_t oid : doomed)
			markers.remove(oid);
	}

	void MapFieldMarkers::set_summon_stance(int32_t oid, int8_t stance)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->set_stance(stance);
		}
	}

	void MapFieldMarkers::summon_die(int32_t oid)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->play_die();
			else
				markers.remove(oid);
		}
	}

	void MapFieldMarkers::summon_spawn(int32_t oid)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* sum = dynamic_cast<MapSummon*>(existing.get()))
				sum->play_spawn();
		}
	}

	void MapFieldMarkers::move(int32_t oid, Point<int16_t> pos)
	{
		if (auto existing = markers.get(oid))
			existing->set_position(pos);
	}

	void MapFieldMarkers::set_label(int32_t oid, const std::string& label)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->set_label(label);
		}
	}

	bool MapFieldMarkers::contains(int32_t oid) const
	{
		return markers.contains(oid);
	}

	bool MapFieldMarkers::try_click_guide(bool pressed, Point<int16_t> cursor, Point<int16_t> viewpos, Cursor::State& out)
	{
		// 与 SpawnGuideHandler 约定的场标 oid 一致。
		constexpr int32_t GUIDE_OID = 0x70000001;
		auto existing = markers.get(GUIDE_OID);
		if (!existing)
			return false;

		Point<int16_t> absp = existing->get_position() + viewpos;
		Rectangle<int16_t> hit(
			static_cast<int16_t>(absp.x() - 40),
			static_cast<int16_t>(absp.x() + 40),
			static_cast<int16_t>(absp.y() - 80),
			static_cast<int16_t>(absp.y() + 20));
		if (!hit.contains(cursor))
			return false;

		if (pressed)
			ClickGuidePacket().dispatch();
		out = pressed ? Cursor::State::IDLE : Cursor::State::CANCLICK;
		return true;
	}

	void MapFieldMarkers::apply_damage(int32_t oid, int32_t damage)
	{
		if (auto existing = markers.get(oid))
		{
			if (auto* fm = dynamic_cast<FieldMarker*>(existing.get()))
				fm->apply_damage(damage);
		}
	}

	void MapFieldMarkers::remove(int32_t oid)
	{
		markers.remove(oid);
	}

	void MapFieldMarkers::clear()
	{
		markers.clear();
		door_tip_shown = false;
		last_summon_n = 0;
		last_dragon_pos = {};
		attack_cd_ms = 0;
	}

	void MapFieldMarkers::report_owned_c2s(int32_t local_cid, Point<int16_t> playerpos, MapMobs& mobs)
	{
		if (local_cid <= 0)
			return;

		if (attack_cd_ms > 0)
			attack_cd_ms -= Constants::TIMESTEP;

		for (auto& kv : markers)
		{
			MapObject* obj = kv.second.get();
			if (!obj || !obj->is_active())
				continue;

			if (auto* sum = dynamic_cast<MapSummon*>(obj))
			{
				if (sum->get_owner() != local_cid)
					continue;

				Point<int16_t> pos = sum->get_position();
				// 跟随：远离玩家时拉近并上报 MOVE_SUMMON
				int dx = playerpos.x() - pos.x();
				int dy = playerpos.y() - pos.y();
				if (std::abs(dx) > 120 || std::abs(dy) > 80)
				{
					int16_t nx = static_cast<int16_t>(pos.x() + dx / 4);
					int16_t ny = static_cast<int16_t>(pos.y() + dy / 4);
					sum->set_position(nx, ny);
					pos = sum->get_position();
				}

				bool moved = true;
				for (uint8_t i = 0; i < last_summon_n; ++i)
				{
					if (last_summon_oid[i] == sum->get_oid())
					{
						moved = last_summon_pos[i] != pos;
						last_summon_pos[i] = pos;
						break;
					}
				}
				if (moved)
				{
					bool found = false;
					for (uint8_t i = 0; i < last_summon_n; ++i)
						if (last_summon_oid[i] == sum->get_oid()) { found = true; break; }
					if (!found && last_summon_n < 8)
					{
						last_summon_oid[last_summon_n] = sum->get_oid();
						last_summon_pos[last_summon_n] = pos;
						++last_summon_n;
					}
					Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);
					MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();
				}

				// 近战召唤攻击（CD 1s）：找最近怪
				if (attack_cd_ms <= 0)
				{
					int32_t best = 0;
					int best_d2 = 200 * 200;
					if (auto* list = mobs.get_mobs())
					{
						for (auto& mk : *list)
						{
							auto* mob = dynamic_cast<Mob*>(mk.second.get());
							if (!mob || !mob->is_active())
								continue;
							Point<int16_t> mp = mob->get_position();
							int ddx = mp.x() - pos.x();
							int ddy = mp.y() - pos.y();
							int d2 = ddx * ddx + ddy * ddy;
							if (d2 < best_d2) { best_d2 = d2; best = mob->get_oid(); }
						}
					}
					if (best > 0)
					{
						std::vector<SummonAttackHit> hits{ { best, 50 } };
						SummonAttackPacket(sum->get_oid(), 0, hits).dispatch();
						attack_cd_ms = 1000;
					}
				}

				// 暗骑凝视者技能位（Aura/Hex）——有召唤在场时周期性空报会打扰；仅包类就绪，
				// SoftKey/技能面另接。这里不自动发 BEHOLDER。
			}
			else if (auto* drag = dynamic_cast<MapDragon*>(obj))
			{
				if (drag->get_owner() != local_cid)
					continue;
				Point<int16_t> pos = drag->get_position();
				int dx = playerpos.x() - pos.x();
				int dy = playerpos.y() - pos.y();
				if (std::abs(dx) > 80 || std::abs(dy) > 60)
				{
					int16_t nx = static_cast<int16_t>(pos.x() + dx / 3);
					int16_t ny = static_cast<int16_t>(pos.y() + dy / 3);
					drag->set_position(nx, ny);
					pos = drag->get_position();
				}
				if (last_dragon_pos != pos)
				{
					last_dragon_pos = pos;
					Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);
					MoveDragonPacket(pos, mv).dispatch();
				}
			}
		}
	}
}
