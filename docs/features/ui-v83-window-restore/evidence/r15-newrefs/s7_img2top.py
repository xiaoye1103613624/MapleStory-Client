# r15 s7: exact vertical colour runs for img2/img1 tops and img2 right-edge check.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P2 = r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png"
P1 = r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png"
log = io.open(os.path.join(OUTDIR, "_s7_img2top.txt"), "w", encoding="utf-8")
im2 = Image.open(P2).convert("RGBA"); p = im2.load(); W, H = im2.size
for x in (10, 40, 60, 78):
    log.write("img2 x=%d y=0..30:\n   " % x)
    for y in range(0, 31):
        log.write("%d:%s " % (y, p[x, y][:3]))
    log.write("\n")
log.write("\nimg2 last 5 cols for representative rows (right border present?)\n")
for y in (5, 30, 60, 90, 120, 126, 127, 128, 130):
    log.write("  y=%3d : %s\n" % (y, " ".join("%d%s" % (x, p[x, y][:3]) for x in range(74, 82))))
im1 = Image.open(P1).convert("RGBA"); q = im1.load()
log.write("\nimg1 x=40 y=0..30:\n   ")
for y in range(0, 31):
    log.write("%d:%s " % (y, q[40, y][:3]))
log.write("\n")
log.close()
print("done")
