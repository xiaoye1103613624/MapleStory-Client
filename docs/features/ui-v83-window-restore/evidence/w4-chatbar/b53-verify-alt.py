# -*- coding: utf-8 -*-
"""R27-B5-3 修后复验读数（第二账号实例帧）。

判据与改前那轮 _b53_verify2.py 完全同一把尺子，只是这次量的是修好后的帧：
  1) ENTER 橙徽标 bbox —— 目标窗口坐标 x252..279（原生折算值，改前实测 250..277）。
  2) 聊天历史 4 行墨迹行段 —— 目标 y240..249 / 254..263 / 268..277 / 282..291。
  3) ESC 后窗体是否真消失 —— 用「窗口矩形内 backgrnd 顶边框行暗像素」判定，
     顶边框是满幅 295px 的黑线，场景里不会自己长出来（改前基线 295）。
"""
import io
import os
import numpy as np
from PIL import Image

SRC = r'E:\project\_ms_probe\alt'
# 帧名前缀：b53-alt-messenger.py 产出 _b53ms_*（第二账号实例的聊天窗专项复验）
PREFIX = os.environ.get('R27_PREFIX', '_b53ms')
OUT = io.open(SRC + r'\%s_verify.txt' % PREFIX, 'w', encoding='utf-8')
BORDER = (8, 31)
WX, WY = 500, 80


def w(m):
    OUT.write(str(m) + '\n')


def load(tag):
    # 驱动里开窗帧名带重试后缀（02_open_tryN），这里两个名字都认
    for cand in (tag, tag + '_try0'):
        p = '%s/%s_%s.png' % (SRC, PREFIX, cand)
        if os.path.exists(p):
            a = np.array(Image.open(p).convert('RGB'))
            return a[WY + BORDER[1]:WY + BORDER[1] + 364,
                     WX + BORDER[0]:WX + BORDER[0] + 295, :3].astype(int)
    raise FileNotFoundError('%s_%s.png' % (PREFIX, tag))


def orange(p):
    return int(p[0]) > 190 and 90 < int(p[1]) < 190 and int(p[2]) < 110


def bbox(a, pred, x0, x1, y0, y1):
    xs, ys = [], []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if pred(a[y, x]):
                xs.append(x)
                ys.append(y)
    return None if not xs else (min(xs), max(xs), min(ys), max(ys))


def fmt(bb):
    return 'None' if bb is None else 'x %d..%d  y %d..%d' % bb


def dark(a):
    return (a[:, :, 0] < 90) & (a[:, :, 1] < 90) & (a[:, :, 2] < 90)


def white(a):
    return (a[:, :, 0] > 180) & (a[:, :, 1] > 180) & (a[:, :, 2] > 180)


def ink_rowseg(a, x0, x1, y0, y1, th=60, minn=3):
    hits = []
    for y in range(y0, y1):
        row = a[y, x0:x1]
        med = np.median(row, axis=0)
        # 括号位置就是判据本身：先数「偏离中位 >th 的像素个数」，再和 minn 比。
        # 写成 sum(dev) > th 会让每行都恒真（本仓早期脚本踩过，一度把「已出字」读成空）。
        hits.append((y, int(np.sum(np.max(np.abs(row - med), axis=1) > th))))
    seg, cur = [], None
    for y, n in hits:
        if n >= minn:
            cur = [y, y] if cur is None else [cur[0], y]
        elif cur:
            seg.append(tuple(cur))
            cur = None
    if cur:
        seg.append(tuple(cur))
    return seg


openf = load('02_open')
lines = load('03_four_lines')
typed = load('04_typed')
esc = load('05_after_esc')
base = load('01_baseline')
wbase = int(white(base).sum())

# 新手村这张图整片是黑的，「顶边框行满幅暗像素」在这种场景里恒等于 295，
# 不能当「窗还在不在」的判据（上一版就是这么误判的）。改用白底计数：
# backgrnd 里有两块大白框（历史区 + 输入槽），窗体在 = 七万量级，窗体没了 = 三百量级。
# 判据必须**对着本实例自己的无窗基线**比，不能写死绝对值（换张地图基线就变了）。
w('0) 白底像素：baseline=%d  开窗=%d  4行=%d  打字后=%d  ESC后=%d'
  % (wbase, int(white(openf).sum()), int(white(lines).sum()),
     int(white(typed).sum()), int(white(esc).sum())))
w('1) ENTER 橙徽标 ours = %s   原生折算目标 = x 252..279  y 328..332   （改前实测 x 250..277）'
  % fmt(bbox(typed, orange, 230, 295, 310, 345)))
w('2) 白框内墨迹行段（点 4 次空名牌后）= %s' % ink_rowseg(lines, 12, 288, 236, 312))
w('   白框内墨迹行段（打字后）        = %s' % ink_rowseg(typed, 12, 288, 236, 312))
w('   目标 = [(240,249),(254,263),(268,277),(282,291)]')
w('   输入槽墨迹行段（打字后）= %s   目标 = 约 (318,325)' % ink_rowseg(typed, 12, 240, 314, 336))
d0 = dark(esc)
w('3) ESC 后（黑场景下仅作旁证）：顶边框行 y0 暗像素 = %d  输入槽黑沿 y317 = %d'
  % (int(d0[0].sum()), int(d0[317].sum())))
we = int(white(esc).sum())
w('   判定：ESC 收窗 %s（白底 %d -> %d，无窗基线 %d）'
  % ('PASS（窗体消失）' if we <= wbase + 60 else 'FAIL（窗体仍在）',
     int(white(typed).sum()), we, wbase))
bb = bbox(typed, orange, 230, 295, 310, 345)
w('   判定：BtEnter x 归零 %s' % ('PASS' if bb and bb[0] == 252 and bb[1] == 279 else 'FAIL/需人工核对'))
seg = ink_rowseg(lines, 12, 288, 236, 312)
w('   判定：聊天历史出字 %s（%d 个行段）'
  % ('PASS' if len(seg) >= 4 else 'FAIL/需人工核对', len(seg)))
OUT.close()
print('ok')
