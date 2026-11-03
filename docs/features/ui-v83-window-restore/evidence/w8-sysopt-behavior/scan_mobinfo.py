import re, sys, io

targets = [
    r"E:/MXD/BeiDou-ClientV17.7/BeiDou.exe",
    r"E:/MXD/Maplestory95.i64",
]
needles_txt = ["名字和HP都显示", "名字和HP", "怪物情报", "只显示名字", "HP都显示"]

pats = []
for t in needles_txt:
    try:
        pats.append((t + "|gbk", t.encode("gbk")))
    except Exception:
        pass
    pats.append((t + "|utf16le", t.encode("utf-16-le")))

out = io.open("mobinfo_scan.txt", "w", encoding="utf-8")
for path in targets:
    try:
        data = open(path, "rb").read()
    except Exception as e:
        out.write("!! %s: %r\n" % (path, e)); continue
    out.write("== %s size=%d\n" % (path, len(data)))
    for label, pat in pats:
        hits = [m.start() for m in re.finditer(re.escape(pat), data)]
        if hits:
            out.write("   HIT %-28s n=%d first=%s\n" % (label, len(hits), [hex(h) for h in hits[:6]]))
    out.flush()

# also scan every .nx under the native client's Data dir
import glob
for nx in glob.glob(r"E:/MXD/BeiDou-ClientV17.7/Data/**/*.nx", recursive=True):
    data = open(nx, "rb").read()
    for label, pat in pats:
        if pat in data:
            out.write("   NX %-40s %s\n" % (nx.replace("\\", "/"), label))
out.close()
print("done")
