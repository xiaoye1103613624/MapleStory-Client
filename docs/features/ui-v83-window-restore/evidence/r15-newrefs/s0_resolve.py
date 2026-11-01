# r15 s0: resolve the 5 target screenshots purely by (width,height) so that the
# exotic quote characters in the filenames never have to be typed/escaped.
import os, io, json
from PIL import Image

REFDIR = r"C:\Users\11036\Desktop\正确客户端截图"
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
TARGETS = {"img1": (94, 278), "img2": (82, 182), "img3": (524, 313),
           "img4": (509, 86), "img5": (234, 67)}

log = io.open(os.path.join(OUTDIR, "_s0_resolve.txt"), "w", encoding="utf-8")
found = {}
for fn in sorted(os.listdir(REFDIR)):
    if not fn.lower().endswith(".png"):
        continue
    p = os.path.join(REFDIR, fn)
    try:
        with Image.open(p) as im:
            sz = im.size
    except Exception as e:
        log.write("ERR %r %s\n" % (fn, e))
        continue
    for key, t in TARGETS.items():
        if sz == t:
            found.setdefault(key, []).append((fn, p, sz))
for key in sorted(TARGETS):
    hits = found.get(key, [])
    log.write("%s target=%s hits=%d\n" % (key, TARGETS[key], len(hits)))
    for fn, p, sz in hits:
        log.write("    name=%s\n    path=%s\n" % (fn, p))
        with Image.open(p) as im:
            log.write("    fmt=%s mode=%s size=%s info_keys=%s\n" % (im.format, im.mode, im.size, sorted(im.info.keys())))
json.dump({k: [h[1] for h in v] for k, v in found.items()},
          io.open(os.path.join(OUTDIR, "_s0_paths.json"), "w", encoding="utf-8"), indent=1)
log.close()
print("done")
