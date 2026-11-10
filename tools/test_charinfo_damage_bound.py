#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""round47 / Wave2 2-15 -- UICharInfo::draw() damage 子窗循环上界语义守卫。

缺陷（第 14 例同族：主体路径用枚举分段、旁路分支拿元素个数当键域）：
    IO/UITypes/UICharInfo.cpp:303（修复前）
        for (size_t i = Buttons::BtFAQ; i < buttons.size(); i++)
    buttons 是 稀疏 std::map<uint16_t, unique_ptr<Button>>，try_btn 只在资源
    节点存在时插入。buttons.size() 是"元素个数"，与 map 键域无关：
      - 元素少（size < 16）时 damage 循环一次都不跑 -> BtFAQ(16)/BtRegist(17) 漏绘；
      - size == 17 时只跑到 i=16 -> BtRegist(17) 漏绘；
      - size >= 19（含 BtBook=18）时循环多跑 i=18 -> 与 :317 的 BtBook 单独绘重复。
    主窗 [0,BtArrayGet)、collect [BtArrayGet,BtFAQ) 都是枚举分段，damage 理应
    是 [BtFAQ,BtBook)。修复：上界 buttons.size() -> Buttons::BtBook。

本 harness 复刻三种上界下的"被 draw 的按钮 id 集合"，与 expected（键域 ∩ [16,18)）
逐字对比；统计 BtBook(18) 重复绘次数。合法构图（size==18、无 damage 键、空 map）
回归一致。纯静态 + 场景模拟，不启动客户端。
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "IO", "UITypes", "UICharInfo.cpp")
HDR = os.path.join(ROOT, "IO", "UITypes", "UICharInfo.h")

BtArrayGet = 14
BtFAQ = 16
BtRegist = 17
BtBook = 18


def _read(path):
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read()


def static_guards():
    src = _read(SRC)
    hdr = _read(HDR)
    # (name, needle, where, present)
    g = [
        ("new_damage_loop", "for (size_t i = Buttons::BtFAQ; i < Buttons::BtBook; i++)", src, True),
        ("old_size_loop_removed", "i < buttons.size(); i++", src, False),
        ("comment_enum_segment", "同为枚举分段", src, True),
        ("comment_size_semantics", "是元素个数而非键域", src, True),
        ("comment_btbook_dup", "重复绘 BtBook", src, True),
        ("find_guard_kept", "auto it = buttons.find(static_cast<uint16_t>(i));", src, True),
        ("null_guard_kept", "if (it != buttons.end() && it->second)", src, True),
        ("main_enum_bound", "i < Buttons::BtArrayGet; i++", src, True),
        ("collect_enum_bound", "i < Buttons::BtFAQ; i++", src, True),
        ("btbook_separate_draw", "buttons.find(Buttons::BtBook)", src, True),
        ("enum_BtArrayGet", "BtArrayGet,", hdr, True),
        ("enum_BtFAQ", "BtFAQ,", hdr, True),
        ("enum_BtRegist", "BtRegist,", hdr, True),
        ("enum_BtBook", "BtBook", hdr, True),
    ]
    failed = [n for (n, needle, where, want) in g if (needle in where) != want]
    return len(g), failed


def enum_values(hdr_text):
    """Parse Buttons enum order -> {name: value}."""
    start = hdr_text.find("enum Buttons")
    brace = hdr_text.find("{", start)
    body = hdr_text[brace + 1:hdr_text.find("};", brace)]
    names = []
    for tok in body.replace("\n", " ").split(","):
        tok = tok.strip().split()[0] if tok.strip() else ""
        if tok.startswith("Bt") and tok not in names:
            names.append(tok)
    return {n: i for i, n in enumerate(names)}


def damage_drawn(keys, bound):
    """DAMAGE 子窗循环 for(i=BtFAQ; i<bound; i++) 命中存在的键 -> id 列表。"""
    out = []
    for i in range(BtFAQ, bound):
        if i in keys:
            out.append(i)
    return out


def btbook_dup(damage_ids):
    """BtBook(18) 被 damage 循环与 :317 各绘一次 -> 重复度。"""
    return 1 if BtBook in damage_ids else 0


