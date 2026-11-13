# -*- coding: utf-8 -*-
"""全帧扫描：statlabels 黑字是否被 y_for_ink_top 甩到窗口外别处。
判据：纯黑 (0,0,0)±40 的成簇像素（黑字 A11M value_color=BLACK）。
排除地图背景黑（大片连续黑），只找 1..12px 高、1..60px 宽的字形簇。"""
from PIL import Image
import numpy as np

OUT = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'

for name in ('reg_03_stats.png', 'reg_04_inventory.png'):
    im = Image.open(OUT + '/' + name).convert('RGB')
    a = np.asarray(im).astype(int)
    h, w = a.shape[:2]
    black = (a.max(axis=2) < 60)
    # 排除背景：背景黑是大片连通；用行内 black 计数 3..80 的窄带找文字
    print('===', name, w, h)
    # 窗口区域附近 (60..320, 100..460) 全查
    sub = black[100:460, 60:320]
    rows = np.where(sub.sum(axis=1) >= 2)[0]
    # 聚簇
    clusters = []
    cur = []
    prev = -10
    for r in rows:
        if r - prev > 3:
            if cur:
                clusters.append((cur[0], cur[-1]))
            cur = [r]
        else:
            cur.append(r)
        prev = r
    if cur:
        clusters.append((cur[0], cur[-1]))
    for c0, c1 in clusters:
        seg = sub[c0:c1+1]
        cols = np.where(seg.sum(axis=0) > 0)[0]
        if len(cols) == 0:
            continue
        n = int(seg.sum())
        width_span = int(cols.max() - cols.min()) + 1
        # 文字簇：像素数 >=6、横向跨度 8..120、纵向 4..16
        if n >= 6 and 8 <= width_span <= 120 and (c1 - c0) <= 16:
            print('  ink cluster y=%d..%d x=%d..%d px=%d' % (
                c0 + 100, c1 + 100, cols.min() + 60, cols.max() + 60, n))
