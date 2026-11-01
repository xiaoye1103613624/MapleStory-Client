# r15 s9: img3 targeted feature measurement (compact output).
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P3 = r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png"
im = Image.open(P3).convert("RGBA"); W, H = im.size; px = im.load()
log = io.open(os.path.join(OUTDIR, "_s9_img3.txt"), "w", encoding="utf-8")
log.write("img3 %dx%d\n" % (W, H))

def counts(pred):
    col = [sum(1 for y in range(H) if pred(px[x, y])) for x in range(W)]
    row = [sum(1 for x in range(W) if pred(px[x, y])) for y in range(H)]
    return col, row

def dark(p): return p[0] + p[1] + p[2] < 210
def white(p): return min(p[:3]) >= 240
def beige(p):  # the dithered panel body ~ (221..238) grey-warm
    r, g, b = p[:3]
    return 200 <= r <= 245 and 200 <= g <= 245 and 200 <= b <= 245 and abs(r - b) <= 22
def bluebtn(p):
    r, g, b = p[:3]
    return b > 170 and b > r + 40 and g > r
def yellow(p):
    r, g, b = p[:3]
    return r > 170 and g > 170 and b < 130

for name, pred in (("dark(sum<210)", dark), ("white(min>=240)", white), ("beige", beige), ("bluebtn", bluebtn), ("yellow", yellow)):
    col, row = counts(pred)
    log.write("\n--- %s ---\n" % name)
    log.write("  cols with count>=%d: %s\n" % (H * 0.5, [x for x in range(W) if col[x] >= H * 0.5]))
    log.write("  cols with count>=%d: %s\n" % (H * 0.3, [x for x in range(W) if col[x] >= H * 0.3]))
    log.write("  rows with count>=%d: %s\n" % (W * 0.5, [y for y in range(H) if row[y] >= W * 0.5]))
    log.write("  rows with count>=%d: %s\n" % (W * 0.3, [y for y in range(H) if row[y] >= W * 0.3]))

# exact corner/edge colours
log.write("\n--- border colours ---\n")
for (x, y) in [(0,0),(1,1),(2,2),(3,3),(4,4),(5,5),(6,6),(7,7),(8,8),(W-1,0),(W-2,1),(W-3,2),(W-4,3),(W-5,4),(W-6,5),
               (0,H-1),(1,H-2),(2,H-3),(W-1,H-1),(W-2,H-2)]:
    log.write("  (%d,%d)=%s\n" % (x, y, px[x, y][:3]))
log.write("\n--- first/last x of each band along y=150 and y=250 ---\n")
for y in (150, 250):
    log.write("  y=%d : " % y)
    prev = None
    for x in range(W):
        c = px[x, y][:3]
        q = (c[0] // 12, c[1] // 12, c[2] // 12)
        if q != prev:
            log.write("%d%s " % (x, c))
            prev = q
    log.write("\n")
log.close()
print("done")
