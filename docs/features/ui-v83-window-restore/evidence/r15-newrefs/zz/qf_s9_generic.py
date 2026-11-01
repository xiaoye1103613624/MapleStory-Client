# zz/qf_s9_generic.py: per-image structural measurement on pinned snapshots.
# Usage: python qf_s9_generic.py imgN [imgM ...]
# Writes zz/qf_<key>.txt (md5-guarded input) and zz/qf_<key>_grid.png (3x/2x view).
import os, io, sys, json, hashlib
from PIL import Image, ImageDraw
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))

def lum(c): return 0.299*c[0] + 0.587*c[1] + 0.114*c[2]

def runs(idxs):
    out = []
    for i in idxs:
        if out and i == out[-1][1] + 1: out[-1][1] = i
        else: out.append([i, i])
    return [tuple(o) for o in out]

def measure(key):
    p = os.path.join(ZZ, "src", key + ".png")
    h = hashlib.md5(open(p, "rb").read()).hexdigest()
    assert h == man[key]["md5"], "MD5 DRIFT on " + key
    imrgba = Image.open(p).convert("RGBA")
    ah = Counter(imrgba.getchannel("A").getdata())
    im = imrgba.convert("RGB")
    W, H = im.size
    px = im.load()
    log = io.open(os.path.join(ZZ, "qf_%s.txt" % key), "w", encoding="utf-8")
    log.write("%s md5=%s size=%dx%d\n" % (key, h, W, H))
    log.write("corners TL=%s TR=%s BL=%s BR=%s\n" % (px[0, 0], px[W-1, 0], px[0, H-1], px[W-1, H-1]))
    log.write("alpha hist top6: %s\n" % ah.most_common(6))
    ab = imrgba.getchannel("A").getbbox()
    log.write("alpha bbox=%s\n" % (ab,))
    def b(c):
        l = lum(c)
        return "#" if l < 40 else "-" if l < 80 else "+" if l < 130 else "=" if l < 200 else "."
    log.write("ASCII map rows every 2:\n")
    for y in range(0, H, 2):
        log.write("%3d|%s\n" % (y, "".join(b(px[x, y]) for x in range(W))))
    # structural line detectors
    def brown(c): return c[0] > c[2] + 15 and abs(c[0]-108) < 55 and abs(c[1]-91) < 55 and abs(c[2]-73) < 55
    bcols = runs([x for x in range(W) if sum(1 for y in range(H) if brown(px[x, y])) > 0.5 * H])
    brows = runs([y for y in range(H) if sum(1 for x in range(W) if brown(px[x, y])) > 0.5 * W])
    log.write("brown frame cols=%s rows=%s\n" % (bcols, brows))
    red = runs([y for y in range(H) if sum(1 for x in range(W) if px[x, y][0] > 150 and px[x, y][1] < 60 and px[x, y][2] < 60) > 0.5 * W])
    log.write("red rows=%s\n" % red)
    wht = runs([y for y in range(H) if sum(1 for x in range(W) if px[x, y] == (255, 255, 255)) > 0.6 * W])
    log.write("white full rows=%s\n" % wht)
    whc = runs([x for x in range(W) if sum(1 for y in range(H) if px[x, y] == (255, 255, 255)) > 0.6 * H])
    log.write("white full cols=%s\n" % whc)
    blk = runs([y for y in range(H) if sum(1 for x in range(W) if px[x, y] == (0, 0, 0)) > 0.5 * W])
    log.write("black full rows=%s\n" % blk)
    # dominant colours
    c = Counter(px[x, y] for y in range(0, H, 2) for x in range(0, W, 2))
    log.write("top16 colours: %s\n" % c.most_common(16))
    # grid view
    S = 3 if max(W, H) <= 200 else (2 if max(W, H) <= 400 else 1)
    g = im.resize((W*S, H*S), Image.NEAREST) if S > 1 else im.copy()
    d = ImageDraw.Draw(g)
    step = 20 if S == 1 else 10
    for x in range(0, W, step):
        d.line([(x*S, 0), (x*S, H*S)], fill=(0, 255, 0), width=1)
        d.rectangle([x*S, 0, x*S+16, 9], fill=(0, 0, 0))
        d.text((x*S+1, 1), str(x), fill=(255, 255, 0))
    for y in range(0, H, step):
        d.line([(0, y*S), (W*S, y*S)], fill=(0, 255, 0), width=1)
        d.rectangle([0, y*S, 13, y*S+9], fill=(0, 0, 0))
        d.text((1, y*S+1), str(y), fill=(0, 255, 255))
    g.save(os.path.join(ZZ, "qf_%s_grid.png" % key))
    log.write("saved qf_%s_grid.png scale=%d\n" % (key, S))
    log.close()
    # verify readback
    got = io.open(os.path.join(ZZ, "qf_%s.txt" % key), encoding="utf-8").read()
    print("%s lines=%d head=%r" % (key, len(got.splitlines()), got.splitlines()[0][:60]))

for key in sys.argv[1:]:
    measure(key)
print("ALLDONE")
