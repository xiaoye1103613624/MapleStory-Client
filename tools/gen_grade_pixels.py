#!/usr/bin/env python3
"""Convert exported 192 Grade PNGs (0..5) into BGRA byte arrays for C++ header.
Reuses the EquipStarPixels.h pattern (MemoryTexture raw atlas, no WZ edit).
"""
import os
from PIL import Image

D = r"E:\project\MapleStory-Client\docs\features\equip-tooltip-native-fields-2026-11-20\resources\grade192"
OUT = r"E:\project\MapleStory-Client\Graphics\EquipGradePixels.h"

# 6 grades -> RGBA png index. Grade/0-2 small 7x7 solid, 3-5 large glow.
SRC = [f"_tree_children_{i}_children_0.png" for i in range(6)]
NAMES = ["NONE", "HIDDEN", "RARE", "EPIC", "UNIQUE", "LEGENDARY"]

imgs = []
for f in SRC:
    im = Image.open(os.path.join(D, f)).convert("RGBA")
    imgs.append(im)

lines = []
lines.append("#pragma once")
lines.append("// 装备品级角标像素，源自 192 客户端 UIWindow.img/Item/Grade/0..5")
lines.append("// （orange-wz 从 Data/UI/UI_003.wz 导出 PNG，零 IV + 128B key）。")
lines.append("// 0-2 = 7x7 实心小角标，3-5 = 34-36px 发光角标；BGRA，直传 GraphicsGL atlas。")
lines.append("#include <cstdint>")
lines.append("namespace ms { namespace equipgrade {")

# atlas ids
for i, nm in enumerate(NAMES):
    tag = f"G{i}"
    lines.append(f"constexpr size_t ID_{nm} = 0x475244{i}; // 'GRD{i}'")

for i, (im, nm) in enumerate(zip(imgs, NAMES)):
    w, h = im.size
    lines.append(f"constexpr int16_t W_{nm} = {w};")
    lines.append(f"constexpr int16_t H_{nm} = {h};")
    lines.append(f"static const uint8_t {nm}_BGRA[W_{nm}*H_{nm}*4] = {{")
    px = list(im.getdata())
    row = []
    for p in px:
        r, g, b, a = p
        row += [f"0x{b:02X}", f"0x{g:02X}", f"0x{r:02X}", f"0x{a:02X}"]
    # 每行 16 字节(4像素)
    for j in range(0, len(row), 16):
        lines.append("  " + ",".join(row[j:j+16]) + ",")
    lines.append("};")

lines.append("} }")
open(OUT, "w", encoding="utf-8").write("\n".join(lines) + "\n")
print("WROTE", OUT, "bytes", os.path.getsize(OUT))
for i, im in enumerate(imgs):
    print(f"  {NAMES[i]}: {im.size}")
