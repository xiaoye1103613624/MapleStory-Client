import io
import numpy as np
from PIL import Image

dump = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"
shot = r"E:/MXD/BeiDou-Client-xy/_seik_all.png"

A = np.asarray(Image.open(shot).convert("RGB")).astype(np.int32)
H, W = A.shape[:2]
out = ["shot %dx%d" % (W, H)]

templates = {
    "Stat": "stat_backgrnd.png",
    "Equip": "equip_backgrnd.png",
    "Skill": "skill_backgrnd.png",
    "Item": "item_backgrnd.png",
}

for name, tpl in templates.items():
    rgba = np.asarray(Image.open(dump + tpl).convert("RGBA"))
    T = rgba[:, :, :3].astype(np.int32)
    alpha = rgba[:, :, 3]
    th, tw = T.shape[:2]

    # 采样模板中不透明的像素点（最多 60 个）
    ys, xs = np.where(alpha > 200)
    idx = np.linspace(0, len(ys) - 1, 60).astype(int)
    pts = list(zip(ys[idx], xs[idx]))

    acc = np.zeros((H - th + 1, W - tw + 1), dtype=np.int32)
    for py, px in pts:
        sub = A[py:py + (H - th + 1), px:px + (W - tw + 1)]
        acc += np.abs(sub - T[py, px]).sum(axis=2)

    acc = acc / float(len(pts))
    minv = acc.min()
    my, mx = np.unravel_index(acc.argmin(), acc.shape)
    out.append("%s min_diff=%.2f at (%d,%d) size=%dx%d  [match if < 25]" % (name, minv, mx, my, tw, th))
    # 次优值（第二小区域）用于判断是否唯一
    flat = acc.copy()
    flat[max(0, my - 20):my + 20, max(0, mx - 20):mx + 20] = 10 ** 9
    out.append("    next_best=%.2f at %s" % (flat.min(), np.unravel_index(flat.argmin(), flat.shape)))

with io.open(dump + "_verify_open.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
