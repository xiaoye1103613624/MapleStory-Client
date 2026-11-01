# zz/s2_overview: trustworthy overview on the private snapshots.
import os, io, json
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
log = io.open(os.path.join(ZZ, "overview.txt"), "w", encoding="utf-8")
for key in sorted(man, key=lambda k: int(k[3:])):
    p = os.path.join(ZZ, "src", key + ".png")
    with Image.open(p) as im:
        raw_mode = im.mode
        rgba = im.convert("RGBA")
    W, H = rgba.size
    a = rgba.getchannel("A")
    ah = Counter(a.getdata())
    n0 = ah.get(0, 0)
    n255 = ah.get(255, 0)
    nmix = W*H - n0 - n255
    ab = a.getbbox()  # bbox of nonzero alpha
    px = rgba.load()
    corners = {"TL": px[0, 0], "TR": px[W-1, 0], "BL": px[0, H-1], "BR": px[W-1, H-1]}
    log.write("%s size=%dx%d raw_mode=%s alpha: n0=%d(%.1f%%) n255=%d(%.1f%%) nmix=%d alpha_bbox=%s\n"
              % (key, W, H, raw_mode, n0, 100.0*n0/(W*H), n255, 100.0*n255/(W*H), nmix, ab))
    log.write("   corners=%s\n" % corners)
    # distinct alpha values if few
    if len(ah) <= 12:
        log.write("   alpha values: %s\n" % sorted(ah.items()))
    else:
        log.write("   alpha distinct=%d top: %s\n" % (len(ah), ah.most_common(6)))
    # RGB of opaque pixels dominant colours
    rgb = rgba.convert("RGB")
    pxr = rgb.load()
    c = Counter()
    for y in range(0, H, 2):
        for x in range(0, W, 2):
            if px[x, y][3] > 0:
                c[pxr[x, y]] += 1
    log.write("   top RGB (opaque, 2px stride): %s\n" % c.most_common(8))
    # composite over magenta for visual inspection
    base = Image.new("RGBA", (W, H), (255, 0, 255, 255))
    comp = Image.alpha_composite(base, rgba).convert("RGB")
    if max(W, H) <= 300:
        comp = comp.resize((W*2, H*2), Image.NEAREST)
    comp.save(os.path.join(ZZ, "crop", key + "_mag.png"))
log.close()
print("OK2")
