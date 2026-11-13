# -*- coding: utf-8 -*-
"""金币槽周边更大范围：查数字是否画在框外/偏移。"""
from PIL import Image
import numpy as np

OUT = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'

im = Image.open(OUT + '/reg_04_inventory.png').convert('RGB')
# 道具窗整体（预览 x8..180 y55..215 -> 原图 x13..288 y88..344）
c = im.crop((13, 88, 288, 350))
c = c.resize((c.width * 3, c.height * 3), Image.NEAREST)
c.save(OUT + '/zoom_inv_full.png')
print('inv_full', c.size)

# 数字墨迹检测：白框内找非白像素行（原图坐标）
a = np.asarray(im).astype(int)
# 白框大致 y333..365 x20..270；扩到 y320..380 x10..288
reg = a[320:380, 10:288]
dev = np.abs(reg - reg).sum()  # placeholder
# 黑字判据：三通道都 < 120
dark = (reg.max(axis=2) < 120)
rows = np.where(dark.sum(axis=1) > 2)[0]
cols = np.where(dark.sum(axis=0) > 1)[0]
print('dark rows (y-320):', rows[:20], 'cols (x-10):', cols[:30])
