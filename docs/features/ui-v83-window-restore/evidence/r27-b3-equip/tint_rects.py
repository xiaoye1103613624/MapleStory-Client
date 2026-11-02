"""整窗扫描「被机械调色的矩形」清单。

判据：对素材不透明的每个像素，预测 pred = snap17(0.75*R+68.25, 0.75*G, 0.75*B)，
若帧色 == pred（tol=1）则该像素属于「调色区」。装备图标等叠绘内容不会满足这个
逐像素等式，因此不会被误计成调色区；反之，纯平涂（不看素材）也满足不了 —— 平涂
会让同一格内不同素材色映射到同一帧色，pred 命中率会掉下去。
"""
import numpy as np
from PIL import Image

FRAME = "C:/Users/11036/Desktop/正确客户端截图/角色装备.png"
ARTS = [
    ("backgrnd", "docs/features/ui-v83-window-restore/evidence/r27-b3-equip/Equip_backgrnd.png", (0, 0)),
    ("pet", "docs/features/ui-v83-window-restore/evidence/r27-b3-equip/Equip_pet.png", (171, 123)),
    ("FullBackgrnd", "docs/features/ui-v83-window-restore/evidence/r27-b3-equip/Equip_FullBackgrnd.png", (0, 0)),
]
S, K = 0.75, 68.25
OUT = "E:/project/_b3/tint_rects.txt"

fr = np.asarray(Image.open(FRAME).convert("RGB"), dtype=np.int16)
lines = ["frame=%dx%d  pred = snap17(%.2f*R+%.2f, %.2f*G, %.2f*B)" % (fr.shape[1], fr.shape[0], S, K, S, S)]

grid = np.arange(0, 256, 17)


def snap(v):
    return np.abs(grid[None, :] - v[:, None]).argmin(axis=1) * 17


for tag, path, (ox, oy) in ARTS:
    ar = np.asarray(Image.open(path).convert("RGBA"), dtype=np.int16)
    h, w = ar.shape[:2]
    if ox + w > fr.shape[1] or oy + h > fr.shape[0]:
        lines.append("\n== %s 跳过：帧放不下 (%d,%d)+%dx%d" % (tag, ox, oy, w, h))
        continue
    a = ar[:, :, :3]
    al = ar[:, :, 3] > 0
    f = fr[oy:oy + h, ox:ox + w]
    pred = np.empty_like(f)
    for c, off in enumerate((K, 0.0, 0.0)):
        v = np.clip(S * a[:, :, c] + off, 0, 255)
        pred[:, :, c] = snap(v.reshape(-1)).reshape(h, w)
    hit = (np.abs(pred - f).max(axis=2) <= 1) & al
    opaque = al.sum()
    lines.append("\n== %s @(%d,%d) %dx%d  不透明像素=%d  命中 pred=%d (%.2f%%)" % (
        tag, ox, oy, w, h, opaque, hit.sum(), 100.0 * hit.sum() / max(1, opaque)))
    # 连通矩形：先按行聚合，再按列聚合
    rows = np.where(hit.any(axis=1))[0]
    if len(rows) == 0:
        continue
    segs = []
    start = rows[0]
    prev = rows[0]
    for r in rows[1:]:
        if r > prev + 2:
            segs.append((start, prev))
            start = r
        prev = r
    segs.append((start, prev))
    for (r0, r1) in segs:
        band = hit[r0:r1 + 1]
        cols = np.where(band.any(axis=0))[0]
        csegs = []
        cs = cols[0]
        cp = cols[0]
        for c in cols[1:]:
            if c > cp + 2:
                csegs.append((cs, cp))
                cs = c
            cp = c
        csegs.append((cs, cp))
        for (c0, c1) in csegs:
            n = int(hit[r0:r1 + 1, c0:c1 + 1].sum())
            area = (r1 - r0 + 1) * (c1 - c0 + 1)
            lines.append("   矩形 帧坐标 x=%d..%d y=%d..%d  (%dx%d)  pred命中=%d/%d=%.0f%%" % (
                ox + c0, ox + c1, oy + r0, oy + r1, c1 - c0 + 1, r1 - r0 + 1, n, area, 100.0 * n / area))

with open(OUT, "w", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
print("written " + OUT)
