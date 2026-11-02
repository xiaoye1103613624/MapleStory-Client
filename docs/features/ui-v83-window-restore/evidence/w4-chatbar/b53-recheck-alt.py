# -*- coding: utf-8 -*-
"""R27-B5-3 修后复验（第二账号并行实例）。

背景：并发的另一个会话在跑同机客户端（本机另一测试号），同账号双开会互相踢线，
所以修后复验只能借本机第二个测试号起一个**独立实例**。

隔离做法（不动别人的运行目录与配置）：
  * 工作目录 = E:/MXD/ms-alt-b53（那里有自己的 xy-config.yaml，登录段指向第二测试号）
    —— 口令只落在这个「仓库外」的部署副本里，驱动脚本与证据文件都不含口令。
  * 资源目录 = XY_DATA_DIR 环境变量指回原部署的 Data（资源定位第一优先级，见
    Util/Resource/ResourceLocator.cpp:83），无需复制 9 GB。
  * 窗口句柄按 **pid** 绑定（两台实例标题都含 XiaoYE，find_hwnd 会锚错窗，
    这是项目记忆 live-client-verification-gotchas 里反复踩过的坑）。

跑法：
    python b53-recheck-alt.py
产物：E:/project/_ms_probe/alt/_b53alt_*.png + _b53alt.txt（日志）
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import subprocess
import sys
import time

ALT_DIR = r'E:\MXD\ms-alt-b53'
EXE = r'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
DATA_DIR = r'E:\MXD\BeiDou-Client-xy\Data'
OUT_DIR = r'E:\project\_ms_probe\alt'
os.makedirs(OUT_DIR, exist_ok=True)

LOG = open(OUT_DIR + '/_b53alt.txt', 'w', encoding='utf-8')


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
    """按进程绑定窗口：两个实例标题相同，只能靠 pid 区分。"""
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


def title_of(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


log('launching second instance from %s' % ALT_DIR)
env = dict(os.environ, XY_DATA_DIR=DATA_DIR)
proc = subprocess.Popen([EXE], cwd=ALT_DIR, env=env)
log('pid=%d' % proc.pid)

hwnd = None
t0 = time.time()
while time.time() - t0 < 60:
    time.sleep(1.0)
    h, t = hwnd_of_pid(proc.pid)
    if h:
        hwnd = h
        log('hwnd=%d title=%s' % (hwnd, t.encode('gbk', 'replace').decode('gbk')))
        break
if not hwnd:
    raise SystemExit('NO WINDOW for pid %d' % proc.pid)

# 等自动登录 -> 选服 -> 选角 -> 进图。哨兵用标题里的「【在线时长：…」段：
# 这一段只在 Stage 有角色（cid>0）时才出现（IO/Window.cpp build_title）。
# 第一版误用「标题含 xiaoye」当哨兵 —— 基础标题本就是 XiaoYE，登录页也会命中，
# 结果在选角界面就开始打键（实录 _b53alt.txt 第 5 行只有【当前时间】段），已纠正。
in_game = False
t0 = time.time()
last = ''
while time.time() - t0 < 150:
    time.sleep(2.0)
    t = title_of(hwnd)
    if t != last:
        log('title: %s' % t.encode('gbk', 'replace').decode('gbk'))
        last = t
    if '\u5728\u7ebf\u65f6\u957f' in t:
        in_game = True
        break
    if not in_game and int(time.time() - t0) % 30 == 0:
        subprocess.run([sys.executable, 'E:/project/_cap2.py',
                        OUT_DIR + '/_b53alt_wait%d.png' % int(time.time() - t0), str(hwnd)],
                       capture_output=True)
if not in_game:
    log('ABORT: 150s 内未进图（可能没有角色 / 登录失败），未发送任何输入')
    LOG.close()
    raise SystemExit('NOT IN GAME')

time.sleep(3.0)
ox, oy = d.client_origin(hwnd)
log('client_origin=(%d,%d)' % (ox, oy))

WX, WY = 500, 80  # UIMessenger 硬编码 position，view 坐标


def cap(tag):
    if not d.foreground(hwnd):
        log('WARN cap %s: 抢不到前台' % tag)
    out = OUT_DIR + '/_b53alt_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)
    return out


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
    log('click %s view=(%d,%d)' % (why, vx - WX, vy - WY))


def post_esc():
    """ESC 走 PostMessage：SDL 对 ESC 的处理在收不到前台输入时更稳（_postkey 同路）。"""
    lparam_down = 0x00000001
    lparam_up = 0xC0000001
    user32.PostMessageW(hwnd, WM_KEYDOWN, 0x1B, lparam_down)
    time.sleep(0.05)
    user32.PostMessageW(hwnd, WM_KEYUP, 0x1B, lparam_up)
    log('post ESC -> hwnd=%d' % hwnd)


cap('01_baseline')
# 开窗键：本实例的工作目录是 E:\MXD\ms-alt-b53，那里没有 Settings 文件，
# 快捷键取代码默认 —— UIKeyConfig.h:220 把 MAPLECHAT 绑在 **C** 上。
# （主运行目录 E:\MXD\BeiDou-Client-xy 的 Settings 里存着 V，先前几轮按 V 能开；
#   换运行目录就换键 —— 「键位是用户态存档」，不是功能坏了。）
OPEN_KEY = os.environ.get('R27_OPENKEY', 'c')
log('open key = %s' % OPEN_KEY)
key(OPEN_KEY)
time.sleep(1.2)
cap('02_open')

# 点四枚空名牌 -> 走 prompt_invite 分支，和真收消息同一条 append_chat/draw 路径
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

post_esc()
time.sleep(1.0)
cap('05_after_esc')
log('done pid=%d hwnd=%d' % (proc.pid, hwnd))
LOG.close()
