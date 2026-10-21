import io
from PIL import Image

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/skill_backgrnd.png"
im = Image.open(p).convert("RGB")
w, h = im.size
px = im.load()

out = []
out.append("size=%dx%d" % (w, h))

# 扫描列表区：取 x=20..150 的平均亮度，找突变（分隔线/带边界）
def rowmean(y, x0=20, x1=150):
    s = 0
    n = 0
    for x in range(x0, min(x1, w)):
        r, g, b = px[x, y]
        s += (r + g + b) / 3.0
        n += 1
    return s / n

prevm = rowmean(40)
for y in range(41, h):
    m = rowmean(y)
    d = m - prevm
    if abs(d) >= 4.0:
        out.append("y=%3d mean=%6.1f delta=%6.1f" % (y, m, d))
    prevm = m

with io.open(r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/_skillbands.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(out))
print("ok")
