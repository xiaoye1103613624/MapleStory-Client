import io
from PIL import Image

SRC = r"C:/Users/11036/.qoder/tmp/E--project-MapleStory-Client/images/0a2cb70c-a3a9-453e-8f4b-ad1f92fc6066/7780ed0f-ce55-4ecf-aab9-0a66e53e8568.png"
out = io.open(r"E:/project/_r25_dlg_sum.txt", "w", encoding="utf-8")
im = Image.open(SRC).convert("RGB")
W, H = im.size
px = im.load()


def m(p):
    return (p[0] + p[1] + p[2]) // 3


def dark(x, y, thr=170):
    return m(px[x, y]) < thr


def runs(idx, pred):
    """contiguous runs where pred true, as (start,end,len)."""
    r = []
    s = None
    for i in idx:
        if pred(i):
            if s is None:
                s = i
        else:
            if s is not None:
                r.append((s, i - 1, i - s))
                s = None
    if s is not None:
        r.append((s, idx[-1], idx[-1] - s + 1))
    return r


# 1) text horizontal extent, using only the 4 solid text lines y=58..123
cols = [x for x in range(W) if sum(1 for y in range(58, 124) if dark(x, y)) >= 3]
out.write("== text cols (y58-123, >=3 dark): first=%d last=%d n=%d\n" % (cols[0], cols[-1], len(cols)))
out.write("   runs(len>=2): %s\n" % [r for r in runs(range(W), lambda x: x in set(cols)) if r[2] >= 2])

# 2) all text line bands across whole window (x restricted to 165..505)
out.write("\n== line bands (rows with >=8 dark px in x165..505)\n")
for s, e, n in runs(range(15, 219), lambda y: sum(1 for x in range(165, 505) if dark(x, y)) >= 8):
    out.write("   y %3d..%3d  h=%d\n" % (s, e, n))

# 3) portrait / coloured art bbox: pixels whose channel spread is large
def sat(x, y):
    p = px[x, y]
    return max(p) - min(p) > 60


xs = [x for x in range(W) if sum(1 for y in range(15, 219) if sat(x, y)) >= 6]
out.write("\n== saturated cols>=6 -> runs: %s\n" % [r for r in runs(range(W), lambda x: x in set(xs)) if r[2] >= 3])

# 4) bottom band: find the button row (y>200)
out.write("\n== rows y=200..260 dark counts (whole width)\n")
for y in range(200, H):
    n = sum(1 for x in range(W) if dark(x, y))
    out.write("  y%3d %4d %s\n" % (y, n, "#" * min(n // 3, 110)))

# 5) column profile for y=225..258 (button band)
cols2 = [x for x in range(W) if sum(1 for y in range(225, 258) if dark(x, y)) >= 6]
out.write("\n== button-band dark cols runs: %s\n" % [r for r in runs(range(W), lambda x: x in set(cols2)) if r[2] >= 2])

# 6) light-parchment (content) region: find where bg is light per row/col
out.write("\n== light-run profile: for chosen rows, runs of light (m>200)\n")
for y in (30, 100, 150, 200, 210, 218):
    r = [run for run in runs(range(W), lambda x: m(px[x, y]) > 200) if run[2] > 20]
    out.write("  y%3d light runs: %s\n" % (y, r))
for x in (20, 100, 170, 300, 450, 520):
    r = [run for run in runs(range(H), lambda y: m(px[x, y]) > 200) if run[2] > 20]
    out.write("  x%3d light runs: %s\n" % (x, r))
out.close()
print("done")
