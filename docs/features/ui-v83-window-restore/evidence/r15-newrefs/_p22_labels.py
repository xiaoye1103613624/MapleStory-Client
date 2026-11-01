# -*- coding: utf-8 -*-
"""_p22_labels.py -- build high-zoom, ASCII-labelled strips for (a) every
dynamic-text band found by _p15_content.py and (b) every matched control
rectangle from _p8_chrome.txt, so the CJK glyphs can actually be read and the
readings tied back to exact source coordinates.

Nothing is measured here except the crop boxes (those come from _p15/_p8);
this script only renders.  Output images:
    _crops/<stem>.labels.png    text bands, 4x..8x NEAREST, stacked
    _crops/<stem>.buttons.png   control rects, 6x NEAREST, stacked
Every strip is prefixed by a magenta banner carrying
    "y=<top> x=<left>  <w>x<h>"  in the PIL default font (ASCII only).
"""
import io, os, re
import numpy as np
from PIL import Image, ImageDraw

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
THRESH = 40
BANNER = 14
GAP = 6
MAG = (255, 0, 255)

SHOTS = [
    ("\u52cb\u7ae0\u6846.png", "medal", "_nx/Title_backgrnd.png", (0, 1)),
    ("\u597d\u53cb\u5217\u8868\u6846.png", "friend", "_nx/UserList_backgrnd3.png", (-4, -4)),
    ("\u7ec4\u5458\u5217\u8868\u6846.png", "party", "_nx/UserList_backgrnd.png", (1, -3)),
    ("\u62d2\u7edd\u5217\u8868\u6846.png", "blacklist", "_nx/UserList_backgrnd.png", (0, 2)),
    ("\u5bb6\u65cf\u8054\u76df.png", "guildunion", "_nx/UserList_backgrnd2.png", (-1, 0)),
    ("\u597d\u53cb\u3001\u7ec4\u5458\u3001\u5bb6\u65cf\u3001\u5bb6\u65cf\u8054\u76df\u3001\u62d2\u7edd\u5217\u8868.png",
     "allfive", "_nx/UserList_backgrnd2.png", (-1, -2)),
    ("\u4e2a\u4eba\u8bf7\u62a5\u6846.png", "userinfo", "_nx/UserInfo_backgrnd.png", (0, -1)),
    ("\u5b66\u9662.png", "academy", "_nx/Family_backgrnd.png", (1, -1)),
]

HIT_RE = re.compile(r"^\s{4}(\S+)\s+(\d+)x(\d+)\s+x\[(\d+)\.\.(\d+)\] y\[(\d+)\.\.(\d+)\]"
                    r"\s+exact=([\d.]+) mae=([\d.]+) n=(\d+)")


def parse_hits(path):
    res, shot, weak = {}, None, False
    for line in io.open(path, encoding="utf-8"):
        if line.startswith("SCREENSHOT "):
            shot = line.split(None, 1)[1].strip().split("  ")[0]
            res.setdefault(shot, [])
            weak = False
            continue
        if "weak, tiny art" in line:
            weak = True
            continue
        if "below 120-opaque-px floor" in line:
            weak = False
            continue
        if shot is None:
            continue
        m = HIT_RE.match(line)
        if not m or weak:
            continue
        p, w, h, x0, x1, y0, y1, ex, mae, n = m.groups()
        res[shot].append((int(x0), int(y0), int(x1), int(y1), p, int(w), int(h),
                          float(ex), float(mae), int(n)))
    return res


def runs_1d(mask, gap=2, minlen=1):
    idx = np.nonzero(mask)[0]
    if len(idx) == 0:
        return []
    out, s, p = [], idx[0], idx[0]
    for i in idx[1:]:
        if i - p > gap + 1:
            out.append((s, p))
            s = i
        p = i
    out.append((s, p))
    return [(a, b) for a, b in out if b - a + 1 >= minlen]


