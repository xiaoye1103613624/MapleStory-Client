"""UINpcTalk::strip_simple_choices 的 Python 同构镜像 + 真实形态自证。

为什么存在：#L 令牌的剥除是纯字符串算法，错一位就会把半截令牌漏进共享布局器
（表现为满屏乱码块），而这类缺陷在真机上很难定点复现。所以先离线跑全服脚本语料
和手工拼装的真实形态，再上真机。

用法：python npctalk_strip_check.py [scripts_root]
输出：stdout（ASCII 安全，重定向到文件时用 UTF-8）
"""
import io
import os
import re
import sys

DEFAULT_ROOT = r"E:/project/BeiDou-Server_s9/gms-server/scripts-zh-CN"


def stoi(s):
    """模拟 std::stoi：跳过前导空白、可选符号，必须至少一位数字，否则抛。"""
    i, n = 0, len(s)
    while i < n and s[i] in " \t\r\n":
        i += 1
    sign = 1
    if i < n and s[i] in "+-":
        if s[i] == "-":
            sign = -1
        i += 1
    j = i
    while j < n and s[j].isdigit():
        j += 1
    if j == i:
        raise ValueError("no digits")
    return sign * int(s[i:j])


def strip(tx):
    """与 C++ strip_simple_choices 同构：返回 (剥净正文, [(sel, first, last, label)], [畸形令牌])."""
    out = []
    choices = []
    malformed = []
    pos = 0
    while True:
        l = tx.find("#L", pos)
        if l == -1:
            out.append(tx[pos:])
            break
        num_end = tx.find("#", l + 2)
        if num_end == -1:
            out.append(tx[pos:])
            break
        try:
            sel = stoi(tx[l + 2:num_end])
        except ValueError:
            out.append(tx[pos:num_end + 1])
            malformed.append(tx[l:num_end + 1])
            pos = num_end + 1
            continue

        label_begin = num_end + 1
        label_end = tx.find("#l", label_begin)
        next_l = tx.find("#L", label_begin)
        terminated = label_end != -1 and (next_l == -1 or label_end < next_l)
        if not terminated:
            label_end = len(tx)
            if next_l != -1 and next_l < label_end:
                label_end = next_l
            cands = [i for i in (tx.find("\r", label_begin), tx.find("\n", label_begin)) if i != -1]
            brk = min(cands) if cands else -1
            if brk != -1 and brk < label_end:
                label_end = brk

        out.append(tx[pos:l])
        if label_end > label_begin:
            first = len("".join(out))
            body = tx[label_begin:label_end]
            out.append(body)
            choices.append((sel, first, len("".join(out)), body))
        pos = label_end + 2 if terminated else label_end
    return "".join(out), choices, malformed


# expect: "clean" = 令牌必须剥净；"verbatim" = 结构本身残缺，须原样保留正文且不吞字
CASES = [
    ("canonical 行内三列 (9900001)",
     "#L3#传送自由#l \t #L69#快速转职#l \t #L70#学习技能#l\r\n#L5#野外BOSS刷新#l\r\n", "clean"),
    ("无闭合 #l，逐行一项 (1012100)",
     "好的，当你做出决定后#b\r\n#L0#请解释一下成为猎人的意义。\r\n#L1#请解释一下成为弩弓手的意义。\r\n#L3#我会选择我的职业！", "clean"),
    ("变量拼装 (1002007 实际形态)",
     "请选择：\r\n#L0##m100000000# (10 金币)#l\r\n#L1##m103000000# (25 金币)#l", "clean"),
    ("标签内嵌色标 (当前地图掉落)",
     "#L1##r战士武器#k\r\n#L2##r法师武器#k\r\n#L12##r关闭#k", "clean"),
    ("#b 铺在令牌前 (1022000)",
     "请选择你的转职方向#b\r\n#L0#剑客\r\n#L1#准骑士\r\n#L2#枪战士", "clean"),
    ("标签内嵌道具名 #t/#i (1012103)",
     "#L1#理发：#i5150001##t5150001#\r\n#L2#染发：#i5151001##t5151001#", "clean"),
    ("畸形：#L 后无数字", "正文#Lx#坏令牌#l尾巴", "verbatim"),
    ("畸形：#L 悬在末尾", "正文\r\n#L", "verbatim"),
    ("畸形：只有 #L0# 无标签", "#L0##l#L1#正常#l", "clean"),
    ("单 #l 缺失且整段一行", "#L0#第一项 #L1#第二项 #L2#第三项", "clean"),
]

LITERAL = re.compile(r'"((?:[^"\\\n]|\\.)*)"')


def unesc(lit):
    return (lit.replace("\\r", "\r").replace("\\n", "\n").replace("\\t", "\t")
             .replace('\\"', '"').replace("\\\\", "\\"))


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_ROOT
    w = sys.stdout.write

    w("=== 真实形态终判 ===\n")
    failed = 0
    for name, tx, expect in CASES:
        st, ch, mf = strip(tx)
        bad = []
        for sel, f, la, body in ch:
            if st[f:la] != body:
                bad.append("slice_mismatch sel=%d" % sel)
            if "\r" in body or "\n" in body:
                bad.append("break_in_label sel=%d" % sel)
        if expect == "clean":
            if "#L" in st:
                bad.append("leftover_token")
            if "#l" in st:
                bad.append("leftover_close_token")
        else:
            # 结构残缺时唯一的硬要求：正文一个字都不丢
            if st != tx:
                bad.append("body_mutated")
            if ch:
                bad.append("phantom_choice")
        status = "PASS" if not bad else "FAIL " + ",".join(bad)
        if bad:
            failed += 1
        w("%-28s [%s] choices=%-3d %s\n" % (name, expect, len(ch), status))
        for sel, f, la, body in ch:
            w("    sel=%-4d [%3d,%3d) %r\n" % (sel, f, la, body))

    w("\n=== 全服语料 ===\n")
    files = lits = choices = leftover = malformed = 0
    break_rows = []
    for dirpath, _, names in os.walk(root):
        for n in names:
            if not n.endswith(".js"):
                continue
            try:
                src = io.open(os.path.join(dirpath, n), encoding="utf-8", errors="replace").read()
            except Exception:
                continue
            files += 1
            for m in LITERAL.finditer(src):
                lit = m.group(1)
                if "#L" not in lit:
                    continue
                lits += 1
                st, ch, mf = strip(unesc(lit))
                choices += len(ch)
                malformed += len(mf)
                if "#L" in st:
                    leftover += 1
                for sel, _, _, body in ch:
                    if "\r" in body or "\n" in body:
                        break_rows.append("%s sel=%d %r" % (n, sel, body))

    w("js files=%d  literals with #L=%d  choices parsed=%d\n" % (files, lits, choices))
    w("leftover-literals=%d (绝大多数是 \"#L\" + i + \"#…\" 变量拼接把令牌切碎，属扫描器切分口径)\n" % leftover)
    w("malformed tokens=%d  labels swallowing a line break=%d\n" % (malformed, len(break_rows)))
    for r in break_rows[:10]:
        w("  " + r + "\n")
    w("\nFAILING CASES: %d\n" % failed)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
