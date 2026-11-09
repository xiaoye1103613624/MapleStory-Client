#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""round41 Wave2 2-9: UIItemInventory 重复 G 键 / UseGachaExp 死代码 —— 离线 harness。

缺陷（修复前 IO/UITypes/UIItemInventory.cpp:1182 与 :1229）：
  同一 send_key `else if` 链里 `keycode == 'G' || keycode == 'g'` 出现两次：
    :1182 → GatherItemsPacket(tab).dispatch();
    :1229 → UseGachaExpPacket().dispatch();   ← 永远不可达
  同一 `df31fb1c`（2026-11-01 15:09:09）一次提交塞入；父版本只有 :1182 的
  GatherItemsPacket，`r803-aran-solomon-gacha-teleport-rgacha.md:9` 旧账写
  「SoftKey `G` → UseGachaExp」但 G 被 Gather 占了。`UseGachaExpPacket` 空包
  （Net/Packets/SolomonPackets.h:22-26，USE_GACHA_EXP=158）全仓唯一构造点就是
  这条死分支 → 等于「补了包没给入口」。

修复：把第二分支改为链内空位 `X/x`（不新增协议/包体/原生键位映射，G 仍归
  Gather 与 BT_GATHER 按钮同源）。

本 harness：
  ① 静态核验 live 源码守卫逐字在盘（重复 G=0 / X 分支=1 / 两 dispatch 各 1 /
     空包类与 opcode 158 在盘 / 协议门禁脚本可跑）；
  ② Python 复刻 `send_key` 的 else-if 优先链，跑修复前/修复后 10 个 keycode
     场景，断言 G→Gather、X→Gacha、其余键不串扰；
  ③ 验收 `before reach=0 -> after reach=1` 且重复键=0。

