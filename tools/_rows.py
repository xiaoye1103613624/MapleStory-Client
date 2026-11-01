"""Measure label-chip row positions in a window backgrnd via left-column ink profile."""
import sys
from PIL import Image

path = sys.argv[1]
x0 = int(sys.argv[2]) if len(sys.argv) > 2 else 6
x1 = int(sys.argv[3]) if len(sys.argv) > 3 else 56
im = Image.open(path).convert('RGB')
W, H = im.size
px = im.load()

# background = the light window fill; sample corner
bg = px[W // 2, 2]
def is_ink(x, y):
    r, g, b = px[x, y]
    return abs(r - bg[0]) + abs(g - bg[1]) + abs(b - bg[2]) > 60

prof = []
for y in range(H):
    n = sum(1 for x in range(x0, x1) if is_ink(x, y))
    prof.append(n)

bands = []
y = 0
while y < H:
    if prof[y] > 3:
        y0 = y
        while y < H and prof[y] > 3:
            y += 1
        if y - y0 >= 6:
            bands.append((y0, y - 1))
    else:
        y += 1
print(path, 'size', W, H)
for b in bands:
    print('  chip band y=%d..%d h=%d center=%d' % (b[0], b[1], b[1] - b[0] + 1, (b[0] + b[1]) // 2))
