import io
from PIL import Image

out = io.open(r"E:/project/_r25_dlg_frames.txt", "w", encoding="utf-8")

CASES = [
    (r"C:/Users/11036/.qoder/tmp/E--project-MapleStory-Client/images/0a2cb70c-a3a9-453e-8f4b-ad1f92fc6066/7780ed0f-ce55-4ecf-aab9-0a66e53e8568.png", "ref", [(58, 158)]),
    (r"C:/Users/11036/Desktop/正确客户端截图/NPC对话2.png", "npc2", [(39, 122)]),
    (r"C:/Users/11036/Desktop/正确客户端截图/NPC对话.png", "npc1", [(72, 101)]),
]


def runs(idx, pred):
    r, s = [], None
    for i in idx:
        if pred(i):
            if s is None:
                s = i
        else:
            if s is not None:
                r.append((s, i - 1, i - s))
                s = None
    if s is not None:
        r.append((s, idx[-1], idx[-1] - s + 1))
    return r


for path, tag, bands in CASES:
    im = Image.open(path).convert("RGB")
    W, H = im.size
    px = im.load()

    def dk(x, y):
        p = px[x, y]
        return (p[0] + p[1] + p[2]) // 3 < 170

    for (ya, yb) in bands:
        cs = set(x for x in range(W) if sum(1 for y in range(ya, yb + 1) if dk(x, y)) >= 3)
        rr = [r for r in runs(range(145, W - 6), lambda x: x in cs) if r[2] >= 2]
        out.write("%s y%d..%d: first text col=%s last=%s  nruns=%d\n" % (tag, ya, yb, rr[0], rr[-1], len(rr)))
        # per-line extents
        for b in runs(range(ya, yb + 1), lambda y: sum(1 for x in range(150, W - 15) if dk(x, y)) >= 8):
            y0, y1 = b[0], b[1]
            mid = (y0 + y1) // 2
            xs = [x for x in range(145, W - 6) if dk(x, mid)]
            out.write("    line y%d..%d  x %s..%s\n" % (y0, y1, min(xs) if xs else "-", max(xs) if xs else "-"))
out.close()
print("done")
