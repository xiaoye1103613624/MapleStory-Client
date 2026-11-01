# r15 s8b: run-length scans of img3 with coarse buckets + short-run merging.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P3 = r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png"
im = Image.open(P3).convert("RGBA"); W, H = im.size; px = im.load()
B = 24          # coarse bucket
MERGE = 3       # runs shorter than this get absorbed
def band(p):
    v = max(p[0], p[1], p[2]); n = min(p[0], p[1], p[2])
    # coarse: luminance class + hue class
    lum = min(4, v // 64)
    if v - n <= 18: h = 'g'                      # grey
    elif p[2] >= p[0] and p[2] >= p[1]: h = 'b'  # blue dominant
    elif p[0] >= p[1] and p[1] >= p[2]: h = 'r'  # red/warm dominant
    elif p[1] >= p[0] and p[1] >= p[2]: h = 'G'  # green dominant
    else: h = '?'
    return "%s%d" % (h, lum)
def rle(get, n):
    runs = []
    s = 0; cur = get(0)
    for i in range(1, n):
        k = get(i)
        if k != cur: runs.append([s, i - 1, cur]); s = i; cur = k
    runs.append([s, n - 1, cur])
    out = []
    for r in runs:
        if out and (r[1] - r[0] + 1) < MERGE:
            out[-1][1] = r[1]
        elif out and (r[1] - r[0] + 1) < 2 and out[-1][2] == r[2]:
            out[-1][1] = r[1]
        else:
            out.append(r)
    return [r for r in out if r[1] - r[0] >= 1]
log = io.open(os.path.join(OUTDIR, "_s8b_img3_rle.txt"), "w", encoding="utf-8")
log.write("img3 %dx%d ; band class = hue(luminance-quartile); runs>=2px shown\n" % (W, H))
log.write("format [a..b](RGB)\n")
log.write("\n#### HORIZONTAL\n")
for y in (0,1,2,3,4,5,7,9,12,20,40,80,120,130,133,136,140,145,150,155,160,170,180,190,200,210,220,230,240,245,250,255,258,260,262,265,270,275,280,290,300,305,308,310,311,312):
    log.write("y=%3d: " % y)
    for s,e,k in rle(lambda x: band(px[x,y]), W):
        log.write("[%d..%d]%s " % (s, e, px[(s + e) // 2, y][:3]))
    log.write("\n")
log.write("\n#### VERTICAL\n")
for x in (0,1,2,3,4,5,7,9,12,20,40,60,80,100,120,130,140,145,148,150,155,160,165,170,200,250,300,350,400,430,450,470,480,490,500,505,510,515,518,520,522,523):
    log.write("x=%3d: " % x)
    for s,e,k in rle(lambda y: band(px[x,y]), H):
        log.write("[%d..%d]%s " % (s, e, px[x, (s+e)//2][:3]))
    log.write("\n")
log.close()
print("done")
