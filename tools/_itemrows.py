from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\item_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
x = 20
prev = None
for y in range(15, 289):
    c = px[x, y]
    key = (c[0] // 16, c[1] // 16, c[2] // 16)
    if key != prev:
        print('y=%d rgb=%s' % (y, c))
        prev = key
