#!/usr/bin/env python3
"""用 orange-wz 取出的【真】nw / ne / se 模板，独立量目标帧与本端帧的框跨度。

这是裁决 box_w=184 vs 196 的决定性判据：
 - 不用任何 (box_w-title_w)/2 居中公式去反推（那是自证）；
 - 直接在两端帧上用 WZ 真角件做 alpha-mask 滑窗 NCC，定位 nw_left / ne_left；
 - span = ne_left - nw_left 即两个内角距离，与代码里 cl->cr 直接对应。
若 target_span ≈ ours_span ⇒ 框宽没差，box_w=196 是过拟合，应回滚；
若 target_span - ours_span ≈ +12 ⇒ 目标框更宽 12，196 成立；
若 ≈ +8 ⇒ 呼应 ne 镜像判据，需另调。

模板来源：mcp__orange_wz__get_node_detail(nodes=[ne,se,e,n]) on
  E:\\MXD\\BeiDou-Client-xy\\Data\\UI\\Login.img (128B GMS128 key)
  WorldNotice/BalloonForLoginProcess/{ne,se,e,n}
真 ne=22x40, se=22x24, e=22x9, n=12x40, ARGB8888。
"""
import base64
import io
import json
import os

import numpy as np
from PIL import Image

OUT = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r3"
os.makedirs(OUT, exist_ok=True)

B64 = {
    "ne": "iVBORw0KGgoAAAANSUhEUgAAABYAAAAoCAYAAAD6xArmAAABuElEQVR4Xt2WgW2DMBBFWSErdIWukBVYgRW8QeUVvAIreAWvwAqscL1v49TxHcaOIkVqpCdoCM/fHwOdVmdoFGcNmWWO/JjFTNpn3wKNYs1CwTvaGOzXzvgh2qkfYtHMiRdaI2kGSM6qWyXeaATDYosa5nvcYgCz3CG+M18P8b556gc1zBx+i0CO9IBV6Hp+yOXJbZAY/WIQiJH4ELtD/j2hFlyAESDewpr2jyrQ8yG2U6rkNvnVUT82ypAYW7sgcbqAhTjVEUcfJEmP1cHSY8k9i/FlC0w93gxYDSwCsVc+5p2lwDNRE8epjII7kLeoJniri3GwF3Sbwd+oZWdUcfnjK/LKyPt5CarivNj/CM93W32spClWbltd+i5xS1hI0XOfWJGVt3Rey6n3dFHxfL4Wq4PI1OVAemLlpJq9kRqJz8V1Uo2TejCAXzVxLWihzA7ywKnHxGK51UCcKnm/OHY9klhINLK4N7EQnDEq7pb/b7E4uUWvWJx4xcfFw/KPiMWPR3hJnJ9g9ffdYvlg1wSC4nmsi8V/7buQnL098jtPf4PUUkWspS4HejmxlPd0XEtPxbW0Lf4FCO0Bik3esfIAAAAASUVORK5CYII=",
    "se": "iVBORw0KGgoAAAANSUhEUgAAABYAAAAYCAYAAAD+vg1LAAABJklEQVR4Xs2SAY3EIBBFsbAWsFALWMACFrCABSxgAQu1gIVamJtfaGALu+Vyze2SvjS09M1nqCDaqGNbL0iFla9Aa/QkhPCMYzQjRSf9fnH30Qy3irOoSj8nbhfWD3p+KyZeADp5W+A8r6RL8Uv5CE4KUtzFMbiB+EWaLR3EIjgIJWVOCmlwZiBuE79NfS5cC40Tn8XvOBU52nHd41lOBZD6/8THYczinc3SUgDtwEF2YvRnlrR6MlpVcZHHMOgxTnSGNTpKLDdG5xY0YmcHv1vwhuawXMCSZTG23vZZLbIXW6NohAO2YZ9r8oyUj337oEhDJ9ZqoSd0xjTkd7K+5zvkoEhbsWIeojy4C8ss4kYxkkKKtFKUgZ7cwZMUA5O/gq3v2z/GDzHnCMeN7rSXAAAAAElFTkSuQmCC",
}

# 真 nw 来自 spill tpl2_noticeNw.png（22x40），已在 ui_r3
NWP = os.path.join(OUT, "tpl2_noticeNw.png")

