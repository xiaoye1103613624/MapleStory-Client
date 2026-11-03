"""逐行剖面：原生 频道变更.png 与 Channel/t,s,c 三条画布的对应关系。

判据全部从帧里量，不写死结论。
"""
import numpy as np
from PIL import Image

NATIVE = r"C:/Users/11036/Desktop/正确客户端截图/频道变更.png"
DUMP = r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/evidence/w7-option-channel/_dump"

out = []


def load(p):
    return np.array(Image.open(p).convert('RGB')).astype(np.int16)


n = load(NATIVE)
out.append('native %dx%d (w=%d h=%d)' % (n.shape[1], n.shape[0], n.shape[1], n.shape[0]))

# 1) 每行的「左右边缘是否不透明」和「中位色」，用来找条带边界
for name in ('t', 's', 'c'):
    a = load('%s/%s.png' % (DUMP, name))
    out.append('%s: %dx%d' % (name, a.shape[1], a.shape[0]))

# 2) 原生帧：按行给出该行不同颜色的占比摘要（量化到 32 的格子）
def rowq(row):
    q = (row // 32) * 32
    return tuple(q[len(q) // 2])


out.append('')
out.append('--- native rows: y, 左边缘RGB, 中心RGB, 右边缘RGB, 量化中位 ---')
for y in range(n.shape[0]):
    r = n[y]
    if y % 1 == 0:
        out.append('%3d  L=%s  C=%s  R=%s' % (
            y, tuple(r[2]), tuple(r[r.shape[0] // 2]), tuple(r[r.shape[0] - 3])))

with open(r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/evidence/w7-option-channel/_rows.txt", 'w', encoding='utf-8') as f:
    f.write('\n'.join(out))
print('ok')
