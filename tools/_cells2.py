"""Detect slot-cell grid in equip backgrnd: cells are ~(187,204,221)."""
import sys
from PIL import Image

path = sys.argv[1] if len(sys.argv) > 1 else r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png'
im = Image.open(path).convert('RGB')
W, H = im.size
px = im.load()

def is_cell(x, y):
    r, g, b = px[x, y]
    return abs(r - 187) <= 12 and abs(g - 204) <= 12 and abs(b - 221) <= 12

# row profile
rows = []
for y in range(H):
    n = sum(1 for x in range(W) if is_cell(x, y))
    rows.append(n)

# find row bands where n > 10
bands = []
y = 0
while y < H:
    if rows[y] > 10:
        y0 = y
        while y < H and rows[y] > 10:
            y += 1
        bands.append((y0, y - 1))
    else:
        y += 1
print('row bands (y0,y1,height):')
for b in bands:
    print('  ', b, b[1] - b[0] + 1)

# for each band, find x runs
print('x runs per band:')
for (y0, y1) in bands:
    ym = (y0 + y1) // 2
    runs = []
    x = 0
    while x < W:
        if is_cell(x, ym):
            x0 = x
            while x < W and is_cell(x, ym):
                x += 1
            if x - x0 >= 8:
                runs.append((x0, x - 1))
        else:
            x += 1
    print('  band y=%d..%d:' % (y0, y1), runs)
