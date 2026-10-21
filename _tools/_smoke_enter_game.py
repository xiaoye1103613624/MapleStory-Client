# -*- coding: utf-8 -*-
"""Smoke-test: login -> world -> char select -> enter game; capture + log check."""
from __future__ import annotations

import ctypes
import ctypes.wintypes as wt
import os
import struct
import subprocess
import sys
import time
from pathlib import Path

CLIENT_DIR = Path(r"E:\MXD\BeiDou-Client-xy")
EXE = CLIENT_DIR / "MapleStory.exe"
ENTER_LOG = CLIENT_DIR / "_enter_game.log"
OUT_DIR = Path(r"E:\project\MapleStory-Client\docs\features\enter-game-blackscreen-fix\evidence")
OUT_DIR.mkdir(parents=True, exist_ok=True)

INPUT_KEYBOARD = 1
KEYEVENTF_KEYUP = 0x0002
KEYEVENTF_UNICODE = 0x0004
MOUSEEVENTF_LEFTDOWN = 0x0002
MOUSEEVENTF_LEFTUP = 0x0004
VK_RETURN = 0x0D
VK_TAB = 0x09
VK_ESCAPE = 0x1B

user32 = ctypes.windll.user32
gdi32 = ctypes.windll.gdi32
kernel32 = ctypes.windll.kernel32


class KEYBDINPUT(ctypes.Structure):
    _fields_ = [
        ("wVk", wt.WORD),
        ("wScan", wt.WORD),
        ("dwFlags", wt.DWORD),
        ("time", wt.DWORD),
        ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong)),
    ]


class MOUSEINPUT(ctypes.Structure):
    _fields_ = [
        ("dx", wt.LONG),
        ("dy", wt.LONG),
        ("mouseData", wt.DWORD),
        ("dwFlags", wt.DWORD),
        ("time", wt.DWORD),
        ("dwExtraInfo", ctypes.POINTER(ctypes.c_ulong)),
    ]


class HARDWAREINPUT(ctypes.Structure):
    _fields_ = [
        ("uMsg", wt.DWORD),
        ("wParamL", wt.WORD),
        ("wParamH", wt.WORD),
    ]


class INPUT_UNION(ctypes.Union):
    _fields_ = [("ki", KEYBDINPUT), ("mi", MOUSEINPUT), ("hi", HARDWAREINPUT)]


class INPUT(ctypes.Structure):
    _fields_ = [("type", wt.DWORD), ("union", INPUT_UNION)]


def send_vk(vk: int, up: bool = False) -> None:
    inp = INPUT()
    inp.type = INPUT_KEYBOARD
    inp.union.ki = KEYBDINPUT(vk, 0, KEYEVENTF_KEYUP if up else 0, 0, None)
    user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(INPUT))


def tap_vk(vk: int, pause: float = 0.05) -> None:
    send_vk(vk, False)
    time.sleep(pause)
    send_vk(vk, True)
    time.sleep(pause)


def type_text(text: str) -> None:
    for ch in text:
        inp_down = INPUT()
        inp_down.type = INPUT_KEYBOARD
        inp_down.union.ki = KEYBDINPUT(0, ord(ch), KEYEVENTF_UNICODE, 0, None)
        inp_up = INPUT()
        inp_up.type = INPUT_KEYBOARD
        inp_up.union.ki = KEYBDINPUT(0, ord(ch), KEYEVENTF_UNICODE | KEYEVENTF_KEYUP, 0, None)
        user32.SendInput(1, ctypes.byref(inp_down), ctypes.sizeof(INPUT))
        user32.SendInput(1, ctypes.byref(inp_up), ctypes.sizeof(INPUT))
        time.sleep(0.02)


def find_game_hwnd(pid: int) -> int:
    found = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def enum_proc(hwnd, _lparam):
        if not user32.IsWindowVisible(hwnd):
            return True
        proc_id = wt.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(proc_id))
        if proc_id.value != pid:
            return True
        # Prefer windows with a non-empty title and decent size.
        length = user32.GetWindowTextLengthW(hwnd)
        buf = ctypes.create_unicode_buffer(max(length, 1) + 1)
        if length > 0:
            user32.GetWindowTextW(hwnd, buf, length + 1)
        title = buf.value if length > 0 else ""
        rect = wt.RECT()
        user32.GetWindowRect(hwnd, ctypes.byref(rect))
        w = rect.right - rect.left
        h = rect.bottom - rect.top
        if w >= 640 and h >= 400:
            found.append((hwnd, title, w, h))
        return True

    user32.EnumWindows(enum_proc, 0)
    if not found:
        return 0
    found.sort(key=lambda x: x[2] * x[3], reverse=True)
    return found[0][0]


