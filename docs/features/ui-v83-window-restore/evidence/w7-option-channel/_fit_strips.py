"""Channel 画布 t/s/c 在原生 频道变更.png 里的落位：逐 oy 求最优 ox 的 SAD 剖面。

允许左右裁边（帧宽 428 < 画布宽 436），因为截图是按窗口可见区截的。
"""
import os
import numpy as np
from PIL import Image

NATIVE = r"C:/Users/11036/Desktop/正确客户端截图/频道变更.png"
HERE = os.path.dirname(os.path.abspath(__file__))
DUMP = HERE + r"/_dump"

n = np.array(Image.open(NATIVE).convert('RGB')).astype(np.int16)
lines = ['native %dx%d' % (n.shape[1], n.shape[0])]

for name in ('t', 's', 'c'):
    a = np.array(Image.open('%s/%s.png' % (DUMP, name)).convert('RGB')).astype(np.int16)
    best = (1e9, None, None)
    prof = []
    for oy in range(0, n.shape[0] - a.shape[0] + 1):
        row_best = (1e9, None)
        for ox in range(-(a.shape[1] - 80), n.shape[1] - 80 + 1):
            xs0, xs1 = max(0, ox), min(n.shape[1], ox + a.shape[1])
            if xs1 - xs0 < 100:
                continue
            as0 = xs0 - ox
            patch = n[oy:oy + a.shape[0], xs0:xs1]
            sad = float(np.abs(patch - a[:, as0:as0 + (xs1 - xs0)]).mean())
            if sad < row_best[0]:
                row_best = (sad, ox)
        prof.append((oy, row_best[0], row_best[1]))
        if row_best[0] < best[0]:
            best = (row_best[0], row_best[1], oy)
    prof.sort(key=lambda r: r[1])
    lines.append('%s %dx%d  最优 SAD=%.2f @(x=%d, y=%d)' % (
        name, a.shape[1], a.shape[0], best[0], best[1], best[2]))
    lines.append('   次优 5: ' + ', '.join('y=%d SAD=%.2f x=%d' % (p[0], p[1], p[2]) for p in prof[:6]))

# 通道槽位：channel0/1/2 (68x20) 与 ch/0..5 标签
for name in ('channel0', 'channel1', 'channel2'):
    a = np.array(Image.open('%s/%s.png' % (DUMP, name)).convert('RGBA')).astype(np.int16)
    rgb, al = a[:, :, :3], a[:, :, 3]
    best = (1e9, None, None)
    for oy in range(0, n.shape[0] - a.shape[0] + 1):
        for ox in range(0, n.shape[1] - a.shape[1] + 1):
            m = al > 200
            patch = n[oy:oy + a.shape[0], ox:ox + a.shape[1]]
            sad = float(np.abs(patch[m] - rgb[m]).mean())
            if sad < best[0]:
                best = (sad, ox, oy)
    lines.append('%s %dx%d  最优 SAD=%.2f @(x=%d, y=%d)' % (
        name, a.shape[1], a.shape[0], best[0], best[1], best[2]))

for name in ('BtCancel_normal_0', 'BtChange_normal_0'):
    a = np.array(Image.open('%s/%s.png' % (DUMP, name)).convert('RGBA')).astype(np.int16)
    rgb, al = a[:, :, :3], a[:, :, 3]
    best = (1e9, None, None)
    for oy in range(0, n.shape[0] - a.shape[0] + 1):
        for ox in range(0, n.shape[1] - a.shape[1] + 1):
            m = al > 200
            patch = n[oy:oy + a.shape[0], ox:ox + a.shape[1]]
            sad = float(np.abs(patch[m] - rgb[m]).mean())
            if sad < best[0]:
                best = (sad, ox, oy)
    lines.append('%s %dx%d  最优 SAD=%.2f @(x=%d, y=%d)' % (
        name, a.shape[1], a.shape[0], best[0], best[1], best[2]))

with open(HERE + r"/_strip_fit.txt", 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('ok')
