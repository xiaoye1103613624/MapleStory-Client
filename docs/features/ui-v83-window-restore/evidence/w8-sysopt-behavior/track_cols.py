# -*- coding: utf-8 -*-
"""沿每条滑轨的中线打印逐列颜色，人工找滑钮的色块边界。"""
import os
from PIL import Image

SRC = os.path.join(os.environ["USERPROFILE"], "Desktop", "\u6b63\u786e\u5ba2\u6237\u7aef\u622a\u56fe", "\u7cfb\u7edf\u8bbe\u7f6e.png")
im = Image.open(SRC).convert("RGB")
px = im.load()

rows = [("quality", 26, 56), ("mouse", 146, 176), ("hp_warn", 176, 206), ("mp_warn", 206, 236)]

with open(r"E:\project\_r28\nat_track_cols.txt", "w", encoding="utf-8") as f:
    for name, y0, y1 in rows:
        # 该行里找「最不像背景」的那一列扫描线：先取轨道行的中点
        ymid = (y0 + y1) // 2
        f.write("== %s band=%d..%d scan y=%d\n" % (name, y0, y1, ymid))
        # 逐列：把颜色量化成 32 的桶，压缩输出
        seq = []
        for x in range(88, 258):
            r, g, b = px[x, ymid]
            seq.append("%d:%02x%02x%02x" % (x, r, g, b))
        f.write(" ".join(seq) + "\n")
        # 再扫 ymid-3 .. ymid+3，看滑钮的竖直范围
        for dy in (-4, -2, 0, 2, 4):
            y = ymid + dy
            runs = []
            start = None
            for x in range(88, 258):
                r, g, b = px[x, y]
                knob = (b > r + 12) or (r < 170 and g < 170 and b < 200 and b >= r)
                if knob and start is None:
                    start = x
                elif not knob and start is not None:
                    if x - start >= 4:
                        runs.append((start, x - 1, x - start))
                    start = None
            if start is not None:
                runs.append((start, 257, 258 - start))
            f.write("  y=%d runs=%s\n" % (y, runs))
print("done")
