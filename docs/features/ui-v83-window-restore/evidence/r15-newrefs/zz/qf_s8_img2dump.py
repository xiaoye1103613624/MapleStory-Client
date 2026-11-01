# zz/qf_s8_img2dump: raw colour grid of img2 x[84..173] y[12..44], 1px/char codes.
import os, io, hashlib, json
from PIL import Image
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
p = os.path.join(ZZ, "src", "img2.png")
assert hashlib.md5(open(p, "rb").read()).hexdigest() == man["img2"]["md5"]
im = Image.open(p).convert("RGB")
px = im.load()
pal = {}
def code(c):
    if c not in pal:
        pal[c] = chr(ord('a') + len(pal)) if len(pal) < 26 else chr(ord('A') + len(pal) - 26)
    return pal[c]
out = io.open(os.path.join(ZZ, "qf_img2dump.txt"), "w", encoding="utf-8")
hdr = "    " + "".join(str((x // 10) % 10) for x in range(84, 174))
out.write(hdr + "\n")
for y in range(12, 45):
    out.write("%3d " % y + "".join(code(px[x, y]) for x in range(84, 174)) + "\n")
out.write("palette:\n")
for c, k in sorted(pal.items(), key=lambda kv: kv[1]):
    out.write("  %s = %s\n" % (k, c))
out.close()
print("DUMPED md5ok")
