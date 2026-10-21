import io
from PIL import Image

base = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"
out = []

for name in ["item_backgrnd.png", "equip_backgrnd.png"]:
    im = Image.open(base + name).convert("RGBA")
    w, h = im.size
    px = im.load()
    out.append("===== %s %dx%d" % (name, w, h))

    # 横向格线：某一行 y 上，x=8..150 中“暗像素(亮度<150)”的数量
    for y in range(0, h):
        cnt = 0
        for x in range(8, min(150, w)):
            r, g, b, a = px[x, y]
            if a > 10 and (r + g + b) / 3.0 < 150:
                cnt += 1
        if cnt >= 100:
            out.append("hline y=%d dark=%d" % (y, cnt))

    # 纵向格线：某一列 x 上，y=30..260 中暗像素数量
    for x in range(0, w):
        cnt = 0
        for y in range(30, min(265, h)):
            r, g, b, a = px[x, y]
            if a > 10 and (r + g + b) / 3.0 < 150:
                cnt += 1
        if cnt >= 150:
            out.append("vline x=%d dark=%d" % (x, cnt))

with io.open(base + "_itemgrid2.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
