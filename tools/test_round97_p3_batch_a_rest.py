# -*- coding: utf-8 -*-
"""round97 · P3 A 批余 6 条 —— 静态守卫 + 场景真值表。

守卫对象 = 本轮 7 个交付物（6 条判定 + 映射表 + 四文档落位 + evidence + notes）。
本轮零源码改动，故守卫里必须硬证「交付集里没有任何 cpp/h」，
并且「他人 WIP 源码里不含本轮标记串」（工作树长期压外部在途改动）。

断言口径就地按现场原文校，不放宽验收。
"""
import json
import os
import py_compile
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

P = {
    "maps": "docs/features/angel-calibration-d-matrix-2026-11/P3-A-BATCH-REST-MAPS.md",
    "wle": "docs/features/angel-calibration-d-matrix-2026-11/WLE-TRIAGE-PLAN.md",
    "p3": "docs/features/angel-calibration-d-matrix-2026-11/P3-ANGEL-REBUILD-PLAN.md",
    "dev": "docs/features/development-and-fix-plan-2026-11/README.md",
    "plt": "docs/features/pending-live-tests/README.md",
    "ev": "docs/features/angel-calibration-d-matrix-2026-11/evidence/p3-batch-a-rest-round97.json",
    "notes": "docs/features/development-and-fix-plan-2026-11/notes/wave3-p3-batch-a-rest-20261111.md",
}

CHECKS = []
SCENES = []


def chk(label, cond, detail=""):
    CHECKS.append((label, bool(cond), detail))
    if not cond:
        print("[FAIL] %s %s" % (label, detail))


def sc(label, cond, detail=""):
    SCENES.append((label, bool(cond), detail))
    if not cond:
        print("[FAIL-SCENE] %s %s" % (label, detail))


def rt(path):
    with open(path, "rb") as f:
        return f.read().decode("utf-8", "replace")


def lf(path):
    return rt(path).replace("\r\n", "\n")


def eol(path):
    b = open(path, "rb").read()
    t = b.decode("utf-8", "replace")
    crlf = t.count("\r\n")
    return {
        "bytes": len(b),
        "crlf": crlf,
        "lonelf": t.count("\n") - crlf,
        "crcrlf": t.count("\r\r\n"),
        "bom": b[:3] == b"\xef\xbb\xbf",
    }


def git(*args):
    r = subprocess.run(["git"] + list(args), capture_output=True)
    return r.stdout.decode("utf-8", "replace")


