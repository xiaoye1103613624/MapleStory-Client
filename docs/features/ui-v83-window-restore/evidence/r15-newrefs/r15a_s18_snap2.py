# r15a_s18_snap2: re-snapshot with md5 verification, then measure img2 tooltip
# from the fresh snapshot in the SAME process. Also compares against the md5s
# recorded by s16 at 04:21 to prove the desktop sources are changing.
import os, io, json, shutil, hashlib, time
from PIL import Image
from collections import Counter
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
paths = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))
snapdir = os.path.join(OUTDIR, "src")
log = io.open(os.path.join(OUTDIR, "_n_snap2.txt"), "w", encoding="utf-8")
old = {}
oldfile = os.path.join(OUTDIR, "_l_snap.txt")
for line in io.open(oldfile, encoding="utf-8"):
    parts = line.split()
    if parts and parts[0].startswith("img"):
        old[parts[0]] = parts[4]  # md5_before of s16 run
manifest = {}
for key in sorted(paths, key=lambda k: int(k[3:])):
    p = paths[key]
    b1 = open(p, "rb").read()
    dst = os.path.join(snapdir, "%s.png" % key)
    with open(dst, "wb") as f:
        f.write(b1)
    b2 = open(dst, "rb").read()
    h1, h2 = hashlib.md5(b1).hexdigest(), hashlib.md5(b2).hexdigest()
    manifest[key] = {"md5": h2, "size": len(b2), "ts": time.ctime()}
    log.write("%s md5=%s snap_ok=%s s16_md5=%s changed_since_s16=%s\n"
              % (key, h2, h1 == h2, old.get(key), old.get(key) != h2))
json.dump(manifest, io.open(os.path.join(OUTDIR, "_n_manifest.json"), "w", encoding="utf-8"), indent=1)

# ---- measure img2 tooltip on fresh snapshot ----
im = Image.open(os.path.join(snapdir, "img2.png")).convert("RGB")
W, H = im.size
px = im.load()
log.write("img2 snapshot size=%dx%d\n" % (W, H))
def lum(c): return 0.299*c[0]+0.587*c[1]+0.114*c[2]
fam = {(66, 66, 66), (51, 51, 51)}
log.write("count(66,66,66)=%d\n" % sum(1 for y in range(H) for x in range(W) if px[x, y] == (66, 66, 66)))
log.write("-- rows with >20 family px --\n")
for y in range(H):
    xs = [x for x in range(W) if px[x, y] in fam]
    if len(xs) > 20:
        log.write("y=%3d n=%3d x[%d..%d]\n" % (y, len(xs), min(xs), max(xs)))
log.write("-- cols with >10 family px (y0..60) --\n")
for x in range(W):
    ys = [y for y in range(0, 60) if px[x, y] in fam]
    if len(ys) > 10:
        log.write("x=%3d n=%3d y[%d..%d]\n" % (x, len(ys), min(ys), max(ys)))
log.close()
print("OK")
