# -*- coding: utf-8 -*-
"""B5-3 真机补测（客户端已在图、聊天窗已开）：

  07) 点空名牌 -> prompt_invite() 走「不在房内」分支，append_chat 落一行历史，
      用来量聊天文字到底落在白框 y240..301 的哪一段（服务端没建房，没法靠
      真消息灌行，这一行走的是同一条 append_chat + draw 路径）。
  08) 连点四次 -> 4 行，验证 CHAT_LOG_VISIBLE=4 不溢出到输入槽。
  09) 点输入槽并打字 -> 量光标列与字形顶。
  10) 按 ESC -> 关窗（回归）。

坐标：view = capture - (8, 31)；聊天窗左上 view=(500,80)。
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

LOG = open('E:/project/_ms_probe/_b53_step2.txt', 'w', encoding='utf-8')


def log(m):
    LOG.write(str(m) + '\n')
    LOG.flush()


hwnd = d.find_hwnd()
if not hwnd:
    raise SystemExit('NO WINDOW')
if not d.foreground(hwnd):
    raise SystemExit('ABORT: 抢不到前台，未发送任何输入')
ox, oy = d.client_origin(hwnd)
log('hwnd=%d client_origin=(%d,%d)' % (hwnd, ox, oy))

WX, WY = 500, 80


def cap(tag):
    out = 'E:/project/_b53_%s.png' % tag
    subprocess.run([sys.executable, 'E:/project/_cap2.py', out, str(hwnd)],
                   check=True, capture_output=True)
    log('cap %s' % tag)


def click(vx, vy, why):
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @%s' % why)
    d.user32.SetCursorPos(ox + vx, oy + vy + 3)
    time.sleep(0.18)
    d.user32.SetCursorPos(ox + vx, oy + vy)
    time.sleep(0.18)
    d.send_click()
    time.sleep(0.4)
    log('click %s view=(%d,%d)' % (why, vx, vy))


# 07) 点第一枚空名牌（窗口内 14..102 / 108..127，取中心）
click(WX + 58, WY + 117, 'namebar0')
cap('07_one_line')

# 08) 再点三枚，凑四行
for x in (147, 235, 147):
    click(WX + x, WY + 117, 'namebar@%d' % x)
    time.sleep(0.35)
cap('08_four_lines')

# 09) 点输入槽并打字（黑框内沿 y318..332，取 y=325）
click(WX + 60, WY + 325, 'input slot')
for ch in 'hello':
    if not d.foreground(hwnd):
        raise SystemExit('ABORT @type')
    scan, ext = d.KEYS[ch]
    d.send_key(scan, ext, False)
    time.sleep(0.045)
    d.send_key(scan, ext, True)
    time.sleep(0.06)
time.sleep(0.4)
cap('09_typed')

# 10) ESC 关窗
subprocess.run([sys.executable, 'E:/project/_postkey.py', 'esc', '1'], capture_output=True)
time.sleep(0.9)
cap('10_after_esc')
log('done')
LOG.close()