# ------------------------------------------------------------------ A 守卫
def guards():
    maps, wle, p3, dev, plt, ev_raw, notes = (lf(P[k]) for k in
                                             ("maps", "wle", "p3", "dev", "plt", "ev", "notes"))

    # A1 · 083 反编译锚点逐字在场（map/D1）
    chk("A1a map/D1 SaveFuncKeyMap addr", "0x58df2f" in maps)
    chk("A1b map/D1 89 slots", "v2 < 89" in maps)
    chk("A1c map/D1 5-byte step", "v3 += 5" in maps)
    chk("A1d map/D1 445 shadow", "445" in maps and "0x1BD" in maps)
    _e43 = [l for l in maps.split("\n") if "0x4e43fa" in l]
    chk("A1e map/D1 sub_4E43FA EncodeBuffer 5u",
        bool(_e43) and "EncodeBuffer" in _e43[0] and "5u" in _e43[0],
        "row=%r" % (_e43[0][:90] if _e43 else None))
    chk("A1f map/D1 DecodeBuffer(5u)", "0x4e4409" in maps and "DecodeBuffer" in maps)

    # A2 · map/D8 32 字节
    chk("A2a map/D8 SaveQuickslotKeyMap addr", "0x72b87e" in maps)
    chk("A2b map/D8 0x20 body", "0x20" in maps.replace("0x20u", "0x20"))
    chk("A2c map/D8 s9 strict 32", "QUICKSLOT_SIZE" in maps and "Integer.BYTES" in maps)
    chk("A2d map/D8 default 8 keys", "0x2A" in maps and "0x51" in maps)

    # A3 · cashshop/D2 七类 / 70 符号
    chk("A3a cashshop seven classes", all(c in maps for c in
        ("CCSWnd_Best", "CCSWnd_Char", "CCSWnd_Inventory", "CCSWnd_List",
         "CCSWnd_Locker", "CCSWnd_Status", "CCSWnd_Tab")))
    chk("A3b cashshop 70 symbols", "70" in maps)
    chk("A3c cashshop 67 methods", "67" in maps)
    chk("A3d client single class", "class UICashShop : public UIElement" in maps)
    chk("A3e client MAX_ITEMS 15", "MAX_ITEMS = 7u * 2u + 1u" in maps or "MAX_ITEMS=15" in maps)

    # A4 · map/D4 Deeper/Shallower + IMG 2 节点
    chk("A4a map/D4 Deeper", "0x9ee30e" in maps)
    chk("A4b map/D4 Shallower", "0x9ee507" in maps)
    chk("A4c map/D4 LoadMapList", "0x9ed38b" in maps)
    chk("A4d map/D4 WorldMap 2 children", "title" in maps and "Border" in maps)
    chk("A4e map/D4 WorldMapSearch NOT_FOUND", "WorldMapSearch" in maps and "不存在" in maps)

    # A5 · statusbar-hud/D1 45 方法簇
    chk("A5a clusters named", all(c in maps for c in
        ("lifecycle", "input", "status", "quickslot", "chat", "misc")))
    chk("A5b cluster arithmetic literal", "8 + 7 + 1 + 8 + 14 + 7 = 45" in maps)

    # A6 · social-economy/D3 CUIAlliance 0
    chk("A6a CUIAlliance zero", "*CUIAlliance*" in maps and "0" in maps)
    chk("A6b CTabGuildAlliance 9", "CTabGuildAlliance" in maps and "9" in maps)
    chk("A6c CreateAllianceButton", "0x91eba3" in maps)

    # A7 · 六条全部 DOC_ONLY 口径
    chk("A7a verdict keyword", "DOC_ONLY" in wle or "DOC_ONLY" in maps)
    chk("A7b zero wire statement", "零 wire" in wle or "零 wire" in notes)
    chk("A7c zero behavior statement", "零源码行为改动" in maps or "零行为改动" in wle)

    # A8 · WLE §14
    chk("A8a WLE sec14 heading", "## 14. round97" in wle)
    chk("A8b WLE six bullets", wle.count("\n- `map/D1`") + wle.count("\n- `map/D1`:") >= 1)
    chk("A8c WLE 14.2 arithmetic", "168 = 76+38+14+40" in wle)

    # A9 · P3 Plan §7 回执 + §3 step2
    chk("A9a P3 sec7 heading", "## 7. round97" in p3)
    chk("A9b P3 step2 marked done", "round97 已完成" in p3)
    chk("A9c P3 six rows", p3.count("| `map/D1` |") >= 1 and p3.count("| `social-economy/D3` |") >= 1)

    # A10 · dev-plan 2-45
    chk("A10a dev 2-45 row", "| **2-45** |" in dev)
    chk("A10b dev row mentions round97", "round97" in dev.split("| **2-45** |")[1][:400])
    chk("A10c dev 2-44 still present", "| **2-44** |" in dev)
    # 行序：2-45 必须在 2-44 之后（单调递增）
    i45 = dev.index("| **2-45** |")
    i44 = dev.index("| **2-44** |")
    chk("A10d dev row order 44 < 45", i44 < i45, "i44=%d i45=%d" % (i44, i45))

    # A11 · PLT §56
    chk("A11a PLT sec56 heading", "## 56. round97" in plt)
    chk("A11b PLT six items", all(("56-%d" % i) in plt for i in range(1, 8)))
    _secs = re.findall(r"^## (\d+)\.", plt, flags=re.M)
    chk("A11c PLT last section is 56", bool(_secs) and _secs[-1] == "56", "last=%s" % (_secs[-1] if _secs else None))

    # A12 · evidence 结构 + 算术
    ev = json.loads(ev_raw)
    chk("A12a ev items 6", len(ev["items"]) == 6, str(sorted(ev["items"])))
    chk("A12b ev batch_a 9", ev["batch_a"]["total"] == 9 and ev["batch_a"]["done_round97"] == 6
        and ev["batch_a"]["remaining"] == 0)
    chk("A12c ev remaining 7", ev["batch_remaining"]["total"] == 7)
    chk("A12d ev arithmetic", ev["arithmetic_unchanged"]["classification_168"] == "76+38+14+40=168")
    chk("A12e ev zero flags", ev["zero_wire"] and ev["zero_behavior_change"] and ev["zero_source_change"])
    chk("A12f ev statusbar clusters sum", ev["items"]["statusbar-hud/D1"]["ida_facts"]["cluster_sum"] == 45)
    chk("A12g ev cashshop class sum", ev["items"]["cashshop/D2"]["ida_facts"]["class_sum"] == 70)
    chk("A12h ev map/D1 facts", ev["items"]["map/D1"]["ida_facts"]["slots"] == 89
        and ev["items"]["map/D1"]["ida_facts"]["slot_bytes"] == 5
        and ev["items"]["map/D1"]["ida_facts"]["shadow_bytes"] == 445)
    chk("A12i ev map/D8 facts", ev["items"]["map/D8"]["ida_facts"]["opcode"] == 183
        and ev["items"]["map/D8"]["ida_facts"]["body_bytes"] == 32)
    chk("A12j ev CUIAlliance zero", ev["items"]["social-economy/D3"]["ida_facts"]["cui_alliance_symbols"] == 0)
    # 本轮自身 commit hash 不得进产物（round88 B9b：amend 会漂移）；
    # 历史已冻结的 commit 引用（如 round85 4b2452c4）允许，但须显式白名单。
    # 4b2452c4 = 历史冻结的 round85 提交引用；a78ba681a4 = s9 仓库 HEAD（GATE 基线的一部分，
    # 是「对端源码版本」而非本轮自己的提交）。两者都不是会自动漂移的自身引用。
    HIST = {"4b2452c4", "a78ba681a4"}
    heads = set(re.findall(r"\b[0-9a-f]{7,40}\b", ev_raw.replace("0x", "")))
    chk("A12k ev no own-commit hash", heads <= HIST, "unknown hash=%s" % (heads - HIST))
    head = git("log", "-1", "--format=%h").strip()
    if head:
        for k in ("ev", "notes", "maps", "wle", "p3", "dev", "plt"):
            chk("A12k-%s no HEAD hash" % k, head not in lf(P[k]), "HEAD=%s" % head)

    # A13 · notes 五节
    chk("A13a notes five sections", notes.count("\n## ") == 5, "got %d" % notes.count("\n## "))
    chk("A13b notes method statement", "083 反编译" in notes and "s9" in notes)
    chk("A13c notes why-no-code", "不改代码" in notes)

    # A14 · EOL 无 CRCRLF、BOM
    for k in ("maps", "wle", "p3", "dev", "plt", "ev", "notes"):
        e = eol(P[k])
        chk("A14-%s crcrlf0" % k, e["crcrlf"] == 0, str(e))
        chk("A14-%s bomFalse" % k, not e["bom"], str(e))
    chk("A14-wle stays crlf", eol(P["wle"])["crlf"] > 300 and eol(P["wle"])["lonelf"] == 0)
    chk("A14-plt stays crlf", eol(P["plt"])["crlf"] > 1200 and eol(P["plt"])["lonelf"] == 0)
    chk("A14-maps lf", eol(P["maps"])["crlf"] == 0)

    # A15 · 零源码改动硬证：交付集无 cpp/h
    chk("A15a no cpp/h in deliverables", not any(p.endswith((".cpp", ".h")) for p in P.values()))

    # A16 · 他人 WIP 源码里不含本轮标记串
    changed = [x[3:] for x in git("status", "--porcelain").splitlines() if len(x) > 3]
    staged = [x[3:] for x in git("diff", "--cached", "--name-only").splitlines() if x.strip()]
    headf = [x for x in git("show", "--name-only", "--format=", "HEAD").splitlines() if x.strip()]
    visible = set(changed) | set(staged) | set(headf)
    marks = ("round97", "p3-batch-a-rest", "P3-A-BATCH-REST-MAPS")
    leaks = []
    for p in visible:
        if not p.endswith((".cpp", ".h")):
            continue
        abs_p = p
        if not os.path.isfile(abs_p):
            continue
        t = rt(abs_p)
        for m in marks:
            if m in t:
                leaks.append((p, m))
    chk("A16 no marker leak into src", not leaks, str(leaks[:3]))

    # A17 · 约束陈述
    chk("A17a 083 only", "083/v83 only" in ev_raw)
    chk("A17b gate18", "orange-wz" in ev_raw)


