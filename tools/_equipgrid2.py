from PIL import Image
for name in ('equip_backgrnd', 'item_backgrnd', 'stat_backgrnd'):
    im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\%s.png' % name).convert('RGB')
    W, H = im.size
    px = im.load()
    print('=== %s %dx%d ===' % (name, W, H))
    for y in range(20, H - 1):
        dark = 0
        for x in range(6, 168):
            r, g, b = px[x, y]
            if r + g + b < 520:
                dark += 1
        if dark > 100:
            print('y=%d dark=%d' % (y, dark))
