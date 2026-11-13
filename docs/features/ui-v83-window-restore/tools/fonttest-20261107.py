# -*- coding: utf-8 -*-
"""决定性实验：cwd=仓库根（有 fonts/SourceHanSerifSC），XY_DATA_DIR 指部署 Data。
与 00:33 回归唯一差别 = cwd 字体齐不齐。
  值出现  -> 根因 = 部署缺思源宋体（R28-B5 换字体没同步装机目录）
  仍空白  -> 根因在代码（statlabels 通道），再上诊断落盘
"""
import ctypes
import ctypes.wintypes as wt
import importlib.util
import os
import shutil
import subprocess
import sys
import time

REPO = r'E:\project\MapleStory-Client'
EXE = r'E:\MXD\BeiDou-Client-xy\MapleStory.exe'
DATA_DIR = r'E:\MXD\BeiDou-Client-xy\Data'
ALT_CFG = r'E:\MXD\ms-alt-b53\xy-config.yaml'   # xiaoye 测试号（口令不落仓库，只临时拷贝）
TMP_CFG = os.path.join(REPO, 'xy-config.yaml.fonttest')
OUT_DIR = os.path.join(REPO, 'docs', 'features', 'ui-v83-window-restore', 'evidence', 'regress-20261107')
CAP2 = 'E:/project/_cap2.py'

LOG = open(OUT_DIR + '/_fonttest.txt', 'w', encoding='utf-8')


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


def title_of(hwnd):
    n = user32.GetWindowTextLengthW(hwnd)
    buf = ctypes.create_unicode_buffer(n + 1)
    user32.GetWindowTextW(hwnd, buf, n + 1)
    return buf.value


# 临时配置：auto=true 的 xiaoye 号
src = open(ALT_CFG, encoding='utf-8', errors='replace').read()
open(TMP_CFG, 'w', encoding='utf-8').write(src.replace('auto: false', 'auto: true', 1))

env = dict(os.environ, XY_DATA_DIR=DATA_DIR)
proc = subprocess.Popen([EXE], cwd=REPO, env=env)
log('pid=%d cwd=REPO(fonts ok)' % proc.pid)

hwnd = None
t0 = time.time()
while time.time() - t0 < 60:
    time.sleep(1.0)
    h, t = hwnd_of_pid(proc.pid)
    if h:
        hwnd = h
        break
if not hwnd:
    os.remove(TMP_CFG)
    raise SystemExit('NO WINDOW')

in_game = False
t0 = time.time()
while time.time() - t0 < 180:
    time.sleep(2.0)
    if '\u5728\u7ebf\u65f6\u957f' in title_of(hwnd):
        in_game = True
        break
if not in_game:
    subprocess.run([sys.executable, CAP2, OUT_DIR + '/fonttest_not_ingame.png', str(hwnd)], capture_output=True)
    os.remove(TMP_CFG)
    proc.terminate()
    LOG.close()
    raise SystemExit('NOT IN GAME')

log('in game')
time.sleep(3.0)


def key(name):
    if not d.foreground(hwnd):
        log('WARN fg')
    time.sleep(0.15)
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.06)
    d.send_key(scan, ext, True)
    time.sleep(0.3)


def cap(tag):
    out = OUT_DIR + ('/fonttest_%s.png' % tag)
    subprocess.run([sys.executable, CAP2, out, str(hwnd)], capture_output=True)
    log('cap %s' % tag)


key('scrolllock')
time.sleep(0.5)
cap('01_game')

key('s')          # 能力值窗（statlabels 数值行）
time.sleep(1.2)
cap('02_stats')
key('escape')
time.sleep(0.5)

key('i')          # 道具窗（金币）
time.sleep(1.2)
cap('03_inv')
key('escape')

proc.terminate()
time.sleep(3)
if proc.poll() is None:
    proc.kill()
os.remove(TMP_CFG)
log('tmp cfg removed')
LOG.close()
print('DONE')
