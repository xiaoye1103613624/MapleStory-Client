#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""round40 Wave2 2-8: MOVE_SUMMON 影子缓存洪泛缺陷 —— 离线 harness。

缺陷（修复前 Gameplay/MapleMap/MapFieldMarkers.cpp:344-367）：
  last_summon_oid[8] / last_summon_pos[8] / last_summon_n 三件套只在
  clear() 复位；die 动画播完 / lifetime 到期 / RemoveSummonHandler 回收
  召唤兽时都从 markers 移除对象，却从不回收数组槽。8 槽被僵尸 oid 占满后
  新召唤 `moved` 恒 true、`found` 恒 false 且 `n<8` 不成立 → 每 TIMESTEP
  发一次 MoveSummonPacket（≈60/s）直到换图。

修复（live 现状）：
  改成 std::unordered_map<int32_t, Point<int16_t>> last_summon_pos 键控缓存，
  每帧先按 markers 的 live 状态自愈裁剪 gone/inactive/non-MapSummon/foreign
  条目，再按 oid 查缓存；无 8 槽上限，新召唤首帧登记后静止不再重发。

本 harness：
  ① 静态核验 live 源码守卫逐字在盘（且旧数组三字段已彻底移除）；
  ② Python 复刻修复前 OldCache(8 槽数组) 与修复后 NewCache(键控 map + prune)，
     跑「8 槽僵尸 + 新静止召唤 60 帧」发洪泛曲线对照；
  ③ 10 个场景分 F(僵尸洪泛)/N(常规回归)/P(裁剪)，断言 before flood>0 → after 0，
     且正常移动/多召唤/未死亡等场景前后行为一致。

只覆盖静态守卫 + 状态机算法复刻，真机观感/回包 hex 进 pending-live-tests §19。
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = os.path.join(ROOT, "Gameplay", "MapleMap", "MapFieldMarkers.cpp")
HDR = os.path.join(ROOT, "Gameplay", "MapleMap", "MapFieldMarkers.h")


def read(path):
    with io.open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read().replace("\r\n", "\n")


# ---------------------------------------------------------------------------
# ① 静态守卫
# ---------------------------------------------------------------------------
# (label, file, needle, expect_present)
GUARDS = [
    ("hdr_unordered_map_include", HDR, "#include <unordered_map>", True),
    ("hdr_keyed_cache", HDR,
     "std::unordered_map<int32_t, Point<int16_t>> last_summon_pos;", True),
    ("cpp_clear_cache", CPP, "last_summon_pos.clear();", True),
    ("cpp_prune_live_state", CPP,
     "if (!ex || !ex->is_active() || !ex_sum || ex_sum->get_owner() != local_cid)", True),
    ("cpp_prune_erase", CPP, "it = last_summon_pos.erase(it);", True),
    ("cpp_emplace_keyed", CPP,
     "last_summon_pos.emplace(sum->get_oid(), pos);", True),
    ("cpp_still_dispatches_move", CPP,
     "MoveSummonPacket(sum->get_oid(), pos, mv).dispatch();", True),
    ("cpp_old_array_gone_n", CPP, "last_summon_n", False),
    ("cpp_old_array_gone_oid", CPP, "last_summon_oid[]", False),
    ("hdr_old_array_gone", HDR, "last_summon_oid[8]", False),
]


def check_guards():
    cache = {}
    bad = []
    for label, path, needle, expect in GUARDS:
        if path not in cache:
            cache[path] = read(path)
        present = needle in cache[path]
        ok = (present == expect)
        print("[%s] %s (%s)" % ("OK" if ok else "FAIL", label,
                                "present" if present else "absent"))
        if not ok:
            bad.append(label)
    return len(cache), bad


# ---------------------------------------------------------------------------
# ② 状态机复刻
# ---------------------------------------------------------------------------
class OldCache:
    """修复前：8 槽数组，永不回收（只有 clear() 复位，这里不模拟换图）。"""
    CAP = 8

    def __init__(self):
        self.oid = [0] * self.CAP
        self.pos = [None] * self.CAP
        self.n = 0

    def frame(self, summons):
        # summons: list of (oid, pos) 当前 live 且归本角色的召唤兽
        packets = 0
        for (oid, pos) in summons:
            moved = True
            for i in range(self.n):
                if self.oid[i] == oid:
                    moved = self.pos[i] != pos
                    self.pos[i] = pos
                    break
            if moved:
                found = any(self.oid[i] == oid for i in range(self.n))
                if not found and self.n < self.CAP:
                    self.oid[self.n] = oid
                    self.pos[self.n] = pos
                    self.n += 1
                packets += 1  # MoveSummonPacket.dispatch()
        return packets

    def clear(self):
        self.n = 0


class NewCache:
    """修复后：键控 map + 每帧自愈裁剪（gone/inactive/non-summon/foreign）。"""
    def __init__(self):
        self.cache = {}  # oid -> pos

    def frame(self, summons, live_oids):
        # prune：缓存里 oid 不在 live marker 集合 → 删除
        for oid in list(self.cache.keys()):
            if oid not in live_oids:
                del self.cache[oid]
        packets = 0
        for (oid, pos) in summons:
            if oid in self.cache:
                moved = self.cache[oid] != pos
                self.cache[oid] = pos
            else:
                moved = True
                self.cache[oid] = pos  # emplace
            if moved:
                packets += 1
        return packets

    def clear(self):
        self.cache.clear()


