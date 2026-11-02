# -*- coding: utf-8 -*-
"""R27-B5-3 复验：BtEnter 补回 2px + 输入框聚焦时 ESC 收窗。

跑法同 b53-live-driver（登录进图 -> V 开窗 -> 点空名牌灌 4 行 -> 点输入槽打字 ->
ESC），差别是这次把 ESC 断言做成两帧：打字后一帧、ESC 后一帧，交给 _b53_verify2.py
量 ENTER 徽标 x 与窗体是否消失。
"""
import importlib.util
import os
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
print('creds len user=%d pass=%d' % (len(vals['account']), len(vals['pass'])))

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv
d.user32.SetProcessDPIAware()

LOG = open('E:/project/_ms_probe/_b53_v2.txt', 'w', encoding='utf-8')


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


env = dict(os.environ, R27_USER=vals['account'], R27_PASS=vals['pass'])
r = subprocess.run([sys.executable,
                    r'E:\project\MapleStory-Client\docs\features\ui-v83-window-restore\tools\live\login.py'],
                   env=env, capture_output=True, text=True)
log('login rc=%d' % r.returncode)
time.sleep(12)

hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
ox, oy = d.client_origin(hwnd)
log('hwnd=%d client_origin=(%d,%d)' % (hwnd, ox, oy))

WX, WY = 500, 80


def cap(tag):
    out = 'E:/project/_b53v2_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)


def fg(step):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s' % step)


def click(vx, vy, why):
    fg('click ' + why)
    d.user32.SetCursorPos(ox + vx, oy + vy + 3)
    time.sleep(0.18)
    d.user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.18)
    d.send_click()
    time.sleep(0.4)
    log('click %s view=(%d,%d)' % (why, vx, vy))


def key(name):
    fg('key ' + name)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.25)


cap('01_baseline')
key('v')
time.sleep(1.2)
cap('02_open')

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
# 鼠标移开，免得光标压住输入槽/ENTER 徽标影响逐格读数
d.user32.SetCursorPos(ox + 900, oy + 300)
time.sleep(0.3)
cap('04_typed')

subprocess.run([sys.executable, 'E:/project/_postkey.py', 'esc', '1'], capture_output=True)
time.sleep(1.0)
cap('05_after_esc')
log('done')
LOG.close()
