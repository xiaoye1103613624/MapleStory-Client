"""装备窗底行：原生帧 vs 同一张 backgrnd 素材，逐格求「代码做了什么操作」。

三条独立判据：
  A 区域限定性：整行逐像素 Δ(帧−素材)，看 Δ≠0 的连通域是不是恰好那两格。
    整张图统一染色 => 全有 Δ；只画矩形 => Δ 只出现在格内。
  B 映射单值性：格内 素材色 -> 帧色 是否多对一/一对一（机械调色 or 画死色块）。
    若是「不透明纯色块」，格内帧色应只有 1~2 种；实测帧色种类数决定这一点。
  C 仿射可解性：帧 = a*素材 + b（逐通道最小二乘，只在格内拟合）。
    a≈1,b≈0 => 无操作；a<1,b>0 => alpha 混合一个纯色；a≈1,b≠0 => 加性偏移。
"""
import sys
import numpy as np
from PIL import Image

FRAME = "C:/Users/11036/Desktop/正确客户端截图/角色装备.png"
ART = "docs/features/ui-v83-window-restore/evidence/r27-b3-equip/Equip_backgrnd.png"
# 来自 r27-oracle/Equip.json 的原生帧实测（锚点 (0,0)，故素材坐标 == 帧坐标）
CELLS = {"pink1": (72, 234, 31, 31), "pink2": (6, 234, 64, 31)}
OUT = "E:/project/_b3/disabled_measure.txt"

fr = np.asarray(Image.open(FRAME).convert("RGB"), dtype=np.int16)
ar = np.asarray(Image.open(ART).convert("RGBA"), dtype=np.int16)
arh, arw = ar.shape[:2]
frh, frw = fr.shape[:2]
lines = ["frame=%dx%d  art=%dx%d" % (frw, frh, arw, arh)]

op = ar[:, :, :3]
alpha = ar[:, :, 3]

# ---- A 区域限定性：底行带内逐像素 Δ（只统计素材不透明的像素）
y0, y1 = 225, 272
band_art = op[y0:y1, 0:175]
band_fr = fr[y0:y1, 0:175]
band_a = alpha[y0:y1, 0:175]
d = np.abs(band_fr - band_art).sum(axis=2)
diff_mask = (d > 12) & (band_a > 0)
lines.append("")
lines.append("== A 区域限定性 (y=%d..%d, x=0..175, |dRGB|>12 且素材不透明)" % (y0, y1))
lines.append("   差异像素数=%d / 有效像素=%d (%.1f%%)" % (
    diff_mask.sum(), (band_a > 0).sum(), 100.0 * diff_mask.sum() / max(1, (band_a > 0).sum())))
rows = np.where(diff_mask.any(axis=1))[0] + y0
cols = np.where(diff_mask.any(axis=0))[0]
lines.append("   差异行 y=%d..%d   差异列 x=%d..%d" % (rows.min(), rows.max(), cols.min(), cols.max()))
# 按 8px 行段列出差异密度，看是否只集中在 234..264
for ry in range(rows.min(), rows.max() + 1, 4):
    seg = diff_mask[ry - y0: ry - y0 + 4]
    lines.append("     y=%-3d 差异像素=%-5d 列范围=%s" % (
        ry, seg.sum(), (np.where(seg.any(axis=0))[0][[0, -1]] + [0, 0]).tolist() if seg.any() else "-"))

# ---- B / C 每格
for name, (cx, cy, cw, ch) in CELLS.items():
    a = op[cy:cy + ch, cx:cx + cw].reshape(-1, 3)
    f = fr[cy:cy + ch, cx:cx + cw].reshape(-1, 3)
    al = alpha[cy:cy + ch, cx:cx + cw].reshape(-1)
    keep = al > 0
    a, f = a[keep], f[keep]
    lines.append("")
    lines.append("== %s  rect=(%d,%d,%d,%d)  采样=%d" % (name, cx, cy, cw, ch, len(a)))
    ac = {}
    for p in a:
        ac[tuple(p)] = ac.get(tuple(p), 0) + 1
    fc = {}
    for p in f:
        fc[tuple(p)] = fc.get(tuple(p), 0) + 1
    lines.append("   素材色种类=%d  帧色种类=%d" % (len(ac), len(fc)))
    lines.append("   素材 top6: %s" % sorted(ac.items(), key=lambda kv: -kv[1])[:6])
    lines.append("   帧   top6: %s" % sorted(fc.items(), key=lambda kv: -kv[1])[:6])
    # 单值性：同一素材色在格内映射到几种帧色
    m = {}
    for pa, pf in zip(a, f):
        m.setdefault(tuple(pa), set()).add(tuple(pf))
    big = [(k, v) for k, v in m.items() if sum(1 for x, y in zip(a, f) if tuple(x) == k) >= 30]
    lines.append("   映射表（素材色->帧色，样本>=30）：")
    for k in sorted(m, key=lambda t: -ac.get(t, 0)):
        n = ac.get(k, 0)
        if n < 30:
            continue
        outs = sorted(m[k], key=lambda t: -fc.get(t, 0))[:4]
        share = max(fc.get(t, 0) for t in m[k]) / n
        lines.append("     %-18s n=%-5d -> %s  主映射占比=%.3f" % (str(k), n, outs, share))
    # 逐通道仿射拟合
    lines.append("   逐通道拟合 帧 = a*素材 + b ：")
    for ch_i, chn in enumerate("RGB"):
        x = a[:, ch_i].astype(np.float64)
        y = f[:, ch_i].astype(np.float64)
        A = np.vstack([x, np.ones_like(x)]).T
        coef, res, *_ = np.linalg.lstsq(A, y, rcond=None)
        pred = A @ coef
        lines.append("     %s  a=%.4f  b=%+8.2f  残差RMS=%.2f" % (
            chn, coef[0], coef[1], float(np.sqrt(np.mean((y - pred) ** 2)))))
    dd = (f - a).mean(axis=0)
    ds = (f - a).std(axis=0)
    lines.append("   Δ均值=%s  Δ标准差=%s  (整格常数色块 => 标准差应≈0)" % (
        np.round(dd, 1).tolist(), np.round(ds, 1).tolist()))

with open(OUT, "w", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
print("written " + OUT)
