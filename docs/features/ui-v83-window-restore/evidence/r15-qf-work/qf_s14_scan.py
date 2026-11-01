# qf_s14_scan.py - RLE scanline dumper for exact edge finding
import io, hashlib, sys
from PIL import Image

ROOT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\r15-qf-work"
MD5 = {
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

def rle_run(img, fixed, axis, lo, hi, step=1):
    # axis='row': scan x from lo..hi at y=fixed; axis='col': scan y
    px = img.load()
    out = []
    if axis == 'row':
        seq = [px[x, fixed] for x in range(lo, hi+1, step)]
        base = lo
    else:
        seq = [px[fixed, y] for y in range(lo, hi+1, step)]
        base = lo
    i = 0
    while i < len(seq):
        j = i
        while j+1 < len(seq) and seq[j+1] == seq[i]:
            j += 1
        out.append((base+i*(step if step>1 else 1) if step>1 else base+i,
                    base+j if step==1 else base+j*step, seq[i]))
        i = j+1
    return out

def fmt(runs, maxruns=400):
    s = []
    for a,b,c in runs[:maxruns]:
        s.append("%d-%d%s%s" % (a, b, "" if a==b else "(x%d)"%(b-a+1), c))
    return " ".join(s)

def main():
    key = sys.argv[1]
    img = load(key)
    W,H = img.size
    out = io.open(ROOT + r"\qf_s14_%s.txt" % key, 'w', encoding='utf-8')
    out.write(u"%s %dx%d\n" % (key, W, H))
    # rows/cols given as e.g. r180:0:539 c270:0:356
    for spec in sys.argv[2:]:
        axis = 'row' if spec[0]=='r' else 'col'
        rest = spec[1:].split(':')
        fixed = int(rest[0]); lo = int(rest[1]); hi = min(int(rest[2]), (H if axis=='col' else W) - 1)
        out.write(u"%s=%d [%d..%d]: %s\n" % (axis, fixed, lo, hi, fmt(rle_run(img, fixed, axis, lo, hi))))
    out.close()
    print("OK", key)

main()
