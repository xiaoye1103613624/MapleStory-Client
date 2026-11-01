# zz/s7_verify: re-verify snapshot md5s vs manifest, and re-run img2 alpha hist.
import os, io, json, hashlib
from PIL import Image
from collections import Counter
ZZ = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs\zz"
man = json.load(io.open(os.path.join(ZZ, "manifest.json"), encoding="utf-8"))
log = io.open(os.path.join(ZZ, "verify.txt"), "w", encoding="utf-8")
for key in sorted(man, key=lambda k: int(k[3:])):
    p = os.path.join(ZZ, "src", key + ".png")
    h = hashlib.md5(open(p, "rb").read()).hexdigest()
    ok = h == man[key]["md5"]
    extra = ""
    if key == "img2":
        with Image.open(p) as im:
            rgba = im.convert("RGBA")
            ah = Counter(rgba.getchannel("A").getdata())
            extra = " alpha_hist=%s" % ah.most_common(5)
    log.write("%s manifest=%s now=%s match=%s%s\n" % (key, man[key]["md5"], h, ok, extra))
log.close()
print("VERIFY_DONE")
