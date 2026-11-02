"""B3-1 调色核验收：把 C++ 里实现的**整数**公式逐像素跑到原生帧上。

Graphics/MemoryTexture.cpp 的 tint_r / tint_gb 是纯整数式：
    R' = 17 * ((3*R + 306) // 68)
    G' = 17 * ((3*G +  33) // 68)      B' 同 G
证据脚本 tint_rects.py 用的是浮点式（0.75*x 后取最近 17 格、平手向下）。

两层判据，都要过：
  1) 穷举 0..255：整数式与浮点式对每个通道值都必须给出同一结果（等价，不是近似）；
  2) 逐矩形命中：整数式在原生帧的实测调色矩形上，命中率必须不劣于浮点式。
第一版整数实现（先四舍五入到整数再 +68 再平手向上）在第 2 条上只有 61%，浮点式
74%，就是这个门禁把它抓出来的。
"""
import numpy as np
from PIL import Image

FRAME = "C:/Users/11036/Desktop/正确客户端截图/角色装备.png"
OUT = "E:/project/_b3/tint_kernel_verify.txt"

# (素材, 素材在帧内的落位, [(素材内矩形 x,y,w,h) ...])
CASES = [
    ("backgrnd", "Equip_backgrnd.png", (0, 0),
     [(5, 233, 32, 32), (38, 233, 32, 32), (71, 233, 32, 32)]),
    ("pet", "Equip_pet.png", (171, 123),
     [(47, 11, 31, 32), (114, 11, 30, 32), (81, 77, 30, 65), (114, 77, 30, 65)]),
]

def snap_down(v):
    """浮点证据脚本的取整：最近 17 格，平手向下（等价于 tint_rects.py 的 argmin）。"""
    return int(np.ceil(v / 17.0 - 0.5)) * 17


def kernel_int(ch, is_red):
    """C++ 实现，逐字对齐 MemoryTexture.cpp 的 tint_r / tint_gb。"""
    k = (3 * int(ch) + (306 if is_red else 33)) // 68
    return min(17 * k, 255)


def pred_int(rgb):
    return tuple(int(c) for c in (kernel_int(rgb[0], True), kernel_int(rgb[1], False), kernel_int(rgb[2], False)))


def pred_float(rgb):
    v = [min(0.75 * rgb[0] + 68.25, 255.0), 0.75 * rgb[1], 0.75 * rgb[2]]
    return tuple(int(snap_down(np.float32(x))) for x in v)


frame = np.asarray(Image.open(FRAME).convert("RGB"), dtype=np.uint8)
lines = ["整数调色核（= Graphics/MemoryTexture.cpp 的实现）逐矩形验收", ""]

# 判据 1：穷举 0..255，整数式必须与浮点证据式给出完全相同的结果。
mismatch = []

for ch in range(256):
    for (name, is_red) in (("R", True), ("G", False), ("B", False)):
        idx = 0 if name == "R" else (1 if name == "G" else 2)
        rgb = [0, 0, 0]
        rgb[idx] = ch
        if kernel_int(ch, is_red) != pred_float(tuple(rgb))[idx]:
            mismatch.append("%s=%d 整数=%d 浮点=%d" % (name, ch, kernel_int(ch, is_red), pred_float(tuple(rgb))[idx]))

lines.append("判据1 穷举 0..255 x 3 通道 = 768 组：整数式 vs 浮点式 不一致 %d 组 %s" % (
    len(mismatch), "通过" if not mismatch else "不通过"))
lines.extend("        " + m for m in mismatch[:20])
lines.append("")
grand = [0, 0, 0]

for tag, png, (ox, oy), rects in CASES:
    art = np.asarray(Image.open("docs/features/ui-v83-window-restore/evidence/r27-b3-equip/" + png).convert("RGBA"), dtype=np.uint8)
    lines.append("== %s @帧(%d,%d) %dx%d" % (tag, ox, oy, art.shape[1], art.shape[0]))

    for (rx, ry, rw, rh) in rects:
        sub = art[ry:ry + rh, rx:rx + rw]
        fr = frame[oy + ry:oy + ry + rh, ox + rx:ox + rx + rw]
        opaque = sub[:, :, 3] > 0
        n = int(opaque.sum())
        hi = hf = 0
        worse = []

        for y in range(rh):
            for x in range(rw):
                if not opaque[y, x]:
                    continue
                want = tuple(int(c) for c in fr[y, x])
                pi = pred_int(tuple(int(c) for c in sub[y, x, :3]))
                pf = pred_float(tuple(int(c) for c in sub[y, x, :3]))
                if pi == want:
                    hi += 1
                if pf == want:
                    hf += 1
                if pi != want and pf == want:
                    worse.append(((x, y), tuple(int(c) for c in sub[y, x, :3]), want, pi, pf))

        grand[0] += n
        grand[1] += hi
        grand[2] += hf
        lines.append("   矩形 素材内 x=%d..%d y=%d..%d (%dx%d) 不透明=%d" % (rx, rx + rw - 1, ry, ry + rh - 1, rw, rh, n))
        lines.append("      整数式命中 %d = %.2f%%   浮点式命中 %d = %.2f%%   整数式劣化 %d 格" % (
            hi, 100.0 * hi / max(1, n), hf, 100.0 * hf / max(1, n), len(worse)))

        for pos, src, want, pi, pf in worse[:12]:
            lines.append("        劣化 @%s  素材=%s  帧=%s  整数=%s  浮点=%s" % (pos, src, want, pi, pf))

lines.append("")
lines.append("合计 不透明=%d  整数式命中=%d (%.2f%%)  浮点式命中=%d (%.2f%%)" % (
    grand[0], grand[1], 100.0 * grand[1] / max(1, grand[0]), grand[2], 100.0 * grand[2] / max(1, grand[0])))

with open(OUT, "w", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
print("written " + OUT)
