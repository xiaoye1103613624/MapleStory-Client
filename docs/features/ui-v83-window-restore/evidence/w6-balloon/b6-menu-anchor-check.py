# -*- coding: utf-8 -*-
"""R27-B6 底栏气球锚定判据（蓝色钮行法）—— 目录 / 界面 共用一把尺。

跑法：python b6-menu-anchor-check.py <客户帧.png> [颗数=4] [期望x=666] [期望y=545] [首颗y=22] [底板高=140]
  目录：默认参数（4 颗 / 666,545 / 高 140）
  界面：`<帧> 8 722 440 22 245`

不用整张 backgrnd 做模板匹配：气球底板是半透明深蓝，实测在真机帧上 masked mean-SAD
最低点会飘到别处（本轮误命中过 BtShort 那颗，agree .58 / sad 61）—— 没有判别力。
改用「气球里那 4 颗 81x25 蓝钮」的行剖面，原生截帧和我们帧上都一样锐。

原生侧的尺（见 anchor-rule.md）：
  目录按钮点击后的列表.png   -> 气球画布末行 crop y142，BtMenu(54x34) 掩膜配准命中 crop y143
  ”界面“按钮点击后的列表.png -> 气球画布末行 crop y243，BtShort 命中 crop y244
  => 尖角最后一条实像素行贴在触发钮上沿（间隙 1 行）。

判据（view = capture - (8,31)，VWIDTH=1280 / VHEIGHT=720）：
  1) 蓝钮行剖面：N 段各 25 行、步距 26（= 同族气球版式 (8, 22+26i)）
     -> 气球左上角 = (首段左沿列 - 8, 首段顶行 - 22)
  2) 该左上角 == 代码算式期望 (666, 545)，tol 2px
  3) 尖角末行 (top + 底板高 - 1) 与底栏钮上沿 (VHEIGHT-71+36=685) 的间隙 == 1

跑法：python b6-menu-anchor-check.py <客户帧.png>
"""
import sys

import numpy as np
from PIL import Image

BORDER = (8, 31)
VW, VH = 1280, 720

# 底栏几何（与 IO/UITypes/UIStatusBar.cpp:1993/1998 同源，本轮未改）
BAR_H = 71
BTN_Y = 36
BTN_TOP = VH - BAR_H + BTN_Y		# 685
QS_W, GROUP_GAP = 466, 241
GROUP_X = VW - QS_W - GROUP_GAP		# 573

# 期望：尖角中心 x=46 对准 BtMenu(54 宽) 中心，底 = 钮上沿
EXPECT_X = GROUP_X + 112 + 27 - 46	# 666
EXPECT_Y = BTN_TOP - 140		# 545


def blue_rows(frame):
    """整帧蓝钮掩膜（b>=140 且 b-r>=40 且 g>r）。"""
    a = frame[..., :3].astype(np.int16)
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    return (b >= 140) & (b - r >= 40) & (g > r)


def col_runs(row, max_gap=12, max_width=83):
    """列程 + 「宽度封顶」的贪心合并：每次并掉间隙最小的一对，合并后仍 <= max_width 才并。

    为什么要合：钮上的文字笔画会把 81 宽蓝带切成 12+1+8+3+4+42（界面气球第 3 颗
    capture y=550），不合就少认几颗，8 颗只认到 5 颗。
    为什么要封顶：目录气球第 4 颗 capture y=690 左边 7px 处另有一段蓝（650..674，
    底栏邻件），单纯按间隙合并会拼成 113 宽、被 79..83 过滤器整段判死。
    封顶 83 = 「一颗钮最宽也就是 81±2」，邻件碰不进来。
    """
    strict = []
    cols = np.where(row)[0]
    if cols.size == 0:
        return strict
    s = p = int(cols[0])
    for c in cols[1:]:
        c = int(c)
        if c == p + 1:
            p = c
        else:
            strict.append((s, p))
            s = p = c
    strict.append((s, p))

    runs = list(strict)
    while len(runs) > 1:
        cand = None
        for i in range(len(runs) - 1):
            gap = runs[i + 1][0] - runs[i][1] - 1
            if gap > max_gap or runs[i + 1][1] - runs[i][0] + 1 > max_width:
                continue
            if cand is None or gap < runs[cand + 1][0] - runs[cand][1] - 1:
                cand = i
        if cand is None:
            break
        runs[cand:cand + 2] = [(runs[cand][0], runs[cand + 1][1])]
    return runs


