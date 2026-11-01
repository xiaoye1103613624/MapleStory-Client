from PIL import Image
im = Image.open(r'E:\project\MapleStory-Client\docs\features\beidou-v17-parity\notes\ui_layout_dump\stat_backgrnd.png').convert('RGB')
W, H = im.size
px = im.load()
for y in range(180, 246):
    runs = []
    x = 60
    while x < 200:
        r, g, b = px[x, y]
        if r > 240 and g > 240 and b > 240:
            x0 = x
            while x < 200 and px[x, y][0] > 240 and px[x, y][1] > 240 and px[x, y][2] > 240:
                x += 1
            if x - x0 >= 6:
                runs.append((x0, x - 1))
        else:
            x += 1
    if runs:
        print('y=%d white runs: %s' % (y, runs))
