# -*- coding: utf-8 -*-
"""R27-B5-2 弹层逐格落位复验。

判据 = 用**同一把尺子**量两端：原生 oracle `whisper-dlg-native.png`（261x117）与
我们真机 PrintWindow 帧里裁出的同一档 261x117，逐项比 bbox。所有「墨迹」都用
**逐行中位色差**判据（该行主体是底色，偏离底色 >th 的才算字/线），所以蓝面板上
的白字、灰 body 上的橙字都能用同一段代码量，两端可比。

裁图定位：我们窗内唯一能稳定认出的长横是输入框那条黑上沿（Notice4/s 第 4 行，
框坐标 y=FIELD_Y=47、左端 x=FIELD_X0=22）。按它反推弹层左上，再按
`截帧 = 框 + (3, 1)` 裁出 261x117。PrintWindow 帧 -> view：view = capture - (8, 31)
（evidence/w4-chatbar/coords-fold.md）。

输出只写 UTF-8 文件，不 print（本机 stdout 是 cp936，见用户记忆）。
"""
import sys

import numpy as np
from PIL import Image

CAP = sys.argv[1] if len(sys.argv) > 1 else r'E:\project\_r27_b52_04_whisperDlg.png'
NAT = r'E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/evidence/w4-chatbar/whisper-dlg-native.png'
OUT = r'E:\project\_ms_probe\_b52_verify.txt'
BORDER = (8, 31)          # capture -> view
FIELD_Y = 47              # 黑上沿在框坐标的行号
FIELD_X0 = 22             # 黑上沿左端在框坐标的列
EXPECT_LT = (507, 299)    # 266x121 居中在 1280x720 视口的理论值
lines = []


def w(msg):
    lines.append(str(msg))


def dark(p, th=70):
    return int(p[0]) < th and int(p[1]) < th and int(p[2]) < th


def white(p, th=230):
    return int(p[0]) > th and int(p[1]) > th and int(p[2]) > th


def blue(p):
    return int(p[2]) > int(p[0]) + 30 and int(p[2]) > 90


def orange(p):
    return int(p[0]) > 190 and 90 < int(p[1]) < 190 and int(p[2]) < 90


def first_run(rows):
    """连续命中段的长度（从列表首元素开始）。"""
    if not rows:
        return None
    s, p = rows[0], rows[0]
    for y in rows[1:]:
        if y == p + 1:
            p = y
        else:
            break
    return (s, p)


def ink_bbox(a, x0, x1, y0, y1, th=60):
    """逐行中位色差判据的墨迹 bbox。"""
    xs, ys = [], []
    for y in range(y0, y1):
        row = a[y, x0:x1].astype(int)
        med = np.median(row, axis=0)
        for i in range(row.shape[0]):
            if np.max(np.abs(row[i] - med)) > th:
                xs.append(x0 + i)
                ys.append(y)
    if not xs:
        return None
    return (min(xs), max(xs), min(ys), max(ys))


def fmt(bb):
    return 'None' if bb is None else 'x %d..%d  y %d..%d' % bb


def measure(a, tag):
    w('=== %s (%dx%d) ===' % (tag, a.shape[1], a.shape[0]))

    bluerows = [y for y in range(a.shape[0]) if blue(a[y, 100])]
    w('  蓝面板 @x100  连续段=%s  全部命中=%d 行' % (first_run(bluerows), len(bluerows)))

    blacklines = [y for y in range(a.shape[0])
                  if sum(1 for x in range(30, 230, 4) if dark(a[y, x])) >= 45]
    w('  黑上沿行      %s' % blacklines)

    white_rows = [y for y in range(a.shape[0])
                  if sum(1 for x in range(30, 240, 2) if white(a[y, x])) >= 85]
    w('  白内区连续段  %s  (候选 %d 行)' % (first_run(white_rows), len(white_rows)))

    hint = None
    if blacklines:
        top = first_run(bluerows)[0] if bluerows else 0
        hint = ink_bbox(a, 14, 210, top + 2, blacklines[0] - 2, 55)
    w('  提示墨迹      %s' % fmt(hint))

    cur = None
    if white_rows:
        y0, y1 = min(white_rows), max(white_rows)
        for x in range(18, 70):
            hits = sum(1 for y in range(y0 + 1, y1) if not white(a[y, x], 210))
            if hits >= (y1 - y0) * 0.6:
                cur = x
                break
    w('  光标列        x=%s' % cur)

    arrow = ink_bbox(a, 242, 259, 44, 68, 45)
    w('  箭头列墨迹    %s' % fmt(arrow))

    grey = [y for y in range(a.shape[0])
            if int(max(a[y, 100])) - int(min(a[y, 100])) < 8 and 190 <= int(a[y, 100][0]) <= 240]
    w('  灰 body @x100 连续段=%s  (候选 %d 行)' % (first_run(grey), len(grey)))

    w('  确定钮墨迹    %s' % fmt(ink_bbox(a, 150, 206, 85, 112, 60)))
    w('  取消钮墨迹    %s' % fmt(ink_bbox(a, 206, 258, 85, 112, 60)))


