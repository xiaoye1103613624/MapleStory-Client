#!/usr/bin/env python3
"""Match the empty-slot ghost sprite in the reference to derive slot positions."""
import sys

sys.path.insert(0, r"e:/project/MapleStory-Client/tools")
import _wz
from PIL import Image

keys = _wz.load_keys()
_wz.keys_all = keys
data = open(r"E:/MXD/BeiDou-Client-xy/Data/UI/Login.img", "rb").read()
img = _wz.Img(data)
img.parse(keys)

ew, eh, ebgra = img.decode_bitmap(img.find("CharSelect/character/1/0"))
rgba = bytearray(ew * eh * 4)
for i in range(ew * eh):
    b, g, r_, a = ebgra[i * 4:i * 4 + 4]
    rgba[i * 4:i * 4 + 4] = bytes((r_, g, b, a))
ghost = Image.frombytes("RGBA", (ew, eh), bytes(rgba))
gp = ghost.load()

ref = Image.open(r"E:/project/MapleStory-Client/docs/features/beidou-v17-parity/notes/evidence/r20/correct_shots/选择角色页.png").convert("RGB")
rp = ref.load()

# the ghost is translucent light blue: match where the screenshot is light-blue-ish
best = []
for dy in range(350, 450, 2):
    for dx in range(450, 800, 2):
        tot = 0
        n = 0
        for y in range(0, eh, 2):
            for x in range(0, ew, 2):
                gr, gg, gb, ga = gp[x, y]
                if ga < 60:
                    continue
                sr, sg, sb = rp[dx + x, dy + y]
                # ghost = light blue over scene: expect sb high, sr lower
                score = abs((sb - sr) - (gb - gr))
                tot += score
                n += 1
        if n > 50:
            best.append((tot / n, dx, dy))
best.sort()
print("top ghost matches:")
for b in best[:6]:
    print("  %.1f at (%d, %d)" % b)
