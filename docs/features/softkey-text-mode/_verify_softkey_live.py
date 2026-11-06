# -*- coding: utf-8 -*-
"""单人真机对拍：新 UISoftKey（UIWindow.img/SoftKeyboard 315x127 金色卷轴 QWERTY）渲染帧。

链路（xy-config login.auto=false，复用 b53-live-driver.py:64-69 的 login.py 手动登录进图）：
清残留 -> login.py 填账号/密码/连接进图（凭据走 R27_USER/R27_PASS 环境变量） ->
等 HUD -> 按 C 开 Messenger -> 点空 namebar(647,197)
触发 UIMessenger::emplace<UISoftKey>(Mode::Text) -> 抓帧 -> 点几个键帽验证四态。

对拍目标（用户 m00076 硬约束）：键帽必须整齐排列在 315x127 底板内、不漂出、
不散落盖住别的窗口；对照旧错位实锤帧 messenger-look/20261106_p7_admin_softkey.png
（黑色 Login.img 140x280 竖版 + 45px/12 列键帽散落全屏）。

坐标口径：click 用 client 相对坐标（d.client_origin(h) + view 坐标），同 p9:132-141；
UISoftKey.position=(216,224) 即 view 坐标，键位 KEY_X/KEY_Y 是 window-local，
故键 '1'(25,37) 的 view=(241,261)。BORDER=(8,31) 只用于整窗截图裁切(white_ms)，
点击坐标不叠加 BORDER。
凭据：本脚本不需要（启动即自动进图）；明文口令不入库。
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time
from pathlib import Path

REPO = Path(r'E:\project\MapleStory-Client')
OUT = REPO / 'docs/features/softkey-text-mode/evidence/live-20261106'
OUT.mkdir(parents=True, exist_ok=True)
PREFIX = 'v1'
CFG = r'E:/MXD/BeiDou-Client-xy/xy-config.yaml'
LOGIN_PY = str(REPO / 'docs/features/ui-v83-window-restore/tools/live/login.py')
CAP_PY = 'E:/project/_cap2.py'
DRIVE_PY = 'E:/project/_drive.py'

LOG = open(OUT / f'{PREFIX}_verify.txt', 'w', encoding='utf-8')


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


def read_creds():
    import re
    raw = open(CFG, 'rb').read()
    text = None
    for enc in ('utf-8', 'gbk'):
        try:
            text = raw.decode(enc)
            break
        except Exception:
            pass
    vals = {}
    sec = None
    for line in text.splitlines():
        m = re.match(r'^(\w+):', line)
        if m:
            sec = m.group(1)
        m2 = re.match(r'^\s+(account|pass)\s*:\s*(.+?)\s*$', line)
        if m2 and sec == 'login':
            vals[m2.group(1)] = m2.group(2).strip('"')
    assert len(vals) == 2, 'login creds not found'
    return vals['account'], vals['pass']


# --- load _drive.py helper ---
spec = importlib.util.spec_from_file_location('_drive', DRIVE_PY)
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv
user32 = d.user32
user32.SetProcessDPIAware()


def hwnd_of_pid(pid):
    found = []

    @ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)
    def cb(hwnd, lp):
        wpid = wt.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(wpid))
        if wpid.value != pid or not user32.IsWindowVisible(hwnd):
            return True
        n = user32.GetWindowTextLengthW(hwnd)
        if not n:
            return True
        buf = ctypes.create_unicode_buffer(n + 1)
        user32.GetWindowTextW(hwnd, buf, n + 1)
        r = wt.RECT()
        user32.GetWindowRect(hwnd, ctypes.byref(r))
        if 'XiaoYE' in buf.value and (r.right - r.left) > 100 and r.left != -32000:
            found.append(hwnd)
        return True

    user32.EnumWindows(cb, 0)
    return found[0] if found else None


def title_of(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


def wait_hwnd(pid, timeout=60):
    t0 = time.time()
    while time.time() - t0 < timeout:
        h = hwnd_of_pid(pid)
        if h:
            return h
        time.sleep(1)
    return None


def wait_ingame(hwnd, timeout=150):
    t0 = time.time()
    last = ''
    while time.time() - t0 < timeout:
        t = title_of(hwnd)
        if t != last:
            log(f'title: {t}')
            last = t
        if '在线时长' in t:
            return True, t
        time.sleep(2)
    return False, last


def white_ms(path):
    import numpy as np
    from PIL import Image
    a = np.array(Image.open(path).convert('RGB')).astype(int)
    WX, WY, BORDER = 500, 80, (8, 31)
    c = a[WY + BORDER[1]:WY + BORDER[1] + 364, WX + BORDER[0]:WX + BORDER[0] + 295]
    return int((c > 180).all(axis=2).sum())


def cap(hwnd, tag):
    d.foreground(hwnd)
    out = str(OUT / f'{PREFIX}_{tag}.png')
    subprocess.run([sys.executable, CAP_PY, out, str(int(hwnd))],
                   check=True, capture_output=True)
    w = white_ms(out)
    log(f'cap {tag} white={w} -> {out}')
    return out, w


def key(hwnd, name, pause=0.7):
    d.foreground(hwnd)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.05)
    d.send_key(scan, ext, True)
    time.sleep(pause)


def click(hwnd, vx, vy, why):
    d.foreground(hwnd)
    ox, oy = d.client_origin(hwnd)
    user32.SetCursorPos(ox + vx, oy + vy + 2)
    time.sleep(0.1)
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.1)
    d.send_click()
    time.sleep(0.35)
    log(f'click {why} ({vx},{vy})')


def main():
    acc, pw = read_creds()
    log(f'creds len user={len(acc)} pass={len(pw)}')

    # 1) 清残留（含 32K 僵尸），让 login.py 自己启动新编译的 16:45 exe
    subprocess.run(['taskkill', '/F', '/IM', 'MapleStory.exe'], capture_output=True)
    time.sleep(1.5)

    # 2) login.py 手动登录进图（账号(588,302) 密码(642,354) 连接(774,317)）
    env = dict(os.environ, R27_USER=acc, R27_PASS=pw, PYTHONIOENCODING='utf-8')
    r = subprocess.run([sys.executable, LOGIN_PY], env=env,
                       capture_output=True, timeout=120)
    tail = (r.stdout or b'').decode('utf-8', errors='replace')[-300:].replace(chr(10), ' | ')
    log(f'login rc={r.returncode} tail={tail}')

    # 3) 等窗口出现并进图（标题含「在线时长」），最多 90s；进图头几秒帧全黑再等 3s
    hwnd = None
    for _ in range(90):
        hwnd = d.find_hwnd()
        if hwnd and '在线时长' in title_of(hwnd):
            break
        time.sleep(1)
    if not hwnd:
        raise SystemExit('NO WINDOW')
    t = title_of(hwnd)
    log(f'ingame={"在线时长" in t} title={t}')
    if '在线时长' not in t:
        LOG.close()
        raise SystemExit('NOT_INGAME (可能停在选角/登录页)')
    time.sleep(3)

    if not d.foreground(hwnd):
        raise SystemExit('ABORT: 抢不到前台')
    ox, oy = d.client_origin(hwnd)
    log(f'hwnd={int(hwnd)} client_origin=({ox},{oy})')

    cap(hwnd, '00_ingame')

    # 3) 关掉可能残留的主菜单（点空地图，绝不按 Esc——会开 MainMenu）
    click(hwnd, 180, 520, 'dismiss_menu')

    # 4) 按 C 开 Messenger（white>50000 判定为真聊天窗，复用 p9）
    opened = False
    for i, k in enumerate(('c', 'c', 'v', 'c')):
        key(hwnd, k, pause=1.0)
        _, n = cap(hwnd, f'open_{k}{i}')
        if n > 50000:
            opened = True
            break
        if 3000 < n < 25000:
            click(hwnd, 180, 520, 'dismiss_wrong')
    if not opened:
        log('ABORT messenger not open')
        LOG.close()
        raise SystemExit(2)
    cap(hwnd, '01_messenger')

    # 5) 点空 namebar 触发 UIMessenger::invite -> emplace<UISoftKey>(Mode::Text)
    click(hwnd, 647, 197, 'namebar_invite')
    time.sleep(0.8)
    cap(hwnd, '02_softkey')

    # 6) 在新软键盘布局上点几个键帽，验证四态/命中
    #    view = position(216,224) + KEY_X/KEY_Y(window-local)，不叠加 BORDER
    #    键 '1'(25,37)->(241,261) 'Q'(48,60)->(264,284) '-'(255,37)->(471,261)
    #    CapsLock(20,83)->(236,307) Enter(263,83)->(479,307)
    click(hwnd, 241, 261, 'key_1')
    click(hwnd, 264, 284, 'key_Q')
    click(hwnd, 236, 307, 'capslock')
    click(hwnd, 471, 261, 'key_hyphen')
    time.sleep(0.5)
    cap(hwnd, '03_softkey_keys')

    log('DONE')
    LOG.close()


if __name__ == '__main__':
    main()
