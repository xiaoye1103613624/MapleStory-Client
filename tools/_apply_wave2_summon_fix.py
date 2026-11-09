#!/usr/bin/env python3
"""One-shot precise patch: MapFieldMarkers MOVE_SUMMON shadow cache.

Root cause: last_summon_oid[8] / last_summon_pos[8] / last_summon_n only ever
grow (and reset on clear()).  A summon can disappear via die animation
(MapObjects::update implicit erase), lifetime expiry, or RemoveSummonHandler;
the stale oid still occupies a slot.  After 8 stale slots a fresh summon is
never registered => moved stays true => MoveSummonPacket every TIMESTEP (~60/s)
until map change.  Fix: keyed cache (no 8-slot ceiling) + per-frame self-healing
prune of missing / inactive / not-a-MapSummon / not-owned entries.

Avoids edit-tool tab/CRLF old_string misses (round34/35/37/38 lesson).
"""
import io
import re

ROOT = r"E:/project/MapleStory-Client/"
H = ROOT + r"Gameplay/MapleMap/MapFieldMarkers.h"
C = ROOT + r"Gameplay/MapleMap/MapFieldMarkers.cpp"
T = "\t"


def load(p):
    raw = io.open(p, "r", encoding="utf-8", newline="").read()
    return raw, ("\r\n" in raw), raw.replace("\r\n", "\n")


def save(p, s, crlf):
    io.open(p, "w", encoding="utf-8", newline="").write(
        s.replace("\n", "\r\n") if crlf else s)


# ---------------------------------------------------------------- header ----
hraw, hcrlf, h = load(H)
old_inc = "#include <cstdint>\n#include <string>\n"
new_inc = "#include <cstdint>\n#include <string>\n#include <unordered_map>\n"
assert h.count(old_inc) == 1, "HEADER_INCLUDE_FOUND %d" % h.count(old_inc)
h = h.replace(old_inc, new_inc)

old_priv = (
    T + "private:\n"
    + T * 2 + "MapObjects markers;\n"
    + T * 2 + "Point<int16_t> last_summon_pos[8]{};\n"
    + T * 2 + "int32_t last_summon_oid[8]{};\n"
    + T * 2 + "uint8_t last_summon_n = 0;\n"
    + T * 2 + "Point<int16_t> last_dragon_pos{};\n"
    + T * 2 + "int32_t attack_cd_ms = 0;\n"
)
new_priv = (
    T + "private:\n"
    + T * 2 + "MapObjects markers;\n"
    + T * 2 + "// oid -> last MOVE_SUMMON position. Keyed (no 8-slot ceiling);\n"
    + T * 2 + "// report_owned_c2s prunes gone / inactive / non-MapSummon / foreign entries each frame.\n"
    + T * 2 + "std::unordered_map<int32_t, Point<int16_t>> last_summon_pos;\n"
    + T * 2 + "Point<int16_t> last_dragon_pos{};\n"
    + T * 2 + "int32_t attack_cd_ms = 0;\n"
)
assert h.count(old_priv) == 1, "HEADER_PRIVATE_FOUND %d" % h.count(old_priv)
h = h.replace(old_priv, new_priv)
assert "last_summon_n" not in h and "last_summon_oid" not in h, "HEADER_STALE"
save(H, h, hcrlf)
print("HEADER_OK unordered_map=%d" % h.count("std::unordered_map"))

# ------------------------------------------------------------------ cpp -----
craw, ccrlf, c = load(C)

old_clear = (
    T * 2 + "markers.clear();\n"
    + T * 2 + "door_tip_shown = false;\n"
    + T * 2 + "last_summon_n = 0;\n"
    + T * 2 + "last_dragon_pos = {};\n"
)
new_clear = (
    T * 2 + "markers.clear();\n"
    + T * 2 + "door_tip_shown = false;\n"
    + T * 2 + "last_summon_pos.clear();\n"
    + T * 2 + "last_dragon_pos = {};\n"
)
assert c.count(old_clear) == 1, "CLEAR_FOUND %d" % c.count(old_clear)
c = c.replace(old_clear, new_clear)

