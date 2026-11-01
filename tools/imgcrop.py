"""截图量具：裁剪 / 放大 / 并排对照。

用法
  python tools/imgcrop.py size <src>...                       # 只看尺寸
  python tools/imgcrop.py crop <src> x0 y0 x1 y1 <out> [scale] # 裁一块并放大
  python tools/imgcrop.py pair <a> ax0 ay0 ax1 ay1 <b> bx0 by0 bx1 by1 <out> [scale]

像素坐标 = 图片自身坐标（1x）。scale 默认 3。
"""
import sys
from PIL import Image


def argb(im):
    return im.convert("RGBA")


def do_size(argv):
    for p in argv:
        im = Image.open(p)
        print("%-10s %dx%d  %s" % (im.mode, im.size[0], im.size[1], p))


def do_crop(argv):
    src, x0, y0, x1, y1, out = argv[0], int(argv[1]), int(argv[2]), int(argv[3]), int(argv[4]), argv[5]
    scale = int(argv[6]) if len(argv) > 6 else 3
    im = argb(Image.open(src))
    crop = im.crop((x0, y0, x1, y1))
    crop = crop.resize((crop.width * scale, crop.height * scale), Image.NEAREST)
    crop.save(out)
    print("saved %s  region=%dx%d at (%d,%d) -> %dx%d" % (out, x1 - x0, y1 - y0, x0, y0, crop.width, crop.height))


def do_pair(argv):
    a = argv[0]
    ax0, ay0, ax1, ay1 = [int(v) for v in argv[1:5]]
    b = argv[5]
    bx0, by0, bx1, by1 = [int(v) for v in argv[6:10]]
    out = argv[10]
    scale = int(argv[11]) if len(argv) > 11 else 3
    ca = argb(Image.open(a)).crop((ax0, ay0, ax1, ay1))
    cb = argb(Image.open(b)).crop((bx0, by0, bx1, by1))
    h = max(ca.height, cb.height) + 8
    w = ca.width + cb.width + 12
    canvas = Image.new("RGBA", (w, h), (40, 40, 40, 255))
    canvas.paste(ca, (4, 4), ca)
    canvas.paste(cb, (8 + ca.width, 4), cb)
    canvas = canvas.resize((canvas.width * scale, canvas.height * scale), Image.NEAREST)
    canvas.save(out)
    print("saved %s  A=%dx%d@(%d,%d) B=%dx%d@(%d,%d)" % (out, ca.width, ca.height, ax0, ay0, cb.width, cb.height, bx0, by0))


def main():
    mode, argv = sys.argv[1], sys.argv[2:]
    if mode == "size":
        do_size(argv)
    elif mode == "crop":
        do_crop(argv)
    elif mode == "pair":
        do_pair(argv)
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
