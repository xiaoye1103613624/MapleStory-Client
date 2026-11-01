import sys
from PIL import Image

path = sys.argv[1]
target = tuple(int(x) for x in sys.argv[2:5])
tol = int(sys.argv[5]) if len(sys.argv) > 5 else 0
ymax = int(sys.argv[6]) if len(sys.argv) > 6 else 200
im = Image.open(path).convert('RGB')
w = im.width
for y in range(ymax):
    best = (0, 0, 0)
    run_start = -1
    for x in range(w):
        q = im.getpixel((x, y))
        hit = all(abs(q[i] - target[i]) <= tol for i in range(3))
        if hit:
            if run_start < 0:
                run_start = x
        else:
            if run_start >= 0:
                ln = x - run_start
                if ln > best[0]:
                    best = (ln, run_start, x - 1)
                run_start = -1
    if run_start >= 0 and w - run_start > best[0]:
        best = (w - run_start, run_start, w - 1)
    if best[0] > 8:
        print('y=%3d run=%3d x=%d..%d' % (y, best[0], best[1], best[2]))
