# r15 s6: img1/img2 popup geometry -> bands, ink rows, text left x, 4x crops.
#
# Frame model (validated by s4 line probes + s5 projections):
#   outer 1px white bevel line | 2-3px dark frame | content | dark frame | white bevel
import os, io
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
P = {
    "img1": r"C:\Users\11036\Desktop\正确客户端截图\”界面“按钮点击后的列表.png",
    "img2": r"C:\Users\11036\Desktop\正确客户端截图\目录按钮点击后的列表.png",
}
# frame rect measured in s5 (x0,y0,x1,y1) inclusive, and content rect
FRAME = {"img1": (4, 0, 92, 230), "img2": (2, 2, 81, 128)}
CONTENT = {"img1": (8, 3, 88, 228), "img2": (3, 3, 81, 127)}
SEP = {"img1": [17, 47, 73, 99, 125, 151, 177, 203, 229],
       "img2": [23, 50, 76, 102, 128]}

log = io.open(os.path.join(OUTDIR, "_s6_menu_geom.txt"), "w", encoding="utf-8")
for key, path in P.items():
    im = Image.open(path).convert("RGBA"); W, H = im.size; px = im.load()
    fx0, fy0, fx1, fy1 = FRAME[key]
    cx0, cy0, cx1, cy1 = CONTENT[key]
    log.write("=" * 74 + "\n%s  image %dx%d\n" % (key, W, H))
    log.write("frame rect (inclusive) = (%d,%d)-(%d,%d) size %dx%d\n" % (
        fx0, fy0, fx1, fy1, fx1 - fx0 + 1, fy1 - fy0 + 1))
    log.write("content rect (inclusive) = (%d,%d)-(%d,%d) size %dx%d\n" % (
        cx0, cy0, cx1, cy1, cx1 - cx0 + 1, cy1 - cy0 + 1))
    seps = SEP[key]
    # title band = first content row .. first separator-1 ; button bands between separators
    bands = [("title", cy0, seps[0] - 1)]
    for i in range(len(seps) - 1):
        bands.append(("row%d" % (i + 1), seps[i] + 2, seps[i + 1] - 1))
    log.write("separators y=%s\n" % seps)
    log.write("bands (name, y0, y1, height):\n")
    for n, a, b in bands:
        log.write("   %-6s y=%3d..%3d h=%d\n" % (n, a, b, b - a + 1))
    pitches = [seps[i + 1] - seps[i] for i in range(len(seps) - 1)]
    log.write("separator pitch = %s\n" % pitches)

    # ink per band. title text is DARK on white; button text is WHITE on blue.
    for n, a, b in bands:
        whitecols = [0] * W
        darkcols = [0] * W
        rowwhite = []
        rowdark = []
        for y in range(a, b + 1):
            nw = nd = 0
            for x in range(cx0, min(cx1, W - 1) + 1):
                p = px[x, y]
                if min(p[0], p[1], p[2]) >= 230:
                    whitecols[x] += 1; nw += 1
                if p[0] + p[1] + p[2] < 170:
                    darkcols[x] += 1; nd += 1
            rowwhite.append((y, nw)); rowdark.append((y, nd))
        def runs(cols):
            r = []
            s = None
            for x in range(W):
                if cols[x] > 0 and s is None: s = x
                if cols[x] == 0 and s is not None:
                    r.append((s, x - 1)); s = None
            if s is not None: r.append((s, W - 1))
            return r
        wr = [q for q in runs(whitecols) if q[1] - q[0] >= 1]
        dr = [q for q in runs(darkcols) if q[1] - q[0] >= 1]
        log.write("\n band %-6s y=%d..%d\n" % (n, a, b))
        log.write("   white-ink col runs (>=2px): %s\n" % wr)
        log.write("   dark-ink  col runs (>=2px): %s\n" % dr)
        log.write("   per-row white: %s\n" % rowwhite)
        log.write("   per-row dark : %s\n" % rowdark)
        # save 4x crop of the band across the whole frame width
        crop = im.crop((fx0, a, fx1 + 1, b + 1))
        crop = crop.resize((crop.size[0] * 4, crop.size[1] * 4), Image.NEAREST)
        crop.save(os.path.join(OUTDIR, "crop_%s_%s_4x.png" % (key, n)))
log.close()
print("done")
