#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""真机验证 20 格/页 + 30 频道翻页（独立 exe，不覆盖别会话 live MapleStory.exe）。

流程：单实例 → 登录 → SERVERLIST end 锚点 → 点世界名牌 → 抓首页稳态帧
      → 点翻页 NEXT → 抓次页稳态帧 → 点翻页 PREV → 抓回首页帧。
所有帧 BitBlt 真 GL 画面；输出 shots/ch20_* 与 ui_r3/channel20_verdict.json。
"""
import os, sys, json, time, ctypes, subprocess, shutil, hashlib
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc

EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
SRC_EXE = r"C:\Users\11036\AppData\Local\Temp\term_verify\build_channel20\MapleStory.exe"
EXE = os.path.join(EXE_DIR, "MapleStory_ch20.exe")
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
SHOTS = mc.SHOTS
R3 = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_r3"
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
WORLD_BTN = (485, 210)
# 翻页钮：panel_topleft=(436,280) + CH_PAGER_OFF(210,217) => (646,497)/(686,497)
# 中心 +14/+15；避开第 5 行 CH.17..20 格子(464..494)与 GoWorld(756..915)。
PAGE_PREV = (646 + 14, 497 + 15)
PAGE_NEXT = (686 + 14, 497 + 15)


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
            if "[RECV]" in l and ": 0a 00 00 " in l and "world" not in [s[0] for s in seen]:
                seen.append(("world", round(time.time() - t0, 3)))
            if "[SEND]" in l and ": 01 00 " in l and "login" not in [s[0] for s in seen]:
                seen.append(("login", round(time.time() - t0, 3)))
        if "end" in [s[0] for s in seen]:
            return seen, True
        time.sleep(0.12)
    return seen, False


def grab_seq(hwnd, ox, oy, tag, n=6, gap=0.20):
    out = []
    for i in range(n):
        # 别会话可能把大文档窗口重新置前；每帧重抢，否则 BitBlt 抓到白底文档。
        mc.force_game_top(hwnd)
        u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
        rgb = mc.bitblt_grab(ox, oy, 1280, 720)
        if rgb is None:
            out.append({"i": i, "error": "grab_none"}); continue
        im = rgb.astype(np.float64)
        uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
        dst = os.path.join(SHOTS, "ch20_%s_f%02d.png" % (tag, i))
        Image.fromarray(rgb, "RGB").save(dst)
        out.append({"i": i, "file": os.path.basename(dst),
                    "mean": round(float(im.mean()), 2), "std": round(float(im.std()), 2),
                    "uniq": int(uniq), "real_gl_frame": bool(im.std() > 8 and uniq > 40)})
        time.sleep(gap)
    return out


def main():
    res = {"method": "isolated-exe single-instance protocol-anchor + world click + pager click + BitBlt",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "src_exe_sha256": sha(SRC_EXE),
           "config_head_before": repr(mc.cfg_head()),
           "pager_prev_click": list(PAGE_PREV), "pager_next_click": list(PAGE_NEXT)}
    occ_all = []
    try:
        # 先清掉上一轮 ch20 / live 客户端，否则 copy2 会撞 WinError 32 文件占锁。
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory_ch20.exe"], capture_output=True)
        time.sleep(1.5)

        # 部署独立 exe（不改 live MapleStory.exe）
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
        occ, _ = mc.force_game_top(hwnd); occ_all = occ
        ox, oy, cw, ch = mc.client_origin(hwnd)
        res["client_size"] = [cw, ch]

        # 登录：Settings/DefaultAccount 可能预填 tostest1（本轮增量包实证
        # [SEND] 01 00 0c 00 "tostest1admi" + [RECV] 00 00 17 登录被拒）。
        # 不能假设空框：两框都先按 20 次 BACKSPACE 归零，再 admin / TAB / admin /
        # Enter。UILogin.cpp:482 password_text.length() <= 4 仍会拦空密码。
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

        # 点世界名牌 → 频道页首页
        click(hwnd, ox, oy, WORLD_BTN[0], WORLD_BTN[1]); time.sleep(1.2)
        res["frames_home"] = grab_seq(hwnd, ox, oy, "home", n=6, gap=0.20)

        # 点 NEXT → 次页
        click(hwnd, ox, oy, PAGE_NEXT[0], PAGE_NEXT[1]); time.sleep(1.0)
        res["frames_page2"] = grab_seq(hwnd, ox, oy, "page2", n=6, gap=0.20)

        # 点 PREV → 回首页
        click(hwnd, ox, oy, PAGE_PREV[0], PAGE_PREV[1]); time.sleep(1.0)
        res["frames_back"] = grab_seq(hwnd, ox, oy, "back", n=4, gap=0.20)

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
        ev = os.path.join(R3, "channel20_verdict.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "pid": res.get("pid"),
                          "anchor_hit": res.get("anchor_hit"),
                          "deployed_sha256": res.get("deployed_sha256"),
                          "frames_home": [f.get("mean") for f in res.get("frames_home", [])],
                          "frames_page2": [f.get("mean") for f in res.get("frames_page2", [])],
                          "frames_back": [f.get("mean") for f in res.get("frames_back", [])],
                          "title": res.get("title", "")[:80],
                          "config": [res["config_head_before"], res["config_head_after"]]},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
