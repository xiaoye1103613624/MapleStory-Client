# r15 s14: all 4x/6x transcription crops + img5 tooltip + img3 column extents.
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
SRC = {
 "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
 "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
 "img3": r"C:\Users\11036\Desktop\正确客户端截图\点击“帮助”按钮打开的脚本框.png",
 "img4": r"C:\Users\11036\Desktop\正确客户端截图\关闭快捷键向下时 快捷键处于展开状态.png",
 "img5": r"C:\Users\11036\Desktop\正确客户端截图\打开快捷键，向上时快捷键处于收起状态.png",
}
log = io.open(os.path.join(OUTDIR, "_s14_crops.txt"), "w", encoding="utf-8")
def up(im, box, s, name):
    c = im.crop(box)
    c = c.resize((c.size[0]*s, c.size[1]*s), Image.NEAREST)
    c.save(os.path.join(OUTDIR, name))
    log.write("wrote %-46s box=%s scale=%dx size=%s\n" % (name, box, s, c.size))

# ---------- img1: title + 8 button rows (bands y=22+26i .. 46+26i) ----------
i1 = Image.open(SRC["img1"]).convert("RGBA")
up(i1, (4, 0, 93, 22), 4, "crop_img1_title_4x.png")
for i in range(8):
    y0, y1 = 22 + 26*i, 47 + 26*i
    up(i1, (4, y0, 93, y1), 4, "crop_img1_row%d_4x.png" % (i+1))
up(i1, (4, 22, 93, 231), 4, "crop_img1_body_4x.png")
up(i1, (30, 228, 60, 246), 6, "crop_img1_tail_6x.png")

# ---------- img2 ----------
i2 = Image.open(SRC["img2"]).convert("RGBA")
up(i2, (0, 2, 82, 25), 4, "crop_img2_title_4x.png")
for i in range(4):
    y0, y1 = 25 + 26*i, 50 + 26*i
    up(i2, (0, y0, 82, y1), 4, "crop_img2_row%d_4x.png" % (i+1))
up(i2, (0, 25, 82, 132), 4, "crop_img2_body_4x.png")

# ---------- img3 ----------
i3 = Image.open(SRC["img3"]).convert("RGBA"); p3 = i3.load(); W3, H3 = i3.size
BANDS = [(37,48),(55,66),(73,84),(91,102),(109,120),(169,180),(187,198),(205,216),(223,234)]
def darkink(p): return p[0]+p[1]+p[2] < 240
for n,(a,b) in enumerate(BANDS, 1):
    xs = [x for x in range(150, 512) if any(darkink(p3[x,y]) for y in range(a-2, b+3))]
    log.write("img3 band %d y=%d..%d ink x=%s\n" % (n, a, b, (xs[0], xs[-1]) if xs else None))
    # split into column clusters by gaps >= 8px
    cl = []
    if xs:
        s = xs[0]
        for k in range(1, len(xs)):
            if xs[k] - xs[k-1] >= 8:
                cl.append((s, xs[k-1])); s = xs[k]
        cl.append((s, xs[-1]))
    log.write("          clusters: %s\n" % cl)
    up(i3, (150, a-3, 512, b+3), 4, "crop_img3_text%02d_4x.png" % n)
up(i3, (0, 1, 524, 289), 2, "crop_img3_window_2x.png")
up(i3, (0, 160, 150, 200), 4, "crop_img3_nametag_4x.png")
up(i3, (430, 232, 512, 268), 4, "crop_img3_btn_next_4x.png")
up(i3, (0, 283, 130, 313), 4, "crop_img3_btn_end_4x.png")
up(i3, (480, 15, 524, 280), 4, "crop_img3_rightstrip_4x.png")

# ---------- img4 ----------
i4 = Image.open(SRC["img4"]).convert("RGBA")
up(i4, (48, 9, 509, 47), 3, "crop_img4_slotrow1_3x.png")
up(i4, (48, 46, 509, 85), 3, "crop_img4_slotrow2_3x.png")
for i in (0, 4, 10, 12):
    x0 = 52 + 35*i
    up(i4, (x0, 14, x0+36, 47), 6, "crop_img4_slot_r1c%d_6x.png" % (i+1))
    up(i4, (x0, 47, x0+36, 80), 6, "crop_img4_slot_r2c%d_6x.png" % (i+1))
up(i4, (0, 15, 62, 50), 4, "crop_img4_collapsebtn_tip_4x.png")

# ---------- img5 ----------
i5 = Image.open(SRC["img5"]).convert("RGBA"); p5 = i5.load(); W5, H5 = i5.size
def tipbg(p):
    r,g,b = p[:3]
    return b > r + 25 and b > g + 25 and 90 < b < 175
ax=ay=bx=by=None; n=0
for y in range(H5):
    for x in range(W5):
        if tipbg(p5[x,y]):
            n+=1
            ax=x if ax is None else min(ax,x); bx=x if bx is None else max(bx,x)
            ay=y if ay is None else min(ay,y); by=y if by is None else max(by,y)
log.write("\nimg5 tooltip-bg bbox = (%s,%s)-(%s,%s) n=%d size=%dx%d\n" % (
    ax,ay,bx,by,n, (bx-ax+1) if ax is not None else 0, (by-ay+1) if ay is not None else 0))
if ax is not None:
    up(i5, (ax-2, ay-2, bx+3, by+3), 4, "crop_img5_tooltip_4x.png")
up(i5, (0, 0, 60, 67), 4, "crop_img5_leftbtn_4x.png")
up(i5, (0, 25, 45, 67), 4, "crop_img5_bottombar_4x.png")
log.close()
print("done")