def run_scenario(kind, setup, frames):
    """返回 (before_flood, after_flood, before_packets, after_packets, detail)。
    flood = 静止召唤在登记首帧之外多发的空包数 = total_packets - 合法登记数。
    """
    old = OldCache()
    new = NewCache()

    # setup: list of (summons, live_oids)；live_oids 对 OldCache 无意义但保持入参一致
    for (summons, live_oids) in setup:
        old.frame(summons)
        new.frame(summons, live_oids)

    before_total = 0
    after_total = 0
    for (summons, live_oids) in frames:
        before_total += old.frame(summons)
        after_total += new.frame(summons, live_oids)

    # 合法登记数：这段 frames 内「首次出现的新 oid」数（每只召唤首帧 1 个包）
    first_seen = set()
    legit = 0
    for (summons, _live) in frames:
        for (oid, _pos) in summons:
            if oid not in first_seen:
                first_seen.add(oid)
                legit += 1

    before_flood = max(0, before_total - legit)
    after_flood = max(0, after_total - legit)
    return before_flood, after_flood, before_total, after_total, legit


def zombie_setup():
    """8 个召唤登记满槽 → 全部死亡（live 集合空）→ 新静止召唤登场。"""
    summons = [(1000 + k, (0, 0)) for k in range(8)]
    return [
        (summons, set(1000 + k for k in range(8))),  # 8 只存活，各登记一次
        ([], set()),                                  # 全死，markers 空
    ]


def stationary_frames(n, oid=100, pos=(100, 50)):
    return [([(oid, pos)], {oid}) for _ in range(n)]


CASES = [
    # id, kind, setup, frames, expect_before_flood_gt0, expect_after_flood_0
    ("F1", "F", zombie_setup(), stationary_frames(60), True, True),
    ("F2", "F", zombie_setup(), stationary_frames(30, oid=200, pos=(5, 5)), True, True),
    ("F3", "F", zombie_setup(), stationary_frames(120), True, True),
    # 常规回归：未死亡 / 8 槽内静止 → 前后都不该洪泛
    ("N1", "N", [([(1000 + k, (0, 0)) for k in range(8)], set(1000 + k for k in range(8)))],
     [([(1000 + k, (0, 0)) for k in range(8)], set(1000 + k for k in range(8))) for _ in range(60)],
     False, False),
    # 常规回归：单只召唤每帧移动 → 前后每帧都应发包（flood 定义不适用，检验 totals 相等）
    ("N2", "N", [], [([(7, (i, 0))], {7}) for i in range(20)], None, None),
    # 常规回归：新召唤首帧登记后静止（无僵尸）→ 前后都 0 flood
    ("N3", "N", [], stationary_frames(60, oid=300, pos=(1, 1)), False, False),
    # 常规回归：3 只召唤各自静止
    ("N4", "N", [], [([(o, (o, 0)) for o in (11, 12, 13)], {11, 12, 13}) for _ in range(40)],
     False, False),
    # 裁剪：缓存有 1 个 gone oid（槽未满），新召唤登场 → 旧版仍能登记，前后都不洪泛
    # （真正洪泛要 8 槽占满，见 F 系；这里检验新版 prune 不误伤正常登记）
    ("P1", "P", [([(900, (0, 0))], {900}), ([], set())], stationary_frames(60, oid=400), False, False),
    # 裁剪：僵尸不足 8 槽（3 个）→ 旧版仍能登记新召唤，不洪泛；新版同样
    ("P2", "P", [([(1000 + k, (0, 0)) for k in range(3)], set(1000 + k for k in range(3))),
                 ([], set())], stationary_frames(60, oid=500), False, False),
    ("N5", "N", [], [([(7, (i // 3, 0))], {7}) for i in range(30)], None, None),
]


def main():
    files_n, bad = check_guards()
    if bad:
        print("STATIC GUARD FAIL: %s" % bad)
        return 1
    print("STATIC GUARDS OK (%d needles, %d files)" % (len(GUARDS), files_n))

    f_before = 0
    f_after = 0
    regressions_ok = 0
    regressions_total = 0
    for (cid, kind, setup, frames, exp_b, exp_a) in CASES:
        bf, af, bt, at, legit = run_scenario(kind, setup, frames)
        if exp_b is None:  # totals-equal regression case (moving summon)
            ok = (bt == at)
            tag = "totals equal %d==%d" % (bt, at)
            regressions_total += 1
            if ok:
                regressions_ok += 1
        elif exp_b is False:  # 期望前后都不洪泛
            ok = (bf == 0 and af == 0)
            tag = "before flood=%d after flood=%d (expect both 0)" % (bf, af)
            regressions_total += 1
            if ok:
                regressions_ok += 1
        else:  # 期望 before>0 after==0
            ok = (bf > 0 and af == 0)
            tag = "before flood=%d -> after flood=%d (totals %d->%d, legit=%d)" % (
                bf, af, bt, at, legit)
            f_before += bf
            f_after += af
        print("[%s] %s[%s] %s" % ("OK" if ok else "FAIL", cid, kind, tag))
        if not ok:
            bad.append(cid)

    print("[flood] zombie cases cumulative packets: before=%d after=%d (legit excluded)"
          % (f_before, f_after))
    print("[regression] non-flood/normal cases ok: %d/%d" % (regressions_ok, regressions_total))
    if bad:
        print("FAIL: %s" % bad)
        return 1
    print("PASS: %d static guards + %d scenarios (before flood>0 -> after 0; regression %d/%d)"
          % (len(GUARDS), len(CASES), regressions_ok, regressions_total))
    return 0


if __name__ == "__main__":
    sys.exit(main())