TARGET = r"C:\Users\11036\AppData\Local\Temp\term_verify\targets_desktop\desktop_channel_expanded.png"
OURS_V1 = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots\channel_grid_f05.png"


def load_tpl(b64):
    return Image.open(io.BytesIO(base64.b64decode(b64))).convert("RGBA")


def save_tpls():
    paths = {}
    for k, b in B64.items():
        im = load_tpl(b)
        p = os.path.join(OUT, "tpl2_balloon_real_%s.png" % k)
        im.save(p)
        paths[k] = p
    return paths


def alpha_ncc(shot_arr, tpl, roi):
    """alpha-mask 零均值 NCC 滑窗；返回 (best_ncc,(x,y))。"""
    t = np.asarray(tpl, dtype=np.float32)
    a = t[:, :, 3] / 255.0
    m = a > 0.3
    if m.sum() < 20:
        return None
    trgb = t[:, :, :3]
    tv = trgb[m] - trgb[m].mean(axis=0)
    denom_t = np.sqrt((tv ** 2).sum())
    th, tw = t.shape[0], t.shape[1]
    x0, y0, x1, y1 = roi
    best = (-2.0, None)
    img = shot_arr.astype(np.float32)
    for y in range(y0, min(y1, img.shape[0] - th)):
        for x in range(x0, min(x1, img.shape[1] - tw)):
            patch = img[y:y + th, x:x + tw, :3]
            pv = patch[m] - patch[m].mean(axis=0)
            denom_p = np.sqrt((pv ** 2).sum())
            if denom_p < 1e-6 or denom_t < 1e-6:
                continue
            ncc = float((pv * tv).sum() / (denom_p * denom_t))
            if ncc > best[0]:
                best = (ncc, (x, y))
    return best


def main():
    tpl_paths = save_tpls()
    tpls = {k: load_tpl(v) for k, v in B64.items()}
    tpls["nw"] = Image.open(NWP).convert("RGBA")
    print("TPL SIZES:", {k: v.size for k, v in tpls.items()})

    frames = {"TARGET": TARGET, "OURS_V1": OURS_V1}
    res = {}
    for name, path in frames.items():
        arr = np.asarray(Image.open(path).convert("RGB"))  # (720,1280,3)
        entry = {}
        # 左角 nw 在 x 240..320, y 285..345；右角 ne/se 在 x 400..510, y 285..345
        for key, roi in [("nw", (240, 285, 320, 345)),
                         ("ne", (400, 285, 510, 345)),
                         ("se", (400, 320, 510, 385))]:
            b = alpha_ncc(arr, tpls[key], roi)
            entry[key] = {"ncc": round(b[0], 4) if b else None,
                          "pos": list(b[1]) if b else None}
        if entry["nw"]["pos"] and entry["ne"]["pos"]:
            entry["span_nw_ne"] = entry["ne"]["pos"][0] - entry["nw"]["pos"][0]
        res[name] = entry
        print(name, json.dumps(entry))

    if "span_nw_ne" in res["TARGET"] and "span_nw_ne" in res["OURS_V1"]:
        d = res["TARGET"]["span_nw_ne"] - res["OURS_V1"]["span_nw_ne"]
        print("\nSPAN TARGET=%d OURS_V1=%d DIFF=%+d" %
              (res["TARGET"]["span_nw_ne"], res["OURS_V1"]["span_nw_ne"], d))
        if abs(d) <= 3:
            verdict = "BOX_WIDTH_EQUAL_196_OVERFIT_ROLLBACK"
        elif abs(d - 12) <= 3:
            verdict = "BOX_WIDTH_DIFF_12_SUPPORTS_196"
        elif abs(d - 8) <= 3:
            verdict = "BOX_WIDTH_DIFF_8_MIRROR_CONSISTENT"
        else:
            verdict = "BOX_WIDTH_DIFF_%d_UNKNOWN" % d
        print("VERDICT", verdict)
        res["verdict"] = verdict
        res["span_diff"] = d

    with open(os.path.join(OUT, "real_corners_settle.json"), "w") as f:
        json.dump(res, f, indent=2)
    print("SAVED real_corners_settle.json")


if __name__ == "__main__":
    main()