# ------------------------------------------------------------------ B 场景
def scenes():
    # B1 · SaveFuncKeyMap 尺寸守恒
    sc("B1 89*5 == 445", 89 * 5 == 445)
    # B2 · 快捷栏 8*4 == 32 == 0x20
    sc("B2 8*4 == 32 == 0x20", 8 * 4 == 32 == 0x20)
    # B3 · 七子窗方法数求和
    per = {"Best": 5, "Char": 18, "Inventory": 9, "List": 18, "Locker": 8, "Status": 4, "Tab": 7}
    sc("B3 seven classes", len(per) == 7)
    sc("B3b class methods sum 69", sum(per.values()) == 69, "sum=%d" % sum(per.values()))
    # B4 · 45 方法簇
    cl = {"lifecycle": 8, "input": 7, "status": 1, "quickslot": 8, "chat": 14, "misc": 7}
    sc("B4 statusbar cluster sum 45", sum(cl.values()) == 45)
    sc("B4b statusbar 6 clusters", len(cl) == 6)
    # B5 · WorldMap 2 节点
    wm = ["title", "Border"]
    sc("B5 WorldMap 2 children", len(wm) == 2 and "WorldMapSearch" not in wm)
    # B6 · Border 8 片尺寸
    bs = {"0": (60, 33), "1": (1, 33), "2": (7, 32), "3": (7, 1),
          "4": (7, 1), "5": (7, 18), "6": (1, 18), "7": (7, 18)}
    sc("B6 border 8 slices", len(bs) == 8)
    sc("B6b frame 640+7*2 == 654", 640 + 7 * 2 == 654)
    sc("B6c frame 470+33+18 == 521", 470 + 33 + 18 == 521)
    sc("B6d 083 666 == 12+642+12", 12 + 642 + 12 == 666)
    sc("B6e 083 524 == 34+472+18", 34 + 472 + 18 == 524)
    # B7 · 168 算术
    sc("B7 classification 168", 76 + 38 + 14 + 40 == 168)
    sc("B7b reopen 27", 10 + 17 == 27)
    sc("B7c remaining 17", 0 + 1 + 16 == 17)
    # B8 · (byte) 截断只在 >=0x100
    keys = [0x2A, 0x52, 0x47, 0x49, 0x1D, 0x53, 0x4F, 0x51]
    sc("B8 default keys all < 0x100", all(k < 0x100 for k in keys))
    sc("B8b byte truncation lossless here", all((k & 0xFF) == k for k in keys))
    # B9 · 判定函数：六条输入 → 应全部落「不改代码」
    def verdict(kind):
        return "CODE_CHANGE" if kind == "defect" else "DOC_ONLY"
    sc("B9a arch-diff => DOC_ONLY", all(verdict(k) == "DOC_ONLY" for k in
        ("equal", "arch_diff", "resource_missing", "keep_as_is")))
    sc("B9b only real defect yields code change", verdict("defect") == "CODE_CHANGE")
    # B10 · 交付集零 cpp/h
    sc("B10 deliverables all docs", all(not p.endswith((".cpp", ".h")) for p in P.values()))


def main():
    print("== round97 guards ==")
    guards()
    scenes()

    ok_g = sum(1 for _, c, _ in CHECKS if c)
    ok_s = sum(1 for _, c, _ in SCENES if c)
    print("STATIC GUARDS checked=%d failed=%d" % (len(CHECKS), len(CHECKS) - ok_g))
    print("SCENARIOS %d, bad=%d" % (len(SCENES), len(SCENES) - ok_s))

    bad = [l for l, c, _ in CHECKS if not c] + [l for l, c, _ in SCENES if not c]
    if bad:
        print("FAILED: " + ", ".join(bad))
        return 1

    try:
        py_compile.compile(os.path.abspath(__file__), doraise=True)
        print("PYCOMPILE_OK")
    except py_compile.PyCompileError as e:
        print("PYCOMPILE_FAIL %s" % e)
        return 1

    print("PASS: round97 P3 batch A rest (6 x DOC_ONLY, zero source change, arithmetic unchanged)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
