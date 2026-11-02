# -*- coding: utf-8 -*-
"""R27-B5-1 展开态真机自证：底板 / 右端滚动条 / 顶边拖拽加高 / 拖完记住。

坐标口径（evidence/w4-chatbar/coords-expand.md）：
  PrintWindow 帧 -> 视图坐标 = capture - (8, 31)；聊天栏 position = 视图 (0, 653)；
  历史框底边贴白带顶，高 = user_view_y，所以框顶视图 y == 653 - 高度。

判据只吃帧里的真实像素，不吃代码常量：
  F 收起态回归（原 7 条 + 本轮新增「灰带右沿 = 550」，原生收起态截帧实测）
  P 展开态底板（1px 中性灰上沿 = 653-80；框内比收起态压暗）
  S 右端滚动条（灰轨/滑块只出现在 551..565，正文区不得有；行数够一页才出滑块）
  C 滚动（点滚动条后正文换了、几何没变；关框再开换回最新一条）
  R 拖顶边（框顶按拖距上移、加高后轨道还在、chat.y 落盘、重启后高度恢复）
"""
import re
import sys

import numpy as np
from PIL import Image

P = r'E:/project'
SETTINGS = r'E:\MXD\BeiDou-Client-xy\Settings'
OX, OY = 8, 31
BAND_TOP = 653
OUT = P + r'\_ms_probe\_expand_verify.txt'
lines = []


def say(m):
    lines.append(str(m))


def load(tag):
    try:
        return np.array(Image.open(P + r'\_r27_b51_%s.png' % tag).convert('RGB')).astype(int)
    except FileNotFoundError:
        return None


def vp(a, x, y):
    """视图坐标取样 -> 像素"""
    return tuple(int(c) for c in a[y + OY, x + OX])


def vrow(a, y, x0, x1):
    return a[y + OY, x0 + OX:x1 + OX]


def is_neutral(px, lo, hi, tol=14):
    r, g, b = int(px[0]), int(px[1]), int(px[2])
    return lo <= r <= hi and abs(r - g) <= tol and abs(g - b) <= tol


def grey_span(a, x, y0, y1):
    span = [y for y in range(y0, y1)
            if is_neutral(a[y + OY, x + OX], 108, 132, tol=10)]
    segs, p = [], None
    for y in span:
        if p is not None and y == p[1] + 1:
            p[1] = y
        else:
            p = [y, y]
            segs.append(p)
    return max(segs, key=lambda t: t[1] - t[0]) if segs else None


def find_anchor(a, x, grey_top, back=10):
    best, by = None, None
    for y in range(grey_top - back, grey_top):
        m = min(int(c) for c in a[y + OY, x + OX])
        if best is None or m > best:
            best, by = m, y
    return by


def first_ink(a, y0, y1, x0=2, x1=200, floor=447, mode='dark'):
    """第一行有字的行 + 最左列。
    mode='dark' 浅底（白带/灰带）找深色字；mode='bright' 暗底（历史框）找亮字。"""
    for y in range(y0, y1):
        row = a[y + OY, x0 + OX:x1 + OX]
        tot = row.sum(axis=1)
        hit = np.where(tot < floor)[0] if mode == 'dark' else np.where(tot > floor)[0]
        if len(hit) >= 2:
            return y, int(hit.min()) + x0
    return None, None


# 底板：TooltipBackdrop 画的是 alpha 0.9 的黑 + 1px (128,128,128) 中性边。
# 上沿那条边整幅同色，是场景无关的框顶探针。
def panel_top(a, lo=400, hi=BAND_TOP - 1, x0=20, x1=540):
    """历史框顶边 = TooltipBackdrop 那条整幅 (128,128,128) 中性边。
    光看这条边会误报（收起态白带顶缘的抗锯齿行也是灰），所以再要求它下面
    2..6 行是被压暗的框内（均值 < 60）—— 底板真的画出来了才算数。"""
    for y in range(lo, hi):
        seg = vrow(a, y, x0, x1)
        hit = np.where([(abs(int(p[0]) - 128) < 18 and abs(int(p[0]) - int(p[2])) < 14)
                        for p in seg])[0]
        if len(hit) <= (x1 - x0) * 0.8:
            continue
        if all(col_lum(a, x0, x1, y + k, y + k + 1) < 60 for k in range(2, 7)):
            return y
    return None


def lum(a, y, x0, x1):
    return float(vrow(a, y, x0, x1).mean())


def col_lum(a, x0, x1, y0, y1):
    return float(a[y0 + OY:y1 + OY, x0 + OX:x1 + OX].mean())


