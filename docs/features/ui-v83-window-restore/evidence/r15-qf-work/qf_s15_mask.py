# qf_s15_mask.py - family-mask bbox + run profiler
# usage: python qf_s15_mask.py img3 "r>=b-10 and r>=100" [subrect x0:y0:x1:y1]
import io, sys, hashlib
from PIL import Image

ROOT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
MD5 = {
 'img1':'7e736cebeff4d5db35bbfc9ec109ea65','img2':'e4d03c62b88c8eef93904e7c663a79b9',
 'img3':'ca6baeb3ecdf8b0d1ea71eef72455332','img4':'44c1ed4c33415e1fae928645b26bbda8',
 'img5':'6f611be2682cc310d7f59b29ac923e9e','img6':'f7dba52544ae90d7929dd56be6b66cdf',
 'img7':'1a7090c2cadcb4560f4e716fbdd4e1fc','img8':'f7069a850077905ea355a02b10eec84d',
 'img9':'3875b92b444caa39b78bad74611e989d','img10':'558c7c0d545638d3ef612408bcc64ddd',
 'img11':'ae6298dfb5418b929fbfebb93549a0c5','img12':'c1cd8eebec12ca60dbc41bdc3476ba3b',
 'img13':'ef8586a5777a2119f7c3c235df491693','img14':'7d2c9fe479c63a22b789472e374db260',
 'img15':'9ac47a7f7e0e7b1a28e1acc1da07c8ab',
}

def load(key):
    p = ROOT + r"\src\%s.png" % key
    h = hashlib.md5(open(p,'rb').read()).hexdigest()
    assert h == MD5[key], (key, h)
    return Image.open(p).convert('RGB')

def runs(pairs):
    # pairs: sorted list of indices -> list of (start,end)
    out = []
    if not pairs: return out
    a = b = pairs[0]
    for v in pairs[1:]:
        if v == b+1: b = v
        else: out.append((a,b)); a = b = v
    out.append((a,b))
    return out

def main():
    key = sys.argv[1]
    pred = eval("lambda r,g,b: " + sys.argv[2])
    img = load(key)
    W,H = img.size
    x0,y0,x1,y1 = 0,0,W-1,H-1
    if len(sys.argv) > 3:
        a = list(map(int, sys.argv[3].split(':')))
        x0,y0,x1,y1 = a
    px = img.load()
    rows = {}   # y -> runs of x
    cols = {}   # x -> runs of y
    for y in range(y0, y1+1):
        xs = []
        for x in range(x0, x1+1):
            r,g,b = px[x,y]
            if pred(r,g,b): xs.append(x)
        if xs:
            rows[y] = runs(xs)
            for a,b in rows[y]:
                for xx in range(a,b+1):
                    cols.setdefault(xx, []).append(y)
    out = io.open(ROOT + (r"\qf_s15_%s_%s.txt" % (key, sys.argv[4] if len(sys.argv)>4 else 'm')), 'w', encoding='utf-8')
    out.write(u"%s pred=<%s> rect=%d:%d:%d:%d\n" % (key, sys.argv[2], x0,y0,x1,y1))
    allx = sorted(cols)
    ally = sorted(rows)
    out.write(u"bbox=(%d,%d)-(%d,%d) size=%dx%d rows_hit=%d cols_hit=%d\n" % (
        allx[0], ally[0], allx[-1], ally[-1], allx[-1]-allx[0]+1, ally[-1]-ally[0]+1, len(rows), len(cols)))
    out.write(u"--- per-row x-runs (only rows where runs<=8) ---\n")
    for y in ally:
        rr = rows[y]
        if len(rr) <= 8:
            out.write(u"y=%d: %s\n" % (y, " ".join("%d-%d"%t for t in rr)))
        else:
            out.write(u"y=%d: [%d runs] %d..%d\n" % (y, len(rr), rr[0][0], rr[-1][1]))
    out.write(u"--- per-col y-runs summary (cols where <=6 runs) ---\n")
    for x in allx:
        rr = runs(sorted(cols[x]))
        if len(rr) <= 6:
            out.write(u"x=%d: %s\n" % (x, " ".join("%d-%d"%t for t in rr)))
    out.close()
    print("OK")

main()
