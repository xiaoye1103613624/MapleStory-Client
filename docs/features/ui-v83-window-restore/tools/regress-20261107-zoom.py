# -*- coding: utf-8 -*-
"""regress 帧局部放大 v3：预览(810x475)→原图(1296x759) 换算 1.6x。"""
from PIL import Image

OUT = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'

jobs = [
    # 金币槽：预览 (10..180, 208..228) -> 原图 (16..288, 333..365)
    ('reg_04_inventory.png', 'zoom_meso.png', (16, 333, 288, 365), 4),
    # 快捷键栏：预览 (520..810, 415..470) -> 原图 (832..1296, 664..752)
    ('reg_01_game.png', 'zoom_hotkeys.png', (832, 664, 1296, 752), 3),
    # 底部信息栏：预览 (0..330, 435..475) -> 原图 (0..528, 696..759)
    ('reg_01_game.png', 'zoom_infobar.png', (0, 696, 528, 759), 3),
    # 小地图顶带：预览 (8..60, 40..75) -> 原图 (13..96, 64..120)
    ('reg_01_game.png', 'zoom_minimap_top.png', (13, 64, 340, 120), 3),
    # 聊天栏：预览 (0..660, 420..435) -> 原图 (0..1056, 672..700)
    ('reg_08_chatbar.png', 'zoom_chatbar.png', (0, 668, 1060, 704), 2),
]

for src, dst, box, scale in jobs:
    im = Image.open(OUT + '/' + src)
    c = im.crop(box)
    c = c.resize((c.width * scale, c.height * scale), Image.NEAREST)
    c.save(OUT + '/' + dst)
    print(dst, c.size)
