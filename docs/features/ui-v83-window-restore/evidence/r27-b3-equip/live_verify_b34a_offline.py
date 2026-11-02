# -*- coding: utf-8 -*-
"""离线复算 J3b/J3c：用已落盘的真机帧跑脚本里的同一套公式。

真机重跑需要客户端回到世界内（登录口令只走 R27_USER/R27_PASS，不入库）；
本脚本保证「归档的判据」与「脚本里的判据」是同一份代码路径。
"""
import json
import sys

import numpy as np
from PIL import Image

sys.path.insert(0, 'E:/project')
sys.argv = ['x', '12256942']          # 探针脚本要求 argv[1]=hwnd，这里不发送任何输入
import importlib.util  # noqa: E402

spec = importlib.util.spec_from_file_location('h', 'E:/project/_r27_b34a_hover.py')
h = importlib.util.module_from_spec(spec)
spec.loader.exec_module(h)

ROWS = 4
data = json.load(open(h.OUT + '.json', encoding='utf-8'))
bbox = {(p['row'], p['probe']): p['bbox'] for p in data['probes']}
rec = {(p['row'], p['probe']): p for p in data['probes']}
base = np.asarray(Image.open(h.OUT + '_baseline.png').convert('RGB')).astype(np.int16)

sigs = {}
for r in range(ROWS):
    for p in ('in-TL', 'in-C', 'in-BR'):
        cur = np.asarray(Image.open('%s_r%d_%s.png' % (h.OUT, r, p)).convert('RGB')).astype(np.int16)
        _, _, sig = h.largest_blob(base, cur, rec[(r, p)]['frame'])
        sigs[(r, p)] = sig

out = []
for probe in ('in-TL', 'in-BR'):
    for r in range(ROWS):
        ds = [(h.mask_disagree(sigs[(r, probe)], sigs[(rr, 'in-C')]), rr) for rr in range(ROWS)]
        same = min(d for d, rr in ds if rr == r)
        cross = min(d for d, rr in ds if rr != r)
        out.append(dict(probe=probe, row=int(r), same=round(same, 4), cross=round(cross, 4),
                        margin=round(cross - same, 4), win=bool(same < cross)))

tips = h.sheet([(r, bbox[(r, 'in-C')]) for r in range(ROWS)], 'in-C', h.OUT + '_tips.png')
names = h.sheet([(r, (h.cell_frame(r)[0] + 34, h.cell_frame(r)[1] - 8,
                      h.cell_frame(r)[0] + 200, h.cell_frame(r)[1] + 16))
                 for r in range(ROWS)], 'baseline', h.OUT + '_rownames.png')

with open('E:/project/_r27_b34a_j3b.json', 'w', encoding='utf-8') as fh:
    json.dump(out, fh, ensure_ascii=False, indent=1)
with open('E:/project/_r27_b34a_j3b.txt', 'w', encoding='utf-8') as fh:
    fh.write('J3b 1-NN（离线复算，帧来自 2026-11-03 真机 hwnd=12256942）\n')
    for x in out:
        fh.write('  %-6s row%d  同格=%.4f  最近跨行=%.4f  余量=%+.4f  %s\n'
                 % (x['probe'], x['row'], x['same'], x['cross'], x['margin'],
                    'ok' if x['win'] else 'X'))
    fh.write('全过=%s\n' % all(x['win'] for x in out))
    fh.write('J3c 裁片=%s , %s\n' % (tips, names))
print('done')
