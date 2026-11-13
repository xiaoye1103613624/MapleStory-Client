# -*- coding: utf-8 -*-
"""回归取证 2026-11-07：整工程 Rebuild（NUM_TYPES ABI 混尺寸修复）后，
对用户报的 14 项做真机截图取证。

流程：启动部署客户端（xy-config login.auto=true 自动登录进图）→
等进图 → 依次截图：
  01_game      进图首帧（底部信息栏名称/职业 = item3；快捷键栏提示 = item7）
  02_charinfo  按 W 开角色信息（属性数值 = item1；金币 = item4）
  03_stats     能力值窗
  04_inventory 道具窗（装备排版 = item5）
  05_skill     技能窗 + 组合栏（item12）
  06_minimap   小地图（item13）
  07_chatbar   聊天栏（item14）
  08_npc_hover 鼠标悬停 NPC（item8/11）
结束后关客户端。
"""
import os
import subprocess
import sys
import time

import ctypes
import ctypes.wintypes as wt

EXE_DIR = r"E:\MXD\BeiDou-Client-xy"
EXE = os.path.join(EXE_DIR, "MapleStory.exe")
OUT = r"E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\evidence\regress-20261107"
os.makedirs(OUT, exist_ok=True)
CAP_PS = r"E:\project\MapleStory-Client\tools\capture\capture-shot.ps1"

user32 = ctypes.windll.user32
user32.SetProcessDPIAware()

GlfwScan = {  # 物理键 -> (scan, ext) 简化：用 SendInput VK 路径
}

def find_hwnd(pid=None):
    best = []
    EnumWindowsProc = ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)

    def cb(hwnd, lp):
        if not user32.IsWindowVisible(hwnd):
            return True
        length = user32.GetWindowTextLengthW(hwnd)
        if length == 0:
            return True
        buf = ctypes.create_unicode_buffer(length + 1)
        user32.GetWindowTextW(hwnd, buf, length + 1)
        t = buf.value
        if "XiaoYE" in t or "MapleStory" in t:
            pid_out = wt.DWORD()
            user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid_out))
            if pid is None or pid_out.value == pid:
                best.append(hwnd)
        return True

    user32.EnumWindows(EnumWindowsProc(cb), 0)
    return best[0] if best else None


def send_key(hwnd, vk, hold=0.05):
    KEYEVENTF_KEYUP = 0x0002

    class KEYBDINPUT(ctypes.Structure):
        _fields_ = [("wVk", wt.WORD), ("wScan", wt.WORD), ("dwFlags", wt.DWORD),
                    ("time", wt.DWORD), ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong))]

    class INPUT(ctypes.Structure):
        class _I(ctypes.Union):
            _fields_ = [("ki", KEYBDINPUT)]
        _fields_ = [("type", wt.DWORD), ("i", _I)]

    def one(up):
        inp = INPUT()
        inp.type = 1
        inp.i.ki.wVk = vk
        inp.i.ki.dwFlags = KEYEVENTF_KEYUP if up else 0
        user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(INPUT))

    user32.SetForegroundWindow(hwnd)
    time.sleep(0.2)
    one(False)
    time.sleep(hold)
    one(True)


def shot(name):
    out = os.path.join(OUT, name + ".png")
    subprocess.run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
                    "-File", CAP_PS, "-Target", "MapleStory", "-OutFile", out],
                   capture_output=True, text=True)
    print("shot", name, os.path.exists(out))


def main():
    cfg = os.path.join(EXE_DIR, "xy-config.yaml")
    src = open(cfg, encoding="utf-8", errors="replace").read()
    bak = cfg + ".bak.regress"
    if not os.path.exists(bak):
        open(bak, "w", encoding="utf-8").write(src)
    patched = src.replace("auto: false", "auto: true", 1)
    open(cfg, "w", encoding="utf-8").write(patched)

    p = subprocess.Popen([EXE], cwd=EXE_DIR)
    print("pid", p.pid)
    hwnd = None
    for _ in range(60):
        time.sleep(1)
        hwnd = find_hwnd(p.pid)
        if hwnd:
            break
    if not hwnd:
        print("NO HWND"); p.kill(); restore(); return 1
    print("hwnd", hwnd)

    # 等自动登录进图（登录->世界->频道->选角->进图 约 40-60s）
    time.sleep(75)
    shot("01_game")

    # W = 角色信息
    send_key(hwnd, 0x57)  # W
    time.sleep(1.0)
    shot("02_charinfo")

    # S = 能力值（原生默认）
    send_key(hwnd, 0x53)
    time.sleep(1.0)
    shot("03_stats")

    # I = 道具
    send_key(hwnd, 0x49)
    time.sleep(1.0)
    shot("04_inventory")

    # K = 技能
    send_key(hwnd, 0x4B)
    time.sleep(1.0)
    shot("05_skill")

    # M 一次 = 小地图切换
    send_key(hwnd, 0x4D)
    time.sleep(1.0)
    shot("06_minimap")

    # Enter = 聊天聚焦
    send_key(hwnd, 0x0D)
    time.sleep(0.8)
    shot("07_chatbar")
    send_key(hwnd, 0x1B)  # ESC 退聊天

    # 全关：ESC 几次
    for _ in range(3):
        send_key(hwnd, 0x1B)
        time.sleep(0.3)
    shot("08_clean")

    p.terminate()
    time.sleep(3)
    if p.poll() is None:
        p.kill()
    restore()
    return 0


def restore():
    cfg = os.path.join(EXE_DIR, "xy-config.yaml")
    bak = cfg + ".bak.regress"
    if os.path.exists(bak):
        data = open(bak, encoding="utf-8", errors="replace").read()
        open(cfg, "w", encoding="utf-8").write(data)
        print("config restored")


if __name__ == "__main__":
    sys.exit(main())
