# -*- coding: utf-8 -*-
"""PostMessage 驱动真机点「扩充道具栏」(BtFull)，抓折叠/展开两帧。

为什么走 PostMessage 而不是 SendInput：本机另一扇窗长期抢前台，SendInput 只发给前台线程。
SDL3 的 Windows 后端从窗口队列里取鼠标消息，鼠标键事件用的是它自己记录的坐标
（SDL_GetMouseState 读的是缓存，不是 GetCursorPos）—— 所以先发一次 WM_MOUSEMOVE 把缓存
挪到目标点，再发按下/抬起，落点才和「点哪儿」一致。

窗口在屏幕上的位置不写死：用 backgrnd 位图在抓帧里做模板匹配反推（layout.anchor_in），
再减掉 PrintWindow 带上的非客户区边框，得到客户区坐标。
agree<0.5 一律视为「帧里没这个窗」：anchor_in 永远会返回全局最低分那一格，不当闸就用假锚点。
"""
import ctypes
import ctypes.wintypes as wt
import os
import sys
import time

import numpy as np
from PIL import Image

sys.path.insert(0, r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/tools")
import layout  # noqa: E402

u = ctypes.windll.user32
g = ctypes.windll.gdi32
u.SetProcessDPIAware()
u.PostMessageW.argtypes = [wt.HWND, wt.UINT, wt.WPARAM, wt.LPARAM]
u.PostMessageW.restype = wt.BOOL

WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_LBUTTONUP = 0x0200, 0x0201, 0x0202
WM_KEYDOWN, WM_KEYUP = 0x0100, 0x0101
MK_LBUTTON = 0x0001
ORACLE = r"E:/project/MapleStory-Client/docs/features/ui-v83-window-restore/evidence/r27-oracle"
TMPL_FOLD = os.path.join(ORACLE, "tmpl/Item_backgrnd.png")
TMPL_FULL = os.path.join(ORACLE, "tmpl/Item_FullBackgrnd_left400.png")
sys.path.insert(0, r"E:/project")
import _drive  # noqa: E402

HWND = int(os.environ.get("R27_HWND") or _drive.find_hwnd())


class BIH(ctypes.Structure):
    _fields_ = [("s", ctypes.c_uint32), ("w", ctypes.c_int), ("h", ctypes.c_int),
                ("p", ctypes.c_uint16), ("bpc", ctypes.c_uint16), ("c", ctypes.c_uint32),
                ("sz", ctypes.c_uint32), ("x", ctypes.c_int), ("y", ctypes.c_int),
                ("u1", ctypes.c_int), ("u2", ctypes.c_int)]


def grab(out):
    wr = wt.RECT()
    u.GetWindowRect(HWND, ctypes.byref(wr))
    w, h = wr.right - wr.left, wr.bottom - wr.top
    cp = wt.POINT()
    u.ClientToScreen(HWND, ctypes.byref(cp))
    dc = u.GetDC(HWND)
    mem = g.CreateCompatibleDC(dc)
    bmp = g.CreateCompatibleBitmap(dc, w, h)
    g.SelectObject(mem, bmp)
    u.PrintWindow(HWND, mem, 2)
    bi = BIH()
    bi.s, bi.w, bi.h, bi.p, bi.bpc, bi.c, bi.sz = ctypes.sizeof(BIH), w, -h, 1, 32, 0, w * h * 4
    buf = ctypes.create_string_buffer(w * h * 4)
    g.GetDIBits(mem, bmp, 0, h, buf, ctypes.byref(bi), 0)
    a = np.frombuffer(buf.raw, np.uint8).reshape(h, w, 4)[:, :, [2, 1, 0]]
    Image.fromarray(a).save(out)
    g.DeleteDC(mem)
    u.ReleaseDC(HWND, dc)
    return a[:, :, :3].astype(np.float64), (cp.x - wr.left, cp.y - wr.top)


def find(arr, tmpl):
    return layout.anchor_in(arr, tmpl)


def click_client(cx, cy):
    sys.path.insert(0, r"E:/project")
    import _rclick  # noqa: E402
    return _rclick.click(HWND, cx, cy)


def tap(vk, scan):
    u.PostMessageW(HWND, WM_KEYDOWN, vk, scan | 1 << 24)
    time.sleep(0.08)
    u.PostMessageW(HWND, WM_KEYUP, vk, scan | 1 << 24 | (1 << 30) | (1 << 31))
    time.sleep(0.45)


def main():
    lines = []

    def say(s):
        print(s)
        lines.append(str(s))

    arr, coff = grab(r"E:/project/_r27_live_before.png")
    say("抓帧 %s  客户区原点(抓帧内)=(%d,%d)" % (arr.shape, coff[0], coff[1]))
    fold, full = find(arr, TMPL_FOLD), find(arr, TMPL_FULL)
    if not full or full[3] < 0.8:
        if not fold or fold[3] < 0.5:
            say("背包没开（折叠锚 agree=%s）→ 按 i" % ("%.3f" % fold[3] if fold else "None"))
            tap(0x49, 0x17)  # VK_I
            arr, coff = grab(r"E:/project/_r27_live_before.png")
            fold, full = find(arr, TMPL_FOLD), find(arr, TMPL_FULL)
    say("折叠态锚 backgrnd -> %s" % (fold,))
    say("展开态锚 Full_left400 -> %s" % (full,))
    if not fold or fold[3] < 0.5:
        say("!! 仍找不到背包窗口：这张帧判不了，需要人工确认是否已在游戏内")
        return 2
    if full and full[3] > 0.8:
        say("已经是展开态（agree=%.3f）→ 再点一次收回，从折叠态重新展开" % full[3])
        return 0

    ax, ay = fold[0], fold[1]
    say("窗口左上角(抓帧)=(%d,%d) agree=%.3f rmse=%.1f" % (ax, ay, fold[3], fold[2]))
    # BtFull 位图原点 = 窗口相对 (W-51,4)=(124,4)。原生折叠帧上量到它的红色图标本体占
    # 窗口 x=126..135 / y=7..16（从原点内缩 2px、10x10），整颗按钮约 20x20 ⇒ 中心 (134,14)。
    for wx, wy in ((134, 14), (130, 11), (140, 17), (126, 7), (134, 8)):
        cx, cy = ax + wx - coff[0], ay + wy - coff[1]
        rc, msg = click_client(cx, cy)
        say("点客户区 (%d,%d) = 窗口相对 (%d,%d)  [%d] %s" % (cx, cy, wx, wy, rc, msg))
        if rc:
            return rc
        arr2, _ = grab(r"E:/project/_r27_live_after.png")
        f2, o2 = find(arr2, TMPL_FULL), find(arr2, TMPL_FOLD)
        say("   -> 展开锚 agree=%s  折叠锚 agree=%s"
            % ("%.3f" % f2[3] if f2 else "None", "%.3f" % o2[3] if o2 else "None"))
        if f2 and f2[3] > 0.8:
            say("   展开成功")
            break
        if not o2 or o2[3] < 0.5:
            say("   窗口不见了（可能点成了关闭）→ 按 i 再开，然后停止")
            tap(0x49, 0x17)
            return 3
    with open(r"E:/project/_r27_expand.txt", "w", encoding="utf-8") as fh:
        fh.write("\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
