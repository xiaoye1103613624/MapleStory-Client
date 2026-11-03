"""UIChannel 版式逐格取证（原生 频道变更.png 428x122）。

已定：t 画布 y=0..66、s 画布 y=86..121、槽位板 68x20 在 y=66、x=6+70i。
本脚本补齐：①channel1（当前频道蓝板）落位 ②ch[i] 标签相对板的偏移
③world[i] 世界名落位 ④t 里蓝条的 y 范围。
"""
import os
import numpy as np
from PIL import Image

NATIVE = r"C:/Users/11036/Desktop/正确客户端截图/频道变更.png"
HERE = os.path.dirname(os.path.abspath(__file__))
DUMP = HERE + r"/_dump"

n = np.array(Image.open(NATIVE).convert('RGB')).astype(np.int16)
lines = []


def rgba(name):
    return np.array(Image.open('%s/%s.png' % (DUMP, name)).convert('RGBA')).astype(np.int16)


def match(a_rgb, a_alpha, x0, x1, y0, y1, min_alpha=200):
    """在 [x0,x1)x[y0,y1) 的窗口里滑，返回 (SAD, x, y)。只比不透明像素。"""
    m = a_alpha > min_alpha
    best = (1e9, None, None)
    for oy in range(y0, y1 - a_rgb.shape[0] + 1):
        for ox in range(x0, x1 - a_rgb.shape[1] + 1):
            patch = n[oy:oy + a_rgb.shape[0], ox:ox + a_rgb.shape[1]]
            sad = float(np.abs(patch[m] - a_rgb[m]).mean())
            if sad < best[0]:
                best = (sad, ox, oy)
    return best


# --- dump channel1 / channel2 ---
for nm in ('channel1', 'channel2'):
    a = rgba(nm)
    sad, x, y = match(a[:, :, :3], a[:, :, 3], 0, 100, 60, 90)
    lines.append('%s 在首槽区最优 SAD=%.2f @(x=%d,y=%d)' % (nm, sad, x, y))

# --- ch 标签：先看 ch/0 长什么样、多大 ---
for i in range(6):
    p = '%s/ch_%d.png' % (DUMP, i)
    if not os.path.exists(p):
        continue

lines.append('')
lines.append('--- 板列 x 复核：channel0 在 y=66 整行扫描（找全部灰槽位）---')
a = rgba('channel0')
m = a[:, :, 3] > 200
row = []
for ox in range(0, n.shape[1] - 67):
    patch = n[66:86, ox:ox + 68]
    row.append((float(np.abs(patch[m] - a[:, :, :3][m]).mean()), ox))
good = [r for r in row if r[0] < 1.0]
# 聚簇
clusters = []
for sad, ox in good:
    if clusters and ox - clusters[-1][-1][1] <= 2:
        clusters[-1].append((sad, ox))
    else:
        clusters.append([(sad, ox)])
lines.append('channel0 SAD<1.0 的簇: ' + ', '.join(
    'x≈%d(min SAD %.3f)' % (min(c, key=lambda r: r[0])[1], min(c)[0]) for c in clusters))

lines.append('')
lines.append('--- t 画布各行平均色（找蓝条 y 范围）---')
t = np.array(Image.open(DUMP + '/t.png').convert('RGB')).astype(np.int16)
for y in range(t.shape[0]):
    if y % 1 == 0 and (y == 0 or np.abs(t[y] - t[y - 1]).mean() > 12):
        lines.append('  t 行变化 y=%d  平均色=%s' % (y, tuple(int(v) for v in t[y].mean(axis=0))))

lines.append('')
lines.append('--- 原生帧各行平均色（对照）---')
for y in range(n.shape[0]):
    if y == 0 or np.abs(n[y] - n[y - 1]).mean() > 12:
        lines.append('  native 行变化 y=%d  平均色=%s' % (y, tuple(int(v) for v in n[y].mean(axis=0))))

with open(HERE + r"/_channel_grid.txt", 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('ok')
