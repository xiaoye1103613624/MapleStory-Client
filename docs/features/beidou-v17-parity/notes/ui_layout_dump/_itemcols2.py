import io
from PIL import Image

base = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"
out = []
im = Image.open(base + "item_backgrnd.png").convert("RGBA")
w, h = im.size
px = im.load()

for y in (60, 100, 134, 168, 202, 236):
    vals = []
    for x in range(0, w):
        r, g, b, a = px[x, y]
        vals.append((r + g + b) / 3.0 if a > 10 else -1.0)
    out.append("--- row y=%d" % y)
    prev = None
    for x in range(1, w):
        if prev is not None and abs(vals[x] - prev) >= 5:
            out.append("   x=%d %.1f (prev %.1f)" % (x, vals[x], prev))
        prev = vals[x]

with io.open(base + "_itemcols2.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
