"""Composite a native-fidelity preview of UIWindow.img/SoftKeyboard BEFORE writing C++.

Engine geometry (verified):
  MapleButton::draw(parentpos) -> Texture::draw(pos=parentpos+button.position)
  DrawArgument::get_rectangle: rlt = pos - center - origin
  => button top-left = parentpos + button.position - texture.origin
  So ONE shared `anchor` for all 37 key buttons + each key's native origin
  lays the whole QWERTY out automatically (origin.x bigger => further left).

This script renders, from the real runtime UI.nx bitmaps:
  preview_tray_only.png   back frame + title + keyboard tray (no caps)
  preview_caps_only.png   back frame + 36 key caps + funckeys (no tray)
  preview_full.png        frame + title + tray + caps + funckeys
and dumps the exact per-key positions so the C++ constants can be checked.
"""
import os
import sys
import json
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import nx_inspect as ni  # noqa: E402

NX = r"E:/MXD/BeiDou-Client-xy/Data/.nxcache/UI.nx"
OUT = os.path.join(HERE, "evidence", "keys")
B = "UIWindow.img/SoftKeyboard"
nx = ni.Nx(NX)
ORIG = json.load(open(os.path.join(OUT, "origins.json"), encoding="utf-8"))


def img(path):
    i = nx.find(path)
    if i is None:
        return None
    w, h, px = nx.bitmap_pixels(i)
    if px is None:
        return None
    rgba = bytearray(len(px))
    rgba[0::4] = px[2::4]
    rgba[1::4] = px[1::4]
    rgba[2::4] = px[0::4]
    rgba[3::4] = px[3::4]
    return Image.frombytes("RGBA", (w, h), bytes(rgba))


# window geometry (tray-local preview coords)
FRAME = img(B + "/backgrnd/1/back")       # 315x127
TITLE = img(B + "/backgrnd/1/1")          # 147x34
TRAY = img(B + "/backgrnd/1/keyboard")    # 278x95
TRAY_TL = (18, 30)                        # tray inset in frame
ANCHOR = (265, 0)                         # shared button.position (tray-local)
# funckey desired top-left (tray-local), from the tray artwork
FUNC_TL = {
    "capslock": (2, 53),
    "lshift": (2, 76),
    "rshift": (231, 76),
    "enter": (245, 53),
    "backspace": (256, 6),
}
# k36 '-' shares origin with k35 'Z'; place explicitly at row1 col11
HYPHEN_TL = (237, 7)

W, H = 360, 175


def base(with_tray):
    canvas = Image.new("RGBA", (W, H), (150, 152, 160, 255))
    canvas.alpha_composite(FRAME, (10, 6))
    canvas.alpha_composite(TITLE, (10, 6))
    if with_tray:
        canvas.alpha_composite(TRAY, (10 + TRAY_TL[0], 6 + TRAY_TL[1]))
    return canvas


def draw_caps(canvas, with_hyphen=True, pressed=()):
    d = ImageDraw.Draw(canvas)
    pos_report = []
    for n in range(36):  # k00..k35 alphanumeric
        org = ORIG["keys"][str(n)]["normal"]["origin"]
        lt = (ANCHOR[0] - org[0], ANCHOR[1] - org[1])
        state = "pressed" if n in pressed else "normal"
        cap = img("%s/key/%d/%s/0" % (B, n, state))
        if cap is None:
            continue
        ax, ay = 10 + TRAY_TL[0] + lt[0], 6 + TRAY_TL[1] + lt[1]
        canvas.alpha_composite(cap, (ax, ay))
        pos_report.append((n, ORIG["keys"][str(n)]["char"], lt, (ax, ay)))
    if with_hyphen:
        cap = img("%s/key/36/normal/0" % B)
        if cap:
            canvas.alpha_composite(cap, (10 + TRAY_TL[0] + HYPHEN_TL[0], 6 + TRAY_TL[1] + HYPHEN_TL[1]))
        pos_report.append((36, "-", HYPHEN_TL, (10 + TRAY_TL[0] + HYPHEN_TL[0], 6 + TRAY_TL[1] + HYPHEN_TL[1])))
    for name, tl in FUNC_TL.items():
        fk = img("%s/funckey/%s/normal/0" % (B, name))
        if fk is None:
            continue
        canvas.alpha_composite(fk, (10 + TRAY_TL[0] + tl[0], 6 + TRAY_TL[1] + tl[1]))
        pos_report.append((name, "", tl, (10 + TRAY_TL[0] + tl[0], 6 + TRAY_TL[1] + tl[1])))
    return pos_report


# tray only
c = base(True)
c.convert("RGB").resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(OUT, "preview_tray_only.png"))

# caps only (no tray) with two pressed keys to show state feedback
c = base(False)
rep = draw_caps(c, pressed=(14, 26))
c.convert("RGB").resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(OUT, "preview_caps_only.png"))

# full
c = base(True)
rep = draw_caps(c, pressed=())
c.convert("RGB").resize((W * 2, H * 2), Image.NEAREST).save(os.path.join(OUT, "preview_full.png"))

print("wrote preview_tray_only.png / preview_caps_only.png / preview_full.png")
print("\ncap top-left (tray-local) by row:")
byrow = {}
for item in rep:
    n, ch, lt, _ = item
    if isinstance(n, str):
        continue
    byrow.setdefault(lt[1], []).append((lt[0], ch, n))
for y in sorted(byrow):
    print("  y=%3d : " % y + "  ".join("x%3d %r(k%02d)" % (x, ch, n) for x, ch, n in sorted(byrow[y])))
