#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""附着当前活实例(已 settle 的清晰选角色页), 用 WZ 真值 NCC 定位的 BtNew 中心
(865,263) 点击 → UIRaceSelect, 连抓 6 帧。
与 v2 首跑的本质差别: v2 在 GOWORLD 后仅 1.2s 的「过场帧」上点击(按钮未激活),
本脚本等清晰选角色页渲染完毕再点; 且坐标由 WZ CharSelect/BtNew/normal/0 (101x35)
alpha-mask NCC=1.0 @(815,246) 背书, 非肉眼估。
输出 shots/racecreate_click_f*.png + shots/racecreate_click_summary.json
"""
import os, sys, json, time, ctypes
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from capture_charselect_clean import click

EXE, EXE_DIR, SHOTS = mc.EXE, mc.EXE_DIR, mc.SHOTS
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
BTNEW_CENTER = (865, 263)   # WZ 真值 NCC=1.0 @(815,246) + (101//2,35//2)


def grab(hwnd, ox, oy, cw, ch, name, res):
    u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
    time.sleep(0.12)
    rgb = mc.bitblt_grab(ox, oy, cw, ch)
    if rgb is None:
        res.setdefault("errors", []).append(name + ": bitblt None")
        return None
    im = rgb.astype(np.float64)
    uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
    Image.fromarray(rgb, "RGB").save(os.path.join(SHOTS, name + ".png"))
    info = {"file": name + ".png", "mean": round(float(im.mean()), 2),
            "std": round(float(im.std()), 2), "uniq": int(uniq),
            "real_gl_frame": bool(im.std() > 8 and uniq > 40)}
    res.setdefault("frames", {})[name] = info
    return info


def main():
    res = {"method": "attach settled charselect + WZ-NCC BtNew center click → UIRaceSelect + BitBlt",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "btnew_center": list(BTNEW_CENTER),
           "config_head_before": repr(mc.cfg_head())}
    occ_all = []
    hwnd = None
    try:
        prev = json.load(open(os.path.join(SHOTS, "recapture_v2_summary.json"), encoding="utf-8"))
        pid = int(prev.get("pid", 0))
        res["pid"] = pid
        hwnd = mc.enum_proc_window(pid)
        if not hwnd:
            res["error"] = "live window not found for pid %d" % pid
            return
        occ, fgok = mc.force_game_top(hwnd)
        occ_all = occ
        res["fg_match"] = fgok
        ox, oy, cw, ch = mc.client_origin(hwnd)
        res["client_origin"] = [ox, oy, cw, ch]

        # baseline: 确认当前是清晰选角色页(非过场)
        grab(hwnd, ox, oy, cw, ch, "racecreate_click_f00_baseline", res)

        # 点击创建角色(真值坐标)
        click(hwnd, ox, oy, BTNEW_CENTER[0], BTNEW_CENTER[1])
        res["clicked"] = list(BTNEW_CENTER)

        # 连抓 6 帧覆盖 scene step3 相机移动 + fade + UIRaceSelect 构造
        for i in range(1, 7):
            time.sleep(0.5)
            grab(hwnd, ox, oy, cw, ch, "racecreate_click_f%02d" % i, res)

        nb = u.GetWindowTextLengthW(hwnd)
        buf = ctypes.create_unicode_buffer(nb + 1)
        u.GetWindowTextW(hwnd, buf, nb + 1)
        res["title"] = buf.value[:130]
        u.SetWindowPos(hwnd, mc.HWND_NOTOPMOST, 0, 0, 0, 0, SWP)
    finally:
        for h in occ_all:
            try: u.ShowWindow(h, mc.SW_RESTORE)
            except Exception: pass
        res["config_head_after"] = repr(mc.cfg_head())
        ev = os.path.join(SHOTS, "racecreate_click_summary.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "pid": res.get("pid"),
                          "fg_match": res.get("fg_match"),
                          "clicked": res.get("clicked"),
                          "frames": {k: {"mean": v["mean"], "real": v["real_gl_frame"]}
                                     for k, v in res.get("frames", {}).items()},
                          "title": res.get("title", "")[:90],
                          "config": [res["config_head_before"], res["config_head_after"]]},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
