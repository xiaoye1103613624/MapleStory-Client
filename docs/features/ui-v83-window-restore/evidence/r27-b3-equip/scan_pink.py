"""在 UI.nx 全量位图里按「粉色精确色」找素材，判据不是平坦度而是逐像素色。

粉色两条（来自 r27-oracle/Equip.json 的原生帧实测）：
  pink1 = RGB(255,187,187)  底行方格
  pink2 = RGB(204,153,170)  底行宽格
对照组（证明检测器有效）：
  cool1 = RGB(187,204,221)  我们 backgrnd 底行的冷色
"""
import sys
import numpy as np

sys.path.insert(0, "tools")
from nx_inspect import Nx

NX = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
TARGETS = {
    "pink1_255_187_187": (255, 187, 187),
    "pink2_204_153_170": (204, 153, 170),
    "cool1_187_204_221": (187, 204, 221),
}
TOL = 8
MIN_PIX = 60

nx = Nx(NX)
hits = {k: [] for k in TARGETS}
scanned = 0
for i in range(nx.node_count):
    _, _, _, ntype, _, w, h = nx.node(i)
    if ntype != 5 or w == 0 or h == 0:
        continue
    scanned += 1
    _, _, raw = nx.bitmap_pixels(i)
    px = np.frombuffer(raw, dtype=np.uint8).reshape(h, w, 4)
    bgra = px[:, :, :3][:, :, ::-1]  # -> RGB
    for name, (r, g, b) in TARGETS.items():
        m = (np.abs(bgra[:, :, 0].astype(int) - r) <= TOL) & \
            (np.abs(bgra[:, :, 1].astype(int) - g) <= TOL) & \
            (np.abs(bgra[:, :, 2].astype(int) - b) <= TOL) & \
            (px[:, :, 3] > 0)
        c = int(m.sum())
        if c >= MIN_PIX:
            hits[name].append((c, i, w, h))

lines = ["scanned bitmaps: %d  (node_count=%d)  tol=%d min_pix=%d" % (scanned, nx.node_count, TOL, MIN_PIX)]
for name in TARGETS:
    lst = sorted(hits[name], reverse=True)
    lines.append("")
    lines.append("== %s : %d hit(s)" % (name, len(lst)))
    for c, i, w, h in lst[:40]:
        try:
            p = nx.path_of(i)
        except Exception as e:
            p = "<path_of failed: %s>" % e
        lines.append("   n=%-7d %4dx%-4d  %s" % (c, w, h, p))

with open("E:/project/_b3/pink_scan.txt", "w", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
print("done")
