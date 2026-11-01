# -*- coding: utf-8 -*-
"""Side-by-side crop helper: native reference vs our live capture."""
import sys
from PIL import Image

REF = 'C:/Users/11036/Desktop/正确客户端截图/'
CUR = 'C:/Users/11036/Desktop/当前客户端截图/'


def crop(src, box, out, scale=1):
    im = Image.open(src).convert('RGB').crop(box)
    if scale != 1:
        im = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
    im.save(out)
    print(out, im.size)


def stack(pairs, out, gap=8, scale=1):
    """pairs = [(path, box, label_skip)...] -> vertical stack with separators."""
    ims = []
    for src, box in pairs:
        im = Image.open(src).convert('RGB').crop(box)
        if scale != 1:
            im = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
        ims.append(im)
    w = max(i.width for i in ims)
    h = sum(i.height for i in ims) + gap * (len(ims) - 1)
    canvas = Image.new('RGB', (w, h), (40, 40, 40))
    y = 0
    for im in ims:
        canvas.paste(im, (0, y))
        y += im.height + gap
    canvas.save(out)
    print(out, canvas.size)


if __name__ == '__main__':
    globals()[sys.argv[1]](*sys.argv[2:])