def window_title(hwnd: int) -> str:
    length = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(length + 1)
    user32.GetWindowTextW(hwnd, buf, length + 1)
    return buf.value


def is_responding(hwnd: int) -> bool:
    # SMTO_ABORTIFHUNG = 0x0002
    result = wt.DWORD()
    ret = user32.SendMessageTimeoutW(hwnd, 0, 0, 0, 0x0002, 2000, ctypes.byref(result))
    return ret != 0


def capture_window(hwnd: int, path: Path) -> tuple[int, int, float]:
    """Capture client area to BMP; return (w,h, black_ratio)."""
    rect = wt.RECT()
    user32.GetClientRect(hwnd, ctypes.byref(rect))
    w = rect.right - rect.left
    h = rect.bottom - rect.top
    if w <= 0 or h <= 0:
        return 0, 0, 1.0

    hdc_win = user32.GetDC(hwnd)
    hdc_mem = gdi32.CreateCompatibleDC(hdc_win)
    hbmp = gdi32.CreateCompatibleBitmap(hdc_win, w, h)
    gdi32.SelectObject(hdc_mem, hbmp)
    # PrintWindow with PW_CLIENTONLY|PW_RENDERFULLCONTENT
    user32.PrintWindow(hwnd, hdc_mem, 0x3)

    class BITMAPINFOHEADER(ctypes.Structure):
        _fields_ = [
            ("biSize", wt.DWORD),
            ("biWidth", wt.LONG),
            ("biHeight", wt.LONG),
            ("biPlanes", wt.WORD),
            ("biBitCount", wt.WORD),
            ("biCompression", wt.DWORD),
            ("biSizeImage", wt.DWORD),
            ("biXPelsPerMeter", wt.LONG),
            ("biYPelsPerMeter", wt.LONG),
            ("biClrUsed", wt.DWORD),
            ("biClrImportant", wt.DWORD),
        ]

    bmi = BITMAPINFOHEADER()
    bmi.biSize = ctypes.sizeof(BITMAPINFOHEADER)
    bmi.biWidth = w
    bmi.biHeight = -h  # top-down
    bmi.biPlanes = 1
    bmi.biBitCount = 24
    bmi.biCompression = 0
    row = ((w * 3 + 3) // 4) * 4
    buf = (ctypes.c_ubyte * (row * h))()
    gdi32.GetDIBits(hdc_mem, hbmp, 0, h, buf, ctypes.byref(bmi), 0)

    # write BMP
    pixel_size = row * h
    file_header = struct.pack("<2sIHHI", b"BM", 14 + 40 + pixel_size, 0, 0, 14 + 40)
    info_header = struct.pack(
        "<IIIHHIIIIII",
        40,
        w,
        h,
        1,
        24,
        0,
        pixel_size,
        0,
        0,
        0,
        0,
    )
    path.write_bytes(file_header + info_header + bytes(buf))

    # black ratio: sample every 8th pixel
    black = 0
    total = 0
    for y in range(0, h, 8):
        for x in range(0, w, 8):
            i = y * row + x * 3
            b, g, r = buf[i], buf[i + 1], buf[i + 2]
            total += 1
            if r < 12 and g < 12 and b < 12:
                black += 1

    gdi32.DeleteObject(hbmp)
    gdi32.DeleteDC(hdc_mem)
    user32.ReleaseDC(hwnd, hdc_win)
    return w, h, (black / total if total else 1.0)


def focus(hwnd: int) -> None:
    user32.ShowWindow(hwnd, 9)  # SW_RESTORE
    user32.SetForegroundWindow(hwnd)
    time.sleep(0.3)


def main() -> int:
    # kill old client
    subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
    time.sleep(1)

    if ENTER_LOG.exists():
        ENTER_LOG.write_text("", encoding="utf-8")

    print("[1] launch", EXE)
    proc = subprocess.Popen([str(EXE)], cwd=str(CLIENT_DIR))
    hwnd = 0
    for i in range(90):
        time.sleep(1)
        hwnd = find_game_hwnd(proc.pid)
        if hwnd:
            print(f"[2] window hwnd={hwnd} pid={proc.pid} title={window_title(hwnd)!r} t={i+1}s")
            break
        if proc.poll() is not None:
            print("[FAIL] process exited early code=", proc.returncode)
            return 2
    if not hwnd:
        print("[FAIL] no window for pid", proc.pid)
        return 3

    # wait for login UI to settle (data load)
    print("[3] wait login settle 12s")
    time.sleep(12)
    # re-resolve hwnd in case it changed
    hwnd = find_game_hwnd(proc.pid) or hwnd
    focus(hwnd)

    # Login: account focused or password focused; clear + type
    print("[4] type credentials admin/admin")
    # click roughly center-left of book (account field ~ panel)
    # Send clicks via client coords relative to window
    rect = wt.RECT()
    user32.GetWindowRect(hwnd, ctypes.byref(rect))
    # account field approximate for BeiDou wood panel ~ center
    ax = rect.left + (rect.right - rect.left) // 2
    ay = rect.top + int((rect.bottom - rect.top) * 0.48)
    user32.SetCursorPos(ax, ay)
    time.sleep(0.1)
    user32.mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    user32.mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    time.sleep(0.2)
    # select-all / clear via many backspaces then type
    for _ in range(24):
        tap_vk(0x08, 0.01)  # VK_BACK
    type_text("admin")
    tap_vk(VK_TAB)
    time.sleep(0.2)
    for _ in range(24):
        tap_vk(0x08, 0.01)
    type_text("admin")
    time.sleep(0.2)
    tap_vk(VK_RETURN)
    print("[5] submitted login, wait world select")
    time.sleep(5)

    focus(hwnd)
    # world/channel: Enter often selects
    tap_vk(VK_RETURN)
    time.sleep(1)
    tap_vk(VK_RETURN)
    print("[6] world/channel enter x2, wait char select")
    time.sleep(5)

    focus(hwnd)
    shot1 = OUT_DIR / "test_charselect.bmp"
    w, h, br = capture_window(hwnd, shot1)
    print(f"[7] charselect shot {w}x{h} black_ratio={br:.3f} -> {shot1}")
    responding = is_responding(hwnd)
    print(f"    responding={responding} title={window_title(hwnd)!r}")

    # Enter to start game (BtSelect)
    print("[8] press Enter to start game")
    tap_vk(VK_RETURN)
    time.sleep(2)
    # also try clicking Start button region (lower-right-ish of charselect)
    bx = rect.left + int((rect.right - rect.left) * 0.72)
    by = rect.top + int((rect.bottom - rect.top) * 0.78)
    user32.SetCursorPos(bx, by)
    time.sleep(0.05)
    user32.mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    user32.mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)

    # wait enter game
    print("[9] wait enter-game up to 45s")
    entered = False
    for i in range(45):
        time.sleep(1)
        if ENTER_LOG.exists() and "fadeprocedure done" in ENTER_LOG.read_text(encoding="utf-8", errors="ignore"):
            entered = True
            print(f"    enter log done at t={i+1}s")
            break
        if not is_responding(hwnd):
            print(f"    [WARN] Not Responding at t={i+1}s")

    time.sleep(3)
    focus(hwnd)
    # click center to probe hang
    cx = rect.left + (rect.right - rect.left) // 2
    cy = rect.top + (rect.bottom - rect.top) // 2
    user32.SetCursorPos(cx, cy)
    user32.mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    user32.mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    time.sleep(1)

    shot2 = OUT_DIR / "test_ingame.bmp"
    w2, h2, br2 = capture_window(hwnd, shot2)
    resp2 = is_responding(hwnd)
    title2 = window_title(hwnd)
    log_text = ENTER_LOG.read_text(encoding="utf-8", errors="ignore") if ENTER_LOG.exists() else ""
    print(f"[10] ingame shot {w2}x{h2} black_ratio={br2:.3f} responding={resp2}")
    print(f"     title={title2!r}")
    print("--- _enter_game.log ---")
    print(log_text if log_text else "(empty)")
    print("-----------------------")

    # Pass criteria
    ok_log = "fadeprocedure done" in log_text and "Stage::load ok" in log_text
    ok_not_black = br2 < 0.92
    ok_resp = resp2
    ok = ok_log and ok_not_black and ok_resp
    print(f"[RESULT] log={ok_log} not_black={ok_not_black} responding={ok_resp} => {'PASS' if ok else 'FAIL'}")

    # leave client running for a bit then kill
    time.sleep(2)
    subprocess.run(["taskkill", "/F", "/IM", "MapleStory.exe"], capture_output=True)
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
