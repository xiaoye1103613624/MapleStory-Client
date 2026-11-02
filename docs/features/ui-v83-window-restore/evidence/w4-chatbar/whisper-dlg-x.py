# -*- coding: utf-8 -*-
"""B5-2 取证：悄悄话弹层原生截帧的 x 方向区间（whisper-dlg-native.png 261x117）。

判据与 w4 其它取证同源：按 RGB 直接分类，不用阈值糊过去。
输出 UTF-8 文本（stdout 是 cp936，中文/符号不能直打）。
"""
from collections import OrderedDict
import io

PNG = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\w4-chatbar\whisper-dlg-native.png'
OUT = r'E:\project\_ms_probe\_whisper_x.txt'

try:
    from PIL import Image
except ImportError:
    raise SystemExit('PIL missing')

im = Image.open(PNG).convert('RGB')
W, H = im.size
px = im.load()

BLUE = (17, 85, 153)


def cls(r, g, b):
    if (r, g, b) == (255, 255, 255):
        return 'W'          # 纯白
    if r > 200 and g > 200 and b > 200:
        return 'w'          # 浅灰白（221/204 一档）
    if b > 120 and b > r + 30 and g < b:
        return 'B'          # 蓝面板一族
    if r > 150 and g > 90 and b < 90:
        return 'O'          # 橙色按钮
    if r < 60 and g < 60 and b < 60:
        return 'K'          # 近黑（输入框上边线 / 文字阴影）
    if abs(r - g) < 12 and abs(g - b) < 12:
        return 'G'          # 中性灰
    return '?'


def runs(row):
    out = []
    cur = None
    start = 0
    for x in range(W):
        c = cls(*px[x, row])
        if c == cur:
            continue
        if cur is not None:
            out.append((cur, start, x - 1))
        cur = c
        start = x
    out.append((cur, start, W - 1))
    return [r for r in out if r[2] - r[1] >= 2]


lines = ['size %dx%d' % (W, H), '']

# 1) 每一行的分段（>=3px 的段），看整体横向结构
lines.append('== per-row runs (>=3px), rows 0..116 ==')
for y in range(H):
    rr = runs(y)
    lines.append('%3d | %s' % (y, '  '.join('%s %d..%d(%d)' % (c, a, b, b - a + 1) for c, a, b in rr)))

# 2) 关键行的精确取色
lines.append('')
lines.append('== exact colors on key rows ==')
for y in (2, 3, 4, 9, 29, 40, 45, 46, 47, 55, 62, 63, 64, 76, 77, 90, 99, 108, 111, 114):
    seen = OrderedDict()
    for x in range(W):
        t = px[x, y]
        seen.setdefault(t, []).append(x)
    top = sorted(seen.items(), key=lambda kv: -len(kv[1]))[:6]
    lines.append('row %3d: %s' % (y, ' | '.join('%s x%d..%d n%d' % (c, min(v), max(v), len(v)) for c, v in top)))

# 3) 输入框（白内区）与下拉箭头列：取 47..62 的并集区间
lines.append('')
lines.append('== input field band (rows 47..62) column profile ==')
white_cols = []
dark_cols = []
for x in range(W):
    wcnt = sum(1 for y in range(47, 63) if cls(*px[x, y]) == 'W')
    kcnt = sum(1 for y in range(47, 63) if cls(*px[x, y]) in ('K', 'G'))
    if wcnt >= 8:
        white_cols.append(x)
    if kcnt >= 8:
        dark_cols.append(x)


def spans(cols):
    out = []
    for x in cols:
        if out and x == out[-1][1] + 1:
            out[-1][1] = x
        else:
            out.append([x, x])
    return out


lines.append('white-ish cols spans: %s' % ['%d..%d' % (a, b) for a, b in spans(white_cols)])
lines.append('dark/neutral cols spans: %s' % ['%d..%d' % (a, b) for a, b in spans(dark_cols)])

# 4) 按钮行（90..108）橙色列区间
lines.append('')
lines.append('== button band (rows 90..108) orange column spans ==')
oc = [x for x in range(W) if sum(1 for y in range(90, 109) if cls(*px[x, y]) == 'O') >= 6]
lines.append('orange spans: %s' % ['%d..%d' % (a, b) for a, b in spans(oc)])

# 5) 提示文字墨迹（蓝底上的浅像素）x 起点，按行
lines.append('')
lines.append('== prompt ink on blue (rows 28..41): lightest pixel per row ==')
for y in range(28, 42):
    xs = [x for x in range(W) if sum(px[x, y]) > 430]
    lines.append('row %2d: first=%s last=%s n=%d' % (y, xs[0] if xs else '-', xs[-1] if xs else '-', len(xs)))

io.open(OUT, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('wrote', OUT)
