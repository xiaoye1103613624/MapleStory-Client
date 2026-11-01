# -*- coding: utf-8 -*-
"""R27-B1b 真机自证：在已进世界的客户端上开背包(I)/技能(K)窗并 PrintWindow 抓帧。

打键前断言前台（见项目记忆 live-client-verification-gotchas 第 6/8 条），抢不到就中止。
用法：python _r27_open.py i k
"""
import importlib.util
import subprocess
import sys
import time

spec = importlib.util.spec_from_file_location('_drive', 'E:/project/_drive.py')
d = importlib.util.module_from_spec(spec)
argv = sys.argv
sys.argv = ['_drive']
spec.loader.exec_module(d)
sys.argv = argv

d.user32.SetProcessDPIAware()
hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')

for name in argv[1:]:
    scan, ext = d.KEYS[name]
    d.send_key(scan, ext, False)
    time.sleep(0.05)
    d.send_key(scan, ext, True)
    time.sleep(1.2)
    out = 'E:/project/_r27_win_%s.png' % name
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    print('key %s -> %s' % (name, out), flush=True)
