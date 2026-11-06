# -*- coding: utf-8 -*-
"""Drive native BeiDou client: click+type via SendInput, then screenshot."""
import ctypes, time, subprocess, sys
from ctypes import wintypes

user32 = ctypes.windll.user32
INPUT_MOUSE, MOUSEEVENTF_MOVE, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP = 0, 0x0001, 0x0002, 0x0004

class MOUSEINPUT(ctypes.Structure):
    _fields_ = [("dx", ctypes.c_long), ("dy", ctypes.c_long), ("mouseData", ctypes.c_ulong),
                ("dwFlags", ctypes.c_ulong), ("time", ctypes.c_ulong), ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong))]
class KEYBDINPUT(ctypes.Structure):
    _fields_ = [("wVk", ctypes.c_ushort), ("wScan", ctypes.c_ushort), ("dwFlags", ctypes.c_ulong),
                ("time", ctypes.c_ulong), ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong))]
class INPUT(ctypes.Structure):
    class _U(ctypes.Union):
        _fields_ = [("mi", MOUSEINPUT), ("ki", KEYBDINPUT)]
    _anonymous_ = ("u",)
    _fields_ = [("type", ctypes.c_ulong), ("u", _U)]

def click(x, y):
    user32.SetCursorPos(int(x), int(y))
    time.sleep(0.08)
    for f in (MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP):
        i = INPUT(type=INPUT_MOUSE); i.mi = MOUSEINPUT(x, y, 0, f, 0, None)
        user32.SendInput(1, ctypes.byref(i), ctypes.sizeof(INPUT))
        time.sleep(0.05)

def send_key(vk, up=False, ext=False):
    flags = 0x0002 if up else 0
    if ext: flags |= 0x0001
    i = INPUT(type=1); i.ki = KEYBDINPUT(vk, 0, flags, 0, None)
    user32.SendInput(1, ctypes.byref(i), ctypes.sizeof(INPUT))

def type_text(s):
    for ch in s.lower():
        if ch.isdigit(): vk = 0x30 + int(ch)
        elif 'a' <= ch <= 'z': vk = 0x41 + ord(ch) - ord('a')
        else: continue
        send_key(vk); time.sleep(0.03); send_key(vk, up=True); time.sleep(0.05)

def shot(out):
    subprocess.run(["powershell","-NoProfile","-ExecutionPolicy","Bypass",
                    "-File", r"E:\project\MapleStory-Client\docs\features\softkey-text-mode\evidence\_shot.ps1",
                    "-Out", out], check=True, capture_output=True)

WX, WY = 640, 220   # window origin
steps = sys.argv[1] if len(sys.argv) > 1 else "login"
if steps == "login":
    click(WX+755, WY+440); time.sleep(0.3)
    type_text("admin"); time.sleep(0.3)
    click(WX+755, WY+485); time.sleep(0.3)
    type_text("admin"); time.sleep(0.3)
    click(WX+878, WY+437)
    time.sleep(6)
    shot(r"E:\project\MapleStory-Client\docs\features\softkey-text-mode\evidence\native-20261106-after-login.png")
    print("done login")
