"""Overlay a measurement grid on the equip backgrnd for visual slot-position reading."""
import sys
from PIL import Image, ImageDraw

path = sys.argv[1]
im = Image.open(path).convert('RGB')
W, H = im.size
d = ImageDraw.Draw(im)
# grid every 10px light, every 33px strong
for x in range(0, W, 10):
    d.line([(x, 0), (x, H)], fill=(255, 0, 0) if x % 33 == 0 else (255, 200, 200))
for y in range(0, H, 10):
    d.line([(0, y), (W, y)], fill=(0, 0, 255) if y % 33 == 0 else (200, 200, 255))
# axis labels every 33
for x in range(0, W, 33):
    d.text((x + 1, 1), str(x), fill=(200, 0, 0))
for y in range(0, H, 33):
    d.text((1, y + 1), str(y), fill=(0, 0, 200))
out = path.replace('.png', '_grid.png')
im.save(out)
print('saved', out, im.size)
