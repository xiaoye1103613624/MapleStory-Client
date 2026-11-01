# r15 s4: exact RGB line probes to locate straight frame borders.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
    "img3": r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png",
    "img4": r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png",
    "img5": r"C:\Users\11036\Desktop\正确客户端截图\打开快捷键，向上时快捷键处于收起状态.png",
}
def hline(f, px, W, H, y, x0, x1, tag):
    f.write("  %s y=%d x=%d..%d:\n" % (tag, y, x0, x1))
    row = []
    for x in range(x0, min(x1 + 1, W)):
        row.append("%d:(%d,%d,%d)" % (x, px[x, y][0], px[x, y][1], px[x, y][2]))
    f.write("    " + " ".join(row) + "\n")
def vline(f, px, W, H, x, y0, y1, tag):
    f.write("  %s x=%d y=%d..%d:\n" % (tag, x, y0, y1))
    col = []
    for y in range(y0, min(y1 + 1, H)):
        col.append("%d:(%d,%d,%d)" % (y, px[x, y][0], px[x, y][1], px[x, y][2]))
    f.write("    " + " ".join(col) + "\n")

log = io.open(os.path.join(OUTDIR, "_s4_probe.txt"), "w", encoding="utf-8")
im1 = Image.open(P["img1"]).convert("RGBA"); p1 = im1.load(); W1, H1 = im1.size
log.write("### img1 left/right edges at several y\n")
for y in (0, 5, 60, 120, 200, 231, 232, 233, 234, 235, 240, 250):
    hline(log, p1, W1, H1, y, 0, 10, "L")
    hline(log, p1, W1, H1, y, 84, 93, "R")
log.write("### img1 vertical at x=45 (top border + tail/bottom)\n")
vline(log, p1, W1, H1, 45, 0, 24, "T")
vline(log, p1, W1, H1, 45, 225, 250, "B")
log.write("### img1 vertical at x=10 (skips tail)\n")
vline(log, p1, W1, H1, 10, 225, 250, "B")

im2 = Image.open(P["img2"]).convert("RGBA"); p2 = im2.load(); W2, H2 = im2.size
log.write("\n### img2 left/right edges\n")
for y in (0, 5, 60, 120, 140, 145, 150, 155, 160, 170):
    hline(log, p2, W2, H2, y, 0, 10, "L")
    hline(log, p2, W2, H2, y, 72, 81, "R")
log.write("### img2 vertical at x=40\n")
vline(log, p2, W2, H2, 40, 0, 20, "T")
vline(log, p2, W2, H2, 40, 135, 165, "B")

im4 = Image.open(P["img4"]).convert("RGBA"); p4 = im4.load(); W4, H4 = im4.size
log.write("\n### img4 top border vertical scans\n")
for x in (60, 120, 250, 400, 500):
    vline(log, p4, W4, H4, x, 0, 14, "T")
log.write("### img4 bottom border vertical scans\n")
for x in (120, 250, 400):
    vline(log, p4, W4, H4, x, 72, 85, "B")
log.write("### img4 left edge horizontal scans\n")
for y in (20, 43, 70):
    hline(log, p4, W4, H4, y, 40, 70, "L")
log.write("### img4 right edge horizontal scans\n")
for y in (20, 43, 70):
    hline(log, p4, W4, H4, y, 496, 508, "R")

im5 = Image.open(P["img5"]).convert("RGBA"); p5 = im5.load(); W5, H5 = im5.size
log.write("\n### img5 tooltip edges: horizontal y=10\n")
hline(log, p5, W5, H5, 10, 0, 60, "L")
hline(log, p5, W5, H5, 10, 95, 130, "Rt")
log.write("### img5 vertical x=60\n")
vline(log, p5, W5, H5, 60, 0, 40, "T")
log.close()
print("done")
