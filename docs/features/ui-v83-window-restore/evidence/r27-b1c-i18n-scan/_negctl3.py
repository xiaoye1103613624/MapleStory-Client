# -*- coding: utf-8 -*-
"""R27-B1c 运行时负对照 v3（仪器判据版）。

判据链：xy-config 改 ui.language -> 主循环 reload_if_changed()（MapleStory.cpp:240，
每帧 stat mtime）-> Locale::reload()（Configuration.cpp:551）-> ++gen ->
UIState 派发点 -> UIElement::refresh_localized() -> LocaleBindings::refresh()
-> 已开着的窗逐条 change_text(新代次文案)。

仪器：构建 worktree 垫片把 refresh 落 _refresh_debug.log（Release 下 LOG 是 no-op，
台账 §6 判据「Locale refresh gen=」在正式构建里根本不存在 —— 这也是本轮坐实的新事实）。
沙箱 en-US.ini 的 version.prefix 预置标记值 LANGEN {}（zh 侧保持 Ver. {}），
⇒ 切换后仪器行必须同时给出 gen 跳变 与 version.prefix=LANGEN {}，缺一即负对照不成立。
"""
import subprocess, os, sys, time, ctypes, ctypes.wintypes as wt
import numpy as np
from PIL import Image

SB = r'E:\project\_b1c_negctl_sandbox'
CAP = [sys.executable, r'E:\project\_cap_screen.py']
u = ctypes.windll.user32

def kill_all_msh():
    r = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq MapleStory.exe'],
                       capture_output=True, text=True)
    for ln in r.stdout.splitlines():
        parts = ln.split()
        if len(parts) > 1 and parts[0] == 'MapleStory.exe':
            pid = parts[1]
            # 只杀沙箱实例：正式端 34476 是用户进程，绝不碰
            if pid == '34476':
                continue
            subprocess.run(['taskkill', '/F', '/PID', pid], capture_output=True)

def find_hwnd(pid):
    out = []
    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(h, l):
        p = wt.DWORD()
        u.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and u.IsWindowVisible(h):
            r = wt.RECT()
            u.GetWindowRect(h, ctypes.byref(r))
            if r.right - r.left > 100:
                out.append(h)
        return True
    u.EnumWindows(cb, 0)
    return out[0] if out else None

def grab(hwnd, out):
    subprocess.run(CAP + [out, str(hwnd)], capture_output=True)
    return np.asarray(Image.open(out).convert('RGB')).astype(int)

def set_lang(lang):
    p = os.path.join(SB, 'xy-config.yaml')
    s = open(p, encoding='utf-8').read()
    for cur in ('zh-CN', 'en-US'):
        if 'language: %s' % cur in s:
            s = s.replace('language: %s' % cur, 'language: ' + lang, 1)
    open(p, 'w', encoding='utf-8', newline='\n').write(s)

# 0) 部署新 exe + 清场
kill_all_msh()
time.sleep(1)
subprocess.run(['copy', '/y', os.path.join(r'E:\project\_msh_b1c7_out', 'MapleStory.exe'),
                os.path.join(SB, 'MapleStory.exe')], shell=True)
for f in ('_refresh_debug.log', '_init_debug.log'):
    fp = os.path.join(SB, f)
    if os.path.exists(fp):
        os.remove(fp)
set_lang('zh-CN')

# 1) 分离启动
logf = open(os.path.join(SB, 'detached4.log'), 'wb')
pr = subprocess.Popen([os.path.join(SB, 'MapleStory.exe')], stdout=logf,
                      stderr=subprocess.STDOUT, cwd=SB,
                      creationflags=0x00000008 | 0x00000200, close_fds=True)
print('PID=%d' % pr.pid)

# 2) 等窗口 + 等 init:done（主循环没起来之前，reload_if_changed 永远不会跑 —— 上一轮就栽在这）
hwnd = None
t0 = time.time()
while time.time() - t0 < 60:
    hwnd = find_hwnd(pr.pid)
    if hwnd:
        break
    time.sleep(1)
if not hwnd:
    print('NO-WINDOW'); sys.exit(2)
print('HWND=%s (+%.0fs)' % (hex(hwnd), time.time() - t0))

idb = os.path.join(SB, '_init_debug.log')
t0 = time.time()
done = False
while time.time() - t0 < 240:
    if os.path.exists(idb) and 'init:done' in open(idb, encoding='utf-8', errors='replace').read():
        done = True
        break
    time.sleep(2)
print('init:done=%s (+%.0fs)' % (done, time.time() - t0))
if not done:
    print('INIT-NOT-DONE'); sys.exit(4)
time.sleep(6)  # 登录屏首帧

prev = grab(hwnd, os.path.join(SB, 'w0.png'))
stable = 0
t0 = time.time()
while time.time() - t0 < 150:
    time.sleep(2)
    cur = grab(hwnd, os.path.join(SB, 'w1.png'))
    d = np.abs(cur - prev).mean()
    if d < 1.0:
        stable += 1
        if stable >= 2:
            break
    else:
        stable = 0
    prev = cur
else:
    print('NEVER-STABLE'); sys.exit(3)
a = grab(hwnd, os.path.join(SB, 'shot_baseline.png'))
print('baseline stable at +%.0fs mean=%.1f' % (time.time() - t0, a.mean()))

# 3) 切语言
set_lang('en-US')
print('switched -> en-US at %s' % time.strftime('%H:%M:%S'))
rf = os.path.join(SB, '_refresh_debug.log')
t0 = time.time()
while time.time() - t0 < 15:
    if os.path.exists(rf):
        break
    time.sleep(1)
time.sleep(2)
b = grab(hwnd, os.path.join(SB, 'shot_after.png'))

# 4) 读仪器
print('=== _refresh_debug.log ===')
if os.path.exists(rf):
    print(open(rf, encoding='utf-8', errors='replace').read().strip())
else:
    print('(不存在 ⇒ 机制未触发，负对照失败)')
idb = os.path.join(SB, '_init_debug.log')
print('=== _init_debug.log 尾 ===')
if os.path.exists(idb):
    print('\n'.join(open(idb, encoding='utf-8', errors='replace').read().splitlines()[-4:]))

# 5) 收尾：杀掉沙箱实例（正式端 34476 不碰），语言复位
kill_all_msh()
print('done')
