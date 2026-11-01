# r15a_s16_snap: snapshot the 15 sources into the evidence dir; measure only
# snapshots from now on. Record md5 twice (before/after copy) for stability proof.
import os, io, json, shutil, hashlib, time
OUTDIR = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-newrefs"
paths = json.load(io.open(os.path.join(OUTDIR, "_a_paths.json"), encoding="utf-8"))
snapdir = os.path.join(OUTDIR, "src")
os.makedirs(snapdir, exist_ok=True)
log = io.open(os.path.join(OUTDIR, "_l_snap.txt"), "w", encoding="utf-8")
newpaths = {}
for key in sorted(paths, key=lambda k: int(k[3:])):
    p = paths[key]
    h1 = hashlib.md5(open(p, "rb").read()).hexdigest()
    st = os.stat(p)
    dst = os.path.join(snapdir, "%s.png" % key)
    shutil.copyfile(p, dst)
    h2 = hashlib.md5(open(p, "rb").read()).hexdigest()
    hs = hashlib.md5(open(dst, "rb").read()).hexdigest()
    newpaths[key] = dst
    log.write("%s src_size=%9d mtime=%s md5_before=%s md5_after=%s md5_snap=%s stable=%s\n"
              % (key, st.st_size, time.ctime(st.st_mtime), h1, h2, hs, h1 == h2 == hs))
json.dump(newpaths, io.open(os.path.join(OUTDIR, "_l_snap_paths.json"), "w", encoding="utf-8"), indent=1)
log.close()
print("OK")
