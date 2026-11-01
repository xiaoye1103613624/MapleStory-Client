# r15a_s14_det: md5 the img2 source, dump key pixels twice in one process,
# and also md5 all 15 sources + record mtimes/sizes.
import os, io, json, hashlib, time
from PIL import Image
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
paths = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))
log = io.open(os.path.join(OUTDIR, "_j_det.txt"), "w", encoding="utf-8")
p2 = paths["img2"]
st = os.stat(p2)
log.write("img2 size=%d mtime=%s now=%s\n" % (st.st_size, time.ctime(st.st_mtime), time.ctime()))
h = hashlib.md5(open(p2, "rb").read()).hexdigest()
log.write("img2 md5=%s\n" % h)
for rep in range(2):
    with Image.open(p2) as im:
        im2 = im.convert("RGB")
        px = im2.load()
        log.write("rep%d size=%s px[82,23]=%s px[86,20]=%s px[100,30]=%s px[86,14]=%s\n"
                  % (rep, im2.size, px[82, 23], px[86, 20], px[100, 30], px[86, 14]))
        n66 = sum(1 for y in range(im2.size[1]) for x in range(im2.size[0]) if px[x, y] == (66, 66, 66))
        log.write("rep%d count(66,66,66)=%d\n" % (rep, n66))
        time.sleep(0.3)
for key in sorted(paths, key=lambda k: int(k[3:])):
    p = paths[key]
    st = os.stat(p)
    log.write("%s size=%9d mtime=%s md5=%s\n" % (key, st.st_size, time.ctime(st.st_mtime),
              hashlib.md5(open(p, "rb").read()).hexdigest()))
log.close()
print("OK")
