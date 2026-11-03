"""B7-4 取证 v2：先对齐（帧与底板差 1~2px），再输出 ASCII 差异地图。

地图上一格 = 8x8 像素，`#` = 该格内差异像素超过 1/4，`.` = 少量，空格 = 底板与帧一致
（也就是「这块不需要客户端画，底板里已经烘好了」）。
"""
import os
import numpy as np
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
DUMP = HERE + r"/_dump"
DESK = r"C:/Users/11036/Desktop/正确客户端截图"
out = []


def analyze(name, sprite, thr=28):
    fr = np.array(Image.open(DESK + '/' + name).convert('RGB')).astype(np.int16)
    sp = np.array(Image.open(DUMP + '/' + sprite).convert('RGB')).astype(np.int16)

    best = None
    for oy in range(-4, 5):
        for ox in range(-4, 5):
            y0, y1 = max(0, oy), min(fr.shape[0], sp.shape[0] + oy)
            x0, x1 = max(0, ox), min(fr.shape[1], sp.shape[1] + ox)
            f = fr[y0:y1, x0:x1]
            s = sp[y0 - oy:y1 - oy, x0 - ox:x1 - ox]
            sad = float(np.abs(f - s).mean())
            if best is None or sad < best[0]:
                best = (sad, ox, oy, x0, x1, y0, y1)

    sad, ox, oy, x0, x1, y0, y1 = best
    f = fr[y0:y1, x0:x1]
    s = sp[y0 - oy:y1 - oy, x0 - ox:x1 - ox]
    d = (np.abs(f - s).max(axis=2) > thr)
    out.append('%s 帧%dx%d / 底板%dx%d  对齐偏移=(%d,%d) 对齐后 SAD=%.2f  差异像素 %d (%.1f%%)'
               % (name, fr.shape[1], fr.shape[0], sp.shape[1], sp.shape[0], ox, oy, sad,
                  d.sum(), 100.0 * d.sum() / d.size))

    # ASCII 地图：8x8 一格
    gh, gw = d.shape[0] // 8, d.shape[1] // 8
    out.append('   列号（每格 8px，格内差异像素数 >=16 记 #，>=4 记 +，>=1 记 .）')
    hdr = '      ' + ''.join(str((gx * 8) // 100 % 10) for gx in range(gw))
    hdr2 = '      ' + ''.join(str((gx * 8) // 10 % 10) for gx in range(gw))
    hdr3 = '      ' + ''.join(str((gx * 8) % 10) for gx in range(gw))
    out.append(hdr)
    out.append(hdr2)
    out.append(hdr3)
    for gy in range(gh):
        row = []
        for gx in range(gw):
            c = int(d[gy * 8:(gy + 1) * 8, gx * 8:(gx + 1) * 8].sum())
            row.append('#' if c >= 16 else ('+' if c >= 4 else ('.' if c >= 1 else ' ')))
        out.append('  y%3d %s' % (gy * 8, ''.join(row)))


analyze('游戏设置.png', 'GameOpt_backgrnd.png')
out.append('')
analyze('系统设置.png', 'SysOpt_backgrnd.png')

with open(HERE + r"/_opt_map.txt", 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
print('ok')
