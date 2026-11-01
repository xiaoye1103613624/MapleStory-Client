"""Detect grid border lines (dark rows/cols) in a window backgrnd."""
import sys
from PIL import Image

path = sys.argv[1]
im = Image.open(path).convert('RGB')
W, H = im.size
px = im.load()

bg = px[W // 2, 2]
def ink(x, y):
    r, g, b = px[x, y]
    return abs(r - bg[0]) + abs(g - bg[1]) + abs(b - bg[2]) > 90

# vertical lines: columns with high ink count in the grid region
print('vertical line candidates (x, count):')
for x in range(W):
    n = sum(1 for y in range(20, H - 25) if ink(x, y))
    if n > (H - 45) * 0.6:
        print('  x=%d n=%d' % (x, n))
print('horizontal line candidates (y, count):')
for y in range(20, H - 25):
    n = sum(1 for x in range(2, W - 2) if ink(x, y))
    if n > (W - 4) * 0.6:
        print('  y=%d n=%d' % (y, n))
