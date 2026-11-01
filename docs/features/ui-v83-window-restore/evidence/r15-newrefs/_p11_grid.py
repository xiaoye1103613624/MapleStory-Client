#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""_p11_grid.py -- annotated 1x-pixel coordinate grids so crop readings can be
converted back to exact pixel positions.

For every screenshot: upscale 3x (NEAREST), then draw
  * a cyan vertical line at every source x divisible by 10, a red one every 50
  * a cyan horizontal line at every source y divisible by 10, a red one every 50
  * the source coordinate (ASCII digits) just below/right of each line
Nothing else is changed, so a reader can state "the glyph starts at x=19".
"""
import io, os
from PIL import Image, ImageDraw

SRC = r"C:\Users\11036\Desktop\正确客户端截图"
OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "_crops")
S = 3
CYAN = (0, 255, 255)
RED = (255, 0, 0)

SHOTS = [("勋章框.png", "medal"), ("好友列表框.png", "friend"),
         ("组员列表框.png", "party"), ("拒绝列表框.png", "blacklist"),
         ("家族联盟.png", "guildunion"),
         ("好友、组员、家族、家族联盟、拒绝列表.png", "allfive"),
         ("个人请报框.png", "userinfo"), ("学院.png", "academy")]

man = io.open(os.path.join(OUT, "_p11_grid.txt"), "w", encoding="utf-8")
for fn, stem in SHOTS:
    im = Image.open(os.path.join(SRC, fn)).convert("RGB")
    w, h = im.size
    big = im.resize((w * S, h * S), Image.NEAREST)
    d = ImageDraw.Draw(big)
    for x in range(0, w, 10):
        c = RED if x % 50 == 0 else CYAN
        d.line([(x * S, 0), (x * S, h * S - 1)], fill=c, width=1)
        t = str(x)
        d.text((x * S + 1, 1), t, fill=(255, 0, 255))
        d.text((x * S + 1, h * S - 12), t, fill=(255, 0, 255))
    for y in range(0, h, 10):
        c = RED if y % 50 == 0 else CYAN
        d.line([(0, y * S), (w * S - 1, y * S)], fill=c, width=1)
        d.text((1, y * S + 1), str(y), fill=(0, 128, 255))
        d.text((w * S - 26, y * S + 1), str(y), fill=(0, 128, 255))
    big.save(os.path.join(OUT, stem + ".grid.png"))
    man.write("%s -> %s.grid.png  src %dx%d out %dx%d\n" % (fn, stem, w, h, w * S, h * S))
man.close()
print("ok")
