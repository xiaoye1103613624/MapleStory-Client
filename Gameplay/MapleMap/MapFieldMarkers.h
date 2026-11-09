//////////////////////////////////////////////////////////////////////////////////
//	BeiDou V17 parity — collection for field placeholders
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FieldMarker.h"
#include "MapObjects.h"

#include "../../IO/Cursor.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ms
{
	class MapFieldMarkers
	{
	public:
		void draw(Layer::Id layer, double viewx, double viewy, float alpha) const;
		void update(const Physics& physics);

		void spawn(int32_t oid, FieldMarker::Kind kind, Point<int16_t> pos, const std::string& label, int32_t skill_id = 0, int32_t lifetime_ms = 0);
		void spawn_summon(int32_t oid, int32_t owner_cid, int32_t skill_id, Point<int16_t> pos,
			int8_t skill_level = 0, int32_t lifetime_ms = 0);
		// SPAWN_MIST：矩形 box + mist_type/level 驱动 Skill tile 平铺（见 MapMist）。
		void spawn_mist(int32_t oid, int32_t owner_cid, int32_t skill_id, int32_t mist_type,
			int8_t skill_level, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
			int32_t lifetime_ms = 0);
		// G6 mystic door（真对象化）：owner 即门 id（s9 spawnDoor/RemoveDoor 同键）。
		// 重复下发只刷新 launched/位置（放门→收回→再放门场景）。
		void spawn_door(int32_t owner, Point<int16_t> pos, bool launched);
		// G5 Evan dragon（真对象化）：oid 沿用原生键 cid^0x40000000。
		void spawn_dragon(int32_t oid, int32_t owner_cid, Point<int16_t> pos, int8_t stance, int16_t job);
		void dragon_move(int32_t oid, Point<int16_t> pos, uint8_t state_byte);
		// G6 门交互（Stage::check_doors）：玩家踩到任一 MapDoor 门框 ⇒ 发
		// USE_DOOR(owner)。归属/队伍裁决在服务端（DoorHandler.java），客户端只管
		// 上行——与 check_seats/check_portals 同构。内部带换图冷却防逐帧刷屏。
		void check_doors(Point<int16_t> playerpos);
		// G7：按 kind 清除全部场标（SPAWN_PORTAL town/target=NONE 清动态门）。
		void remove_by_kind(FieldMarker::Kind kind);
		void set_summon_stance(int32_t oid, int8_t stance);
		// 真对象化（波3）：召唤兽入场/死亡动画需要拿到 MapSummon 实例，
		// 只经 FieldMarker 基类接口够不到 play_die/play_spawn。
		void summon_die(int32_t oid);
		void summon_spawn(int32_t oid);
		void move(int32_t oid, Point<int16_t> pos);
		void set_label(int32_t oid, const std::string& label);
		bool contains(int32_t oid) const;
		void apply_damage(int32_t oid, int32_t damage);
		void remove(int32_t oid);
		void clear();

		// SPAWN_GUIDE 场标点击：命中写 out 并返回 true（pressed 时已发 CLICK_GUIDE），
		// 未命中返回 false。由 Stage::send_cursor 优先于 NPC 命中调用。
		// （本仓 Optional<T> 只包指针，不能 Optional<Cursor::State>。）
		bool try_click_guide(bool pressed, Point<int16_t> cursor, Point<int16_t> viewpos, Cursor::State& out);

		// W2：本角色召唤兽/龙上行（移动/攻击/龙移）
		void report_owned_c2s(int32_t local_cid, Point<int16_t> playerpos, class MapMobs& mobs);

	private:
		MapObjects markers;
		// oid -> last MOVE_SUMMON position. Keyed (no 8-slot ceiling);
		// report_owned_c2s prunes gone / inactive / non-MapSummon / foreign entries each frame.
		std::unordered_map<int32_t, Point<int16_t>> last_summon_pos;
		Point<int16_t> last_dragon_pos{};
		int32_t attack_cd_ms = 0;
	};
}
