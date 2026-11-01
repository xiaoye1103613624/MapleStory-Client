# r15a_s3_rgb: dump alpha-free RGB copies (tRNS is junk, see _b_probe.txt) and
# 2x NEAREST enlargements of the small images so they can be inspected.
import os, io, json
from PIL import Image

OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
paths = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))
log = io.open(os.path.join(OUTDIR, "_c_rgb.txt"), "w", encoding="utf-8")
os.makedirs(os.path.join(OUTDIR, "rgb"), exist_ok=True)
for key in sorted(paths, key=lambda k: int(k[3:])):
    p = paths[key]
    with Image.open(p) as im:
        rgb = im.convert("RGB")
        w, h = rgb.size
        fn = "%s_rgb.png" % key
        rgb.save(os.path.join(OUTDIR, "rgb", fn))
        log.write("%s %dx%d -> rgb/%s\n" % (key, w, h, fn))
        if max(w, h) <= 400:
            big = rgb.resize((w * 2, h * 2), Image.NEAREST)
            fn2 = "%s_rgb2x.png" % key
            big.save(os.path.join(OUTDIR, "rgb", fn2))
            log.write("   also 2x: rgb/%s (%dx%d)\n" % (fn2, w * 2, h * 2))
log.close()
print("OK")
