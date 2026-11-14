# -*- coding: utf-8 -*-
"""m02632 三项修复真机验收：登录页(版本号+品牌LOGO) / 选角色页(属性板多余带)。
复用既有链路（manual_chain BitBlt/force_top + recapture_all_v2.clear_and_type + grab）。
验收阈值：ver_dark>=100（目标端 130）| logo_roi_ncc>=0.90 | 属性板底部带墨水不高于目标端+0.02。
"""
import os, sys, json, time
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import manual_chain as mc
from clean_world_capture import wait_anchor
from capture_charselect_clean import wait_charlist, click
from recapture_all_v2 import clear_and_type, grab, ACCOUNT_FIELD, PASSWORD_FIELD

SHOTS = mc.SHOTS; T = os.path.join(os.path.dirname(SHOTS), "targets_desktop")
PDUMP = os.path.join(mc.EXE_DIR, "packet_dump.log")
OUT = os.path.join(SHOTS, "ui_final_verdict.json")
VER_ROI = (1000, 0, 1280, 45); LOGO = (434, 80, 397, 219)

def arr(p): return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
def ncc(x, y):
    x = x - x.mean(); y = y - y.mean()
    d = np.sqrt((x*x).sum() * (y*y).sum())
    return round(float((x*y).sum()/d), 4) if d > 1e-9 else 0.0

def ver_dark(a):
    x0, y0, x1, y1 = VER_ROI; r = a[y0:y1, x0:x1]
    lum = 0.299*r[:,:,0]+0.587*r[:,:,1]+0.114*r[:,:,2]
    m = (lum < 95).copy(); m[:, 230:] = False
    ys, xs = np.where(m)
    return int(m.sum()), (None if len(ys) == 0 else [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())])

def logo_roi(a):
    x, y, w, h = LOGO; return a[y:y+h, x:x+w]

def board_bottom_ink(a, x0, y0, w, h):
    r = a[y0:y0+h, x0:x0+w]
    lum = 0.299*r[:,:,0]+0.587*r[:,:,1]+0.114*r[:,:,2]
    return round(float((lum[int(h*0.60):] < 150).mean()), 4)

res = {"ts": time.strftime("%Y-%m-%d %H:%M:%S"), "cfg_before": repr(mc.cfg_head())}
start = os.path.getsize(PDUMP) if os.path.exists(PDUMP) else 0
pr = mc.subprocess.Popen([mc.EXE], cwd=mc.EXE_DIR, creationflags=mc.DETACHED_PROCESS)
res["pid"] = pr.pid; time.sleep(3.0)
hwnd = None; t0 = time.time()
while time.time()-t0 < 20:
    hwnd = mc.enum_proc_window(pr.pid)
    if hwnd: break
    time.sleep(0.3)
if not hwnd:
    res["error"] = "no window"; print(json.dumps(res, ensure_ascii=False, indent=2)); raise SystemExit(2)
occ, fgok = mc.force_game_top(hwnd); res["fg_match"] = fgok
ox, oy, cw, ch = mc.client_origin(hwnd); res["client_origin"] = [ox, oy, cw, ch]

clear_and_type(hwnd, ox, oy, ACCOUNT_FIELD[0], ACCOUNT_FIELD[1], "admin", res, "account")
clear_and_type(hwnd, ox, oy, PASSWORD_FIELD[0], PASSWORD_FIELD[1], "admin", res, "password")
time.sleep(0.5)
res["login_shot"] = grab(hwnd, ox, oy, cw, ch, "ui_final_login", res)
click(hwnd, ox, oy, 880, 300)
anchors, newtxt, ok = wait_anchor(PDUMP, start, timeout=12.0); res["serverlist_anchor_hit"] = ok
time.sleep(0.9)
click(hwnd, ox, oy, 485, 210); time.sleep(1.1)
click(hwnd, ox, oy, 490, 350); time.sleep(0.6)
click(hwnd, ox, oy, 820, 518)
ev, newtxt2, charlist_ok = wait_charlist(PDUMP, start, timeout=12.0); res["charlist_hit"] = charlist_ok
time.sleep(2.4)
res["char_shot"] = grab(hwnd, ox, oy, cw, ch, "ui_final_charselect", res)
res["cfg_after"] = repr(mc.cfg_head())

p_login = os.path.join(SHOTS, "ui_final_login.png"); p_char = os.path.join(SHOTS, "ui_final_charselect.png")
a = arr(p_login); t = arr(os.path.join(T, "desktop_login.png"))
vd, vbox = ver_dark(a); tvd, tvbox = ver_dark(t)
res["version"] = {"ours_dark": vd, "ours_bbox": vbox, "target_dark": tvd, "target_bbox": tvbox, "pass": vd >= 100}
lr = ncc(logo_roi(a).mean(2), logo_roi(t).mean(2))
res["logo"] = {"roi_ncc_vs_target": lr, "full_ncc": ncc(a.mean(2), t.mean(2)), "pass": lr >= 0.90}
c = arr(p_char); ct = arr(os.path.join(T, "desktop_char_select.png"))
ob = board_bottom_ink(c, 350, 150, 190, 210); tb = board_bottom_ink(ct, 320, 150, 190, 210)
res["charboard"] = {"ours_bottom_ink": ob, "target_bottom_ink": tb, "pass": ob <= tb + 0.02}
res["all_pass"] = bool(res["version"]["pass"] and res["logo"]["pass"] and res["charboard"]["pass"])
json.dump(res, open(OUT, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
print(json.dumps(res, ensure_ascii=False, indent=2))
