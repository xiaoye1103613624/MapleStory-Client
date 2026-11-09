#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
round45 / Wave2 2-13 离线 harness
UIItemInventory `if (pos)` 资源旁路 slot_col / slot_row <= 0 守卫。

复刻修复前/后的资源解析算法：
  before = 裸赋值 pos 子节点（缺失/非整数 -> nlnx get_integer() 返 0），
           随后 :310 / :689 / :690 三处整数除法以 slot_col 为分母。
  after  = 裸赋值后补 `if (slot_col <= 0 || slot_row <= 0)` 整组回退到
           与 else 分支相同的 v83 BeiDou 位图实测常量 (4,6,4,2,(8,50))。

验收信号：before DIV0/BAD > 0 -> after 0；合法带 pos 包 + 无 pos 包逐字回归。
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = os.path.join(ROOT, "IO", "UITypes", "UIItemInventory.cpp")
NODE_HPP = os.path.join(ROOT, "includes", "NoLifeNx", "nlnx", "node.hpp")
NODE_CPP = os.path.join(ROOT, "includes", "NoLifeNx", "nlnx", "node.cpp")


def read(path):
    with io.open(path, "r", encoding="utf-8") as f:
        return f.read()


# ---------------------------------------------------------------- 静态守卫
# (name, needle, path, want_present)
STATIC_GUARDS = [
    ("guard_condition", 'if (slot_col <= 0 || slot_row <= 0)', CPP, True),
    ("fallback_col_4", 'slot_col = 4;', CPP, True),
    ("fallback_row_6", 'slot_row = 6;', CPP, True),
    ("fallback_space_x", 'slot_space_x = 4;', CPP, True),
    ("fallback_space_y", 'slot_space_y = 2;', CPP, True),
    ("fallback_pos", 'Point<int16_t>(8, 50)', CPP, True),
    ("comment_get_integer", 'get_integer()', CPP, True),
    ("comment_node_cpp", 'node.cpp:161-179', CPP, True),
    ("comment_div0", '整数除法除零', CPP, True),
    ("pos_source_kept", 'nl::node pos = Item["pos"];', CPP, True),
    ("raw_assign_kept", 'slot_col = pos["slot_col"];', CPP, True),
    ("div_310_kept", 'inventory.get_slotmax(tab) / slot_col', CPP, True),
    ("div_689_kept", 'slotrange.at(tab).first / slot_col', CPP, True),
    ("multi_resource_window", 'UiAssetResolve::window("Item")', CPP, True),
    ("node_hpp_get_integer_sig", 'int64_t get_integer(int64_t = 0) const;', NODE_HPP, True),
    ("node_cpp_returns_def", 'return def;', NODE_CPP, True),
    ("old_bare_guard_absent_note", 'if (pos)\n\t\t{', CPP, True),
]


def check_static():
    texts = {}
    failed = 0
    for name, needle, path, want_present in STATIC_GUARDS:
        if path not in texts:
            texts[path] = read(path)
        present = needle in texts[path]
        ok = present == want_present
        if not ok:
            print("  STATIC FAIL", name, "present=", present, "want=", want_present)
            failed += 1
    # 额外计数守卫：回退常量应出现 >=2 次（if(pos) 守卫块 + else 分支）
    cpp = texts.get(CPP) or read(CPP)
    for needle, minimum in [("slot_col = 4;", 2), ("slot_row = 6;", 2),
                            ("Point<int16_t>(8, 50)", 2)]:
        if cpp.count(needle) < minimum:
            print("  COUNT FAIL", needle, "count=", cpp.count(needle), "want>=", minimum)
            failed += 1
    print("STATIC GUARDS: %d checked (+3 count guards), %d failed"
          % (len(STATIC_GUARDS), failed))
    return failed, len(STATIC_GUARDS)


# ---------------------------------------------------------------- 算法复刻
DEFAULT = (4, 6, 4, 2, (8, 50))  # col,row,space_x,space_y,slot_pos


def resolve(pos_fields, guarded):
    """pos_fields 缺键 = 子节点缺失/非整数 -> nlnx get_integer() 返 0。"""
    col = pos_fields.get("slot_col", 0)
    row = pos_fields.get("slot_row", 0)
    sx = pos_fields.get("slot_space_x", 0)
    sy = pos_fields.get("slot_space_y", 0)
    spos = pos_fields.get("slot_pos", (0, 0))
    if guarded and (col <= 0 or row <= 0):
        col, row, sx, sy, spos = DEFAULT
    return col, row, sx, sy, spos


def evaluate(pos_present, pos_fields, slotmax, guarded):
    if not pos_present:                 # else 分支：v83 BeiDou 位图实测常量
        col, row, sx, sy, spos = DEFAULT
    else:
        col, row, sx, sy, spos = resolve(pos_fields, guarded)
    max_slots = row * col
    max_full = col * max_slots
    div0 = 0
    if col == 0:
        # :310 / :689 / :690 三处整数除法皆以 slot_col 为分母
        div0 = 3
        totalrows_310 = row_689 = totalrows_690 = None
    else:
        totalrows_310 = slotmax // col
        row_689 = 1 // col
        totalrows_690 = slotmax // col
    bad = 1 if (col <= 0 or row <= 0 or max_slots <= 0) else 0
    return {
        "col": col, "row": row, "sx": sx, "sy": sy, "spos": spos,
        "max_slots": max_slots, "max_full": max_full,
        "t310": totalrows_310, "r689": row_689, "t690": totalrows_690,
        "div0": div0, "bad": bad,
    }


