from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\item_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
y = 60
prev = None
for x in range(0, W):
    c = px[x, y]
    key = (c[0] // 16, c[1] // 16, c[2] // 16)
    if key != prev:
        print('x=%d rgb=%s' % (x, c))
        prev = key
