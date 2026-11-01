# -*- coding: utf-8 -*-
"""R15: parse the nx_inspect tree dump into full paths + bitmap sizes, then list frame candidates."""
import io, os, re, sys

OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
SRC = os.path.join(OUT, "_uiwin_full.txt")
import subprocess

def parse(text):
    """yield (fullpath, type, rest) for each node line of an nx_inspect `list` dump."""
    stack = {}
    for line in text.splitlines():
        if line.startswith("====="):
            continue
        m = re.match(r"^(\s*)(\S+) \[(\w+)\](.*)$", line)
        if not m:
            continue
        ind, name, typ, rest = m.groups()
        lvl = len(ind) // 2
        stack[lvl] = name
        for k in list(stack):
            if k > lvl:
                del stack[k]
        full = "/".join(stack[i] for i in range(0, lvl + 1))
        yield full, typ, rest.strip()


def main():
    text = io.open(SRC, encoding="utf-8").read()
    rows = list(parse(text))
    L = []
    ap = L.append
    ap("total parsed nodes: %d" % len(rows))
    ap("")
    ap("### nodes named backgrnd* that are bitmaps")
    for full, typ, rest in rows:
        base = full.split("/")[-1]
        if base.startswith("backgrnd") and "bitmap" in rest:
            ap("  %-72s %s" % (full, rest))
    ap("")
    ap("### ALL bitmaps with 200<=W<=340 and 300<=H<=410 (possible window frames)")
    for full, typ, rest in rows:
        m = re.search(r"bitmap (\d+)x(\d+)", rest)
        if m:
            w, h = int(m.group(1)), int(m.group(2))
            if 200 <= w <= 340 and 300 <= h <= 410:
                ap("  %-72s %dx%d" % (full, w, h))
    with io.open(os.path.join(OUT, "_p4_nxframes.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("OK %d rows" % len(rows))


if __name__ == "__main__":
    main()
