# r15a_s10_rows: dump exact colours of selected rows x=80..174 step 2 for img2.
import os, io, json
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
px = im.load()
log = io.open(os.path.join(OUTDIR, "_f_img2rows.txt"), "w", encoding="utf-8")
for y in [0, 5, 10, 12, 14, 16, 18, 20, 25, 30, 35, 38, 40, 42, 44, 46, 50, 55, 60, 65, 70, 72, 74, 76, 80]:
    log.write("y=%3d: %s\n" % (y, " ".join("%d:%02X%02X%02X" % (x, *px[x, y]) for x in range(80, 175, 3))))
log.close()
print("OK")
