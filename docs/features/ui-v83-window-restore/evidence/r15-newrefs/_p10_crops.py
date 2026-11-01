#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p10_crops.py -- build readable crops for transcription.

Inputs:  the 8 screenshots + _p8_chrome.txt (accepted template hits, parseable).
Outputs: _crops/<stem>.montage.png  one vertical strip: every accepted control
                                    crop upscaled 4x (NEAREST), preceded by an
                                    ASCII label panel drawn by PIL's default
                                    bitmap font (ASCII only, so cp936 is safe).
         _crops/<stem>.full3x.png   whole screenshot upscaled 3x (NEAREST).
         _crops/<stem>.rows3x.png   the list/body region upscaled 3x.
         _crops/_p10_crops.txt      ASCII manifest of what was written.
"""
import io, os, re, sys
import numpy as np
from PIL import Image, ImageDraw

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.dirname(os.path.abspath(__file__))
CROPDIR = os.path.join(OUT, "_crops")
if not os.path.isdir(CROPDIR):
    os.makedirs(CROPDIR)

STEMS = {
    "勋章框.png": "medal",
    "好友列表框.png": "friend",
    "组员列表框.png": "party",
    "拒绝列表框.png": "blacklist",
    "家族联盟.png": "guildunion",
    "好友、组员、家族、家族联盟、拒绝列表.png": "allfive",
    "个人请报框.png": "userinfo",
    "学院.png": "academy",
}

# body / list region in IMAGE coords (y0,y1) chosen from the measured frame +
# control bboxes; x is taken automatically (frame interior, left of scrollbar).
BODY = {
    "勋章框.png":            (22, 345),
    "好友列表框.png":         (16, 336),
    "组员列表框.png":         (16, 336),
    "拒绝列表框.png":         (21, 340),
    "家族联盟.png":           (19, 312),
    "好友、组员、家族、家族联盟、拒绝列表.png": (19, 312),
    "个人请报框.png":         (0, 197),
    "学院.png":               (0, 392),
}

HIT_RE = re.compile(r"^\s{4}(\S+)\s+(\d+)x(\d+)\s+x\[(\d+)\.\.(\d+)\] y\[(\d+)\.\.(\d+)\]"
                    r"\s+exact=([\d.]+) mae=([\d.]+) n=(\d+)")


def parse_hits(path):
    """accepted hits only (the file's 'weak' block is indented the same, so we
    track the current sub-block header instead)."""
    res, shot, weak = {}, None, False
    for line in io.open(path, encoding="utf-8"):
        if line.startswith("SCREENSHOT "):
            shot = line.split(None, 1)[1].strip().split("  ")[0]
            res.setdefault(shot, [])
            weak = False
            continue
        if "below 120-opaque-px floor" in line:
            weak = False          # header of an accepted block
            continue
        if "weak, tiny art" in line:
            weak = True
            continue
        if shot is None:
            continue
        m = HIT_RE.match(line)
        if not m:
            continue
        p, w, h, x0, x1, y0, y1, ex, mae, n = m.groups()
        if weak:
            continue
        res[shot].append(dict(path=p, w=int(w), h=int(h), x0=int(x0), x1=int(x1),
                              y0=int(y0), y1=int(y1), exact=float(ex), mae=float(mae),
                              n=int(n)))
    return res


def dedupe(hits):
    """drop duplicate pixels caused by identical bitmaps under different node
    names: keep one per (x0,y0,w,h), preferring the shortest path."""
    by = {}
    for hh in hits:
        k = (hh["x0"], hh["y0"], hh["w"], hh["h"])
        if k not in by or len(hh["path"]) < len(by[k]["path"]):
            by[k] = hh
    return sorted(by.values(), key=lambda d: (d["y0"], d["x0"]))


def main():
    hits = parse_hits(os.path.join(OUT, "_p8_chrome.txt"))
    man = io.open(os.path.join(OUT, "_p10_crops.txt"), "w", encoding="utf-8")
    W = man.write
    for fn, stem in STEMS.items():
        im = Image.open(os.path.join(SRC, fn)).convert("RGB")
        iw, ih = im.size
        hl = dedupe(hits.get(fn, []))
        # 3x whole
        im.resize((iw * 3, ih * 3), Image.NEAREST).save(
            os.path.join(CROPDIR, stem + ".full3x.png"))
        # 3x body
        by0, by1 = BODY[fn]
        sbx = None
        for hh in hl:
            if "VScr" in hh["path"]:
                sbx = hh["x0"] if sbx is None else min(sbx, hh["x0"])
        bx0 = 0
        bx1 = (sbx - 1) if sbx else iw
        bx1 = min(bx1, iw)
        body = im.crop((bx0, max(0, by0), bx1, min(ih, by1)))
        bw, bh = body.size
        body.resize((bw * 3, bh * 3), Image.NEAREST).save(
            os.path.join(CROPDIR, stem + ".body3x.png"))
        # montage of the crops at 4x, left label panel 300px, ASCII
        SC = 4
        rows = []
        for hh in hl:
            c = im.crop((max(0, hh["x0"]), max(0, hh["y0"]),
                         min(iw, hh["x1"] + 1), min(ih, hh["y1"] + 1)))
            cw, ch = c.size
            c = c.resize((cw * SC, ch * SC), Image.NEAREST)
            pan = Image.new("RGB", (300, max(c.size[1], 16)), (250, 250, 250))
            d = ImageDraw.Draw(pan)
            d.text((2, 1), hh["path"][:56], fill=(0, 0, 0))
            d.text((2, 10), hh["path"][56:], fill=(0, 0, 0))
            d.text((2, 19), "%dx%d x[%d..%d] y[%d..%d] e=%.4f m=%.3f n=%d"
                   % (hh["w"], hh["h"], hh["x0"], hh["x1"], hh["y0"], hh["y1"],
                      hh["exact"], hh["mae"], hh["n"]), fill=(180, 0, 0))
            if pan.size[1] < c.size[1]:
                p2 = Image.new("RGB", (300, c.size[1]), (250, 250, 250))
                p2.paste(pan, (0, 0)); pan = p2
            if c.size[1] < pan.size[1]:
                c2 = Image.new("RGB", (c.size[0], pan.size[1]), (255, 255, 255))
                c2.paste(c, (0, 0)); c = c2
            row = Image.new("RGB", (300 + c.size[0] + 4, c.size[1]), (255, 0, 255))
            row.paste(pan, (0, 0)); row.paste(c, (304, 0))
            rows.append(row)
        if rows:
            mw = max(r.size[0] for r in rows)
            mh = sum(r.size[1] + 2 for r in rows)
            mont = Image.new("RGB", (mw, mh), (20, 20, 20))
            y = 0
            for r in rows:
                mont.paste(r, (0, y)); y += r.size[1] + 2
            mont.save(os.path.join(CROPDIR, stem + ".montage.png"))
        W("%-42s stem=%-11s img=%dx%d hits=%d body3x=%dx%d montage=%s\n"
          % (fn, stem, iw, ih, len(hl), bw * 3, bh * 3,
             (stem + ".montage.png") if rows else "n/a"))
        for hh in hl:
            W("      %-70s x[%d..%d] y[%d..%d]\n" % (hh["path"], hh["x0"], hh["x1"],
                                                      hh["y0"], hh["y1"]))
    man.close()
    print("ok")


main()
