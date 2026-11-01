"""Analyze equip backgrnd PNG: find slot cell rectangles (light blue cells)."""
import sys
from PIL import Image

path = sys.argv[1] if len(sys.argv) > 1 else r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png'
im = Image.open(path).convert('RGB')
W, H = im.size
print('size', W, H)
px = im.load()

# Sample colors along a horizontal line through the grid to identify cell fills.
# Print unique colors with counts (top 15)
from collections import Counter
c = Counter()
for y in range(0, H, 2):
    for x in range(0, W, 2):
        c[px[x, y]] += 1
for col, n in c.most_common(15):
    print(col, n)
