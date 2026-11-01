"""Row profile of pink/green chip pixels across left region x=8..55."""
from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()

def is_pink(x, y):
    r, g, b = px[x, y]
    return r > 120 and g < 110 and b > 70 and b < 150 and r - g > 50

def is_green(x, y):
    r, g, b = px[x, y]
    return g > 110 and r < 160 and b < 110 and g - r > 30 and g - b > 40

for y in range(H):
    np_ = sum(1 for x in range(8, 56) if is_pink(x, y))
    ng = sum(1 for x in range(8, 56) if is_green(x, y))
    if np_ > 20 or ng > 20:
        print('y=%d pink=%d green=%d' % (y, np_, ng))
