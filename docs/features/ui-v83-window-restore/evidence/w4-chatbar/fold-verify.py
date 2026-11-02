# -*- coding: utf-8 -*-
"""收起态常量复验：把我们的整窗 PrintWindow 帧换算到 view 坐标，逐格对上原生
「聊天框收起的时候.png」的 oracle。

对齐锚点 = 白输入带顶缘那条 (255,255,255) 高光行：
  原生截帧里在 y=8，我们的帧里在 capture y=684（= 带 position.y() 653 + 31）。
换算：view_y = capture_y - 31，view_x = capture_x - 8（PrintWindow 边框偏移，
见 windows-dpi-aware-capture / 既有 evidence）。

原生 oracle（view 口径，锚点记作 +0）：
  +2 暗线 (87,87,95) / +3..+25 平涂灰 (h=23) / +26 (153,170,170) / +27 (102,119,119)
  文字墨迹顶 +9、墨迹左沿 x=6 / 蓝 + 钮左沿 x=537
"""
import numpy as np
from PIL import Image

OUT = r'E:\project\_ms_probe\_fold_verify.txt'
lines = []


def grey_span(a, x):
    span = [y for y in range(a.shape[0])
            if abs(int(a[y, x][0]) - int(a[y, x][1])) < 12
            and abs(int(a[y, x][1]) - int(a[y, x][2])) < 16
            and 100 <= int(a[y, x][0]) <= 140]
    segs = []
    s = p = span[0]
    for y in span[1:]:
        if y == p + 1:
            p = y
        else:
            segs.append((s, p))
            s = p = y
    segs.append((s, p))
    return max(segs, key=lambda t: t[1] - t[0])


def find_anchor(a, x, grey_top, back=10):
    """灰带上方 10 行里最亮（min 通道最大）的那一行 = 白带顶缘高光行。
    原生是纯 (255,255,255)，我们的是 (245,254,255)，所以不能按「全通道>248」找。"""
    best, by = None, None
    for y in range(max(0, grey_top - back), grey_top):
        m = min(int(v) for v in a[y, x])
        if best is None or m > best:
            best, by = m, y
    return by


def ink_top_x(a, gy0, gy1, x0, x1):
    for y in range(gy0, gy1):
        row = a[y, x0:x1].astype(int)
        bright = np.where(row.sum(axis=1) > 119 * 3 + 90)[0]
        if len(bright) >= 2:
            return y, int(bright.min() + x0)
    return None, None


def blue_left(a, y0, y1, x0, x1):
    for x in range(x0, x1):
        for y in range(y0, y1):
            r, g, b = (int(v) for v in a[y, x])
            if b > 170 and b - r > 60 and g > 120:
                return x
    return None


nat = np.array(Image.open(
    r'C:\Users\11036\Desktop\正确客户端截图\聊天框收起的时候.png').convert('RGB'))
ours = np.array(Image.open(r'E:\project\_r27_f7_sent.png').convert('RGB'))

# --- 原生 ---
ng = grey_span(nat, 300)
na = find_anchor(nat, 300, ng[0])           # 锚点：灰带上方最亮的高光行
lines.append('native: anchor(white top) y=%s, grey %s h=%s' % (na, ng, ng[1] - ng[0] + 1))
lines.append('  rel: grey top +%s, bottom +%s' % (ng[0] - na, ng[1] - na))
nink = ink_top_x(nat, ng[0], ng[1] + 1, 2, 200)
lines.append('  ink top y=%s x=%s -> rel +%s, ink x=%s' % (nink[0], nink[1], nink[0] - na, nink[1]))
lines.append('  blue + left x=%s' % blue_left(nat, ng[0], ng[1] + 1, 500, nat.shape[1]))

# --- 我们（换算到 view） ---
OX, OY = 8, 31
sub = ours[640:759, :, :]
og = grey_span(sub, 300)
oa = find_anchor(sub, 300, og[0])
lines.append('')
lines.append('ours: anchor capture y=%s -> view y=%s' % (oa + 640, oa + 640 - OY))
lines.append('  grey capture %s -> view %s..%s h=%s'
             % (og, og[0] + 640 - OY, og[1] + 640 - OY, og[1] - og[0] + 1))
lines.append('  rel to anchor: top +%s, bottom +%s' % (og[0] - oa, og[1] - oa))
oink = ink_top_x(sub, og[0], og[1] + 1, 2, 200)
lines.append('  ink top view y=%s x=%s -> rel +%s'
             % (oink[0] + 640 - OY, oink[1] - OX, oink[0] - oa))
lines.append('  blue + left view x=%s' % (blue_left(sub, og[0], og[1] + 1, 500, sub.shape[1]) - OX))

lines.append('')
lines.append('rows @x300, anchor..anchor+4 (native | ours view)')
for k in range(0, 5):
    n = tuple(int(v) for v in nat[na + k, 300])
    o = tuple(int(v) for v in ours[oa + 640 + k, 300])
    lines.append('  +%d  %s | %s' % (k, n, o))

# --- 断言 ---
edge = tuple(int(v) for v in ours[oa + 640 + 2, 300])
# 输入态：白带内必须有深色正文墨迹（此前 WHITE-on-white，带内除那条暗线外零墨迹）
typed = np.array(Image.open(r'E:\project\_r27_f7_typed.png').convert('RGB')).astype(int)
ty0 = 686
# 白带正文从 view x=83 起（chatTarget 框占 0..81），换算 capture x=91 起
typed_ink = int(((typed[ty0:707, 91:538].sum(axis=2) < 450)).sum())
checks = [
    ('grey top rel == +3', og[0] - oa == 3),
    ('grey h == 23', og[1] - og[0] + 1 == 23),
    ('ink top rel == +9', oink[0] - oa == 9),
    ('ink x == 6', oink[1] - OX == 6),
    ('blue left == 537', blue_left(sub, og[0], og[1] + 1, 500, sub.shape[1]) - OX == 537),
    ('edge @+2 dark (native 87,87,95) got %s' % (edge,), max(edge) < 130),
    ('typed text visible in white band (ink px=%d)' % typed_ink, typed_ink > 20),
]
lines.append('')
for name, ok in checks:
    lines.append('  %s  %s' % ('PASS' if ok else 'FAIL', name))

open(OUT, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('written')