nat = np.array(Image.open(NAT).convert('RGB'))
measure(nat, 'NATIVE oracle')

cap = np.array(Image.open(CAP).convert('RGB'))

# 定位器只在弹层理论位附近找那条黑上沿。之前从 capture y=0 全帧扫，先撞上了
# 窗口自己的深色边框行（实测 y=31 一段 413px 的 (8,8,0)），反推出负的裁图原点，
# 于是 `a[44]` 越界。理论左上 (507,299) -> capture (515,330)，输入框黑沿在
# 框坐标 y=47、x=22..259 -> capture y≈377、x≈545..782，给 ±60px 容差足够。
BAND_Y = (330, 470)
BAND_X = (500, 820)

hit = None
for y in range(*BAND_Y):
    # 认「输入框白内区」这条连续白段，不认黑描边：弹层压在洞穴场景上，场景里
    # 同样有 200px 以上的连续暗行（实测 capture y=326..332 一整片暗背景），黑线
    # 定位器会先撞上背景。白内区是这层独有的，且左右都卡在框宽以内。
    # 白内区在框坐标 = x 27..（FIELD_X+combo_left_w=27）、y 48（FIELD_Y+1）。
    # 阈值取 250 且限宽 150..235：Notice4 顶带的浅灰（约 240）会通不过 250，
    # 而顶带实测连续段 244..254px，也比输入框的 217px 宽 —— 两条判据合起来才
    # 认得准（先只用「长白段」时撞上的是顶带，反推出偏差 (-21,-45) 的假落位）。
    start = None
    best = (0, 0, 0)
    for x in range(*BAND_X):
        if white(cap[y, x], 250):
            if start is None:
                start = x
        else:
            if start is not None and x - start > best[0]:
                best = (x - start, start, x - 1)
            start = None
    if 150 <= best[0] <= 235:
        hit = (y, best[1], best[2])
        break

if not hit:
    w('!! 弹层理论位附近没找到 150..260px 的连续白段：ComboBox 输入框没画出来（资产缺失？看 _ui_asset_diag.log 的 whisperdlg.frame）')
else:
    cy, x0, x1 = hit
    frame_cap_lt = (x0 - 27, cy - 48)
    frame_lt = (frame_cap_lt[0] - BORDER[0], frame_cap_lt[1] - BORDER[1])
    w('输入框白内区 capture y=%d x=%d..%d (宽 %d) -> 弹层左上 view=(%d,%d)  理论%s  偏差=(%d,%d)'
      % (cy, x0, x1, x1 - x0 + 1, frame_lt[0], frame_lt[1], EXPECT_LT,
         frame_lt[0] - EXPECT_LT[0], frame_lt[1] - EXPECT_LT[1]))

    ox, oy = frame_lt[0] + 3 + BORDER[0], frame_lt[1] + 1 + BORDER[1]
    mine = cap[oy:oy + 117, ox:ox + 261, :3]
    w('裁图 capture 起点=(%d,%d) shape=%s' % (ox, oy, mine.shape))
    measure(mine, 'OURS (crop 261x117)')

open(OUT, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')
