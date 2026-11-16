#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""R5 属性板真机：fork 自 verify_r4_channel.py——登录→点世界→频道页→GoWorld→选角色页抓帧。
独立 exe MapleStory_r4.exe，不覆盖 live；进程收敛/配置护栏由 run_r5_guarded.py 负责。"""
import os, sys, json, time, ctypes, subprocess, shutil, hashlib
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc

EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
SRC_EXE = r"C:\Users\11036\AppData\Local\Temp\term_verify\build_r4\MapleStory.exe"
EXE = os.path.join(EXE_DIR, "MapleStory_r4.exe")
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
SHOTS = mc.SHOTS
R5 = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r5"
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
WORLD_BTN = (485, 210)
GOWORLD = (820, 518)   # charselect_clean_summary 实测 goworld_btn_client


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


def click(hwnd, ox, oy, cx, cy):
    u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
    u.SetCursorPos(int(ox + cx), int(oy + cy)); time.sleep(0.14)
    u.mouse_event(0x0002, 0, 0, 0, 0); time.sleep(0.08); u.mouse_event(0x0004, 0, 0, 0, 0)


def wait_anchor(start, timeout=30.0):
    t0, seen = time.time(), []
    while time.time() - t0 < timeout:
        with open(PDUMP, "rb") as fh:
            fh.seek(start); txt = fh.read().decode("utf-8", "replace")
        for l in txt.splitlines():
            if "[RECV]" in l and ": 0a 00 ff" in l and "end" not in [s[0] for s in seen]:
                seen.append(("end", round(time.time() - t0, 3)))
            if "[SEND]" in l and ": 01 00 " in l and "login" not in [s[0] for s in seen]:
                seen.append(("login", round(time.time() - t0, 3)))
        if "end" in [s[0] for s in seen]:
            return seen, True
        time.sleep(0.12)
    return seen, False


def grab_seq(hwnd, ox, oy, tag, n=8, gap=0.30):
    out = []
    for i in range(n):
        mc.force_game_top(hwnd)
        u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
        rgb = mc.bitblt_grab(ox, oy, 1280, 720)
        if rgb is None:
            out.append({"i": i, "error": "grab_none"}); continue
        im = rgb.astype(np.float64)
        uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
        dst = os.path.join(SHOTS, "r7_%s_f%02d.png" % (tag, i))
        Image.fromarray(rgb, "RGB").save(dst)
        out.append({"i": i, "file": os.path.basename(dst),
                    "mean": round(float(im.mean()), 2), "std": round(float(im.std()), 2),
                    "uniq": int(uniq), "real_gl_frame": bool(im.std() > 8 and uniq > 40)})
        time.sleep(gap)
    return out


def main():
    res = {"method": "R7 isolated-exe charselect attribute board capture",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "src_exe_sha256": sha(SRC_EXE),
           "config_head_before": repr(mc.cfg_head())}
    occ_all = []
    try:
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory_r4.exe"], capture_output=True)
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory_r4.exe"], capture_output=True)
        time.sleep(1.5)
        shutil.copy2(SRC_EXE, EXE)
        res["deployed_exe"] = EXE
        res["deployed_sha256"] = sha(EXE)
        res["live_exe_untouched_sha256"] = sha(os.path.join(EXE_DIR, "MapleStory.exe"))
        start = os.path.getsize(PDUMP)
        res["packet_dump_start_offset"] = start
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
        occ, _ = mc.force_game_top(hwnd); occ_all = [occ]
        ox, oy, cw, ch = mc.client_origin(hwnd)
        res["client_size"] = [cw, ch]

        def clear_field(n=20):
            for _ in range(n):
                u.keybd_event(0x08, 0, 0, 0)
                u.keybd_event(0x08, 0, mc.KEYEVENTF_KEYUP, 0)
            time.sleep(0.15)

        mc.force_game_top(hwnd); time.sleep(0.4)
        click(hwnd, ox, oy, 720, 336); time.sleep(0.3)
        clear_field()
        res["unicode_account_ok"] = all(x == 1 for x in mc.unicode_type("admin"))
        time.sleep(0.2)
        u.keybd_event(0x09, 0, 0, 0)
        u.keybd_event(0x09, 0, mc.KEYEVENTF_KEYUP, 0)
        time.sleep(0.2)
        clear_field()
        res["unicode_password_ok"] = all(x == 1 for x in mc.unicode_type("admin"))
        time.sleep(0.2)
        u.keybd_event(0x0D, 0, 0, 0)
        u.keybd_event(0x0D, 0, mc.KEYEVENTF_KEYUP, 0)
        anchors, ok = wait_anchor(start, timeout=45.0)
        res["anchors"] = anchors; res["anchor_hit"] = ok
        time.sleep(1.0)

        # 点世界 → 频道页 → 先点 CH.1 格选红圈（否则 enter_world channelid==0xFF 直接 return）
        #   → GoWorld → 选角色页
        CH1_BTN = (490, 350)
        click(hwnd, ox, oy, WORLD_BTN[0], WORLD_BTN[1]); time.sleep(1.5)
        click(hwnd, ox, oy, CH1_BTN[0], CH1_BTN[1]); time.sleep(0.8)
        click(hwnd, ox, oy, GOWORLD[0], GOWORLD[1]); time.sleep(2.5)
        res["frames_charselect"] = grab_seq(hwnd, ox, oy, "charselect", n=8, gap=0.30)

        nb = u.GetWindowTextLengthW(hwnd)
        b = ctypes.create_unicode_buffer(nb + 1)
        u.GetWindowTextW(hwnd, b, nb + 1)
        res["title"] = b.value[:130]
        u.SetWindowPos(hwnd, mc.HWND_NOTOPMOST, 0, 0, 0, 0, SWP)
    finally:
        for h in occ_all:
            try: u.ShowWindow(h, mc.SW_RESTORE)
            except Exception: pass
        res["config_head_after"] = repr(mc.cfg_head())
        ev = os.path.join(R5, "r7_charselect_verdict.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "pid": res.get("pid"),
                          "anchor_hit": res.get("anchor_hit"),
                          "deployed": (res.get("deployed_sha256") or "")[:12],
                          "means": [f.get("mean") for f in res.get("frames_charselect", [])],
                          "title": (res.get("title") or "")[:70]}, ensure_ascii=False))


if __name__ == "__main__":
    main()