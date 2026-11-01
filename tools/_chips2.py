"""Extract contiguous chip bands (pink/green) in Stat backgrnd at x=20."""
from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()

def classify(y):
    r, g, b = px[20, y]
    if abs(r - 153) < 40 and abs(g - 51) < 45 and abs(b - 102) < 45:
        return 'PINK'
    if abs(r - 136) < 45 and abs(g - 153) < 45 and abs(b - 51) < 45:
        return 'GREEN'
    return None

bands = []
cur = None
for y in range(H):
    c = classify(y)
    if c and (cur is None or cur[0] != c):
        if cur:
            bands.append(cur)
        cur = [c, y, y]
    elif c and cur and cur[0] == c:
        cur[2] = y
    elif not c and cur:
        bands.append(cur)
        cur = None
if cur:
    bands.append(cur)
for b in bands:
    if b[2] - b[1] >= 4:
        print('%s y=%d..%d h=%d' % (b[0], b[1], b[2], b[2] - b[1] + 1))
