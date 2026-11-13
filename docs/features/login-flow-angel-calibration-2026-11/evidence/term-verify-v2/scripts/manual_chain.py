#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
手动逐屏停驻抓帧（补中间三屏 UI 证据）。

与 auto_walk(login.auto 三级自动链一闪而过)本质不同：
  login.auto=false → 手动点连接 → 每级停住 → 逐屏 BitBlt 抓帧做 NCC。
字符输入用 SendInput(KEYEVENTF_UNICODE)，产生 WM_CHAR，SDL3 文本输入能收
（不同于 keybd_event 字母 VK，后者只送 keydown 不产生 WM_CHAR，已证失败）。

动作序列由 --plan 文件(JSON list)驱动，每步:
  {"name":..., "click":[x,y] | null, "type":"admin" | null,
   "wait":秒, "frames":N, "gap":秒}
每步产出 shots/manual_<name>.png(+_fNN.png) 与 manual_<name>.json。

配置不动(login.auto 保持 false)；finally 恢复被最小化的遮挡窗。
用法: python manual_chain.py --plan chain1.json --old-pid 0
"""
import os, sys, json, time, argparse, ctypes, subprocess, shutil
from ctypes import wintypes
import numpy as np
from PIL import Image

EXE = r"E:\MXD\BeiDou-Client-xy\MapleStory.exe"
EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
CFG = os.path.join(EXE_DIR, "xy-config.yaml")
SHOTS = r"C:\Users\11036\AppData\Local\Temp\term_verify\shots"
DETACHED_PROCESS = 0x00000008

u = ctypes.windll.user32
g = ctypes.windll.gdi32
k32 = ctypes.windll.kernel32
HWND_TOPMOST, HWND_NOTOPMOST = -1, -2
SWP_NOMOVE, SWP_NOSIZE, SWP_SHOWWINDOW = 0x0002, 0x0001, 0x0040
SW_MINIMIZE, SW_RESTORE = 6, 9
SRCCOPY = 0x00CC0020
MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP = 0x0002, 0x0004
INPUT_KEYBOARD = 1
KEYEVENTF_UNICODE = 0x0004
KEYEVENTF_KEYUP = 0x0002
EnumWindowsProc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)


class MOUSEINPUT(ctypes.Structure):
    # union 必须含最大成员(MOUSEINPUT 32B)，否则 x64 下 sizeof(INPUT)=32 会被
    # SendInput 判 cbSize 错、整批拒收(返回 0)。真实 sizeof(INPUT)=40。
    _fields_ = [("dx", wintypes.LONG), ("dy", wintypes.LONG),
                ("mouseData", wintypes.DWORD), ("dwFlags", wintypes.DWORD),
                ("time", wintypes.DWORD), ("dwExtraInfo", ctypes.c_void_p)]


class KEYBDINPUT(ctypes.Structure):
    _fields_ = [("wVk", wintypes.WORD), ("wScan", wintypes.WORD),
                ("dwFlags", wintypes.DWORD), ("time", wintypes.DWORD),
                ("dwExtraInfo", ctypes.c_void_p)]


class INPUTUNION(ctypes.Union):
    _fields_ = [("mi", MOUSEINPUT), ("ki", KEYBDINPUT)]


class INPUT(ctypes.Structure):
    _fields_ = [("type", wintypes.DWORD), ("u", INPUTUNION)]


class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG),
                ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD),
                ("biSizeImage", wintypes.DWORD), ("biXPelsPerMeter", wintypes.LONG),
                ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD),
                ("biClrImportant", wintypes.DWORD)]


class BITMAPINFO(ctypes.Structure):
    _fields_ = [("bmiHeader", BITMAPINFOHEADER), ("bmiColors", wintypes.DWORD * 3)]


def cfg_head():
    with open(CFG, "rb") as f:
        return f.read(3)


def enum_proc_window(pid):
    out = []

    def cb(h, l):
        p = wintypes.DWORD()
        u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            r = wintypes.RECT()
            u.GetWindowRect(h, ctypes.byref(r))
            out.append((int(h), (r.right - r.left) * (r.bottom - r.top)))
        return True
    u.EnumWindows(EnumWindowsProc(cb), 0)
    big = [x for x in out if x[1] > 600 * 400]
    return max(big, key=lambda x: x[1])[0] if big else None


def force_game_top(hwnd):
    P = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
    occ = []

    def cb(h, l):
        if h == hwnd or not u.IsWindowVisible(h):
            return True
        c = ctypes.create_unicode_buffer(256)
        u.GetClassNameW(h, c, 256)
        r = wintypes.RECT()
        u.GetWindowRect(h, ctypes.byref(r))
        if (r.right - r.left) >= 1200 and (r.bottom - r.top) >= 700:
            occ.append(int(h))
        return True
    u.EnumWindows(P(cb), 0)
    for h in occ:
        u.ShowWindow(h, SW_MINIMIZE)
    fg = int(u.GetForegroundWindow())
    tid_fg = u.GetWindowThreadProcessId(fg, None)
    tid_me = k32.GetCurrentThreadId()
    u.AttachThreadInput(tid_me, tid_fg, True)
    u.keybd_event(0x12, 0, 0, 0)
    u.ShowWindow(hwnd, SW_RESTORE)
    u.SetForegroundWindow(hwnd)
    u.keybd_event(0x12, 0, KEYEVENTF_KEYUP, 0)
    u.AttachThreadInput(tid_me, tid_fg, False)
    u.SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)
    time.sleep(0.4)
    return occ, int(u.GetForegroundWindow()) == int(hwnd)


def client_origin(hwnd):
    r = wintypes.RECT()
    u.GetClientRect(hwnd, ctypes.byref(r))
    pt = wintypes.POINT(0, 0)
    u.ClientToScreen(hwnd, ctypes.byref(pt))
    return int(pt.x), int(pt.y), int(r.right - r.left), int(r.bottom - r.top)


def bitblt_grab(sx, sy, w, h):
    hdc = u.GetDC(None)
    mem = g.CreateCompatibleDC(hdc)
    bmp = g.CreateCompatibleBitmap(hdc, w, h)
    g.SelectObject(mem, bmp)
    ok = g.BitBlt(mem, 0, 0, w, h, hdc, sx, sy, SRCCOPY)
    bi = BITMAPINFO()
    bi.bmiHeader.biSize = ctypes.sizeof(BITMAPINFOHEADER)
    bi.bmiHeader.biWidth = w
    bi.bmiHeader.biHeight = -h
    bi.bmiHeader.biPlanes = 1
    bi.bmiHeader.biBitCount = 32
    bi.bmiHeader.biCompression = 0
    buf = (ctypes.c_ubyte * (w * h * 4))()
    lines = g.GetDIBits(mem, bmp, 0, h, buf, ctypes.byref(bi), 0)
    g.DeleteObject(bmp)
    g.DeleteDC(mem)
    u.ReleaseDC(None, hdc)
    if not ok or lines != h:
        return None
    arr = np.frombuffer(buf, dtype=np.uint8).reshape(h, w, 4)
    return arr[..., [2, 1, 0]].copy()


def unicode_type(text):
    """SendInput KEYEVENTF_UNICODE：每字符送 WM_CHAR(SDL3 文本输入可收)。"""
    U = ctypes.WinDLL("user32", use_last_error=True)
    U.SendInput.argtypes = [wintypes.UINT, ctypes.POINTER(INPUT), ctypes.c_int]
    U.SendInput.restype = wintypes.UINT
    sent = []
    for chx in text:
        for up in (False, True):
            ki = KEYBDINPUT(wVk=0, wScan=ord(chx),
                            dwFlags=KEYEVENTF_UNICODE | (KEYEVENTF_KEYUP if up else 0),
                            time=0, dwExtraInfo=None)
            inp = INPUT(type=INPUT_KEYBOARD, u=INPUTUNION(ki=ki))
            n = U.SendInput(1, ctypes.byref(inp), ctypes.sizeof(INPUT))
            sent.append(n)
        time.sleep(0.05)
    return sent


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--plan", required=True)
    ap.add_argument("--old-pid", type=int, default=0)
    ap.add_argument("--boot-wait", type=float, default=3.0)
    args = ap.parse_args()

    plan = json.load(open(args.plan, encoding="utf-8"))
    res = {"ts": time.strftime("%Y-%m-%d %H:%M:%S"), "method": "manual stop + UNICODE + BitBlt",
           "config_head_before": repr(cfg_head()), "steps": []}
    os.makedirs(SHOTS, exist_ok=True)
    occ_all = []
    hwnd = None
    try:
        if args.old_pid:
            subprocess.run(["taskkill", "/F", "/PID", str(args.old_pid)], capture_output=True)
            time.sleep(1.0)
        pr = subprocess.Popen([EXE], cwd=EXE_DIR,
                              creationflags=DETACHED_PROCESS, close_fds=True)
        pid = pr.pid
        res["pid"] = pid
        time.sleep(args.boot_wait)
        t = time.time()
        while time.time() - t < 20:
            hwnd = enum_proc_window(pid)
            if hwnd:
                break
            time.sleep(0.4)
        if not hwnd:
            res["error"] = "no window"
            return
        res["hwnd"] = hwnd
        occ, fgok = force_game_top(hwnd)
        occ_all = occ
        res["force_top"] = {"minimized": occ, "fg_ok": fgok}

        for st in plan:
            name = st["name"]
            ox, oy, cw, ch = client_origin(hwnd)
            # 每步重新压顶(遮挡窗可能被恢复)
            u.SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)
            time.sleep(0.25)
            rec = {"name": name, "client_origin": [ox, oy, cw, ch]}
            if st.get("click"):
                cx, cy = st["click"]
                sx, sy = int(ox + cx), int(oy + cy)
                u.SetCursorPos(sx, sy)
                time.sleep(0.12)
                u.mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
                time.sleep(0.07)
                u.mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
                rec["click_client"] = [cx, cy]
                time.sleep(0.4)
            if st.get("type"):
                rec["unicode_sent"] = unicode_type(st["type"])
                rec["typed"] = st["type"]
            if st.get("wait"):
                time.sleep(st["wait"])
            n = int(st.get("frames", 1))
            gap = float(st.get("gap", 0.45))
            rec["frames"] = []
            for i in range(n):
                u.SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                               SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)
                rgb = bitblt_grab(ox, oy, cw, ch)
                f = {"i": i}
                if rgb is None:
                    f["ok"] = False
                else:
                    im = rgb.astype(np.float64)
                    uniq = len(np.unique(rgb[::16, ::16].reshape(-1, 3), axis=0))
                    dst = os.path.join(SHOTS, "manual_%s%s.png" %
                                       (name, "" if n == 1 else "_f%02d" % i))
                    Image.fromarray(rgb, "RGB").save(dst)
                    f.update({"ok": True, "file": os.path.basename(dst),
                              "mean": round(float(im.mean()), 2),
                              "std": round(float(im.std()), 2),
                              "uniq": int(uniq),
                              "real_gl_frame": bool(im.std() > 8 and uniq > 40),
                              "bytes": os.path.getsize(dst)})
                rec["frames"].append(f)
                if i + 1 < n:
                    time.sleep(gap)
            # 标题留痕
            nb = u.GetWindowTextLengthW(hwnd)
            b = ctypes.create_unicode_buffer(nb + 1)
            u.GetWindowTextW(hwnd, b, nb + 1)
            rec["title"] = b.value[:130]
            res["steps"].append(rec)
            ev = os.path.join(SHOTS, "manual_%s.json" % name)
            json.dump(rec, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        u.SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)
    finally:
        restored = []
        for h in occ_all:
            try:
                u.ShowWindow(h, SW_RESTORE)
                restored.append(h)
            except Exception:
                pass
        res["config_head_after"] = repr(cfg_head())
        res["restored_occluders"] = len(restored)
        ev = os.path.join(SHOTS, "manual_chain_summary.json")
        json.dump(res, open(ev, "w", encoding="utf-8"), ensure_ascii=False, indent=2)
        res["evidence"] = ev
        print(json.dumps({"evidence": ev, "pid": res.get("pid"),
                          "config": [res["config_head_before"], res["config_head_after"]],
                          "steps": [{"name": r["name"],
                                     "frames": sum(1 for f in r["frames"] if f.get("real_gl_frame")),
                                     "mean": r["frames"][0].get("mean"),
                                     "title": r.get("title", "")[:80]}
                                    for r in res["steps"]],
                          "restored_occluders": len(restored)},
                         ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
