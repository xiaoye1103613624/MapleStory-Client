"""Detect colored label-chip rows in Stat backgrnd via saturation at x=20."""
from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
prev = False
for y in range(H):
    r, g, b = px[20, y]
    sat = max(r, g, b) - min(r, g, b)
    cur = sat > 50
    if cur and not prev:
        print('chip start y=%d rgb=(%d,%d,%d)' % (y, r, g, b))
    prev = cur
