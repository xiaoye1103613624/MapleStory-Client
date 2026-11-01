import io
from PIL import Image

SRC = r"C:/Users/11036/.qoder/tmp/E--project-MapleStory-Client/images/0a2cb70c-a3a9-453e-8f4b-ad1f92fc6066/7780ed0f-ce55-4ecf-aab9-0a66e53e8568.png"
out = io.open(r"E:/project/_r25_dlg_measure.txt", "w", encoding="utf-8")

im = Image.open(SRC).convert("RGB")
W, H = im.size
px = im.load()
out.write("size=%dx%d\n" % (W, H))


def cls(p):
    r, g, b = p
    mx = max(r, g, b)
    mn = min(r, g, b)
    if mx > 200 and mx - mn < 40:
        return "."          # light parchment
    if mx < 140 and mx - mn < 45:
        return "#"          # dark, low-saturation -> text/outline
    return "o"              # saturated -> portrait / button art


# 1) row profile: count of "#" per row
out.write("\n== per-row dark(cheap text) counts, y=0..H-1\n")
for y in range(H):
    c = 0
    for x in range(W):
        if cls(px[x, y]) == "#":
            c += 1
    out.write("y=%3d %4d %s\n" % (y, c, "#" * min(c, 90)))

# 2) column profile over the text rows only (chosen after looking); use y=20..130
out.write("\n== per-col dark counts over y=18..130 (x=60..W-1)\n")
for x in range(60, W):
    c = 0
    for y in range(18, 130):
        if cls(px[x, y]) == "#":
            c += 1
    out.write("x=%3d %3d %s\n" % (x, c, "#" * min(c, 60)))

# 3) saturated (portrait) extent, rows 15..115
out.write("\n== per-col saturated counts over y=15..115\n")
for x in range(0, W):
    c = 0
    for y in range(15, 115):
        if cls(px[x, y]) == "o":
            c += 1
    if c:
        out.write("x=%3d %3d\n" % (x, c))

out.close()
print("done")
