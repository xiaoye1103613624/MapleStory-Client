# -*- coding: utf-8 -*-
"""UIStatsInfo 值区像素检测 + 放大。"""
from PIL import Image
import numpy as np

OUT = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'

im = Image.open(OUT + '/reg_03_stats.png').convert('RGB')
a = np.asarray(im).astype(int)

# 窗口左上约 (85, 120)（预览*1.6），行 x 值区预览 x60..175 -> 原图 96..280
# 先整窗裁出来
win = im.crop((85, 118, 285, 440))
win = win.resize((win.width * 4, win.height * 4), Image.NEAREST)
win.save(OUT + '/zoom_stats_win.png')
print('stats_win', win.size)

# 值区（窗口内 x 55..185，行 y 预览 65..185 -> 原图相对窗口 (104..280)-(85..)=19..195）
sub = a[118+19:118+200, 85+19:85+195]
dark = (sub.max(axis=2) < 100)
print('dark px total:', int(dark.sum()))
rowsum = dark.sum(axis=1)
for i, v in enumerate(rowsum):
    if v > 3:
        print('row', 118 + 19 + i, 'dark', v)
