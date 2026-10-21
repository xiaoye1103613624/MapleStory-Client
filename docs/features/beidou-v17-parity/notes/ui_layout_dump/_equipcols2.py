import io
from PIL import Image

base = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"
out = []
im = Image.open(base + "equip_backgrnd.png").convert("RGBA")
w, h = im.size
px = im.load()


def val(x, y):
    r, g, b, a = px[x, y]
    return (r + g + b) / 3.0 if a > 10 else -1.0


# 槽位格内部应为浅色（>230），格间为较暗的标签/边框
out.append("size %dx%d" % (w, h))

for y in (40, 73, 106, 139, 172, 205, 238):
    out.append("--- y=%d" % y)
    prev = None
    for x in range(1, w):
        if prev is not None and abs(val(x, y) - prev) >= 12:
            out.append("   x=%d %.1f (prev %.1f)" % (x, val(x, y), prev))
        prev = val(x, y)

with io.open(base + "_equipcols2.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