def button_rows(m):
    """每行找「81 宽」的蓝色列程（气球里的真按钮）。返回 [(col_start, col_end) | None]。

    多个候选时取「离众数列程最近」的那个，不取第一个：界面气球第 8 颗 capture y=694
    那行，底栏自己的蓝件正好也是 79 宽（647..725），按顺序取会串台、段被劈断。
    众数 = 全帧里出现行数最多的列程，也就是那一条钮列本身，仍然是从帧里量出来的。
    """
    per_row = []
    counts = {}
    for y in range(m.shape[0]):
        cand = [r for r in col_runs(m[y]) if 79 <= r[1] - r[0] + 1 <= 83]
        per_row.append(cand)
        for r in cand:
            counts[r] = counts.get(r, 0) + 1

    if not counts:
        return [None] * m.shape[0]

    ref = max(counts.items(), key=lambda kv: (kv[1], -kv[0][0]))[0]
    hits = []
    for cand in per_row:
        if not cand:
            hits.append(None)
            continue
        hits.append(min(cand, key=lambda r: abs(r[0] - ref[0]) + abs(r[1] - ref[1])))
    return hits


def segments(hits):
    """把「同一条 81 宽列程」连续的列堆成段，返回 [(top, bottom, x0, x1)]。"""
    out = []
    cur = None
    for y, h in enumerate(hits):
        if h and (cur is None or abs(h[0] - cur[2]) <= 1):
            cur = [y, y, h[0], h[1]] if cur is None else [cur[0], y, cur[2], h[1]]
        else:
            if cur:
                out.append(tuple(cur))
            cur = [y, y, h[0], h[1]] if h else None
    if cur:
        out.append(tuple(cur))
    return out


TAIL_CX = 46  # 气球底板下摆尖端的对称中心 x（原生 93 宽画布 = (93-1)/2）


