"""Cluster pink/green chip pixels in Stat backgrnd into rectangles."""
from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()

pink = set()
green = set()
for y in range(H):
    for x in range(W):
        r, g, b = px[x, y]
        if r > 120 and r - g > 50 and 70 < b < 160 and g < 110:
            pink.add((x, y))
        elif g > 110 and g - r > 25 and g - b > 35 and r < 170:
            green.add((x, y))

def clusters(pix, tag):
    ys = sorted(set(y for (x, y) in pix))
    bands = []
    for y in ys:
        if bands and y - bands[-1][1] <= 2:
            bands[-1][1] = y
        else:
            bands.append([y, y])
    print('---', tag, '---')
    for (y0, y1) in bands:
        xs = [x for (x, y) in pix if y0 <= y <= y1]
        if len(xs) > 100:
            print('  y=%d..%d x=%d..%d' % (y0, y1, min(xs), max(xs)))

clusters(pink, 'PINK')
clusters(green, 'GREEN')