# (label, pos_present, pos_fields, slotmax, expect_before_bad, expect_after_bad, is_regression)
SCENARIOS = [
    # --- 异常/裁剪资源包：before 除零或归零，after 回退安全 ---
    ("S1 pos 全空（全字段缺失）", True, {}, 24, 1, 0, False),
    ("S2 缺 slot_col，row=6", True, {"slot_row": 6, "slot_space_x": 4,
                                    "slot_space_y": 2, "slot_pos": (8, 50)}, 24, 1, 0, False),
    ("S3 缺 slot_row，col=4", True, {"slot_col": 4}, 24, 1, 0, False),
    ("S4 slot_col=0 显式", True, {"slot_col": 0, "slot_row": 6}, 24, 1, 0, False),
    ("S5 slot_col=-1 负数据", True, {"slot_col": -1, "slot_row": 6}, 24, 1, 0, False),
    ("S6 slot_row=0 且 col=4", True, {"slot_col": 4, "slot_row": 0}, 24, 1, 0, False),
    # --- 合法带 pos 包 + 当前 shipped 无 pos 包：before/after 逐字等价 ---
    ("R1 col=4 row=6", True, {"slot_col": 4, "slot_row": 6}, 24, 0, 0, True),
    ("R2 col=12 row=6", True, {"slot_col": 12, "slot_row": 6}, 48, 0, 0, True),
    ("R3 col=5 row=8", True, {"slot_col": 5, "slot_row": 8}, 40, 0, 0, True),
    ("R4 col=6 row=4", True, {"slot_col": 6, "slot_row": 4}, 24, 0, 0, True),
    ("R5 col=1 row=1", True, {"slot_col": 1, "slot_row": 1}, 4, 0, 0, True),
    ("R6 col=8 row=12", True, {"slot_col": 8, "slot_row": 12}, 96, 0, 0, True),
    ("R7 当前 shipped 无 pos（else 分支）", False, {}, 24, 0, 0, True),
]


def check_scenarios():
    failed = 0
    div0_before = 0
    bad_before = 0
    bad_after = 0
    regression_total = 0
    regression_pass = 0
    for (label, pos_present, fields, slotmax,
         exp_bbad, exp_abad, is_regression) in SCENARIOS:
        before = evaluate(pos_present, fields, slotmax, guarded=False)
        after = evaluate(pos_present, fields, slotmax, guarded=True)
        ok = True
        if before["bad"] != exp_bbad:
            print("  SCENARIO FAIL(before)", label, "bad=", before["bad"],
                  "want=", exp_bbad)
            ok = False
        if after["bad"] != exp_abad:
            print("  SCENARIO FAIL(after)", label, "bad=", after["bad"],
                  "want=", exp_abad)
            ok = False
        div0_before += before["div0"]
        bad_before += before["bad"]
        bad_after += after["bad"]
        if is_regression:
            regression_total += 1
            keys = ["col", "row", "sx", "sy", "spos", "max_slots", "max_full",
                    "t310", "r689", "t690", "div0", "bad"]
            if all(before[k] == after[k] for k in keys):
                regression_pass += 1
            else:
                print("  SCENARIO FAIL(regression)", label,
                      "before=", before, "after=", after)
                ok = False
        if not ok:
            failed += 1
        else:
            print("  OK", label, "| before div0=%d bad=%d -> after div0=%d bad=%d"
                  % (before["div0"], before["bad"], after["div0"], after["bad"]))
    print("SCENARIOS: div0_before=%d bad_before=%d -> bad_after=%d regression=%d/%d"
          % (div0_before, bad_before, bad_after, regression_pass, regression_total))
    if bad_after != 0:
        print("  FAIL: bad_after != 0")
        failed += 1
    if regression_pass != regression_total:
        print("  FAIL: regression not fully identical")
        failed += 1
    return failed, len(SCENARIOS), div0_before, bad_before, bad_after, \
        regression_pass, regression_total


def main():
    s_failed, s_total = check_static()
    (c_failed, c_total, div0_before, bad_before, bad_after,
     reg_pass, reg_total) = check_scenarios()
    total_failed = s_failed + c_failed
    if total_failed:
        print("FAIL: %d check(s) failed" % total_failed)
        return 1
    print("PASS: %d static guards + %d scenarios "
          "(before DIV0 %d / BAD %d -> after BAD %d; regression %d/%d)"
          % (s_total, c_total, div0_before, bad_before, bad_after,
             reg_pass, reg_total))
    return 0


if __name__ == "__main__":
    sys.exit(main())