def tail_probe(base_path, open_path, x0, x1, y0, thr=18):
    """第二把尺：不开/开两帧做差，直接量「气球剪影」的下摆尖端。

    为什么还要它：蓝钮行法把气球顶算成「首颗钮顶 - 22」，那个 22 是同族版式的推断值；
    剪影尺只看底板自己的像素（alpha 叠出来的轮廓），尖端中心 x 与末行 y 都是帧上直接读的，
    和「首颗钮在版式里第几行」无关 —— 两条独立来源都命中才算锚定真的落位。
    """
    a = np.array(Image.open(base_path).convert('RGB')).astype(np.int16)
    b = np.array(Image.open(open_path).convert('RGB')).astype(np.int16)
    d = (np.abs(a - b).max(axis=2) > thr)

    rows = []
    for y in range(y0, BTN_TOP + BORDER[1]):		# 上界截在底栏钮上沿，不吃底栏自己的变化
        cols = np.where(d[y, x0:x1])[0] + x0
        rows.append((y, cols.size, int(cols[0]) if cols.size else -1, int(cols[-1]) if cols.size else -1))

    # 从窗口最底往上收「一条比一条窄」的尾巴；宽 >12 就说明收到底板本体了
    tip_rows = []
    for y, n, s, e in reversed(rows):
        if n == 0 or n > 12:
            break
        tip_rows.append((y, n, (s + e) / 2.0))

    if not tip_rows:
        return None
    tip_y = max(r[0] for r in tip_rows)
    tip_x = np.mean([r[2] for r in tip_rows if r[0] >= tip_y - 2])
    body = [(y, n) for y, n, s, e in rows if n >= (x1 - x0) // 2]
    return {'tip_view_y': tip_y - BORDER[1],
            'tip_view_x': tip_x - BORDER[0],
            'tail_rows': len(tip_rows),
            'first_body_view_y': (body[0][0] - BORDER[1]) if body else None}


def main_tail(argv):
    """用法：b6-menu-anchor-check.py --tail <未开帧> <气球帧> <期望气球左上角x> <期望y>

    判据：尖端末行 view y == BTN_TOP-1（间隙 1）；尖端中心 x - 46 == 期望左上角 x（tol 2）。
    """
    base_path, open_path = argv[0], argv[1]
    exp_x = int(argv[2])
    exp_y = int(argv[3])
    art_h = int(argv[4]) if len(argv) > 4 else 140

    # x 窗口按期望左上角铺开设（宽 93，左右各留 30 余量）；y 从气球顶上方 8px 起
    r = tail_probe(base_path, open_path, exp_x + BORDER[0] - 30,
                   exp_x + BORDER[0] + 93 + 30, exp_y + BORDER[1] - 8)
    if not r:
        print('MISSING: 差分里找不到收窄的下摆尖（气球未开 / 该窗口无变化）')
        return 1

    gap = BTN_TOP - r['tip_view_y']
    dx = int(round(r['tip_view_x'])) - (exp_x + TAIL_CX)
    print('剪影：尖角末行 view=(%d,%d)  收边行数=%d  底板首条整宽行 view y=%s'
          % (int(round(r['tip_view_x'])), r['tip_view_y'], r['tail_rows'], r['first_body_view_y']))
    print('尖角中心 x - %d = %d（期望气球左上角 x=%d，Δ=%d）  与底栏钮上沿 %d 的间隙=%d（原生判据 1）'
          % (TAIL_CX, int(round(r['tip_view_x'])) - TAIL_CX, exp_x, dx, BTN_TOP, gap))
    ok = gap == 1 and abs(dx) <= 2
    print('VERDICT: %s' % ('PASS' if ok else 'FAIL'))
    return 0 if ok else 2


def main():
    """用法：b6-menu-anchor-check.py <客户帧> [颗数=4] [期望x=666] [期望y=545] [版式首颗y=22]

    目录 = (4, 666, 545)；界面 = (8, 722, 440)（同一族 93 宽气球，尖角中心 x=46、钮 x=8、版式 y=22+26i）。
    """
    path = sys.argv[1]
    n_btn = int(sys.argv[2]) if len(sys.argv) > 2 else 4
    exp_x = int(sys.argv[3]) if len(sys.argv) > 3 else EXPECT_X
    exp_y = int(sys.argv[4]) if len(sys.argv) > 4 else EXPECT_Y
    btn_y0 = int(sys.argv[5]) if len(sys.argv) > 5 else 22
    art_h = int(sys.argv[6]) if len(sys.argv) > 6 else 140  # 底板高：目录 140 / 界面 245

    fr = np.array(Image.open(path).convert('RGBA'))
    m = blue_rows(fr)

    segs = [s for s in segments(button_rows(m)) if s[1] - s[0] + 1 >= 20]
    print('81 宽蓝钮列段（capture rows / cols）:')
    for (y0, y1, x0, x1) in segs:
        print('   y %d..%d (h=%d)  x %d..%d (w=%d)   -> view y %d..%d'
              % (y0, y1, y1 - y0 + 1, x0, x1, x1 - x0 + 1,
                 y0 - BORDER[1], y1 - BORDER[1]))

    btn = [s for s in segs if s[1] - s[0] + 1 == 25]
    if len(btn) != n_btn:
        print('MISSING: 找不到 %d 段 25 高 / 81 宽蓝钮 -> 气球未开 / 版式不符（got %d）' % (n_btn, len(btn)))
        return 1

    pitch = [btn[i + 1][0] - btn[i][0] for i in range(n_btn - 1)]
    top_view = btn[0][0] - BORDER[1] - btn_y0
    left_view = btn[0][2] - BORDER[0] - 8
    tail_view = top_view + art_h - 1

    print('首颗蓝钮顶 = 气球顶 + %d -> 气球左上角(view)=(%d,%d)；期望=(%d,%d)；Δ=(%d,%d)'
          % (btn_y0, left_view, top_view, exp_x, exp_y,
             left_view - exp_x, top_view - exp_y))
    print('步距=%s（原生 26）  尖角末行 view y=%d  底栏钮上沿 view y=%d  间隙=%d（原生判据 1）'
          % (pitch, tail_view, BTN_TOP, BTN_TOP - tail_view))

    ok = (abs(left_view - exp_x) <= 2
          and abs(top_view - exp_y) <= 2
          and set(pitch) == {26}
          and BTN_TOP - tail_view == 1)
    print('VERDICT: %s' % ('PASS' if ok else 'FAIL'))
    return 0 if ok else 2


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--tail':
        sys.exit(main_tail(sys.argv[2:]))
    sys.exit(main())
