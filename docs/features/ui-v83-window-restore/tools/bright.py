from PIL import Image
import glob

for p in sorted(glob.glob('E:/project/_*.png')):
    im = Image.open(p).convert('RGB')
    px = im.getdata()
    r = max(q[0] for q in px)
    g = max(q[1] for q in px)
    b = max(q[2] for q in px)
    bright = sum(1 for q in px if min(q) > 200)
    print('%-24s %5dx%-5d max=(%3d,%3d,%3d) px>200=%d' % (
        p.split('/')[-1], im.width, im.height, r, g, b, bright))
