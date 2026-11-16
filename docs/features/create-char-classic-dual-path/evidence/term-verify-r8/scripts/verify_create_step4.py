#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""创建角色第二页(命名)+第三页(step4 属性/皮肤/装备)真机捕获。
登录→CH1→GoWorld→选角页→创建→职业团(冒险家/骑士团/战神)→命名页→
输入名字→确定→等查重包→step4 外观页抓帧。
独立 exe MapleStory_r4.exe（文件名放行），不覆盖 live；配置护栏由 run_create_step4_guarded.py 负责。
输出 shots/create_step4_<class>_*.png + ui_creation/create_step4_capture.json
"""
import os, sys, json, time, ctypes, subprocess, shutil, hashlib
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from clean_world_capture import wait_anchor
from capture_charselect_clean import wait_charlist, click

EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
SRC_EXE = r"C:\Users\11036\AppData\Local\Temp\term_verify\build_r4\MapleStory.exe"
EXE = os.path.join(EXE_DIR, "MapleStory_r4.exe")
PDUMP = os.path.join(EXE_DIR, "packet_dump.log")
SHOTS = mc.SHOTS
OUT = r"C:\Users\11036\AppData\Local\Temp\term_verify\ui_creation"
u = mc.u
SWP = mc.SWP_NOMOVE | mc.SWP_NOSIZE | mc.SWP_SHOWWINDOW

WORLD_BTN = (485, 210)
CH1_BTN = (490, 350)
GOWORLD = (820, 518)
CREATE_BTN = (865, 263)       # BtNew 中心（WZ NCC=1.0）
# 职业团页窗口 position=(240,40)，卡局部 (40,90)/(170,90)/(300,90)，卡宽~120高~160
# 卡中心屏幕: 骑士团 (240+100,40+170)=(340,210), 冒险家 (470,210), 战神 (600,210)
CLASS_CARDS = {"cygnus": (340, 210), "adventurer": (470, 210), "aran": (600, 210)}
SELECT_BTN = (815, 477)       # classic「选择」按钮（实测）
# 命名页输入框/按钮：冒险家 sign(721,155)+slot(28,104)=(749,259)，输入槽 150x24
# yes(748,333)/no(822,333) 是按钮左上，中心 +~(40,20)
# 命名页锚点按职业分别推导（源码 UICommonCreation.cpp:424-447 sign_pos+slot_pos，
# 输入框中心 = sign+slot+(75,12)，确定中心 = yes_pos + 冒险家实测偏移(40,20)）。
# 旧脚本把冒险家坐标写死，aran 的 yes y=304 与冒险家 333 差 29px 会点空 → 从未进 step4。
CLASS_ANCHORS = {
    "adventurer": {"name": (824, 271), "yes": (788, 353)},  # sign(721,155)+slot(28,104)
    "cygnus":     {"name": (828, 271), "yes": (796, 356)},  # 实测进 step4 成功（新包 19:18）
    # 战神不能用源码 yes_pos=(750,304) 推屏幕坐标：源码是窗口内逻辑坐标，
    # try_maple_btn 有 origin 补偿。模板匹配 locate_aran_yes.py 实测：
    # 战神命名页 yes=(788,352)、name=(821,270)，与冒险家几乎同位置（delta 0,-1 / -3,-1）。
    "aran":       {"name": (821, 270), "yes": (788, 352)},
}


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


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
            "real_gl_frame": bool(im.std() > 8 and uniq > 40)}
    res.setdefault("frames", {})[name] = info
    store[name] = rgb
    return info


def clear_and_type(hwnd, ox, oy, cx, cy, text, res, tag):
    click(hwnd, ox, oy, cx, cy)
    time.sleep(0.2)
    for _ in range(15):
        u.keybd_event(0x08, 0, 0, 0)
        u.keybd_event(0x08, 0, mc.KEYEVENTF_KEYUP, 0)
    time.sleep(0.12)
    sent = mc.unicode_type(text)
    res.setdefault("inputs", {})[tag] = {"text": text, "ok": all(x == 1 for x in sent)}
    time.sleep(0.2)


def main():
    which = sys.argv[1] if len(sys.argv) > 1 else "adventurer"
    name = "s4probe" + str(len(which))  # 名字查重用，避免重名
    res = {"method": "step4 capture " + which, "ts": time.strftime("%Y-%m-%d %H:%M:%S"),
           "class": which, "char_name": name,
           "src_exe_sha256": sha(SRC_EXE), "config_head_before": repr(mc.cfg_head())}
    store = {}
    occ_all = []
    try:
        # 部署隔离 exe
        shutil.copy2(SRC_EXE, EXE)
        res["deployed_sha256"] = sha(EXE)
        start = os.path.getsize(PDUMP)
        pr = subprocess.Popen([EXE], cwd=EXE_DIR, creationflags=mc.DETACHED_PROCESS, close_fds=True)
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

        # 登录
        clear_and_type(hwnd, ox, oy, 720, 308, "admin", res, "account")
        clear_and_type(hwnd, ox, oy, 720, 336, "admin", res, "password")
        click(hwnd, ox, oy, 880, 300)
        anchors, atxt, ok = wait_anchor(PDUMP, start, timeout=45.0)
        res["server_anchor"] = ok
        time.sleep(1.0)
        # 世界 → CH1 → GoWorld
        click(hwnd, ox, oy, *WORLD_BTN)
        time.sleep(1.2)
        click(hwnd, ox, oy, *CH1_BTN)
        time.sleep(0.6)
        click(hwnd, ox, oy, *GOWORLD)
        ev, txt, ok2 = wait_charlist(PDUMP, start, timeout=15.0)
        res["charlist"] = ok2
        time.sleep(2.5)
        grab(hwnd, ox, oy, cw, ch, "create_s4_00_charselect", res, store)

        # 创建 → 职业团页
        click(hwnd, ox, oy, *CREATE_BTN)
        time.sleep(2.2)
        grab(hwnd, ox, oy, cw, ch, "create_s4_01_raceselect", res, store)

        # 选职业卡 + 选择
        card = CLASS_CARDS[which]
        click(hwnd, ox, oy, *card)
        time.sleep(0.6)
        grab(hwnd, ox, oy, cw, ch, "create_s4_02_card_" + which, res, store)
        click(hwnd, ox, oy, *SELECT_BTN)
        time.sleep(2.2)
        grab(hwnd, ox, oy, cw, ch, "create_s4_03_naming_" + which, res, store)

        # 输入名字 → 确定 → 等服务端查重 → step4（锚点按职业分别取，见 CLASS_ANCHORS）
        anchors = CLASS_ANCHORS[which]
        res["input_anchors"] = anchors
        clear_and_type(hwnd, ox, oy, *anchors["name"], name, res, "charname")
        grab(hwnd, ox, oy, cw, ch, "create_s4_04_named_" + which, res, store)
        click(hwnd, ox, oy, *anchors["yes"])
        # 连抓覆盖：查重往返 + enter_classic_look_stage
        for i in range(10):
            time.sleep(0.5)
            grab(hwnd, ox, oy, cw, ch, "create_s4_05_step4_%s_f%02d" % (which, i), res, store)

        nb = u.GetWindowTextLengthW(hwnd)
        buf = ctypes.create_unicode_buffer(nb + 1)
        u.GetWindowTextW(hwnd, buf, nb + 1)
        res["title"] = buf.value[:130]
        res["config_head_after"] = repr(mc.cfg_head())
    finally:
        for h in occ_all:
            try:
                u.ShowWindow(h, mc.SW_RESTORE)
            except Exception:
                pass
        ev = os.path.join(OUT, "create_step4_capture_%s.json" % which)
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        print(json.dumps({"evidence": ev, "class": which, "name": name,
                          "error": res.get("error"), "frames": {
                              k: {"mean": v["mean"], "real": v["real_gl_frame"]}
                              for k, v in res.get("frames", {}).items()},
                          "title": res.get("title", "")[:90]},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
