# -*- coding: utf-8 -*-
"""R27-B5-3 UIMessenger（聊天窗）真机自证驱动。

链路：登录进图 -> 按 V（MAPLECHAT）开聊天窗 -> 抓基线帧 -> 点输入槽打字 ->
回车发送 -> 再抓帧。每步都断言前台（抢不到就中止，不往别的窗口发键），
PrintWindow 帧交给 b53-verify.py 逐格断言。

坐标口径：view = capture - (8, 31)（evidence/w4-chatbar/coords-fold.md）。
"""
import importlib.util
import re
import subprocess
import sys
import time

CLIENT_DIR = r'E:\MXD\BeiDou-Client-xy'

raw = open(CLIENT_DIR + r'\xy-config.yaml', 'rb').read()
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
assert len(vals) == 2, 'config login keys not found'
# 口令只在本进程内存里活，长度进日志，明文不落盘。
print('creds len user=%d pass=%d' % (len(vals['account']), len(vals['pass'])))

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv
d.user32.SetProcessDPIAware()

LOG = open('E:/project/_ms_probe/_b53_live.txt', 'w', encoding='utf-8')
TAG = sys.argv[0]


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


def cap(tag):
    out = 'E:/project/_b53_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s -> %s' % (tag, out))


env = dict(__import__('os').environ, R27_USER=vals['account'], R27_PASS=vals['pass'])
r = subprocess.run([sys.executable,
                    r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\tools\live\login.py'],
                   env=env, capture_output=True, text=True)
log('login rc=%d tail=%s' % (r.returncode, (r.stdout or '')[-200:].replace('\n', ' | ')))
# 进图头几秒 PrintWindow 是全黑帧，等首帧 HUD 出齐（B5-1 实测 12s）。
time.sleep(12)

hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
ox, oy = d.client_origin(hwnd)
log('hwnd=%d client_origin=(%d,%d)' % (hwnd, ox, oy))

cap('01_baseline')


def must_fg(step):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s: 抢不到前台' % step)


def tap_key(name, count=1):
    must_fg('key ' + name)
    scan, ext = d.KEYS[name]
    for _ in range(count):
        d.send_key(scan, ext, False)
        time.sleep(0.06)
        d.send_key(scan, ext, True)
        time.sleep(0.2)


def click(vx, vy):
    must_fg('click')
    d.user32.SetCursorPos(ox + vx, oy + vy + 3)
    time.sleep(0.18)
    d.user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.18)
    d.send_click()
    time.sleep(0.35)


# 2) 开聊天窗：V = MAPLECHAT
tap_key('v')
time.sleep(1.2)
cap('02_messenger')

# 3) 点输入槽正中（窗口左上 view(500,80)，输入内沿 x12..240 / y318..332）
click(500 + 60, 80 + 325)
cap('03_afterclick')

# 4) 灌三条消息，验证历史行只落在白框 y240..301 里
for line in ('abc', 'hello', 'msg3'):
    for ch in line:
        must_fg('type')
        scan, ext = d.KEYS[ch]
        d.send_key(scan, ext, False)
        time.sleep(0.04)
        d.send_key(scan, ext, True)
        time.sleep(0.05)
    subprocess.run([sys.executable, 'E:/project/_postkey.py', 'enter', '1'],
                   capture_output=True)
    time.sleep(0.8)
cap('04_three_lines')

# 5) 再按 V 关窗（toggle_active -> leave_room）
tap_key('v')
time.sleep(1.0)
cap('05_closed')

log('done')
LOG.close()