def ink_bands(a, y0, y1, x0=10, x1=540, floor=330):
    """暗底上的亮字：逐行墨迹签名（行数、左右沿、总量）。"""
    sig = []
    for y in range(y0, y1):
        row = a[y + OY, x0 + OX:x1 + OX]
        bright = np.where(row.sum(axis=1) > floor)[0]
        sig.append((len(bright), int(bright.min()) if len(bright) else -1,
                    int(bright.max()) if len(bright) else -1,
                    int(row[bright].sum()) if len(bright) else 0))
    return sig


# ================================================================ F 收起态
fold = load('01_folded')
if fold is None:
    raise SystemExit('没有 01_folded 帧')
g0 = grey_span(fold, 300, BAND_TOP - 20, BAND_TOP + 40)
if not g0:
    raise SystemExit('收起态灰带找不到，后面全部作废')
a0 = find_anchor(fold, 300, g0[0])
ink_y, ink_x = first_ink(fold, g0[0], g0[1] + 1, mode='bright', floor=447)
blue = next((x for x in range(500, 566)
             if any(int(fold[y + OY, x + OX][2]) > 170
                    and int(fold[y + OY, x + OX][2]) - int(fold[y + OY, x + OX][0]) > 60
                    for y in range(g0[0], g0[1] + 1))), None)
# 灰带右沿：原生收起态截帧（566 宽）灰只铺到 x=550，551..565 是滚动条列
grey_right = next((x for x in range(565, 400, -1)
                   if is_neutral(fold[BAND_TOP + 21 + OY, x + OX], 108, 132, tol=10)), None)
say('fold: 锚点视图 y=%s 灰带 %s..%s h=%s | 墨迹 top=%s(+%s) 左=%s | 蓝+ 左=%s | 灰带右沿=%s'
    % (a0, g0[0], g0[1], g0[1] - g0[0] + 1, ink_y, (ink_y - a0) if ink_y else None,
       ink_x, blue, grey_right))
F = [
    ('F1 灰带顶 = 锚点+3', g0[0] - a0 == 3),
    ('F2 灰带高 = 23', g0[1] - g0[0] + 1 == 23),
    ('F3 墨迹顶 = 锚点+9', ink_y is not None and ink_y - a0 == 9),
    ('F4 墨迹左沿 = 6', ink_x == 6),
    ('F5 蓝色 + 左沿 = 537', blue == 537),
    ('F6 灰带上沿暗线 (原生 87,87,95) 实测=%s' % (vp(fold, 300, a0 + 2),),
     max(vp(fold, 300, a0 + 2)) < 130),
    ('F7 灰带右沿 = 550（原生：551 起是滚动条列）', grey_right == 550),
]

# ================================================================ P 底板
exp2, exp3 = load('02_expand_h80'), load('03_expand_full')
pt2 = panel_top(exp2) if exp2 is not None else None
say('expand h80: 底板上沿 y=%s（期望 %s = 653-80）' % (pt2, BAND_TOP - 80))
say('  y600 收起=%s 展开=%s | 正文列亮度 收起=%.1f 展开=%.1f'
    % (vp(fold, 300, 600), vp(exp2, 300, 600),
       col_lum(fold, 20, 540, 580, 650), col_lum(exp2, 20, 540, 580, 650)))
P_ = [
    ('P1 底板顶边 = 653-80 (+-%s)' % (pt2 - 573 if pt2 else None),
     pt2 is not None and abs(pt2 - 573) <= 2),
    ('P2 底板把框内压暗（半透明黑底）',
     col_lum(exp2, 20, 540, 580, 650) < col_lum(fold, 20, 540, 580, 650) - 20),
]

