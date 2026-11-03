"""B7-4 oracle：原生 游戏设置 / 系统设置 逐格读数（勾选框 / 滑条 / 下拉 / 底部按钮）。

判据 = 逐格模板 SAD（alpha>200 掩膜），命中去重后写 _settings_map2.txt。
全部数值来自测量，不手抄；原生帧与画布的平移差也一并测出。
"""
import os

import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
DUMP = os.path.join(HERE, "_dump")

# (原生截图, 画布节点导出, 画布相对原生的平移 dx,dy 由脚本测出)
FRAMES = (
    ("native_gameopt.png", "GameOpt_backgrnd.png", "游戏设置"),
    ("native_sysopt.png", "SysOpt_backgrnd.png", "系统设置"),
)

TEMPLATES = (
    ("btn_BtOK2.png", "Basic.img/BtOK2/normal/0 (47x19 橙·确定)"),
    ("btn_BtCancel2.png", "Basic.img/BtCancel2/normal/0 (47x19 橙·取消)"),
    ("btn_BtDecide.png", "Basic.img/BtDecide/normal/0 (44x17)"),
    ("cb0.png", "Basic.img/CheckBox/0 (12x12 未勾)"),
    ("cb1.png", "Basic.img/CheckBox/1 (12x12 已勾)"),
    ("sl_thumb.png", "Basic.img/Slider/thumbNormal (22x14)"),
    ("cb_arrow.png", "Basic.img/ComboBox/normal/2 (18x17 下拉箭头)"),
)


def rgb(path):
    return np.array(Image.open(path).convert("RGB")).astype(np.int16)


def rgba(path):
    a = np.array(Image.open(path).convert("RGBA")).astype(np.int16)
    return a[:, :, :3], a[:, :, 3]


def align(native, canvas, rng=3):
    """原生帧相对画布的平移：native(x,y)=canvas(x+dx,y+dy)。"""
    best = None
    for dy in range(-rng, rng + 1):
        for dx in range(-rng, rng + 1):
            ys0, ys1 = max(0, dy), min(native.shape[0], canvas.shape[0] - max(0, -dy))
            xs0, xs1 = max(0, dx), min(native.shape[1], canvas.shape[1] - max(0, -dx))
            n = native[ys0:ys1, xs0:xs1]
            c = canvas[ys0 + dy:ys1 + dy, xs0 + dx:xs1 + dx]
            if n.shape != c.shape:
                continue
            score = float(np.abs(n - c).mean())
            if best is None or score < best[0]:
                best = (score, dx, dy)
    return best


def match(v, tpl_rgb, tpl_al, amin=200, thresh=2.0):
    """返回所有 SAD<thresh 的命中（含同一条边框 AA 的相邻位移）。

    不能截 top-N：本图的等价命中全为 SAD=0.00，排序后并列项会被行序截断，
    表现为「只命中第一行」的假阴 —— 全部保留，交给 dedupe 聚合。
    """
    mask = tpl_al > amin
    h, w = tpl_rgb.shape[:2]
    hits = []
    best = (1e9, None, None)
    for oy in range(0, v.shape[0] - h + 1):
        for ox in range(0, v.shape[1] - w + 1):
            score = float(np.abs(v[oy:oy + h, ox:ox + w][mask] - tpl_rgb[mask]).mean())
            if score < best[0]:
                best = (score, ox, oy)
            if score < thresh:
                hits.append((score, ox, oy))
    hits.sort()
    return hits, best


def dedupe(hits, gap=3):
    """非极大值抑制：按分数升序，命中若已落在某个代表点 gap 邻域内则丢弃。

    不能拿「上一枚命中」当锚点比对 —— 同一边框的 AA 位移会逐格 1px 链接下去，
    把相隔 18px 的 12 个勾选框串成 1 组，表现为「只命中第一行」的假阴。
    """
    keep = []
    for score, ox, oy in sorted(hits):
        if any(abs(kx - ox) <= gap and abs(ky - oy) <= gap for _, kx, ky in keep):
            continue
        keep.append((score, ox, oy))
    return sorted(keep, key=lambda h: (h[2], h[1]))


lines = []

for native_name, canvas_name, cn in FRAMES:
    native = rgb(os.path.join(DUMP, native_name))
    canvas = rgb(os.path.join(DUMP, canvas_name))
    sad, dx, dy = align(native, canvas)
    lines.append("== %s  原生 %dx%d / 画布 %dx%d  平移(原生→画布) dx=%d dy=%d SAD=%.2f"
                 % (cn, native.shape[1], native.shape[0],
                    canvas.shape[1], canvas.shape[0], dx, dy, sad))

    for fn, label in TEMPLATES:
        tpl_rgb, tpl_al = rgba(os.path.join(DUMP, fn))
        hits, best = match(native, tpl_rgb, tpl_al)
        keep = dedupe(hits)
        lines.append("  %-52s SAD<2 命中 %d 处" % (label, len(keep)))
        for score, ox, oy in keep:
            lines.append("      原生(%3d,%3d) 画布(%3d,%3d) SAD=%.2f"
                         % (ox, oy, ox + dx, oy + dy, score))
        if not keep:
            lines.append("      （无 SAD<2 命中；最佳 SAD=%.2f @ %s）" % (best[0], (best[1], best[2])))

with open(os.path.join(HERE, "_settings_map2.txt"), "w", encoding="utf-8") as f:
    f.write("\n".join(lines) + "\n")

print("wrote _settings_map2.txt")
