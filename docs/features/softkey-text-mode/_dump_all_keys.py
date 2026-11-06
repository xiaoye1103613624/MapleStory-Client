"""Dump UIWindow.img/SoftKeyboard bitmaps from runtime UI.nx into contact sheets.

Purpose: read the key-number -> character mapping, which is printed INTO each
21x22 key bitmap. The JSON structure dump has no pixel data, so only the actual
artwork can settle which of key/0..36 is which character.

Output (all under docs/features/softkey-text-mode/evidence/):
  keys/kNN.png             individual key bitmaps, normal/0, N=0..36
  keys/keys_sheet.png      37 keys, 5x NEAREST, numbered, 10 columns
  keys/funckey_sheet.png   capslock/lshift/rshift/enter/backspace normal/0
  keys/backgrnd_sheet.png  backgrnd/0 + /1 back & keyboard (downscaled to fit)
"""
import os
import sys

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import nx_inspect as ni  # noqa: E402

NX_PATH = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
OUT = os.path.join(HERE, "evidence", "keys")
os.makedirs(OUT, exist_ok=True)
BASE = "UIWindow.img/SoftKeyboard"

nx = ni.Nx(NX_PATH)


def get_img(path):
    idx = nx.find(path)
    if idx is None:
        return None
    w, h, px = nx.bitmap_pixels(idx)
    if px is None:
        return None
    rgba = bytearray(len(px))
    rgba[0::4] = px[2::4]  # R <- B
    rgba[1::4] = px[1::4]  # G
    rgba[2::4] = px[0::4]  # B <- R
    rgba[3::4] = px[3::4]  # A
    return Image.frombytes("RGBA", (w, h), bytes(rgba))


def numbered_sheet(items, cols, scale, cell_pad=6, label_h=18):
    """items: list of (label, PIL.Image). Composite on a light checker bg."""
    if not items:
        return None
    cw = max(im.width for _, im in items) * scale + cell_pad * 2
    ch = max(im.height for _, im in items) * scale + cell_pad * 2 + label_h
    rows = (len(items) + cols - 1) // cols
    sheet = Image.new("RGBA", (cols * cw, rows * ch), (210, 210, 214, 255))
    dr = ImageDraw.Draw(sheet)
    for i, (label, im) in enumerate(items):
        cx = (i % cols) * cw
        cy = (i // cols) * ch
        big = im.resize((im.width * scale, im.height * scale), Image.NEAREST)
        sheet.alpha_composite(big, (cx + cell_pad, cy + cell_pad + label_h))
        dr.text((cx + cell_pad, cy + 2), label, fill=(10, 10, 10, 255))
    return sheet


# 1) key/0..36 normal/0
keys = []
for n in range(37):
    path = "%s/key/%d/normal/0" % (BASE, n)
    im = get_img(path)
    tag = "k%02d" % n
    if im is None:
        print("MISS", path)
        continue
    im.save(os.path.join(OUT, "k%02d.png" % n))
    keys.append((tag, im))
print("keys dumped:", len(keys))
sheet = numbered_sheet(keys, cols=10, scale=5)
sheet.convert("RGB").save(os.path.join(OUT, "keys_sheet.png"))
print("wrote keys_sheet.png", sheet.size)

# 2) funckey normal/0 (and list all four states' presence)
fk = []
for name in ("capslock", "lshift", "rshift", "enter", "backspace"):
    for state in ("normal", "mouseOver", "pressed", "disabled"):
        im = get_img("%s/funckey/%s/%s/0" % (BASE, name, state))
        if im is not None:
            fk.append(("%s/%s" % (name, state), im))
print("funckey bitmaps:", len(fk))
if fk:
    s = numbered_sheet(fk, cols=4, scale=4)
    s.convert("RGB").save(os.path.join(OUT, "funckey_sheet.png"))
    print("wrote funckey_sheet.png", s.size)

# 3) backgrnd/0 and /1 : back / keyboard / 1  and Bt close/min/max
bg = []
for skin in ("0", "1"):
    for child in ("back", "keyboard", "1"):
        im = get_img("%s/backgrnd/%s/%s" % (BASE, skin, child))
        if im is not None:
            # downscale big backdrops so the sheet stays viewable
            if im.width > 400 or im.height > 400:
                im.thumbnail((320, 200))
            bg.append(("bg%s/%s %dx%d" % (skin, child, im.width, im.height), im))
    for bt in ("BtClose", "BtMin", "BtMax"):
        for state in ("normal", "mouseOver", "pressed", "disabled"):
            im = get_img("%s/Bt/%s/%s/%s/0" % (BASE, skin, bt, state))
            if im is not None:
                bg.append(("%s/%s/%s" % (skin, bt, state), im))
print("backgrnd+bt bitmaps:", len(bg))
if bg:
    s = numbered_sheet(bg, cols=5, scale=3)
    s.convert("RGB").save(os.path.join(OUT, "backgrnd_sheet.png"))
    print("wrote backgrnd_sheet.png", s.size)