# insert prune after attack_cd_ms decrement, before `for (auto& kv : markers)`
anchor = re.search(
    r"(?m)^(\t\t)if \(attack_cd_ms > 0\)\n"
    r"\t\t\tattack_cd_ms -= Constants::TIMESTEP;\n\n", c)
assert anchor, "PRUNE_ANCHOR_NOT_FOUND"
anchors = len(re.findall(
    r"(?m)^\t\tif \(attack_cd_ms > 0\)\n\t\t\tattack_cd_ms -= Constants::TIMESTEP;\n\n", c))
assert anchors == 1, "PRUNE_ANCHOR_FOUND %d" % anchors
ind = anchor.group(1)
prune = "\n".join([
    ind + "// 影子缓存自愈：die 动画播完 / lifetime 到期 / RemoveSummonHandler 都会让",
    ind + "// 召唤兽从 markers 消失，旧数组槽却永不回收；槽满后新召唤每帧重发",
    ind + "// MOVE_SUMMON（约 60/s）。先按 live 状态裁剪，再按 oid 查缓存。",
    ind + "for (auto it = last_summon_pos.begin(); it != last_summon_pos.end();)",
    ind + "{",
    ind + T + "auto ex = markers.get(it->first);",
    ind + T + "auto* ex_sum = ex ? dynamic_cast<MapSummon*>(ex.get()) : nullptr;",
    ind + T + "if (!ex || !ex->is_active() || !ex_sum || ex_sum->get_owner() != local_cid)",
    ind + T + T + "it = last_summon_pos.erase(it);",
    ind + T + "else",
    ind + T + T + "++it;",
    ind + "}",
    "",
]) + "\n"
c = c[:anchor.end()] + prune + c[anchor.end():]

old_loop = "\n".join([
    T * 4 + "bool moved = true;",
    T * 4 + "for (uint8_t i = 0; i < last_summon_n; ++i)",
    T * 4 + "{",
    T * 5 + "if (last_summon_oid[i] == sum->get_oid())",
    T * 5 + "{",
    T * 6 + "moved = last_summon_pos[i] != pos;",
    T * 6 + "last_summon_pos[i] = pos;",
    T * 6 + "break;",
    T * 5 + "}",
    T * 4 + "}",
    T * 4 + "if (moved)",
    T * 4 + "{",
    T * 5 + "bool found = false;",
    T * 5 + "for (uint8_t i = 0; i < last_summon_n; ++i)",
    T * 6 + "if (last_summon_oid[i] == sum->get_oid()) { found = true; break; }",
    T * 5 + "if (!found && last_summon_n < 8)",
    T * 5 + "{",
    T * 6 + "last_summon_oid[last_summon_n] = sum->get_oid();",
    T * 6 + "last_summon_pos[last_summon_n] = pos;",
    T * 6 + "++last_summon_n;",
    T * 5 + "}",
    T * 5 + "Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);",
    T * 5 + "MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();",
    T * 4 + "}",
])
new_loop = "\n".join([
    T * 4 + "auto cache = last_summon_pos.find(sum->get_oid());",
    T * 4 + "bool moved = cache == last_summon_pos.end() || cache->second != pos;",
    T * 4 + "if (cache == last_summon_pos.end())",
    T * 5 + "last_summon_pos.emplace(sum->get_oid(), pos);",
    T * 4 + "else",
    T * 5 + "cache->second = pos;",
    T * 4 + "if (moved)",
    T * 4 + "{",
    T * 5 + "Movement mv(pos.x(), pos.y(), pos.x(), pos.y(), 0, 1);",
    T * 5 + "MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();",
    T * 4 + "}",
])
assert c.count(old_loop) == 1, "OLD_LOOP_FOUND %d" % c.count(old_loop)
c = c.replace(old_loop, new_loop)

assert "last_summon_n" not in c and "last_summon_oid" not in c, "CPP_STALE"
assert c.count("last_summon_pos.erase(it)") == 1, "PRUNE_MISSING"
assert c.count("auto cache = last_summon_pos.find(sum->get_oid());") == 1, "FIND_MISSING"
assert c.count("MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();") == 1, "PACKET_LOST"
save(C, c, ccrlf)
print("CPP_OK crlf=%s prune=1 find=1 packet=1" % ccrlf)
print("WROTE_OK")
