#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""用户 m01143 质疑后：用【当前 HEAD 重建的 aa0aea67 exe】重抓全部五屏 + 创建角色页。
旧终验所有帧来自 21:11:58 的 b406cac6（不含 21:53 的 806ae254 CHANNEL_PAGE_SIZE=30 改动）。
本轮坐标沿用 capture_charselect_clean.py 实测值，新增「创建角色」按钮点击 → UIRaceSelect。
输出 shots/v2_*.png + shots/recapture_v2_summary.json
"""
import os, sys, json, time, ctypes, subprocess
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from clean_world_capture import wait_anchor
from capture_charselect_clean import wait_charlist, click

EXE, EXE_DIR, SHOTS = mc.EXE, mc.EXE_DIR, mc.SHOTS
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW
WORLD_BTN = (485, 210)
CH1_BTN = (490, 350)
GOWORLD_BTN = (820, 518)
CREATE_BTN = (866, 265)   # 木牌「创建角色」(清晰选角色帧实测: 开始游戏~215/创建~265/删除~320)
ACCOUNT_FIELD = (720, 308)  # 账号输入框(UILogin.cpp:174-175 账号+67,12 / 密码+67,40 差28px；密码实测336→账号308)
PASSWORD_FIELD = (720, 336)  # 密码输入框


def clear_and_type(hwnd, ox, oy, cx, cy, text, res, tag):
    """显式点框 → 15×BACKSPACE 清空 → UNICODE 重填，避免新 exe 预填状态不确定。"""
    click(hwnd, ox, oy, cx, cy)
    time.sleep(0.2)
    for _ in range(15):
        u.keybd_event(0x08, 0, 0, 0)          # VK_BACK down
        u.keybd_event(0x08, 0, mc.KEYEVENTF_KEYUP, 0)
    time.sleep(0.15)
    sent = mc.unicode_type(text)
    res.setdefault("field_input", {})[tag] = {"text": text, "sent_ok": all(x == 1 for x in sent)}
    time.sleep(0.2)


def grab(hwnd, ox, oy, cw, ch, name, res):
    u.SetWindowPos(hwnd, mc.HWND_TOPMOST, 0, 0, 0, 0, SWP)
    time.sleep(0.15)
    rgb = mc.bitblt_grab(ox, oy, cw, ch)
    if rgb is None:
        res.setdefault("errors", []).append(name + ": bitblt None")
        return None
    im = rgb.astype(np.float64)
    uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
    dst = os.path.join(SHOTS, name + ".png")
    Image.fromarray(rgb, "RGB").save(dst)
    info = {"file": name + ".png", "mean": round(float(im.mean()), 2),
            "std": round(float(im.std()), 2), "uniq": int(uniq),
            "real_gl_frame": bool(im.std() > 8 and uniq > 40)}
    res.setdefault("screens", {})[name] = info
    return info


def main():
    res = {"method": "current-HEAD exe aa0aea67 + single-instance + protocol-anchor + BitBlt",
           "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "config_head_before": repr(mc.cfg_head())}
    occ_all = []
    try:
        subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
        time.sleep(1.5)
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
        occ, fgok = mc.force_game_top(hwnd)
        occ_all = occ
        ox, oy, cw, ch = mc.client_origin(hwnd)
        res["client_origin"] = [ox, oy, cw, ch]

        # 1) 登录屏(先清遮挡: force_top 后等 0.5s 再抓)
        time.sleep(0.5)
        grab(hwnd, ox, oy, cw, ch, "v2_login", res)

        # 2) 账号/密码两框都显式清空重填 + 点连接 → 选世界收起态
        clear_and_type(hwnd, ox, oy, ACCOUNT_FIELD[0], ACCOUNT_FIELD[1], "admin", res, "account")
        clear_and_type(hwnd, ox, oy, PASSWORD_FIELD[0], PASSWORD_FIELD[1], "admin", res, "password")
        # 打完字后抓一张「输入态」确认两框都有内容(诊断用)
        grab(hwnd, ox, oy, cw, ch, "v2_login_typed", res)
        click(hwnd, ox, oy, 880, 300)
        anchors, _, ok = wait_anchor(PDUMP, start, timeout=12.0)
        res["serverlist_anchor_hit"] = ok
        res["anchors"] = anchors
        time.sleep(0.8)
        grab(hwnd, ox, oy, cw, ch, "v2_world_collapsed", res)

        # 3) 点世界名牌 → 频道网格展开
        click(hwnd, ox, oy, WORLD_BTN[0], WORLD_BTN[1])
        time.sleep(1.0)
        grab(hwnd, ox, oy, cw, ch, "v2_channel_grid", res)

        # 4) 选 CH.1 (红圈末帧)
        click(hwnd, ox, oy, CH1_BTN[0], CH1_BTN[1])
        time.sleep(0.5)
        grab(hwnd, ox, oy, cw, ch, "v2_channel_selected", res)

        # 5) 到选择的世界去 → 选角色页(清晰常态)
        click(hwnd, ox, oy, GOWORLD_BTN[0], GOWORLD_BTN[1])
        ev, newtxt, charlist_ok = wait_charlist(PDUMP, start, timeout=12.0)
        res["charlist_hit"] = charlist_ok
        time.sleep(1.2)
        grab(hwnd, ox, oy, cw, ch, "v2_charselect", res)

        # 6) 点「创建角色」→ UIRaceSelect (创建角色页, 上一轮硬漏项)
        click(hwnd, ox, oy, CREATE_BTN[0], CREATE_BTN[1])
        time.sleep(2.0)  # 场景 step3 相机移动 + fade
        grab(hwnd, ox, oy, cw, ch, "v2_racecreate", res)
        # 再抓 2 帧确认不是过场
        time.sleep(1.0)
        grab(hwnd, ox, oy, cw, ch, "v2_racecreate_settled", res)

        sends = [l for l in newtxt.splitlines() if l.startswith("[SEND]")]
        res["send_nonheartbeat"] = [l[:90] for l in sends if ": 18 00" not in l][:12]
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
        ev = os.path.join(SHOTS, "recapture_v2_summary.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "pid": res.get("pid"),
                          "serverlist_anchor_hit": res.get("serverlist_anchor_hit"),
                          "charlist_hit": res.get("charlist_hit"),
                          "screens": {k: {"mean": v["mean"], "std": v["std"],
                                          "real": v["real_gl_frame"]}
                                      for k, v in res.get("screens", {}).items()},
                          "send_nonheartbeat": res.get("send_nonheartbeat"),
                          "title": res.get("title", "")[:90],
                          "config": [res["config_head_before"], res["config_head_after"]]},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
