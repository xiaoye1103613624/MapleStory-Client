# r15a_s8_grid: annotate img2 with a 10px coordinate grid to fix layout mapping.
import os, io, json
from PIL import Image, ImageDraw
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
p = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))["img2"]
im = Image.open(p).convert("RGB")
g = im.resize((im.width*2, im.height*2), Image.NEAREST)
d = ImageDraw.Draw(g)
for x in range(0, im.width, 10):
    d.line([(x*2, 0), (x*2, g.height)], fill=(0, 255, 0), width=1)
    d.text((x*2+2, 2), str(x), fill=(255, 0, 255))
for y in range(0, im.height, 10):
    d.line([(0, y*2), (g.width, y*2)], fill=(0, 255, 0), width=1)
    d.text((2, y*2+2), str(y), fill=(0, 0, 255))
g.save(os.path.join(OUTDIR, "rgb", "img2_grid.png"))
print("OK")
