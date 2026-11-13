#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用户 m02128「先做你可以改的」验收：选角色页/职业团页左侧栏补齐后逐块 NCC。
复用 verify_scene_reset.py 的单实例 + 协议锚点 + force_game_top + BitBlt 链路，
只走到「清晰选角色页」与「职业团页」，各抓一帧，对正确端目标帧算
ROI(0,0,160,470) 全条带 NCC + 4 个 y 块 NCC。
验收(自定，m02125)：strip 0.4192/0.5156 → >=0.85，4 块各 >0.5，帧 real_gl_frame。
"""
import os, sys, json, time, subprocess
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from clean_world_capture import wait_anchor
from capture_charselect_clean import wait_charlist, click
from recapture_all_v2 import clear_and_type, ACCOUNT_FIELD, PASSWORD_FIELD

EXE, EXE_DIR, SHOTS = mc.EXE, mc.EXE_DIR, mc.SHOTS
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
T = os.path.join(os.path.dirname(SHOTS), "targets_desktop")
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
WORLD_BTN = (485, 210)
CH1_BTN = (490, 350)
GOWORLD_BTN = (820, 518)
CREATE_BTN = (865, 263)
STRIP_BOX = (0, 0, 160, 470)
BLOCKS = {"title_y36_85": (36, 85), "worldbar_y96_148": (96, 148),
          "viewall_y377_399": (377, 399), "home_y427_449": (427, 449)}


def arr(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def ncc(x, y):
    x = x - x.mean(); y = y - y.mean()
    d = np.sqrt((x * x).sum() * (y * y).sum())
    return round(float((x * y).sum() / d), 4) if d > 1e-9 else 0.0


def strip_metrics(shot_path, target_path):
    a = arr(shot_path)[:470, :160]
    b = arr(target_path)[:470, :160]
    strip = ncc(a.mean(2), b.mean(2))
    blocks = {k: ncc(a[y0:y1].mean(2), b[y0:y1].mean(2)) for k, (y0, y1) in BLOCKS.items()}
    return strip, blocks


def grab(hwnd, ox, oy, cw, ch, name, res):
    u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
    time.sleep(0.15)
    rgb = mc.bitblt_grab(ox, oy, cw, ch)
    if rgb is None:
        res.setdefault("errors", []).append(name + ": bitblt None")
        return None
    path = os.path.join(SHOTS, name + ".png")
    Image.fromarray(rgb, "RGB").save(path)
    im = rgb.astype(np.float64)
    uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
    info = {"file": name + ".png", "path": path,
            "mean": round(float(im.mean()), 2), "std": round(float(im.std()), 2),
            "uniq": int(uniq), "real_gl_frame": bool(im.std() > 8 and uniq > 40)}
    res.setdefault("frames", {})[name] = info
    return info


def main():
    res = {"method": "current-HEAD exe + single-instance + protocol-anchor + force_game_top + BitBlt",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "strip_box": list(STRIP_BOX), "blocks": {k: list(v) for k, v in BLOCKS.items()},
           "config_head_before": repr(mc.cfg_head()),
           "fix_under_test": "UICharSelect/UIRaceSelect 左侧四块：cs_/rs_step(0,33) + world_box(0,96) + BtViewAll(0,377) + BtStart(0,427)，装饰绝对坐标绘制，按钮局部=绝对-position"}
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
            if hwnd:
                break
            time.sleep(0.3)
        if not hwnd:
            res["error"] = "no window"
            return
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
        time.sleep(2.2)   # 清晰选角色页 settle（v2 的 1.2s 是过场帧）

        g_char = grab(hwnd, ox, oy, cw, ch, "sidebar_fix_1_charselect", res)

        # 创建角色 → UIRaceSelect（职业团页）
        click(hwnd, ox, oy, CREATE_BTN[0], CREATE_BTN[1])
        time.sleep(2.0)
        g_race = grab(hwnd, ox, oy, cw, ch, "sidebar_fix_2_raceselect", res)

        # 逐块 NCC
        pairs = [
            ("char_select", g_char, "desktop_char_select.png"),
            ("race_select", g_race, "desktop_racecreate_race.png"),
        ]
        pages = []
        for page, g, tgt in pairs:
            if not g:
                pages.append({"page": page, "error": "no frame"})
                continue
            s, b = strip_metrics(g["path"], os.path.join(T, tgt))
            present = sum(1 for v in b.values() if v > 0.5)
            pages.append({"page": page, "shot": g["file"], "target": tgt,
                          "strip_ncc": s, "blocks": b, "blocks_present_gt0_5": present,
                          "real_gl_frame": g["real_gl_frame"],
                          "strip_pass": bool(s >= 0.85 and present == 4 and g["real_gl_frame"])})
        res["pages"] = pages
        res["verdict"] = {
            "all_pass": bool(all(p.get("strip_pass") for p in pages)),
            "detail": "SIDEBAR_FIX_VERIFIED" if all(p.get("strip_pass") for p in pages)
                      else "NOT_YET / see per-page strip_ncc + blocks",
        }
        sends = [l for l in (newtxt2 or "").splitlines() if l.startswith("[SEND]")]
        res["send_nonheartbeat"] = [l[:80] for l in sends if ": 18 00" not in l][:10]
    finally:
        for h in occ_all:
            try:
                u.ShowWindow(h, 9)
            except Exception:
                pass
        res["config_head_after"] = repr(mc.cfg_head())
        ev = os.path.join(SHOTS, "sidebar_fix_verdict.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "verdict": res.get("verdict"),
                          "pages": res.get("pages"),
                          "frames": res.get("frames"),
                          "config": [res["config_head_before"], res["config_head_after"]]},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
