from PIL import Image
for name in ('equip_backgrnd', 'item_backgrnd', 'stat_backgrnd'):
    im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\%s.png' % name).convert('RGB')
    W, H = im.size
    px = im.load()
    print('=== %s %dx%d ===' % (name, W, H))
    for y in range(20, H - 1):
        brt = 0
        for x in range(6, 168):
            r, g, b = px[x, y]
            if r > 248 and g > 248 and b > 248:
                brt += 1
        if brt > 120:
            print('y=%d bright=%d' % (y, brt))
