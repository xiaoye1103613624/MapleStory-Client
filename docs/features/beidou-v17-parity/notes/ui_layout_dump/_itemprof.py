import io
from PIL import Image

base = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/"
out = []

def prof(name, x0, x1, y0, y1, axis):
    im = Image.open(base + name).convert("RGBA")
    w, h = im.size
    px = im.load()
    out.append("===== %s %s x=[%d,%d] y=[%d,%d]" % (name, axis, x0, x1, y0, y1))
    if axis == "y":
        for y in range(y0, min(y1, h)):
            s = 0
            n = 0
            for x in range(x0, min(x1, w)):
                r, g, b, a = px[x, y]
                s += (r + g + b) / 3.0
                n += 1
            out.append("y=%3d %6.1f" % (y, s / n))
    else:
        for x in range(x0, min(x1, w)):
            s = 0
            n = 0
            for y in range(y0, min(y1, h)):
                r, g, b, a = px[x, y]
                s += (r + g + b) / 3.0
                n += 1
            out.append("x=%3d %6.1f" % (x, s / n))

prof("item_backgrnd.png", 6, 150, 40, 266, "y")
prof("item_backgrnd.png", 0, 175, 45, 262, "x")

with io.open(base + "_itemprof.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
