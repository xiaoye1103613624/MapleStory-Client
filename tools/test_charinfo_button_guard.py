#!/usr/bin/env python3
"""round46 harness: UICharInfo 子窗口稀疏 map 裸 at(i) -> find+判空守卫.

复刻 std::map<uint16_t, unique_ptr<Button>> 的稀疏键语义：
- before 用 buttons.at(i)（python dict 索引，缺键抛 KeyError 模拟 std::out_of_range）
- after 用 buttons.find(i) 判空跳过
验收：before 抛异常的场景 after 归零；合法构图 before/after 画到的按钮集合逐字一致。
期望值全部由 SCENARIOS 自身数据推导，不写死值断言（round43 脚本自身 bug 教训）。
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = os.path.join(ROOT, "IO", "UITypes", "UICharInfo.cpp")
ELEM_CPP = os.path.join(ROOT, "IO", "UIElement.cpp")
ELEM_H = os.path.join(ROOT, "IO", "UIElement.h")


def read(path):
    with open(path, "r", encoding="utf-8") as f:
        return f.read()


# ---- 静态守卫：源码必须逐字出现/消失的串 ----
# (name, needle, path, want_present)
STATIC_GUARDS = [
    ("collect_loop_kept", "for (size_t i = Buttons::BtArrayGet; i < Buttons::BtFAQ; i++)", CPP, True),
    ("damage_loop_kept", "for (size_t i = Buttons::BtFAQ; i < buttons.size(); i++)", CPP, True),
    ("main_guard_paradigm_kept", "for (size_t i = 0; i < Buttons::BtArrayGet; i++)", CPP, True),
    ("find_cast_present", "auto it = buttons.find(static_cast<uint16_t>(i));", CPP, True),
    ("null_check_present", "if (it != buttons.end() && it->second)", CPP, True),
    ("old_at_line_removed", "if (const auto button = buttons.at(i).get())", CPP, False),
    ("any_at_removed", "buttons.at(i)", CPP, False),
    ("comment_sparse_map", "buttons 是稀疏 map", CPP, True),
    ("comment_lib_reference", "IO/UIElement.cpp:168-176", CPP, True),
    ("comment_size_upper_bound", "原上界 buttons.size() 是元素个数", CPP, True),
    ("lib_note_null_required", "必须判空后再解引用", ELEM_CPP, True),
    ("map_decl_sparse", "std::map<uint16_t, std::unique_ptr<Button>> buttons;", ELEM_H, True),
    ("try_btn_conditional_insert", "if (src)\n\t\t\t\tbuttons[id] = std::make_unique<MapleButton>(src, pos);", CPP, True),
]

# 计数守卫：find+判空必须在两处修复循环都出现（>=2），注释也出现 2 次
COUNT_GUARDS = [
    ("find_cast_count>=2", "auto it = buttons.find(static_cast<uint16_t>(i));", CPP, 2),
    ("sparse_comment_count>=2", "稀疏 map", CPP, 2),
]


# ---- 场景：复刻两个子窗口循环的 before/after ----
# Buttons 枚举值（IO/UITypes/UICharInfo.h:55-76 逐字数出）
BT_ARRAY_GET = 14
BT_FAQ = 16
BT_BOOK = 18


def loop_before(buttons, lo, hi):
    """裸 at(i)：对 [lo,hi) 范围内缺失的键抛异常。返回画到的 id 列表或抛 KeyError。"""
    drawn = []
    for i in range(lo, hi):
        b = buttons[i]  # dict.__getitem__ 缺键 KeyError ~ std::map::at out_of_range
        if b is not None:
            drawn.append(i)
    return drawn


def loop_after(buttons, lo, hi):
    """find+判空：缺失/空指针键跳过，不抛。"""
    drawn = []
    for i in range(lo, hi):
        b = buttons.get(i)
        if b is not None:
            drawn.append(i)
    return drawn


def collect_hi():
    return BT_FAQ


def damage_hi(buttons):
    return len(buttons)  # 原上界 buttons.size()（元素个数），本轮保留边界不裁决语义


# 场景 = (label, which_loop, present_keys, empty_ptr_keys)
# present_keys: 该 id 存在一个非空 Button；empty_ptr_keys: 该 id 存在但为 null（try_btn 未插入）
SCENARIOS = [
    # --- 异常场景：稀疏 map 在循环范围内有缺键，before 必抛，after 跳过 ---
    ("S1 collect 缺 BtArrayGet", "collect", [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 18], []),
    ("S2 collect 两个键全缺", "collect", [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 18], []),
    ("S3 collect 缺 BtArrayName", "collect", [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 18], []),
    ("S4 damage 缺 BtFAQ/BtRegist (size=18)", "damage", list(range(0, 16)) + [18, 19], []),
    ("S5 damage 全子窗键缺 (size=17)", "damage", list(range(0, 16)) + [19], []),
    ("S6 damage size=20 但子窗键域有洞", "damage", list(range(0, 16)) + list(range(20, 24)), []),
    # --- 回归：循环范围内键齐备（或范围为空），before 不抛且 before/after 集合逐字一致 ---
    ("R1 collect 全键齐备", "collect", list(range(0, 19)), []),
    ("R2 damage 全键齐备 (size=19)", "damage", list(range(0, 19)), []),
    ("R3 collect 带空 unique_ptr 但键存在", "collect", list(range(0, 19)), [15]),
    ("R4 damage 带空 unique_ptr 但键存在", "damage", list(range(0, 19)), [16]),
    ("R5 collect 边界两键齐备", "collect", list(range(0, 14)) + [14, 15, 18], []),
    ("R6 空 map (size=0 -> damage 范围空)", "damage", [], []),
    ("R7 当前 shipped 包无子窗按钮 (size=13 -> 范围空)", "damage", list(range(0, 13)), []),
]


def main():
    cpp = read(CPP)
    elem_cpp = read(ELEM_CPP)
    elem_h = read(ELEM_H)
    texts = {CPP: cpp, ELEM_CPP: elem_cpp, ELEM_H: elem_h}

    failed = []
    for name, needle, path, want in STATIC_GUARDS:
        present = needle in texts[path]
        if present != want:
            failed.append("%s (want_present=%s got=%s)" % (name, want, present))
    print("STATIC GUARDS: %d checked, %d failed" % (len(STATIC_GUARDS), len(failed)))
    for name, needle, path, minc in COUNT_GUARDS:
        c = texts[path].count(needle)
        if c < minc:
            failed.append("%s (count=%d < %d)" % (name, c, minc))
    print("COUNT GUARDS: %d checked" % len(COUNT_GUARDS))
    if failed:
        for f in failed:
            print("  FAIL:", f)
        return 1

    throw_before = 0
    throw_after = 0
    regression_ok = 0
    regression_total = 0
    for label, which, keys, empties in SCENARIOS:
        buttons = {k: ("btn" if k not in empties else None) for k in keys}
        lo = BT_ARRAY_GET if which == "collect" else BT_FAQ
        hi = collect_hi() if which == "collect" else damage_hi(buttons)
        hi = max(hi, lo)  # range 空时保持空

        # before
        before_threw = False
        before_drawn = []
        try:
            before_drawn = loop_before(buttons, lo, hi)
        except KeyError:
            before_threw = True
        # after（永不抛）
        after_threw = False
        after_drawn = []
        try:
            after_drawn = loop_after(buttons, lo, hi)
        except KeyError:
            after_threw = True

        # 期望：缺失键（或范围外）存在 -> before 必抛；否则 before/after 集合一致
        missing_in_range = [i for i in range(lo, hi) if i not in buttons]
        expect_before_throw = bool(missing_in_range)
        # 期望 after 画到 = 范围内存在且非空的键
        expect_after_drawn = [i for i in range(lo, hi) if buttons.get(i) is not None]

        ok = True
        if before_threw != expect_before_throw:
            ok = False
        if after_threw:
            ok = False
        if after_drawn != expect_after_drawn:
            ok = False
        # 回归场景（before 不抛）：before 与 after 画到集合须逐字一致
        is_regression = label.startswith("R")
        if is_regression:
            regression_total += 1
            if not before_threw and before_drawn == after_drawn == expect_after_drawn:
                regression_ok += 1
            else:
                ok = False
        if before_threw:
            throw_before += 1
        if after_threw:
            throw_after += 1
        if not ok:
            failed.append("%s (before_threw=%s expected=%s after=%s expected=%s)" % (
                label, before_threw, expect_before_throw, after_drawn, expect_after_drawn))
        print("  %-46s before=%-5s after=%-5s drawn=%s" % (
            label, "THROW" if before_threw else "OK", "THROW" if after_threw else "OK", after_drawn))

    print("SCENARIOS: before THROW=%d -> after THROW=%d; regression=%d/%d" % (
        throw_before, throw_after, regression_ok, regression_total))
    if failed or throw_after != 0 or regression_ok != regression_total:
        for f in failed:
            print("  FAIL:", f)
        return 1
    if throw_before == 0:
        print("  FAIL: no before-THROW scenario exercised")
        return 1
    print("PASS: %d static guards + %d count guards + %d scenarios (before THROW %d -> after 0; regression %d/%d)" % (
        len(STATIC_GUARDS), len(COUNT_GUARDS), len(SCENARIOS), throw_before, regression_ok, regression_total))
    return 0


if __name__ == "__main__":
    sys.exit(main())