if pt2 is None:
    # 展开没成功时后面几组算出来的都是假信号（历史上就是这里出 nan），直接作废。
    say('!! 底板探针失效：02_expand_h80 没有展开，P2 之后的 S/C/R 全部作废')
    P_.append(('P3 S 滚动条组（作废）', False))
    C_ = R_ = S_ = []
    fails = 0
    for group in (F, P_, S_, C_, R_):
        for name, ok in group:
            say('  %s  %s' % ('PASS' if ok else 'FAIL', name))
            fails += 0 if ok else 1
    say('')
    say('TOTAL %d FAIL（未展开，S/C/R 未评估）' % fails)
    open(OUT, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
    print('\n'.join(lines[-30:]))
    sys.exit(1)

# ================================================================ S 滚动条列
top, bot = pt2 + 5, BAND_TOP - 4
body = col_lum(exp2, 20, 540, top, bot)
right = col_lum(exp2, 553, 564, top, bot)
left_clean = col_lum(exp2, 400, 540, top, bot)
say('scroll col: 正文区亮度=%.1f  滚动条列(553..564)=%.1f  右侧正文(400..540)=%.1f'
    % (body, right, left_clean))
S_ = [
    ('S1 滚动条列在 551..565 出图（比黑底板亮）', right > body + 25),
    ('S2 正文区右侧 549 以前干净（没有误铺轨道）', left_clean < body + 12),
]

# 空历史（1 条）时 Slider 走 disabled 支路，够一页后走 enabled 支路（prev/next/thumb）
d02 = col_lum(exp2, 553, 564, top, bot)
d03 = col_lum(exp3, 553, 564, top, bot)
rowdiff = [abs(col_lum(exp2, 553, 564, y, y + 1) - col_lum(exp3, 553, 564, y, y + 1))
           for y in range(top, bot)]
changed_rows = sum(1 for x in rowdiff if x > 8)
say('slider: 灌消息前后轨道列逐行差 >8 的行数=%d（帧02 亮=%.1f 帧03 亮=%.1f）'
    % (changed_rows, d02, d03))
S_.append(('S3 消息够一页后滑块出图（轨道列像素变了）', changed_rows >= 6))

iy, ix = first_ink(exp3, top, bot, x0=2, x1=200, floor=400, mode='bright')
say('expand full: 首行墨迹 top=%s 左沿=%s（期望 5..8）' % (iy, ix))
S_.append(('S4 展开态正文左沿 = 5..8（与收起态同列）', ix is not None and 5 <= ix <= 8))

# ================================================================ C 滚动
scr, reop = load('04_scroll_up1'), load('06_reopen')
if scr is None or reop is None:
    say('!! 缺 04_scroll_up1 / 06_reopen 帧，C 组作废')
    C_ = [('C0 帧缺失', False)]
else:
    bands = range(top + 10, BAND_TOP - 2)
    s3 = ink_bands(exp3, top + 10, BAND_TOP - 2)
    s4 = ink_bands(scr, top + 10, BAND_TOP - 2)
    s6 = ink_bands(reop, top + 10, BAND_TOP - 2)
    pt4 = panel_top(scr)
    n = len(s3)
    changed = sum(1 for x, y in zip(s3, s4) if x[0] != y[0] or abs(x[1] - y[1]) > 6 or x[3] != y[3])
    restored = sum(1 for x, y in zip(s3, s6) if x == y)
    say('scroll: 框顶 %s（滚动前 %s）| 正文签名变了 %d/%d 行 | 关框再开回到原样 %d/%d 行'
        % (pt4, pt2, changed, n, restored, n))
    C_ = [
        ('C1 点滚动条只换内容、不换几何（框顶同一行）', pt4 == pt2 and n > 0),
        ('C2 点滚动条后正文确实翻旧了（>=30%% 行不同）', n > 0 and changed >= n * 0.3),
        ('C3 关框再开回到最新一条（>=90%% 行与滚动前一致）', n > 0 and restored >= n * 0.9),
    ]

# ================================================================ R 拖拽加高
res = load('05_resized_h140')
pt5 = panel_top(res) if res is not None else None
say('resize: 框顶 y=%s（期望 %s = 573-60）' % (pt5, 513))
right5 = col_lum(res, 553, 564, (pt5 or 513) + 5, BAND_TOP - 4)
body5 = col_lum(res, 20, 540, (pt5 or 513) + 5, BAND_TOP - 4)
R_ = [
    ('R1 顶边拖 60px -> 框顶 573 -> 513（实测 %s，偏差 %s）'
     % (pt5, (pt5 - 513) if pt5 else None), pt5 is not None and abs(pt5 - 513) <= 6),
    ('R2 加高后滚动条列跟着拉长', right5 > body5 + 25),
]

chat_y = None
sec = None
for ln in open(SETTINGS, 'r', encoding='utf-8').read().splitlines():
    m = re.match(r'^(\w[\w.]*)\s*:', ln)
    if m:
        sec = m.group(1)
    m2 = re.match(r'^\s*y\s*:\s*(-?\d+)', ln)
    if m2 and sec == 'chat':
        chat_y = int(m2.group(1))
say('Settings chat.y（退出时落盘）= %s' % chat_y)
R_.append(('R3 拖完的高度落盘进 chat.y（>=100）', chat_y is not None and chat_y >= 100))

rst = load('08_restored')
if rst is not None and chat_y:
    pt8 = panel_top(rst)
    want = BAND_TOP - chat_y
    say('restart: 框顶 y=%s（期望 %s = 653-%s）' % (pt8, want, chat_y))
    R_.append(('R4 重启后按 chat.y 恢复高度', pt8 is not None and abs(pt8 - want) <= 3))
else:
    say('restart: 没有 08_restored 帧（重启恢复项未测到）')

say('')
fails = 0
for group in (F, P_, S_, C_, R_):
    for name, ok in group:
        say('  %s  %s' % ('PASS' if ok else 'FAIL', name))
        fails += 0 if ok else 1
say('')
say('TOTAL %d FAIL' % fails)
open(OUT, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
print('\n'.join(lines[-30:]))
sys.exit(1 if fails else 0)
