# -*- coding: utf-8 -*-
"""R27-B5-3 复验 · 第二步：挂到已在图内的第二账号实例，跑聊天窗三项断言。

b53-recheck-alt.py 负责起实例 + 登录进图；这一支只做「V 开窗 -> 点四枚空名牌灌
4 行 -> 点输入槽打字 -> ESC」，句柄按 pid 绑定（两实例标题同为 XiaoYE，
find_hwnd 会锚到并发会话那个窗，绝不能用它）。

跑法：
    python b53-alt-messenger.py <pid>
产物：E:/project/_ms_probe/alt/_b53ms_*.png + _b53ms.txt
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time

PID = int(sys.argv[1])
# 开窗键：本实例的工作目录是 E:\MXD\ms-alt-b53，那里没有 Settings 文件，
# 快捷键取代码默认 —— UIKeyConfig.h:220 把 MAPLECHAT 绑在 **C** 上。
# （主运行目录 E:\MXD\BeiDou-Client-xy 的 Settings 里存着 V，先前几轮按 V 能开；
#   换运行目录就换键，这是「键位是用户态存档」的坑，不是功能坏了。）
OPEN_KEY = os.environ.get('R27_OPENKEY', 'c')
OUT_DIR = r'E:\project\_ms_probe\alt'
os.makedirs(OUT_DIR, exist_ok=True)
# 帧名前缀：默认 _b53ms（05:31 那轮，实例缺 fonts/ 目录 → 全窗无文字，只当过程证据）；
# 补上 fonts/ 后重跑用 _b53ms2，那一轮才是「出字」的正式判据。
PREFIX = os.environ.get('R27_PREFIX', '_b53ms')

LOG = open(OUT_DIR + '/%s.txt' % PREFIX, 'w', encoding='utf-8')


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

user32 = d.user32
user32.SetProcessDPIAware()

WM_KEYDOWN = 0x0100
WM_KEYUP = 0x0101


def hwnd_of_pid(pid):
    found = []
    proc = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)

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
            found.append((hwnd, buf.value))
        return True

    user32.EnumWindows(proc(cb), 0)
    return found[0] if found else (None, None)


h, t = hwnd_of_pid(PID)
if not h:
    raise SystemExit('NO WINDOW for pid %d' % PID)
hwnd = h
log('pid=%d hwnd=%d title=%s' % (PID, hwnd, t.encode('gbk', 'replace').decode('gbk')))

if '\u5728\u7ebf\u65f6\u957f' not in t:
    raise SystemExit('ABORT: 该实例不在图内，未发送任何输入')

if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')

ox, oy = d.client_origin(hwnd)
log('client_origin=(%d,%d)' % (ox, oy))

WX, WY = 500, 80  # UIMessenger 硬编码 position（view 坐标）
BORDER = (8, 31)  # 窗口 rect -> 客户区：capture = window_rect.top_left + BORDER（实测）


def cap(tag):
    if not d.foreground(hwnd):
        log('WARN cap %s: 抢不到前台' % tag)
    out = OUT_DIR + '/%s_%s.png' % (PREFIX, tag)
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)
    return out


def white_in_window(path):
    """窗口矩形 (500,80)+295x364 里的白底像素数 —— 聊天窗开没开的硬判据。
    backgrnd 有两块大白框，窗体在 = 七万量级，窗体没了 = 个位/百位量级。
    """
    import numpy as np
    from PIL import Image
    a = np.array(Image.open(path).convert('RGB')).astype(int)
    c = a[WY + BORDER[1]:WY + BORDER[1] + 364,
          WX + BORDER[0]:WX + BORDER[0] + 295, :3]
    return int((c > 180).all(axis=2).sum())


def fg(step):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s: 抢不到前台，未发送输入' % step)


def key(name):
    fg('key ' + name)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.3)


def click(vx, vy, why):
    fg('click ' + why)
    user32.SetCursorPos(ox + vx, oy + vy + 3)
    time.sleep(0.18)
    user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.18)
    d.send_click()
    time.sleep(0.4)
    log('click %s window=(%d,%d)' % (why, vx - WX, vy - WY))


def open_messenger():
    """按 OPEN_KEY 开聊天窗，并用白底计数确认「真的开了」。
    上一轮 _b53alt：进图后 3s 就发第一个键，C 没生效（白底与无窗基线相同），
    但随后打「hello」里的 E 却开出了装备窗 —— 通道是通的，只是那一下丢了。
    所以这里不信任单次打键，开了再断言，没开就补打（最多 3 次）。
    """
    for i in range(3):
        key(OPEN_KEY)
        time.sleep(1.0)
        out = cap('02_open_try%d' % i)
        n = white_in_window(out)
        log('open try %d -> white=%d' % (i, n))
        if n > 5000:
            return True
        time.sleep(0.8)
    return False


cap('01_baseline')
log('open key = %s' % OPEN_KEY)
if not open_messenger():
    log('ABORT: 3 次打键后聊天窗仍未出现，后续步骤未执行')
    LOG.close()
    raise SystemExit('MESSENGER NOT OPENED')

# 点四枚空名牌 -> prompt_invite 分支，和真收消息同一条 append_chat/draw 路径
for x in (58, 147, 235, 147):
    click(WX + x, WY + 117, 'namebar@%d' % x)
    time.sleep(0.3)
cap('03_four_lines')

click(WX + 60, WY + 325, 'input')
for ch in 'hello':
    fg('type')
    scan, ext = d.KEYS[ch]
    d.send_key(scan, ext, False)
    time.sleep(0.045)
    d.send_key(scan, ext, True)
    time.sleep(0.06)
time.sleep(0.4)
user32.SetCursorPos(ox + 900, oy + 300)
time.sleep(0.3)
cap('04_typed')

user32.PostMessageW(hwnd, WM_KEYDOWN, 0x1B, 0x00000001)
time.sleep(0.05)
user32.PostMessageW(hwnd, WM_KEYUP, 0x1B, 0xC0000001)
log('post ESC -> hwnd=%d' % hwnd)
time.sleep(1.0)
cap('05_after_esc')
log('done')
LOG.close()
