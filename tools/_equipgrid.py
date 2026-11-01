from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\equip_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
print('size', W, H)

# vertical scan at x=20 -> find row structure
print('--- scan x=20 ---')
prev = None
for y in range(15, H):
    c = px[20, y]
    key = (c[0] // 16, c[1] // 16, c[2] // 16)
    if key != prev:
        print('y=%d rgb=%s' % (y, c))
        prev = key
