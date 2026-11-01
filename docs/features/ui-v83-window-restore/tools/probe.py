import sys
from PIL import Image
from collections import Counter

path = sys.argv[1]
mode = sys.argv[2]

if mode == 'find':
    target = tuple(int(x) for x in sys.argv[3:6])
    tol = int(sys.argv[6]) if len(sys.argv) > 6 else 6
    im = Image.open(path).convert('RGB')
    w, h = im.size
    mn = [w, h, 0, 0]
    n = 0
    for y in range(h):
        for x in range(w):
            q = im.getpixel((x, y))
            if all(abs(q[i] - target[i]) <= tol for i in range(3)):
                n += 1
                mn[0] = min(mn[0], x); mn[1] = min(mn[1], y)
                mn[2] = max(mn[2], x); mn[3] = max(mn[3], y)
    print('%s target=%s tol=%d count=%d bbox=%s' % (
        path.split('/')[-1], target, tol, n, mn if n else 'none'))
elif mode == 'probe':
    box = tuple(int(x) for x in sys.argv[3:7])
    im = Image.open(path).convert('RGB').crop(box)
    c = Counter(im.getdata())
    print('FILE %s box=%s size=%dx%d' % (path.split('/')[-1], box, im.width, im.height))
    for col, n in c.most_common(16):
        print('   %5d  rgb(%3d,%3d,%3d)' % (n, col[0], col[1], col[2]))
