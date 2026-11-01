#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p12_nxmontage.py -- put exported NX bitmaps on a checkerboard at 8x with an
ASCII label, so alpha shapes are visible when read with the image viewer."""
import io, os, sys
from PIL import Image, ImageDraw

OUT = os.path.dirname(os.path.abspath(__file__))
MD = os.path.join(OUT, "_nx")
S = 8
CELL = 8


def checker(w, h):
    im = Image.new("RGB", (w, h), (255, 255, 255))
    d = ImageDraw.Draw(im)
    for y in range(0, h, CELL):
        for x in range(0, w, CELL):
            if ((x // CELL) + (y // CELL)) % 2:
                d.rectangle([x, y, x + CELL - 1, y + CELL - 1], fill=(210, 210, 210))
    return im


def montage(files, labels, dest, colw=None):
    ims = []
    for f, lb in zip(files, labels):
        a = Image.open(f).convert("RGBA")
        w, h = a.size
        big = a.resize((w * S, h * S), Image.NEAREST)
        bg = checker(big.size[0], big.size[1])
        bg.paste(big, (0, 0), big)
        pan = Image.new("RGB", (300, max(20, big.size[1])), (250, 250, 250))
        d = ImageDraw.Draw(pan)
        d.text((2, 1), lb[:56], fill=(0, 0, 0))
        d.text((2, 10), lb[56:112], fill=(0, 0, 0))
        d.text((2, 19), "%dx%d  shown %dx%d" % (w, h, w * S, h * S), fill=(180, 0, 0))
        hh = max(pan.size[1], bg.size[1])
        row = Image.new("RGB", (300 + bg.size[0] + 4, hh), (255, 0, 255))
        row.paste(pan, (0, 0)); row.paste(bg, (304, 0))
        ims.append(row)
    mw = max(r.size[0] for r in ims)
    mh = sum(r.size[1] + 2 for r in ims)
    m = Image.new("RGB", (mw, mh), (20, 20, 20))
    y = 0
    for r in ims:
        m.paste(r, (0, y)); y += r.size[1] + 2
    m.save(dest)
    return dest, m.size


if __name__ == "__main__":
    args = sys.argv[1:]
    dest = args[0]
    rest = args[1:]
    pairs = [r.split("=", 1) for r in rest]
    files = [os.path.join(MD, p[0]) for p in pairs]
    labels = [p[1] for p in pairs]
    d, sz = montage(files, labels, dest)
    sys.stdout.write("wrote %s %dx%d\n" % (os.path.basename(d), sz[0], sz[1]))
