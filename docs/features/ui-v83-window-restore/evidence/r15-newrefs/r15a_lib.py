# r15a_lib: shared helpers. ALL measurement goes through open_snap(), which
# verifies the snapshot md5 against _n_manifest.json (taken 04:28) and refuses
# to read if the snapshot was touched. Desktop sources are unstable; src/ is final.
import os, io, json, hashlib
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
MANIFEST = json.load(io.open(os.path.join(OUTDIR, "_n_manifest.json"), encoding="utf-8"))

def snap_path(key):
    return os.path.join(OUTDIR, "src", "%s.png" % key)

def check_md5(key):
    p = snap_path(key)
    h = hashlib.md5(open(p, "rb").read()).hexdigest()
    if h != MANIFEST[key]["md5"]:
        raise RuntimeError("SNAPSHOT %s MD5 DRIFT: %s != %s" % (key, h, MANIFEST[key]["md5"]))
    return h

def open_snap(key):
    check_md5(key)
    return Image.open(snap_path(key)).convert("RGB")

def lum(c):
    return 0.299*c[0] + 0.587*c[1] + 0.114*c[2]

def ascii_map(im, ystep=2, xstep=1):
    px = im.load()
    W, H = im.size
    def b(c):
        l = (c[0]+c[1]+c[2])/3
        return "#" if l < 40 else "-" if l < 80 else "+" if l < 130 else "=" if l < 200 else "."
    lines = ["ASCII (1char/px, rows every %d):" % ystep]
    for y in range(0, H, ystep):
        lines.append("%3d|%s" % (y, "".join(b(px[x, y]) for x in range(0, W, xstep))))
    return "\n".join(lines)

def row_profile(px, W, H, pred, minn=1):
    out = []
    for y in range(H):
        xs = [x for x in range(W) if pred(px[x, y])]
        if len(xs) >= minn:
            out.append((y, len(xs), min(xs), max(xs)))
    return out

def col_profile(px, W, H, pred, minn=1):
    out = []
    for x in range(W):
        ys = [y for y in range(H) if pred(px[x, y])]
        if len(ys) >= minn:
            out.append((x, len(ys), min(ys), max(ys)))
    return out

def runs(pairs):
    """compress sorted (start,end) list of occupied indices into runs"""
    res = []
    cur = None
    for i in pairs:
        if cur is None:
            cur = [i, i]
        elif i == cur[1] + 1:
            cur[1] = i
        else:
            res.append(tuple(cur))
            cur = [i, i]
    if cur:
        res.append(tuple(cur))
    return res

def log_rows(log, prof, fmt="%3d n=%4d x[%4d..%4d]", label=""):
    log.write(label + "\n")
    for t in prof:
        log.write("  " + fmt % t + "\n")

def log_cols(log, prof, fmt="%3d n=%4d y[%4d..%4d]", label=""):
    log.write(label + "\n")
    for t in prof:
        log.write("  " + fmt % t + "\n")
