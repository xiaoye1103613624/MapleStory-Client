"""W10 evidence helper (read-only): dump UIWindow.img/Quest bitmaps and build labelled contact sheets."""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from nx_inspect import Nx, write_png  # noqa: E402

NX = "E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
OUT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..",
                                   "docs", "features", "ui-v83-window-restore",
                                   "evidence", "w10-quest", "pngs"))
os.makedirs(OUT, exist_ok=True)

nx = Nx(NX)
try:
    FONT = ImageFont.truetype("C:/Windows/Fonts/arial.ttf", 13)
except Exception:
    FONT = ImageFont.load_default()


def dump(path):
    idx = nx.find(path)
    if idx is None:
        return None
    w, h, px = nx.bitmap_pixels(idx)
    if px is None:
        return None
    fp = os.path.join(OUT, path.replace("/", "_") + ".png")
    write_png(fp, w, h, px)
    return fp, w, h


def sheet(paths, out_name, scale=3, pad=20, col=5):
    items = []
    for p in paths:
        got = dump(p)
        if got:
            im = Image.open(got[0]).convert("RGBA")
            items.append((p, im.resize((im.width * scale, im.height * scale), Image.NEAREST)))
    if not items:
        print("empty", out_name)
        return
    cell_w = max(im.width for _, im in items) + pad
    row_h = max(im.height for _, im in items) + pad + 16
    rows = (len(items) + col - 1) // col
    canvas = Image.new("RGBA", (col * cell_w, rows * row_h), (26, 26, 34, 255))
    d = ImageDraw.Draw(canvas)
    for i, (p, im) in enumerate(items):
        cx = (i % col) * cell_w
        cy = (i // col) * row_h
        short = p.replace("UIWindow.img/", "").replace("Basic.img/", "Basic/")
        d.text((cx + 2, cy + 1), "%s %dx%d" % (short, im.width // scale, im.height // scale),
               fill=(255, 240, 120, 255), font=FONT)
        canvas.alpha_composite(im, (cx + 2, cy + 16))
    canvas.save(os.path.join(OUT, out_name))
    print("wrote", out_name, len(items), canvas.size)


GROUPS = [
    ("w10-buttons.png", [
        "UIWindow.img/Quest/BtAlert/normal/0", "UIWindow.img/Quest/BtDetail/normal/0",
        "UIWindow.img/Quest/BtGiveup/normal/0", "UIWindow.img/Quest/BtMarkNpc/normal/0",
        "UIWindow.img/Quest/BtNo/normal/0", "UIWindow.img/Quest/BtOK/normal/0",
        "UIWindow.img/Quest/BtAlert/disabled/0", "UIWindow.img/Quest/BtAlert/mouseOver/0",
        "UIWindow.img/Quest/BtAlert/pressed/0", "UIWindow.img/Quest/BtAlert/keyFocused/0",
        "UIWindow.img/BtUIClose/normal/0", "UIWindow.img/BtUIClose2/normal/0",
        "Basic.img/BtClose2/normal/0",
    ]),
    ("w10-strips.png", [
        "UIWindow.img/Quest/basic", "UIWindow.img/Quest/prob", "UIWindow.img/Quest/reward",
        "UIWindow.img/Quest/select", "UIWindow.img/Quest/summary",
        "UIWindow.img/Quest/notice0", "UIWindow.img/Quest/notice1", "UIWindow.img/Quest/notice2",
        "UIWindow.img/Quest/notice/0", "UIWindow.img/Quest/notice/1", "UIWindow.img/Quest/notice/2",
    ]),
    ("w10-tabs.png", [
        "UIWindow.img/Quest/Tab/disabled/0", "UIWindow.img/Quest/Tab/disabled/1",
        "UIWindow.img/Quest/Tab/disabled/2", "UIWindow.img/Quest/Tab/enabled/0",
        "UIWindow.img/Quest/Tab/enabled/1", "UIWindow.img/Quest/Tab/enabled/2",
    ]),
    ("w10-icons.png", [
        "UIWindow.img/Quest/icon0", "UIWindow.img/Quest/icon1", "UIWindow.img/Quest/icon4",
        "UIWindow.img/Quest/icon2/0", "UIWindow.img/Quest/icon3/0", "UIWindow.img/Quest/icon5/0",
        "UIWindow.img/Quest/icon6/0", "UIWindow.img/Quest/icon7/0", "UIWindow.img/Quest/icon8/0",
        "UIWindow.img/Quest/icon9/0",
    ]),
    ("w10-gauge.png", [
        "UIWindow.img/Quest/Gauge/frame", "UIWindow.img/Quest/Gauge2/frame",
        "UIWindow.img/Quest/Gauge/gauge", "UIWindow.img/Quest/Gauge/spot",
        "UIWindow.img/Quest/TimeQuest/TimeBar/backgrnd",
    ]),
]

for name, paths in GROUPS:
    sheet(paths, name)

for p in ["UIWindow.img/Quest/backgrnd", "UIWindow.img/Quest/backgrnd2",
          "UIWindow.img/Quest/backgrnd4", "UIWindow.img/Quest/backgrnd5",
          "UIWindow.img/Quest/backgrnd3"]:
    got = dump(p)
    print(("OK " if got else "MISS ") + p, got[1:] if got else "")
