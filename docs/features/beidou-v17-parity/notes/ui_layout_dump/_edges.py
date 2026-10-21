import io

p = r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/_itemprof.txt"
lines = io.open(p, encoding="utf-8").read().splitlines()
res = []
prev = None
for l in lines:
    if not l:
        continue
    if l.startswith("="):
        res.append(l)
        prev = None
        continue
    parts = l.split()
    key = parts[0]
    val = float(parts[1])
    if prev is not None and abs(val - prev) >= 5:
        res.append("  %s %.1f (prev %.1f)" % (key, val, prev))
    prev = val

with io.open(r"e:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/ui_layout_dump/_edges.txt", "w", encoding="utf-8") as f:
    f.write("\n".join(res))
print("ok")