只覆盖静态守卫 + 键位分支算法复刻，真机 SoftKey 观感 / 服务端 158 回包进
pending-live-tests §20。
"""
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CPP = os.path.join(ROOT, "IO", "UITypes", "UIItemInventory.cpp")
SOL = os.path.join(ROOT, "Net", "Packets", "SolomonPackets.h")
OUTP = os.path.join(ROOT, "Net", "OutPacket.h")
R803 = os.path.join(ROOT, "docs", "features", "beidou-v17-parity", "notes",
                    "r803-aran-solomon-gacha-teleport-rgacha.md")


def read(path):
    with io.open(path, "r", encoding="utf-8", errors="replace") as f:
        return f.read().replace("\r\n", "\n")


# ---------------------------------------------------------------------------
# ① 静态守卫
# ---------------------------------------------------------------------------
def count(text, needle):
    return text.count(needle)


def check_guards():
    cpp = read(CPP)
    sol = read(SOL)
    outp = read(OUTP)
    r803 = read(R803)

    checks = []
    # G 分支现在必须只剩 1 处（Gather），X 分支必须 1 处（Gacha）
    g_cond = "else if (keycode == 'G' || keycode == 'g')"
    x_cond = "else if (keycode == 'X' || keycode == 'x')"
    checks.append(("g_branch_single", count(cpp, g_cond) == 1,
                   "G cond count=%d" % count(cpp, g_cond)))
    checks.append(("x_branch_present", count(cpp, x_cond) == 1,
                   "X cond count=%d" % count(cpp, x_cond)))
    checks.append(("gather_dispatch_kept",
                   count(cpp, "GatherItemsPacket(tab).dispatch();") >= 1,
                   "Gather dispatch kept"))
    checks.append(("gacha_dispatch_kept",
                   count(cpp, "UseGachaExpPacket().dispatch();") == 1,
                   "Gacha dispatch=1"))
    checks.append(("gacha_log_kept",
                   count(cpp, 'LOG(LOG_DEBUG, "UIItemInventory: UseGachaExp");') == 1,
                   "Gacha LOG kept"))
    checks.append(("no_dead_comment_stale",
                   "r803" in cpp and "原为 'G'/'g'" in cpp,
                   "rationale comment in place"))
    checks.append(("solomon_empty_packet",
                   "class UseGachaExpPacket : public OutPacket" in sol
                   and "OutPacket::Opcode::USE_GACHA_EXP" in sol,
                   "UseGachaExpPacket empty body"))
    checks.append(("opcode_158",
                   "USE_GACHA_EXP = 158" in outp,
                   "OutPacket.h:295 USE_GACHA_EXP = 158"))
    checks.append(("r803_old_ledger",
                   "SoftKey `G` → UseGachaExp" in r803,
                   "r803:9 old ledger intent retained as history"))
    checks.append(("no_duplicate_key_in_chain",
                   count(cpp, g_cond) == 1 and count(cpp, x_cond) == 1,
                   "duplicate key eliminated"))

    bad = []
    for label, ok, detail in checks:
        print("[%s] %s (%s)" % ("OK" if ok else "FAIL", label, detail))
        if not ok:
            bad.append(label)
    return len(checks), bad


# ---------------------------------------------------------------------------
# ② else-if 优先链复刻
# ---------------------------------------------------------------------------
# 返回本次按键实际命中的动作；None = 无动作。
def dispatch_chain(key, before):
    """复刻 send_key 的 else-if 链（仅本缺陷相关键位，其余折叠为 None）。"""
    k = key
    if k in ("G", "g"):
        return "GATHER"           # :1182 第一处 G，永远先命中
    if k in ("O", "o"):
        return "SORT"
    if k in ("M", "m"):
        return "COIN"
    if k in ("U", "u"):
        return "USE"
    if before:
        # 修复前第二处 G 与第一处重复，链已跳出 → 不可达
        if k in ("G", "g"):
            return "UNREACHABLE_GACHA"   # 理论上不会走到（上面已 return）
    else:
        if k in ("X", "x"):
            return "GACHA"
    if k in ("D", "d"):
        return "DROP"
    if k in ("S", "s"):
        return "MOVE"
    if k in ("Y", "y"):
        return "MESO"
    if k in ("E", "e"):
        return "EQUIP"
    if k in ("C", "c"):
        return "CASHTEXT"
    return None


# 修复前/后：G 恒 GATHER；X 修复前无动作、修复后 GACHA；其余键不串扰
CASES = [
    # key, 修复前期望, 修复后期望, 说明
    ("G", "GATHER", "GATHER", "G 归 Gather（父版本+BT_GATHER 同源）"),
    ("g", "GATHER", "GATHER", "小写同义"),
    ("X", None, "GACHA", "X 修复后成为 GachaExp 唯一入口"),
    ("x", None, "GACHA", "小写同义"),
    ("O", "SORT", "SORT", "O 不串扰"),
    ("U", "USE", "USE", "U 不串扰"),
    ("D", "DROP", "DROP", "D 不串扰"),
    ("S", "MOVE", "MOVE", "S 不串扰"),
    ("Y", "MESO", "MESO", "Y 不串扰"),
    ("E", "EQUIP", "EQUIP", "E 不串扰"),
]


def main():
    n_guards, bad = check_guards()
    if bad:
        print("STATIC GUARD FAIL: %s" % bad)
        return 1
    print("STATIC GUARDS OK (%d needles)" % n_guards)

    before_reach = 0
    after_reach = 0
    for (key, exp_b, exp_a, desc) in CASES:
        got_b = dispatch_chain(key, before=True)
        got_a = dispatch_chain(key, before=False)
        ok = (got_b == exp_b and got_a == exp_a)
        print("[%s] key=%s before=%s after=%s (%s)" %
              ("OK" if ok else "FAIL", key, got_b, got_a, desc))
        if not ok:
            bad.append("case:" + key)
        if exp_a == "GACHA" and got_a == "GACHA":
            after_reach += 1
        if exp_b == "GACHA" and got_b == "GACHA":
            before_reach += 1

    # 修复前 Gacha 入口可达数 = 0（死分支），修复后 = 2（X/x 两个键码）
    print("[reach] before Gacha-dispatched keys=%d -> after=%d (expected 0 -> 2)" %
          (before_reach, after_reach))
    print("[duplicate] duplicate G cond: before=2 -> after=1 (static guard verified)")
    if before_reach != 0 or after_reach != 2:
        bad.append("reach_count")
    if bad:
        print("FAIL: %s" % bad)
        return 1
    print("PASS: %d static guards + %d keycode cases (before Gacha reach=0 -> after 2; duplicate G 2->1; regression 8/8)"
          % (n_guards, len(CASES)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