def scenarios(ev):
    """返回 (name, keys, size, expected_ids, expect_before_ok, note)。

    expected_ids = 键域 [BtFAQ, BtBook) 内实际存在的键（升序）。
    before 用 bound=size（元素个数），after 用 bound=BtBook（枚举上界）。
    expect_before_ok = before 的 damage 集合是否已经等于 expected 且无 BtBook 重复。
    """
    A = ev["BtArrayGet"]; F = ev["BtFAQ"]; R = ev["BtRegist"]; B = ev["BtBook"]
    main = set(range(0, A))                # 主窗按钮（键齐）
    collect = {A, A + 1}
    dmg = {F, R}
    cases = []
    # S1 全资源键齐 19 个（含 BtBook）-> before 多绘 18（重复）
    keys = main | collect | dmg | {B}
    cases.append(("S1 full-19keys", keys, len(keys), [F, R], False,
                  "size=19 -> before 也绘 BtBook(18) 与 :317 重复"))
    # S2 主窗 12 + damage 2 = 14，无 collect 无 BtBook -> before size<16 漏绘 16/17
    keys = set(range(0, 12)) | dmg
    cases.append(("S2 sparse-14keys", keys, len(keys), [F, R], False,
                  "size=14<16 -> before damage 循环不跑，子窗按钮全漏"))
    # S3 主窗 13 + damage 2 = 15 -> 仍漏绘
    keys = set(range(0, 13)) | dmg
    cases.append(("S3 sparse-15keys", keys, len(keys), [F, R], False,
                  "size=15<16 -> before 仍不跑"))
    # S4 主窗 14 + collect 1 + damage 2 = 17 -> before 只绘 16，漏 17
    keys = main | {A} | dmg
    cases.append(("S4 sparse-17keys", keys, len(keys), [F, R], False,
                  "size=17 -> before 只 i=16，BtRegist(17) 漏绘"))
    # S5 主窗 14 + collect 2 + damage 2 = 18，无 BtBook -> before/after 一致（回归）
    keys = main | collect | dmg
    cases.append(("S5 exact-18keys", keys, len(keys), [F, R], True,
                  "size=18 == BtBook -> before 恰好等价（happy-path 回归）"))
    # S6 主窗 14 + collect 2，无 damage 键 = 16 -> before/after 都不绘（回归）
    keys = main | collect
    cases.append(("S6 no-damage-16keys", keys, len(keys), [], True,
                  "无 damage 键，前后都空绘"))
    # S7 仅主窗 11 键（当前 shipped 裁剪包近似）-> 都空绘（回归）
    keys = set(range(0, 11))
    cases.append(("S7 shipped-cut-11keys", keys, len(keys), [], True,
                  "裁剪包子窗键全缺，前后都空绘"))
    # S8 空 map -> 都空绘（回归）
    keys = set()
    cases.append(("S8 empty-map", keys, len(keys), [], True,
                  "空 map，前后都空绘"))
    # S9 只有 BtFAQ(16) 一个子窗键，size=1 -> before 漏绘 16
    keys = {F}
    cases.append(("S9 only-faq-1key", keys, len(keys), [F], False,
                  "size=1 -> before 不跑，BtFAQ 漏绘"))
    # S10 只有 BtRegist(17)，size=1 -> before 漏绘 17
    keys = {R}
    cases.append(("S10 only-regist-1key", keys, len(keys), [R], False,
                  "size=1 -> before 不跑，BtRegist 漏绘"))
    # S11 19 键含 BtBook + 域外填充键 21，但缺 BtRegist -> before 绘 16、18，BtBook 重复
    # 注意 bound=size() 时循环是 i<size；要让 i 走到 BtBook(18) 必须 size>=19。
    keys = (main | collect | {F} | {B, 21})
    cases.append(("S11 btbook-no-regist", keys, len(keys), [F], False,
                  "size=19 -> before 绘 16、18，BtBook 重复且漏 17"))
    # S12 20 键（多一个越界键 19）-> before 多绘 18、19
    keys = main | collect | dmg | {B, 19}
    cases.append(("S12 extra-key-20", keys, len(keys), [F, R], False,
                  "size=20 -> before 绘到 19，重复绘 BtBook"))
    # S13 15 键含 damage 两键但缺一个主窗 -> before 漏绘
    keys = (set(range(0, 13)) | dmg)
    cases.append(("S13 main-gap-15", keys, len(keys), [F, R], False,
                  "size=15 -> before 不跑（与 S3 同数据，独立标注）"))
    return cases


def main():
    n_guard, failed = static_guards()
    print("STATIC GUARDS: %d checked, %d failed" % (n_guard, len(failed)))
    for f in failed:
        print("  FAILED GUARD: %s" % f)
    if failed:
        return 1

    hdr = _read(HDR)
    ev = enum_values(hdr)
    need = {"BtArrayGet": 14, "BtFAQ": 16, "BtRegist": 17, "BtBook": 18}
    for k, v in need.items():
        if ev.get(k) != v:
            print("ENUM MISMATCH %s=%s (expect %s)" % (k, ev.get(k), v))
            return 1
    print("ENUM VALUES OK: " + ", ".join("%s=%d" % (k, ev[k]) for k in need))

    cases = scenarios(ev)
    bad_before = 0
    bad_after = 0
    dup_before = 0
    dup_after = 0
    regression = 0
    print("\n%-26s %5s  %-18s %-18s  %-8s %s" % (
        "SCENARIO", "SIZE", "BEFORE(drawn)", "AFTER(drawn)", "BEFORE", "NOTE"))
    for (name, keys, size, expected, before_ok, note) in cases:
        b = damage_drawn(keys, size)          # 修复前 bound = size()
        a = damage_drawn(keys, ev["BtBook"])  # 修复后 bound = BtBook
        b_ok = (b == expected) and btbook_dup(b) == 0
        a_ok = (a == expected) and btbook_dup(a) == 0
        if not b_ok:
            bad_before += 1
        if not a_ok:
            bad_after += 1
        dup_before += btbook_dup(b)
        dup_after += btbook_dup(a)
        if before_ok:
            regression += 1 if (b_ok and a_ok) else 0
        flag = "OK" if b_ok else "BAD"
        print("%-26s %5d  %-18s %-18s  %-8s %s" % (
            name, size, str(b), str(a), flag, note))
        # cross-check declared expectation against computed before
        assert b_ok == before_ok, (
            "scenario %s declared before_ok=%s but computed=%s (drawn=%s)"
            % (name, before_ok, b_ok, b))
        # after must ALWAYS be correct
        assert a_ok, "scenario %s after still wrong: %s" % (name, a)

    total_reg = sum(1 for c in cases if c[4])
    print("\nSUMMARY bad_before=%d bad_after=%d dupBtBook_before=%d dupBtBook_after=%d regression=%d/%d"
          % (bad_before, bad_after, dup_before, dup_after, regression, total_reg))
    if bad_after != 0 or dup_after != 0 or regression != total_reg or bad_before == 0:
        print("FAIL")
        return 1
    print("PASS: %d static guards + %d scenarios (before BAD %d / BtBook-dup %d -> after 0; regression %d/%d)"
          % (n_guard, len(cases), bad_before, dup_before, regression, total_reg))
    return 0


if __name__ == "__main__":
    sys.exit(main())
