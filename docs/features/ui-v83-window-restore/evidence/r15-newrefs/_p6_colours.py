# -*- coding: utf-8 -*-
"""R15 pass 6: colour census per screenshot, split into grey vs chromatic, to learn the button palette."""
import io, os, collections
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
NAMES = ["勋章框.png", "好友列表框.png", "组员列表框.png", "拒绝列表框.png",
         "家族联盟.png", "好友、组员、家族、家族联盟、拒绝列表.png", "个人请报框.png", "学院.png"]


def main():
    L = []
    ap = L.append
    for n in NAMES:
        a = np.asarray(Image.open(os.path.join(SRC, n)).convert("RGB"), dtype=np.int16)
        mx = a.max(axis=2); mn = a.min(axis=2)
        chroma = mx - mn
        flat = np.nonzero(chroma >= 30)
        c = collections.Counter(map(tuple, a[flat].tolist()))
        ap("=" * 84)
        ap("%s  chromatic(px>=30) count=%d  distinct=%d" % (n, len(flat[0]), len(c)))
        for col, k in c.most_common(18):
            ap("    RGB%-16s n=%6d  %.2f%%" % (str(col), k, 100.0 * k / len(flat[0])))
        grey = np.nonzero(chroma < 12)
        cg = collections.Counter(map(tuple, a[grey].tolist()))
        ap("  greys (chroma<12) distinct=%d top12: %s" % (len(cg), [(g, k) for g, k in cg.most_common(12)]))
    with io.open(os.path.join(OUT, "_p6_colours.txt"), "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    print("OK")


if __name__ == "__main__":
    main()
