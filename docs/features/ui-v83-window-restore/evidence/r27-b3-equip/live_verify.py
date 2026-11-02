# -*- coding: utf-8 -*-
"""R27-B3 真机逐格验收：客户帧上的粉格 == 调色核作用在 Equip/backgrnd 上的结果。

锚 = anchor_in(frame, Equip_backgrnd.png)；判据 = 对底图每个不透明像素跑 C++ 侧
同一套整数式（17*((3C+306)/68) / 17*((3C+33)/68)），与帧逐格比对命中率，
并用「不该染」的控制格反向断言（粉色列占比 < 5%）。
"""
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\tools')
import layout as L

FRAME = r'E:/project/_r27_win_e.png'
TMPL = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r27-oracle\tmpl\Equip_backgrnd.png'
ART = r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r27-b3-equip\Equip_backgrnd.png'

# 与 UIEquipInventory.cpp 同一张 iconpositions（调色矩形 = 格 + (0,3)，32x32）
CELLS = [
    ('TAMEDMOB 骑兽', (5, 230), True),
    ('SADDLE   鞍子', (38, 230), True),
    ('SHIELD   盾牌', (137, 131), False),
    ('WEAPON   武器', (104, 131), False),
    ('HAT      帽子', (71, 32), False),
    ('SHOES    鞋子', (71, 197), False),
    ('ROW7C3   行7第3格', (71, 230), None),   # 归属未定，只记录不断言
]

PINKS = ((255, 187, 187), (204, 153, 170))


def tint(a):
    r = 17 * ((3 * a[:, :, 0].astype(np.int64) + 306) // 68)
    g = 17 * ((3 * a[:, :, 1].astype(np.int64) + 33) // 68)
    b = 17 * ((3 * a[:, :, 2].astype(np.int64) + 33) // 68)
    return np.dstack([r, g, b])


def main():
    f = L.load_rgb(FRAME)
    a = L.anchor_in(f, TMPL)
    if a is None:
        raise SystemExit('NO ANCHOR')
    ax, ay, rmse, agree = a
    art = np.asarray(Image.open(ART).convert('RGBA'))
    fp = np.asarray(Image.open(FRAME).convert('RGB')).astype(np.int64)

    lines = ['anchor=(%d,%d) rmse=%.1f agree=%.4f' % (ax, ay, rmse, agree)]
    ok = True

    for name, (ix, iy), want in CELLS:
        x0, y0 = ix, iy + 3
        sub = art[y0:y0 + 32, x0:x0 + 32]
        msk = sub[:, :, 3] >= 250
        n = int(msk.sum())
        if n == 0:
            lines.append('%-18s 底图该格全透明，跳过' % name)
            continue
        pred = tint(sub[:, :, :3])[msk]
        plain = sub[:, :, :3].astype(np.int64)[msk]
        frame = fp[ay + y0:ay + y0 + 32, ax + x0:ax + x0 + 32][msk]
        hit = float((np.abs(frame - pred).max(axis=1) == 0).mean())
        same = float((np.abs(frame - plain).max(axis=1) == 0).mean())
        if want is None:
            lines.append('%-18s n=%4d 调色核命中=%6.2f%% 原样命中=%6.2f%% (未定归属，不断言)'
                         % (name, n, hit * 100, same * 100))
            continue
        # 判据 = 「这一格的像素 == 调色核作用在底图上的结果」，不是「粉色占比够不够高」。
        # 原生改色保留网点/渐变结构，一格里有多种色阶，粉色列只占 ~46%，按颜色计数会误判。
        good = (hit >= 0.90 and same < 0.10) if want else (same >= 0.90)
        ok = ok and good
        lines.append('%-18s n=%4d 调色核命中=%6.2f%% 原样命中=%6.2f%% 期望=%s %s'
                     % (name, n, hit * 100, same * 100,
                        '染' if want else '不染', 'OK' if good else 'FAIL'))

    verdict = 'PASS' if ok else 'FAIL'
    lines.append('')
    lines.append('总判: %s' % verdict)
    text = '\n'.join(lines)
    with open(r'E:/project/_b3_live_verify.txt', 'w', encoding='utf-8') as fh:
        fh.write(text + '\n')
    print(text.encode('ascii', 'replace').decode())
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
