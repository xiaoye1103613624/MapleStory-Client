from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
for y in (50, 120, 250):
    print('--- scan y=%d ---' % y)
    prev = None
    for x in range(0, W):
        c = px[x, y]
        key = (c[0] // 16, c[1] // 16, c[2] // 16)
        if key != prev:
            print('x=%d rgb=%s' % (x, c))
            prev = key
