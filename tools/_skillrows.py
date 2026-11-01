from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\skill_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
print('size', W, H)
for x in (20, 100):
    print('--- scan x=%d ---' % x)
    prev = None
    for y in range(15, H):
        c = px[x, y]
        key = (c[0] // 16, c[1] // 16, c[2] // 16)
        if key != prev:
            print('y=%d rgb=%s' % (y, c))
            prev = key