def strip(rgb, label, scale, maxw=1500):
    h, w = rgb.shape[:2]
    s = scale
    while w * s > maxw and s > 1:
        s -= 1
    im = Image.fromarray(rgb.astype(np.uint8), "RGB").resize((w * s, h * s), Image.NEAREST)
    canvas = Image.new("RGB", (max(im.size[0], 260), im.size[1] + BANNER), (40, 40, 40))
    canvas.paste(im, (0, BANNER))
    d = ImageDraw.Draw(canvas)
    d.rectangle([0, 0, canvas.size[0] - 1, BANNER - 1], fill=MAG)
    d.text((2, 3), label, fill=(0, 0, 0))
    return canvas, s


def stack(parts, dest):
    if not parts:
        return None
    w = max(p.size[0] for p in parts)
    h = sum(p.size[1] for p in parts) + GAP * (len(parts) - 1)
    out = Image.new("RGB", (w, h), (255, 255, 255))
    y = 0
    for p in parts:
        out.paste(p, (0, y))
        y += p.size[1] + GAP
    out.save(dest)
    return out.size


def main():
    hits = parse_hits(os.path.join(OUT, "_p8_chrome.txt"))
    man = io.open(os.path.join(OUT, "_p22_labels.txt"), "w", encoding="utf-8")
    for fn, stem, tp, (dx, dy) in SHOTS:
        img = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB")).astype(np.int16)
        tpl = np.array(Image.open(os.path.join(OUT, tp)).convert("RGBA")).astype(np.int16)
        th, tw = tpl.shape[:2]
        H, Iw = img.shape[:2]
        content = np.zeros((H, Iw), bool)
        y0, y1 = max(0, dy), min(H, dy + th)
        x0, x1 = max(0, dx), min(Iw, dx + tw)
        sub = tpl[y0 - dy:y1 - dy, x0 - dx:x1 - dx]
        dif = np.abs(img[y0:y1, x0:x1] - sub[:, :, :3]).max(axis=2)
        content[y0:y1, x0:x1] = (dif > THRESH) & (sub[:, :, 3] == 255)
        ctl = np.zeros((H, Iw), bool)
        for rec in hits.get(fn, []):
            ctl[max(0, rec[1]):min(H, rec[3] + 1), max(0, rec[0]):min(Iw, rec[2] + 1)] = True
        txt = content & ~ctl
        man.write("=" * 70 + "\n%s (%s)\n" % (fn, stem))
        # ---- text bands -------------------------------------------------
        parts = []
        for (a, b) in runs_1d(txt.sum(axis=1) > 0, gap=2):
            m = txt[a:b + 1]
            cols = np.nonzero(m.sum(axis=0))[0]
            if len(cols) == 0:
                continue
            cx0, cx1 = max(0, cols[0] - 3), min(Iw - 1, cols[-1] + 3)
            crop = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB"))[a:b + 1, cx0:cx1 + 1]
            lab = "y=%d..%d x=%d..%d px=%d" % (a, b, cx0, cx1, int(m.sum()))
            c, s = strip(crop, lab, max(3, min(8, 260 // max(1, crop.shape[0]))))
            parts.append(c)
            man.write("  band %s  scale=%dx\n" % (lab, s))
        sz = stack(parts, os.path.join(OUT, "_crops", stem + ".labels.png"))
        man.write("  -> _crops/%s.labels.png %s (%d strips)\n" % (stem, sz, len(parts)))
        # ---- control rects ---------------------------------------------
        parts = []
        seen = set()
        src = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB"))
        for (a, b, c, d, p, bw, bh, ex, mae, n) in sorted(hits.get(fn, []),
                                                           key=lambda r: (r[1], r[0])):
            key = (a, b, c, d)
            if key in seen:
                continue
            seen.add(key)
            crop = src[max(0, b):min(H, d + 1), max(0, a):min(Iw, c + 1)]
            if crop.size == 0:
                continue
            lab = "%s  x%d y%d %dx%d e=%.3f m=%.2f" % (p, a, b, c - a + 1, d - b + 1, ex, mae)
            parts.append(strip(crop, lab, 6)[0])
        sz = stack(parts, os.path.join(OUT, "_crops", stem + ".buttons.png"))
        man.write("  -> _crops/%s.buttons.png %s (%d rects)\n\n" % (stem, sz, len(parts)))
    man.close()
    print("ok")


if __name__ == "__main__":
    main()
