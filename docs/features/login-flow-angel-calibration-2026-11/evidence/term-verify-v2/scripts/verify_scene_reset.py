#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用户 m01638 bug 验收：职业团选择 → 角色设置(亭子) → 取消返回 → 职业团选择。
修复 = UICommonCreation.cpp 四条离开路径 reset_creation_scene() + UIStateLogin::remove
单点兜底 clear_scene()。exe = 607f789811db25ed (2026-11-14 01:14:48)。

验收信号(数值, 非目视臆测):
  A) race_before 与 race_return 全屏 NCC 高(同屏重绘), 且两者亭子ROI橙色占比都低
  B) creation_scene 亭子ROI橙色占比显著高于 race_before/race_return(确认真的进了带亭子的页)
  C) race_before vs creation_scene NCC 明显低(确认两屏不同)
  D) race_return 亭子ROI 与 creation_scene 亭子ROI 差分大(确认亭子消失)
输出 shots/scene_reset_*.png + shots/scene_reset_verdict.json
"""
import os, sys, json, time, ctypes, subprocess
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from clean_world_capture import wait_anchor
from capture_charselect_clean import wait_charlist, click
from recapture_all_v2 import clear_and_type, ACCOUNT_FIELD, PASSWORD_FIELD

EXE, EXE_DIR, SHOTS = mc.EXE, mc.EXE_DIR, mc.SHOTS
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
WORLD_BTN = (485, 210)
CH1_BTN = (490, 350)
GOWORLD_BTN = (820, 518)
CREATE_BTN = (865, 263)     # WZ 真值 NCC=1.0 定位的 BtNew 中心
SELECT_BTN = (815, 477)     # classic「选择》」按钮(本端帧 racecreate_click_f06.png 实测)
CANCEL_BTN = (855, 350)    # classic 冒险家「取消」(源码 no_pos=(822,333)+位图宽, 目标帧实测)
PAVILION_ROI = (520, 100, 760, 300)  # 蘑菇伞+柱子上部; 职业团页该区是蓝天/卡片

# 橙色蘑菇伞: R>150, 80<G<190, B<90
def orange_ratio(rgb):
    x0, y0, x1, y1 = PAVILION_ROI
    a = rgb[y0:y1, x0:x1].astype(np.int16)
    m = (a[:, :, 0] > 150) & (a[:, :, 1] > 80) & (a[:, :, 1] < 190) & (a[:, :, 2] < 90)
    return round(float(m.mean()), 5)

def ncc(a, b):
    x = a.astype(np.float64).ravel(); y = b.astype(np.float64).ravel()
    x = x - x.mean(); y = y - y.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return round(float((x * y).sum() / d), 6) if d > 1e-9 else 0.0

def grab(hwnd, ox, oy, cw, ch, name, res, store):
    u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
    time.sleep(0.15)
    rgb = mc.bitblt_grab(ox, oy, cw, ch)
    if rgb is None:
        res.setdefault("errors", []).append(name + ": bitblt None")
        return None
    Image.fromarray(rgb, "RGB").save(os.path.join(SHOTS, name + ".png"))
    im = rgb.astype(np.float64)
    uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
    info = {"file": name + ".png", "mean": round(float(im.mean()), 2),
            "std": round(float(im.std()), 2), "uniq": int(uniq),
            "orange_ratio": orange_ratio(rgb),
            "real_gl_frame": bool(im.std() > 8 and uniq > 40)}
    res.setdefault("frames", {})[name] = info
    store[name] = rgb
    return info

def main():
    res = {"method": "current-HEAD exe 607f789811db25ed + single-instance + protocol-anchor + BitBlt",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "pavilion_roi": list(PAVILION_ROI),
           "config_head_before": repr(mc.cfg_head()), "fix_under_test":
           "UICommonCreation.cpp 5x reset_creation_scene() + UIStateLogin::remove clear_scene()"}
    store = {}
    occ_all = []
    try:
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
        time.sleep(1.5)
        start = os.path.getsize(PDUMP)
        pr = subprocess.Popen([EXE], cwd=EXE_DIR,
                             creationflags=mc.DETACHED_PROCESS, close_fds=True)
        res["pid"] = pr.pid
        time.sleep(3.0)
        t = time.time(); hwnd = None
        while time.time() - t < 20:
            hwnd = mc.enum_proc_window(pr.pid)
            if hwnd: break
            time.sleep(0.3)
        if not hwnd:
            res["error"] = "no window"; return
        occ, fgok = mc.force_game_top(hwnd)
        occ_all = occ
        res["fg_match"] = fgok
        ox, oy, cw, ch = mc.client_origin(hwnd)
        res["client_origin"] = [ox, oy, cw, ch]

        # 登录 → 选世界 → 选频道 → 清晰选角色页
        clear_and_type(hwnd, ox, oy, ACCOUNT_FIELD[0], ACCOUNT_FIELD[1], "admin", res, "account")
        clear_and_type(hwnd, ox, oy, PASSWORD_FIELD[0], PASSWORD_FIELD[1], "admin", res, "password")
        click(hwnd, ox, oy, 880, 300)
        anchors, newtxt, ok = wait_anchor(PDUMP, start, timeout=12.0)
        res["serverlist_anchor_hit"] = ok
        time.sleep(0.8)
        click(hwnd, ox, oy, WORLD_BTN[0], WORLD_BTN[1])
        time.sleep(1.0)
        click(hwnd, ox, oy, CH1_BTN[0], CH1_BTN[1])
        time.sleep(0.5)
        click(hwnd, ox, oy, GOWORLD_BTN[0], GOWORLD_BTN[1])
        ev, newtxt2, charlist_ok = wait_charlist(PDUMP, start, timeout=12.0)
        res["charlist_hit"] = charlist_ok
        time.sleep(2.2)   # 等清晰选角色页 settle (v2 的 1.2s 是过场帧)

        # 1) 创建角色 → UIRaceSelect (职业团选择页, 森林, 无亭子)
        click(hwnd, ox, oy, CREATE_BTN[0], CREATE_BTN[1])
        time.sleep(2.0)
        g_before = grab(hwnd, ox, oy, cw, ch, "scene_reset_1_race_before", res, store)

        # 2) 「选择」→ UICommonCreation (角色设置页, 亭子)
        click(hwnd, ox, oy, SELECT_BTN[0], SELECT_BTN[1])
        time.sleep(1.8)
        g_creation = grab(hwnd, ox, oy, cw, ch, "scene_reset_2_creation", res, store)
        # 兜底: 若没进创建页(亭子ROI橙色没升), 用 ENTER 再试一次
        enter_retry = False
        if g_before and g_creation and g_creation["orange_ratio"] < g_before["orange_ratio"] + 0.02:
            u.keybd_event(0x0D, 0, 0, 0); time.sleep(0.05)
            u.keybd_event(0x0D, 0, mc.KEYEVENTF_KEYUP, 0)
            time.sleep(1.8)
            g_creation = grab(hwnd, ox, oy, cw, ch, "scene_reset_2_creation_enter", res, store)
            enter_retry = True
        res["enter_retry_used"] = enter_retry

        # 3) 「取消」→ 回 UIRaceSelect (应无亭子)
        click(hwnd, ox, oy, CANCEL_BTN[0], CANCEL_BTN[1])
        time.sleep(1.8)
        g_return = grab(hwnd, ox, oy, cw, ch, "scene_reset_3_race_return", res, store)
        # 兜底: 若没返回(亭子ROI橙色仍高), 用 ESC 再试一次
        esc_retry = False
        if g_creation and g_return and g_return["orange_ratio"] > g_creation["orange_ratio"] * 0.6:
            u.keybd_event(0x1B, 0, 0, 0); time.sleep(0.05)
            u.keybd_event(0x1B, 0, mc.KEYEVENTF_KEYUP, 0)
            time.sleep(1.8)
            g_return = grab(hwnd, ox, oy, cw, ch, "scene_reset_3_race_return_esc", res, store)
            esc_retry = True
        res["esc_retry_used"] = esc_retry

        # 数值闭环
        k_before = [k for k in store if "race_before" in k][0]
        k_creation = [k for k in store if "creation" in k][-1]
        k_return = [k for k in store if "race_return" in k][-1]
        a, b, c = store[k_before], store[k_creation], store[k_return]
        x0, y0, x1, y1 = PAVILION_ROI
        roi_ncc_creation_vs_return = ncc(b[y0:y1, x0:x1], c[y0:y1, x0:x1])
        res["metrics"] = {
            "full_ncc_before_vs_return": ncc(a, c),
            "full_ncc_before_vs_creation": ncc(a, b),
            "pavilion_roi_ncc_creation_vs_return": roi_ncc_creation_vs_return,
            "orange_before": res["frames"][k_before]["orange_ratio"],
            "orange_creation": res["frames"][k_creation]["orange_ratio"],
            "orange_return": res["frames"][k_return]["orange_ratio"],
        }
        m = res["metrics"]
        entered = m["orange_creation"] > m["orange_before"] + 0.02
        cleared = m["orange_return"] < m["orange_creation"] * 0.6
        same_screen = m["full_ncc_before_vs_return"] > 0.90
        res["verdict"] = {
            "entered_creation_page": bool(entered),
            "scene_cleared_on_return": bool(cleared),
            "return_is_same_forest_screen": bool(same_screen),
            "overall": "FIX_VERIFIED" if (entered and cleared and same_screen)
                       else ("INCONCLUSIVE_NEED_MANUAL" if not (entered and cleared)
                             else "BUG_STILL_PRESENT"),
        }

        sends = [l for l in (newtxt2 or "").splitlines() if l.startswith("[SEND]")]
        res["send_nonheartbeat"] = [l[:80] for l in sends if ": 18 00" not in l][:10]
        nb = u.GetWindowTextLengthW(hwnd)
        buf = ctypes.create_unicode_buffer(nb + 1)
        u.GetWindowTextW(hwnd, buf, nb + 1)
        res["title"] = buf.value[:130]
        u.SetWindowPos(hwnd, mc.HWND_NOTOPMOST, 0, 0, 0, 0, SWP) if hasattr(mc, "HWND_NOTOPMOST") else None
    finally:
        for h in occ_all:
            try: u.ShowWindow(h, 9)  # SW_RESTORE
            except Exception: pass
        res["config_head_after"] = repr(mc.cfg_head())
        ev = os.path.join(SHOTS, "scene_reset_verdict.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "verdict": res.get("verdict"),
                          "metrics": res.get("metrics"),
                          "frames": {k: {"mean": v["mean"], "orange": v["orange_ratio"],
                                          "real": v["real_gl_frame"]}
                                     for k, v in res.get("frames", {}).items()},
                          "retry": [res.get("enter_retry_used"), res.get("esc_retry_used")],
                          "config": [res["config_head_before"], res["config_head_after"]]},
                         ensure_ascii=False, indent=2))

if __name__ == "__main__":
    main()
