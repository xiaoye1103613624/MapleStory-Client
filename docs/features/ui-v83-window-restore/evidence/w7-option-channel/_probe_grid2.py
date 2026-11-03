"""UIChannel 余下格位：ch[i] 标签、world[i] 世界名、channel1 灰板 在原生帧里的落位。"""
import os
import numpy as np
from PIL import Image

NATIVE = r"C:/Users/11036/Desktop/正确客户端截图/频道变更.png"
HERE = os.path.dirname(os.path.abspath(__file__))
DUMP = HERE + r"/_dump"

n = np.array(Image.open(NATIVE).convert('RGB')).astype(np.int16)
lines = ['native %dx%d' % (n.shape[1], n.shape[0])]


def load(fn):
    a = np.array(Image.open(DUMP + '/' + fn).convert('RGBA')).astype(np.int16)
    return a[:, :, :3], a[:, :, 3]


def match(fn, x0, x1, y0, y1, amin=200):
    rgb, al = load(fn)
    m = al > amin
    best = (1e9, None, None)
    for oy in range(y0, y1 - rgb.shape[0] + 1):
        for ox in range(x0, x1 - rgb.shape[1] + 1):
            p = n[oy:oy + rgb.shape[0], ox:ox + rgb.shape[1]]
            sad = float(np.abs(p[m] - rgb[m]).mean())
            if sad < best[0]:
                best = (sad, ox, oy)
    return best


for i in range(6):
    lines.append('ch_%d   native=(%.2f @ x=%s y=%s)' % ((i,) + match('ch_%d.png' % i, 0, 428, 60, 90, 100)))
for i in range(3):
    lines.append('world_%d native=(%.2f @ x=%s y=%s)' % ((i,) + match('world_%d.png' % i, 0, 200, 20, 60, 100)))

# channel1（灰板）在 2、3 号槽的落位：限定 x>=70
c1rgb, c1al = load('channel1.png')
m1 = c1al > 200
for x in (76, 146):
    sad = float(np.abs(n[66:86, x:x + 68][m1] - c1rgb[m1]).mean())
    lines.append('channel1 @ native x=%d y=66 -> SAD=%.2f' % (x, sad))

with open(HERE + r"/_grid2.txt", 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print('ok')
