#!/usr/bin/env python3
"""定案前最后一测：目标帧 vs 第一版真机帧的公告【正文】暗字带 y 位置。

为什么必须量：真角件硬判据给出 box_w 184->192(+8)、box_h 74->80(+6~8)，
但 ChatBalloon.cpp:297 `text_y = ct + (cb-ct-textlabel.height())/2`，
cb = top + box_h - 24，改 box_h 会让正文下移 (80-74)/2 = +3px。
若本端正文当前已与目标对齐，这 3px 就是新引入的偏差，必须同步补偿 text_y；
若本端正文本来就比目标高 3px，改 box_h 反而一举两得。
measure_notice_body.py 旧判据 bbox 325..360 宽 35px（一行 A12B 仅约 12px），
把标题下沿/木轴暗边混进来了，两端都混所以「shift=0」不可靠 → 本脚本收紧 ROI。

ROI：内容区 x∈[308,442]（cl=302..cr=450 内缩避开 w/e 边件），y∈[332,356]。
判据：暗中性字 R,G,B 均 <120 且 |R-G|<25、|G-B|<25（排除暖色纸与红木轴）。
输出：逐行暗像素数、峰值行、加权中心 y、两端 shift。
"""
import json
import os

import numpy as np
from PIL import Image

OUT = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r3"
TARGET = r"C:\Users\11036\AppData\Local\Temp\term_verify\targets_desktop\desktop_channel_expanded.png"
OURS_V1 = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots\channel_grid_f05.png"
ROI = (308, 332, 442, 356)


def body_profile(path):
    a = np.asarray(Image.open(path).convert("RGB")).astype(np.int16)
    x0, y0, x1, y1 = ROI
    p = a[y0:y1, x0:x1, :]
    r, g, b = p[:, :, 0], p[:, :, 1], p[:, :, 2]
    dark = (r < 120) & (g < 120) & (b < 120)
    neutral = (np.abs(r - g) < 25) & (np.abs(g - b) < 25)
    m = dark & neutral
    rows = m.sum(axis=1).astype(float)
    ys = np.arange(y0, y1)
    total = rows.sum()
    center = float((rows * ys).sum() / total) if total > 0 else None
    top = sorted(zip(rows.tolist(), ys.tolist()), reverse=True)[:6]
    return {
        "total_dark_px": int(total),
        "weighted_center_y": round(center, 2) if center else None,
        "peak_rows": [{"y": int(y), "n": int(n)} for n, y in top],
        "rows": {int(y): int(n) for y, n in zip(ys, rows) if n > 0},
    }


def main():
    res = {}
    for name, path in [("TARGET", TARGET), ("OURS_V1", OURS_V1)]:
        e = body_profile(path)
        res[name] = e
        print(name, "total=%d center=%s" % (e["total_dark_px"], e["weighted_center_y"]))
        print("   peaks", e["peak_rows"])

    if res["TARGET"]["weighted_center_y"] and res["OURS_V1"]["weighted_center_y"]:
        d = res["OURS_V1"]["weighted_center_y"] - res["TARGET"]["weighted_center_y"]
        print("\nBODY_Y_SHIFT (OURS - TARGET) = %+.2f" % d)
        print("  box_h 74->80 会让本端正文再下移 +3 => 预测新 shift = %+.2f" % (d + 3))
        if abs(d) <= 1.5:
            print("  => 当前正文已对齐，改 box_h 需把 text_y 补偿 -3")
        elif abs(d + 3) <= 1.5:
            print("  => 改 box_h 后正文恰好对齐，无需补偿")
        else:
            print("  => 两解都不完美，正文判据可能仍混入非正文暗像素，只作旁证")
        res["body_y_shift"] = round(d, 2)

    # 顺带打印 verify_r3_align.json 的 pass 阈值/结果，避免改 box_w 破坏旧 pass
    vp = os.path.join(OUT, "verify_r3_align.json")
    if os.path.exists(vp):
        v = json.load(open(vp))
        print("\n=== verify_r3_align.json (结构预览) ===")
        print(json.dumps(v, ensure_ascii=False)[:1200])

    with open(os.path.join(OUT, "body_final_settle.json"), "w") as f:
        json.dump(res, f, indent=2)
    print("\nSAVED body_final_settle.json")


if __name__ == "__main__":
    main()
