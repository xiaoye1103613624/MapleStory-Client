# -*- coding: utf-8 -*-
from PIL import Image

OUT = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107'

jobs = [
    ('reg_08_chatbar.png', 'zoom_chatbar2.png', (0, 690, 1100, 730), 2),
    ('reg_01_game.png', 'zoom_chatbar1.png', (0, 690, 1100, 730), 2),
    ('reg_07a_minimap_1.png', 'zoom_mm_a.png', (10, 60, 540, 220), 2),
    ('reg_07b_minimap_2.png', 'zoom_mm_b.png', (10, 60, 540, 220), 2),
    ('reg_07c_minimap_3.png', 'zoom_mm_c.png', (10, 60, 540, 220), 2),
    ('reg_06c_macro_after_close.png', 'zoom_afterclose.png', (0, 40, 1296, 400), 1),
]

for src, dst, box, scale in jobs:
    im = Image.open(OUT + '/' + src)
    c = im.crop(box)
    if scale > 1:
        c = c.resize((c.width * scale, c.height * scale), Image.NEAREST)
    c.save(OUT + '/' + dst)
    print(dst, c.size)
