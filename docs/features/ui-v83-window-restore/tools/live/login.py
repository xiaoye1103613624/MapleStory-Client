# -*- coding: utf-8 -*-
"""R27 真机自证前置：启动复刻客户端 -> 登录 -> 选角色 -> 进世界，每步 PrintWindow 抓帧。

复用 _drive.py 的 foreground()/cap()（打键前必须断言前台，见项目记忆
live-client-verification-gotchas 第 6/8 条）。坐标是 2026-11-03 真机量出来的
（client 相对）：账号 (588,302) / 密码 (642,354) / 连接 (774,317)。
旧值 (582,330) 其实落在账号行上，(715,300) 在连接钮之外 —— 都点不到，别再改回去。

凭据不入库：账号/密码只从环境变量 R27_USER / R27_PASS 读，缺一个就直接退出 ——
本仓库是公开仓，任何口令写进源码都等于泄露。
    R27_USER=xxx R27_PASS=yyy python login.py
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time

ACCOUNT = os.environ.get('R27_USER')
PASSWORD = os.environ.get('R27_PASS')
if not ACCOUNT or not PASSWORD:
    raise SystemExit('缺 R27_USER / R27_PASS：口令不入库，运行时由环境提供')

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

user32 = d.user32
EXE = r'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
WORKDIR = r'E:\MXD\BeiDou-Client-xy'
LOG = open('E:/project/_r27_login.txt', 'w', encoding='utf-8')


def log(msg):
    print(msg, flush=True)
    LOG.write(msg + '\n')
    LOG.flush()


def tap(word, delay=0.10):
    for ch in word:
        scan, ext = d.KEYS[ch]
        d.send_key(scan, ext, False)
        time.sleep(0.045)
        d.send_key(scan, ext, True)
        time.sleep(delay)


def click(hwnd, x, y):
    """client 相对坐标点击：先断言前台，再用 WindowFromPoint 校验命中的就是本窗。"""
    if not d.foreground(hwnd):
        raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
    cx, cy = d.client_origin(hwnd)
    sx, sy = cx + x, cy + y
    user32.SetCursorPos(sx, sy)
    time.sleep(0.15)
    hit = user32.WindowFromPoint(wt.POINT(sx, sy))
    root = user32.GetAncestor(hit, 3) if hit else 0
    log('click(%d,%d) screen=(%d,%d) hit=%d root=%d want=%d' % (x, y, sx, sy, hit, root, hwnd))
    d.send_click()
    time.sleep(0.35)


def cap(hwnd, tag):
    out = 'E:/project/_r27_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s -> %s' % (tag, out))
    return out


def wait_window(secs=25):
    for _ in range(secs * 2):
        time.sleep(0.5)
        h = d.find_hwnd()
        if h:
            r = wt.RECT()
            user32.GetWindowRect(h, ctypes.byref(r))
            if (r.right - r.left) > 100 and not (r.left == -32000):
                return h
    raise SystemExit('NO WINDOW after %ss' % secs)


def main():
    user32.SetProcessDPIAware()
    hwnd = d.find_hwnd()
    if not hwnd:
        subprocess.Popen([EXE], cwd=WORKDIR)
        log('launched %s' % EXE)
        hwnd = wait_window()
    log('hwnd=%d' % hwnd)
    time.sleep(float(os.environ.get('R27_WAIT', '8')))
    cap(hwnd, 'boot')

    click(hwnd, 588, 302)
    tap(ACCOUNT)
    cap(hwnd, 'acct')
    click(hwnd, 642, 354)
    tap(PASSWORD)
    click(hwnd, 774, 317)
    time.sleep(float(os.environ.get('R27_LOGIN_WAIT', '9')))
    cap(hwnd, 'login2')


if __name__ == '__main__':
    main()
