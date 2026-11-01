# r15 s5: frame detection by border projections.
#   dark  = r+g+b < 160  (the popup's black outline / separators)
#   white = min(r,g,b) >= 235 (the popup's 1px outer highlight + white title bar)
# Prints full per-row / per-column profiles so borders and bands can be read off.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
}
log = io.open(os.path.join(OUTDIR, "_s5_borders.txt"), "w", encoding="utf-8")
for key, path in P.items():
    im = Image.open(path).convert("RGBA"); W, H = im.size; px = im.load()
    dark = [[0] * W for _ in range(H)]
    white = [[0] * W for _ in range(H)]
    for y in range(H):
        for x in range(W):
            p = px[x, y]
            if p[0] + p[1] + p[2] < 160: dark[y][x] = 1
            if min(p[0], p[1], p[2]) >= 235: white[y][x] = 1
    cd = [sum(dark[y][x] for y in range(H)) for x in range(W)]
    rd = [sum(dark[y][x] for x in range(W)) for y in range(H)]
    cw = [sum(white[y][x] for y in range(H)) for x in range(W)]
    rw = [sum(white[y][x] for x in range(W)) for y in range(H)]
    log.write("=" * 70 + "\n%s %dx%d\n" % (key, W, H))
    log.write("per-COLUMN: x, darkCount, whiteCount  (dark>0.5H marks vertical borders)\n")
    for x in range(W):
        flag = ""
        if cd[x] > 0.5 * H: flag = "  <== vertical dark border"
        if cw[x] > 0.7 * H: flag += "  <== tall white line"
        log.write("  x=%3d dark=%4d white=%4d%s\n" % (x, cd[x], cw[x], flag))
    log.write("per-ROW: y, darkCount, whiteCount\n")
    for y in range(H):
        flag = ""
        if rd[y] > 0.7 * W: flag = "  <== full-width dark row"
        if rw[y] > 0.7 * W: flag += "  <== full-width white row"
        log.write("  y=%3d dark=%4d white=%4d%s\n" % (y, rd[y], rw[y], flag))
log.close()
print("done")
