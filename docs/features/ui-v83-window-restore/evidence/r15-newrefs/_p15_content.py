#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p15_content.py -- isolate *dynamically drawn* content by subtracting the
matched frame bitmap.

For each screenshot the frame node and its (dx,dy) placement are known from the
template match (_p5/_p7/_p8).  The backgrnd bitmap already contains the baked-in
chrome (title text, banners, labels such as 「学院」/「勋章」).  Therefore:

    content(x,y) = max_c | img(x,y).c - tpl(x-dx, y-dy).c |  >  THRESH
                   for image pixels where the template pixel is FULLY opaque
                   (alpha == 255) -- i.e. real background, not a transparent
                   rounded corner.
    pixels outside the template rectangle are tagged OUTSIDE.

content is then everything the client paints at run time: names, numbers, tab
captions, button captions, icons.  Row projection over `content` (with the
matched control rectangles removed) gives the true text rows -- including
coloured text, which a plain "dark pixel" test misses.

THRESH = 40 (max-abs per channel).  Reported per band: y range, x extent, px.
Also writes:
   _crops/<stem>.content.png  3x view, content pixels red on the grey template
   _p15_content.txt           the numbers
"""
import io, os, re
import numpy as np
from PIL import Image

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
THRESH = 40

SHOTS = [
    ("勋章框.png", "medal", "_nx/Title_backgrnd.png", (0, 1)),
    ("好友列表框.png", "friend", "_nx/UserList_backgrnd3.png", (-4, -4)),
    ("组员列表框.png", "party", "_nx/UserList_backgrnd.png", (1, -3)),
    ("拒绝列表框.png", "blacklist", "_nx/UserList_backgrnd.png", (0, 2)),
    ("家族联盟.png", "guildunion", "_nx/UserList_backgrnd2.png", (-1, 0)),
    ("好友、组员、家族、家族联盟、拒绝列表.png", "allfive",
     "_nx/UserList_backgrnd2.png", (-1, -2)),
    ("个人请报框.png", "userinfo", "_nx/UserInfo_backgrnd.png", (0, -1)),
    ("学院.png", "academy", "_nx/Family_backgrnd.png", (1, -1)),
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
        p, w, h, x0, x1, y0, y1 = m.groups()[:7]
        res[shot].append((int(x0), int(y0), int(x1), int(y1)))
    return res


def runs_1d(mask, gap=2, minlen=1):
    """maximal True runs of a 1-D bool array, bridging gaps <= `gap`."""
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


hits = parse_hits(os.path.join(OUT, "_p8_chrome.txt"))
rep = io.open(os.path.join(OUT, "_p15_content.txt"), "w", encoding="utf-8")
W = rep.write
W("content = |img - frame_template| > %d (max channel) on template px with alpha==255\n"
  "matched control rectangles from _p8_chrome.txt are REMOVED before projecting\n\n" % THRESH)

for fn, stem, tp, (dx, dy) in SHOTS:
    img = np.array(Image.open(os.path.join(SRC, fn)).convert("RGB")).astype(np.int16)
    tpl = np.array(Image.open(os.path.join(OUT, tp)).convert("RGBA")).astype(np.int16)
    th, tw = tpl.shape[:2]
    H, Iw = img.shape[:2]
    content = np.zeros((H, Iw), bool)
    outside = np.zeros((H, Iw), bool)
    y0, y1 = max(0, dy), min(H, dy + th)
    x0, x1 = max(0, dx), min(Iw, dx + tw)
    sub = tpl[y0 - dy:y1 - dy, x0 - dx:x1 - dx]
    dif = np.abs(img[y0:y1, x0:x1] - sub[:, :, :3]).max(axis=2)
    content[y0:y1, x0:x1] = (dif > THRESH) & (sub[:, :, 3] == 255)
    outside[:] = True
    outside[y0:y1, x0:x1] = False
    W("=" * 78 + "\n%s  %dx%d  tpl %s at dx=%d dy=%d  (tpl rect x[%d..%d] y[%d..%d])\n"
      % (fn, Iw, H, tp, dx, dy, dx, dx + tw - 1, dy, dy + th - 1))
    W("  OUTSIDE-template px = %d (%.2f%% of image)\n"
      % (int(outside.sum()), 100.0 * outside.sum() / (H * Iw)))
    ctl = np.zeros((H, Iw), bool)
    for (a, b, c, d) in hits.get(fn, []):
        ctl[max(0, b):min(H, d + 1), max(0, a):min(Iw, c + 1)] = True
    txt = content & ~ctl
    W("  content px = %d ; after removing %d matched control rects -> text px = %d\n"
      % (int(content.sum()), len(hits.get(fn, [])), int(txt.sum())))
    prof = txt.sum(axis=1)
    bands = runs_1d(prof > 0, gap=2)
    W("  TEXT ROW BANDS (image y), pitch between band tops:\n")
    tops = []
    for (a, b) in bands:
        m = txt[a:b + 1]
        cols = np.nonzero(m.sum(axis=0))[0]
        if len(cols) == 0:
            continue
        tops.append(a)
        W("    y[%3d..%3d] h=%-3d px=%-5d x[%d..%d]  local y[%d..%d] x[%d..%d]\n"
          % (a, b, b - a + 1, int(m.sum()), cols[0], cols[-1],
             a - dy, b - dy, cols[0] - dx, cols[-1] - dx))
    if len(tops) > 1:
        diffs = [tops[i + 1] - tops[i] for i in range(len(tops) - 1)]
        W("  band-top deltas: %s\n" % diffs)
    # column projection inside each band, to find where a name/icon starts
    W("  per-band ink column runs (>=3 px wide):\n")
    for (a, b) in bands:
        m = txt[a:b + 1]
        cols = m.sum(axis=0) > 0
        rr = runs_1d(cols, gap=3, minlen=3)
        W("    y[%3d..%3d]: %s\n" % (a, b, " ".join("x[%d..%d]" % q for q in rr[:30])
                                     + (" ..." if len(rr) > 30 else "")))
    vis = Image.open(os.path.join(SRC, fn)).convert("RGB").copy()
    va = np.array(vis)
    va[txt] = (255, 0, 0)
    va[outside] = (0, 0, 0)
    vi = Image.fromarray(va).resize((Iw * 3, H * 3), Image.NEAREST)
    vi.save(os.path.join(OUT, "_crops", stem + ".content.png"))
    W("\n")
rep.close()
print("ok")
