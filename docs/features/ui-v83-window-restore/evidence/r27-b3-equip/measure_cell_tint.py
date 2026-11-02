# -*- coding: utf-8 -*-
"""从 Equip/backgrnd 底图直接量出 32x32 凹槽格子的左上角坐标。

判据：原生格子的画法是「上边 + 左边压暗（凹陷），下边 + 右边提亮」。
对每个候选左上角 (x,y) 打「L 形暗线」分：上沿与左沿同时比格子内部暗得越多，
分越高；非极大抑制后列出。这样得到的槽位表是从素材推出来的，
不是照截图调出来的常数。
"""
import sys
import numpy as np
from PIL import Image

OUT = open('E:/project/_b3/slots_report.txt', 'w', encoding='utf-8')
SIZE = int(sys.argv[2]) if len(sys.argv) > 2 else 32
PAD = 4


def sliding_mean(a, axis, win):
    """沿 axis 的 win 长度滑动均值，输出与 a 同形（尾部不足处为 nan）。"""
    if axis == 1:
        b = np.cumsum(np.hstack([np.zeros((a.shape[0], 1)), a]), axis=1)
        m = (b[:, win:] - b[:, :-win]) / win
        return np.hstack([m, np.full((a.shape[0], win - 1), np.nan)])
    b = np.cumsum(np.vstack([np.zeros((1, a.shape[1])), a]), axis=0)
    m = (b[win:, :] - b[:-win, :]) / win
    return np.vstack([m, np.full((win - 1, a.shape[1]), np.nan)])


def analyse(path, out=None):
    im = Image.open(path).convert('RGBA')
    arr = np.asarray(im).astype(np.float32)
    mask = arr[..., 3] >= 128
    lum = arr[..., :3].sum(axis=2) / 3.0
    H, W = lum.shape
    OUT.write('\n==== %s  %dx%d  格子边长=%d  opaque=%.3f\n' % (path.split('/')[-1], W, H, SIZE, mask.mean()))
    fill = np.nanmean(np.where(mask, lum, np.nan))
    lumf = np.where(mask, lum, fill)

    inner = sliding_mean(sliding_mean(lumf, 0, SIZE - 2 * PAD), 1, SIZE - 2 * PAD)
    inner = np.roll(np.roll(inner, -PAD, 0), -PAD, 1)          # 对齐到格子左上角
    top = sliding_mean(lumf, 1, SIZE)                           # 上沿：行 y 的 x..x+SIZE
    lef = sliding_mean(lumf, 0, SIZE)                           # 左沿：列 x 的 y..y+SIZE
    score = np.minimum(inner - top, inner - lef)
    score[~mask] = np.nan

    s = np.nan_to_num(score, nan=-99.0)
    taken = np.zeros((H, W), bool)
    picked = []
    flat = np.argsort(-s.ravel())
    for idx in flat:
        y, x = divmod(int(idx), W)
        if y > H - SIZE or x > W - SIZE or s[y, x] < 5.0:
            break
        if taken[y, x]:
            continue
        picked.append((x, y, float(s[y, x])))
        taken[max(0, y - SIZE + 1):y + SIZE, max(0, x - SIZE + 1):x + SIZE] = True
    picked.sort(key=lambda t: (t[1], t[0]))
    OUT.write('  检出 %d 个候选格（分=上沿/左沿同时比内部暗的亮度差）\n' % len(picked))
    for x, y, v in picked:
        OUT.write('   (%3d,%3d) score=%.1f\n' % (x, y, v))
    xs = sorted({p[0] for p in picked})
    ys = sorted({p[1] for p in picked})
    OUT.write('  列 x: %s\n  行 y: %s\n' % (xs, ys))
    OUT.write('  相邻列差: %s\n  相邻行差: %s\n' % ([b - a for a, b in zip(xs, xs[1:])],
                                                    [b - a for a, b in zip(ys, ys[1:])]))
    return picked


analyse(sys.argv[1] if len(sys.argv) > 1 else 'E:/project/_b3/Equip_backgrnd.png')
OUT.close()
print('ok')
