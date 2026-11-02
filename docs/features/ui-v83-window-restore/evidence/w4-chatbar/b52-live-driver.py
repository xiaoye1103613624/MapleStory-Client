# -*- coding: utf-8 -*-
"""R27-B5-2 真机自证驱动：点开聊天目标列表 -> 选「悄悄话」-> 弹层输入 -> 确定回填。

坐标全部按 B5-1 已定版的客户端几何反推（见 evidence/w4-chatbar/coords.md）：
  底栏 position.y() = 653（BtMin 行内 (536,8) = 屏幕 (536,661) 同一把尺），
  chatTarget 框 = x 12..81 / y 657..677，目标列表向上贴框顶 = y 597..657，行高 15。
  弹层 266x121 居中 = (507, 299)，箭头列 x=242..259 / y=47..63（框内相对）。

Enter/ESC 走 _postkey.py（SendInput scancode=28 的 Enter 进不了这个 SDL 客户端）。
打键前必须断言前台（项目记忆 live-client-verification-gotchas）。
凭据只从 xy-config.yaml 运行时读，不写进任何输出。
"""
import importlib.util
import os
import re
import subprocess
import sys
import time

raw = open(r'E:\MXD\BeiDou-Client-xy\xy-config.yaml', 'rb').read()
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

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

d.user32.SetProcessDPIAware()
LOG = open('E:/project/_r27_b52.txt', 'w', encoding='utf-8')
WHISPER_NAME = 'test'


def log(m):
    print(m, flush=True)
    LOG.write(m + '\n')
    LOG.flush()


def tap(word, delay=0.13):
    for ch in word:
        scan, ext = d.KEYS[ch]
        d.send_key(scan, ext, False)
        time.sleep(0.045)
        d.send_key(scan, ext, True)
        time.sleep(delay)


def post(key, count=1):
    subprocess.run([sys.executable, 'E:/project/_postkey.py', key, str(count)],
                   capture_output=True)
    time.sleep(0.7)


def click(x, y, note=''):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
    cx, cy = d.client_origin(hwnd)
    sx, sy = cx + x, cy + y
    user32.SetCursorPos(sx, sy)
    time.sleep(0.15)
    hit = user32.WindowFromPoint(wt.POINT(sx, sy))
    log('click(%d,%d) %s screen=(%d,%d) hit=%d want=%d' % (x, y, note, sx, sy, hit, hwnd))
    d.send_click()
    time.sleep(0.4)


def cap(tag):
    out = 'E:/project/_r27_b52_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s -> %s' % (tag, out))
    return out


user32 = d.user32
import ctypes.wintypes as wt

env = dict(os.environ, R27_USER=vals['account'], R27_PASS=vals['pass'])
r = subprocess.run([sys.executable,
                    r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\tools\live\login.py'],
                   env=env, capture_output=True, text=True)
log('login rc=%d' % r.returncode)
time.sleep(6)

hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
log('hwnd=%d' % hwnd)

post('esc')
cap('01_inGame')

# 1) 开输入行（Enter）——收起态灰带里没有 chatTarget 框，必须先开
post('return')
cap('02_inputRow')

# 2) 点 chatTarget 框体（x 12..81 / y 657..677 取 (46,666)）-> 目标列表向上展开
click(46, 666, 'chatTarget box')
cap('03_targetList')

# 3) 点第二行「悄悄话」：列表顶 y=597，行高 15 -> 行心 y=597+15*1+7=619
click(46, 619, 'row whisper')
cap('04_whisperDlg')

# 4) 弹层里直接打名字（open() 已把焦点给输入框）
tap(WHISPER_NAME)
cap('05_named')

# 5) 点下拉箭头（框内 x 242..259 / y 47..63；弹层左上 = (507,299)）
click(507 + 250, 299 + 55, 'combo arrow')
cap('06_dlgList')

# 6) ESC 收列表（列表开着时 ESC 只退一级），再回车 = 确定 -> 回填聊天栏
post('esc')
cap('07_listClosed')
post('return')
cap('08_backfilled')

# 7) 再点一次 chatTarget 框：标签应已变成「悄悄话」，且列表重新展开
click(46, 666, 'chatTarget box again')
cap('09_label_is_whisper')
log('DONE')
