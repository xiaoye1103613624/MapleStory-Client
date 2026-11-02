# -*- coding: utf-8 -*-
"""R27-B5 收起态常量复验（第 7 版）：登录进图 -> 发一条消息 -> 抓收起帧，
交给 _ms_probe/_fold_verify.py 按原生 oracle 逐格断言。

Enter/ESC 一律走 _postkey.py（PostMessage VK_*）：SendInput scancode=28 的 Enter
进不了这个 SDL 客户端（字母键可以），这是驱动侧的限制，不是客户端缺陷。
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
LOG = open('E:/project/_r27_fold7.txt', 'w', encoding='utf-8')


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


def cap(tag):
    out = 'E:/project/_r27_f7_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)


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
cap('folded')      # 进图默认收起态：灰带 + 最近一条
post('return')
tap('hi')
cap('typed')
post('return')     # 发送 -> 回灰带，正文为 Admin : hi
cap('sent')
log('DONE')
