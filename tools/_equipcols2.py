from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
print('--- vertical bright lines (y=32..295) ---')
for x in range(0, W):
    brt = 0
    for y in range(32, 296):
        r, g, b = px[x, y]
        if r > 248 and g > 248 and b > 248:
            brt += 1
    if brt > 150:
        print('x=%d bright=%d' % (x, brt))
