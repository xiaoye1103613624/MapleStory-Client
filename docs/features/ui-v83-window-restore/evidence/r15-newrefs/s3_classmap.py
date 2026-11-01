# r15 s3: per-pixel colour-class ASCII map + dark-border run analysis.
# Small images only (img1 94x278, img2 82x182, img5 234x67).
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
PATHS = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
}
def cls(p):
    r, g, b, a = p[0], p[1], p[2], p[3]
    s = r + g + b
    if a < 128: return '.'
    if s < 165: return 'K'          # near-black border / shadow
    if s < 330: return 'k'          # dark grey
    mn = min(r, g, b)
    if mn > 200 and abs(r - g) < 40 and abs(g - b) < 40: return 'W'   # white
    if b > r + 25 and b > 90:
        if g > r + 25: return 'c'   # cyan-ish
        return 'B'                  # blue
    if r > b + 25 and g > b + 25: return 'O'   # olive / brown / yellow
    if g > r + 20 and g > b + 20: return 'G'
    if r > 150 and g > 120 and b < 110: return 'Y'
    return '?'
for key, path in PATHS.items():
    im = Image.open(path).convert("RGBA")
    W, H = im.size
    px = im.load()
    with io.open(os.path.join(OUTDIR, "_s3_map_%s.txt" % key), "w", encoding="utf-8") as f:
        f.write("col ruler (tens then ones):\n")
        f.write(" " * 6 + "".join(str((x // 10) % 10) for x in range(W)) + "\n")
        f.write(" " * 6 + "".join(str(x % 10) for x in range(W)) + "\n")
        for y in range(H):
            f.write("%4d  %s\n" % (y, "".join(cls(px[x, y]) for x in range(W))))
print("done")
