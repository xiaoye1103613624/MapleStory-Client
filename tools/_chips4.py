"""Clean chip-top detection: pink/green runs >=10px at x=30."""
from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()

def cls(y):
    r, g, b = px[30, y]
    if r > 120 and r - g > 50 and 70 < b < 160:
        return 'P'
    if g > 110 and g - r > 25 and g - b > 35 and r < 170:
        return 'G'
    return '.'

runs = []
cur = None
for y in range(H):
    c = cls(y)
    if c != '.':
        if cur and cur[0] == c:
            cur[2] = y
        else:
            if cur:
                runs.append(cur)
            cur = [c, y, y]
    else:
        if cur:
            runs.append(cur)
            cur = None
if cur:
    runs.append(cur)
for r in runs:
    if r[2] - r[1] >= 9:
        print('%s top=%d bottom=%d h=%d' % (r[0], r[1], r[2], r[2] - r[1] + 1))
