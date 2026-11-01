from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\item_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
for y in range(20, 290):
    runs = []
    x = 5
    while x < 140:
        r, g, b = px[x, y]
        if r > 235 and g > 235 and b > 225:
            x0 = x
            while x < 140 and px[x, y][0] > 235 and px[x, y][1] > 235 and px[x, y][2] > 225:
                x += 1
            if x - x0 >= 20:
                runs.append((x0, x - 1))
        else:
            x += 1
    if runs:
        print('y=%d runs: %s' % (y, runs))
