import io
import numpy as np
from PIL import Image

dump = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"

shots = {
    "stat": r"E:/MXD/BeiDou-Client-xy/_seik_stat.png",
    "equip": r"E:/MXD/BeiDou-Client-xy/_seik_equip.png",
    "skill": r"E:/MXD/BeiDou-Client-xy/_seik_skill.png",
    "item": r"E:/MXD/BeiDou-Client-xy/_seik_item.png",
}

templates = {
    "Stat": "stat_backgrnd.png",
    "Equip": "equip_backgrnd.png",
    "Skill": "skill_backgrnd.png",
    "Item": "item_backgrnd.png",
}

out = []
for sname, spath in shots.items():
    A = np.asarray(Image.open(spath).convert("RGB")).astype(np.int32)
    H, W = A.shape[:2]
    out.append("=== shot %s %dx%d" % (sname, W, H))
    for tname, tpl in templates.items():
        rgba = np.asarray(Image.open(dump + tpl).convert("RGBA"))
        T = rgba[:, :, :3].astype(np.int32)
        alpha = rgba[:, :, 3]
        th, tw = T.shape[:2]
        ys, xs = np.where(alpha > 200)
        idx = np.linspace(0, len(ys) - 1, 60).astype(int)
        pts = list(zip(ys[idx], xs[idx]))
        acc = np.zeros((H - th + 1, W - tw + 1), dtype=np.int32)
        for py, px in pts:
            sub = A[py:py + (H - th + 1), px:px + (W - tw + 1)]
            acc += np.abs(sub - T[py, px]).sum(axis=2)
        acc = acc / float(len(pts))
        my, mx = np.unravel_index(acc.argmin(), acc.shape)
        out.append("   tpl %-6s min_diff=%7.2f at (%d,%d)" % (tname, acc.min(), mx, my))

with io.open(dump + "_verify_open2.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
