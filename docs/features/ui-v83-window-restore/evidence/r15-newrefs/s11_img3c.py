# r15 s11: img3 exact border runs, button rects, nametag rect, scrollbar check.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P3 = r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png"
im = Image.open(P3).convert("RGBA"); W, H = im.size; px = im.load()
log = io.open(os.path.join(OUTDIR, "_s11_img3c.txt"), "w", encoding="utf-8")
def seq_h(y, x0, x1):
    log.write(" y=%d x=%d..%d:\n   " % (y, x0, x1))
    for x in range(x0, x1 + 1): log.write("%d%s " % (x, px[x, y][:3]))
    log.write("\n")
def seq_v(x, y0, y1):
    log.write(" x=%d y=%d..%d:\n   " % (x, y0, y1))
    for y in range(y0, y1 + 1): log.write("%d%s " % (y, px[x, y][:3]))
    log.write("\n")
log.write("### LEFT border\n")
for y in (30, 150, 260): seq_h(y, 0, 14)
log.write("### RIGHT border\n")
for y in (30, 150, 260): seq_h(y, 495, 523)
log.write("### TOP border\n")
for x in (60, 300, 500): seq_v(x, 0, 14)
log.write("### BOTTOM border\n")
for x in (60, 300, 500): seq_v(x, 268, 312)

def bbox(pred, x0, x1, y0, y1):
    ax=ay=bx=by=None; n=0
    for y in range(y0, y1+1):
        for x in range(x0, x1+1):
            if pred(px[x,y]):
                n+=1
                ax=x if ax is None else min(ax,x); bx=x if bx is None else max(bx,x)
                ay=y if ay is None else min(ay,y); by=y if by is None else max(by,y)
    return (ax,ay,bx,by,n)
# 结束对话 (yellow-green) below the frame
yg = lambda p: p[0]>140 and p[1]>140 and p[2]<130 and abs(p[0]-p[1])<70
log.write("\n### 结束对话 yellowgreen bbox y>=283: %s\n" % str(bbox(yg, 0, 200, 283, 312)))
# 下项 blue button: strong blue, inside y 230..280
bl = lambda p: p[2]>170 and p[2]>p[0]+45 and p[1]>90 and p[1]<210
log.write("### blue bbox x>380 y 225..282: %s\n" % str(bbox(bl, 380, 523, 225, 282)))
# nametag: dark navy band
nv = lambda p: p[0]<70 and p[1]<90 and 90<p[2]<170
log.write("### nametag navy bbox x<200 y 140..220: %s\n" % str(bbox(nv, 0, 200, 140, 220)))
# nametag white outline
wo = lambda p: min(p[:3])>=245
log.write("### white bbox x<200 y 140..220 (nametag outline): %s\n" % str(bbox(wo, 0, 200, 140, 220)))
# text area bright rect
br = lambda p: min(p[:3])>=225
log.write("### bright text-area bbox x 150..520 y 15..280: %s\n" % str(bbox(br, 150, 520, 15, 280)))
# scrollbar check: vertical grey band near right of text area
log.write("### scrollbar check: per-col counts of (102,102,102)-ish in x 480..523, y 20..270\n")
for x in range(480, 524):
    n = sum(1 for y in range(20, 271) if abs(px[x,y][0]-102)<28 and abs(px[x,y][1]-102)<28 and abs(px[x,y][2]-102)<28)
    if n > 20: log.write("   x=%d greyish=%d\n" % (x, n))
# avatar sprite bbox (green hoodie)
gr = lambda p: p[1]>p[0]+25 and p[1]>p[2]+40
log.write("### avatar green bbox x<160 y 100..200: %s\n" % str(bbox(gr, 0, 160, 100, 200)))
log.close()
print("done")
