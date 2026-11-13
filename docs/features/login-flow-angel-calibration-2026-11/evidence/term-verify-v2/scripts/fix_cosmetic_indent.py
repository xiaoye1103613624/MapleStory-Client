# -*- coding: utf-8 -*-
"""提交前 cosmetic：补收窄补丁漏掉的前导缩进（C++ 空白不敏感，纯可读性）。

patch_sidebar2.py 的 locate() 把 anchor 的前导 tab 算进 anchor 导致 lead="",
新插入行没缩进。两处：
  1. UICharSelect.h 枚举 BtHome, 无 3-tab 前缀（邻行 BtPreview/CHARACTER_SLOT0 都是 3 tab）
  2. UICharSelect.cpp case BtHome: 块 14 行整体比邻行浅 3 tab
统一给这些行前置 \t\t\t；保 CRLF/LF 原样；带断言，零副作用。
"""
import os

ROOT = r"E:\project\MapleStory-Client"
H = os.path.join(ROOT, "IO", "UITypes", "UICharSelect.h")
C = os.path.join(ROOT, "IO", "UITypes", "UICharSelect.cpp")


def load(p):
    with open(p, "r", encoding="utf-8", newline="") as f:
        raw = f.read()
    crlf = "\r\n" in raw
    return raw, crlf, raw.replace("\r\n", "\n")


def save(p, norm, crlf):
    out = norm.replace("\n", "\r\n") if crlf else norm
    with open(p, "w", encoding="utf-8", newline="") as f:
        f.write(out)


def main():
    # ---- .h : 给无缩进的 BtHome 行补 3 tab ----
    raw, crlf, n = load(H)
    assert n.count("\nBtHome,") == 1, "h: expect exactly one bare BtHome line"
    n = n.replace("\nBtHome,", "\n\t\t\tBtHome,")
    save(H, n, crlf)
    print("FIXED .h BtHome indent")

    # ---- .cpp : case BtHome: 块整段 +3 tab ----
    raw, crlf, n = load(C)
    lines = n.split("\n")
    si = [i for i, l in enumerate(lines) if l == "case Buttons::BtHome:"]
    assert len(si) == 1, "cpp: expect one bare case Buttons::BtHome:, got %d" % len(si)
    si = si[0]
    ei = [i for i, l in enumerate(lines)
          if i > si and l == "\t\t\tcase Buttons::BtCharacter:"]
    assert len(ei) == 1, "cpp: expect one 3-tab BtCharacter after, got %d" % len(ei)
    ei = ei[0]
    # 块 = [si, ei) 的所有行；逐行前置 3 tab（case/{/}/body/nested 一并）
    assert ei - si == 14, "cpp: block line count %d != 14" % (ei - si)
    for i in range(si, ei):
        lines[i] = "\t\t\t" + lines[i]
    save(C, "\n".join(lines), crlf)
    print("FIXED .cpp case BtHome block %d lines (+3 tab each)" % (ei - si))


if __name__ == "__main__":
    main()
