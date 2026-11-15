#!/usr/bin/env python3
"""频道页 20 格/页补丁后，R3 五组件不退化复算。

频道页用新二进制稳态帧 ch20_home_f05.png；角色页二进制未改（本轮只动
UIWorldSelect.cpp），沿用历史已验证稳态帧 charselect_clean_f09.png。
模板 / ROI / 目标 bbox 全部 import verify_r3_align.py 的既有口径，
不新建判据、不放宽阈值，避免「自己改评分器」。
"""
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from verify_r3_align import TPLS, TARGET_BBOX, load_tpl, ncc_best, arr, OUT, SHOTS

NEW_CHAN = os.path.join(SHOTS, "ch20_home_f05.png")
OLD_CHAN = os.path.join(SHOTS, "channel_grid_f05.png")  # 补丁前同页稳态帧（before 对照）
PICK_CHAR = os.path.join(SHOTS, "charselect_clean_f09.png")  # 历史已验证角色页稳态帧


def main():
    tpls = {k: load_tpl(os.path.join(OUT, v[0])) for k, v in TPLS.items()}
    res = {"chan_frame_new": NEW_CHAN, "chan_frame_before": OLD_CHAN,
           "char_frame": PICK_CHAR, "target_bbox": TARGET_BBOX, "components": {}}
    for key, (fn, page, roi) in TPLS.items():
        t, m = tpls[key]
        tb = TARGET_BBOX[key]
        if page == "chan":
            new_p, old_p = NEW_CHAN, OLD_CHAN
        else:
            new_p, old_p = PICK_CHAR, PICK_CHAR
        nr = ncc_best(arr(new_p), t, m, roi)
        orr = ncc_best(arr(old_p), t, m, roi) if os.path.exists(old_p) else {"ncc": None, "bbox": None}
        shift = None if not nr["bbox"] else [nr["bbox"][j] - tb[j] for j in range(4)]
        if shift is None:
            ok = False
        elif key == "nameTagL":
            ok = abs(shift[1]) <= 2 and abs(shift[3]) <= 2 and abs(shift[0]) <= 5
        else:
            ok = all(abs(v) <= 2 for v in shift)
        res["components"][key] = {
            "frame": os.path.basename(new_p), "ncc": nr["ncc"], "bbox": nr["bbox"],
            "before_ncc": orr["ncc"], "before_bbox": orr["bbox"],
            "shift": shift, "pass": ok,
        }
    res["all_pass"] = all(v["pass"] for v in res["components"].values())
    jp = os.path.join(OUT, "verify_r3_noregress.json")
    json.dump(res, open(jp, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
    print(json.dumps({"all_pass": res["all_pass"],
                      "components": {k: {"ncc": v["ncc"], "shift": v["shift"], "pass": v["pass"]}
                                     for k, v in res["components"].items()}},
                     ensure_ascii=False, indent=2))
    print("JSON", jp)


if __name__ == "__main__":
    main()
