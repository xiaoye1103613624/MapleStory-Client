# -*- coding: utf-8 -*-
"""离线复验：拿本目录已归档的四帧，把两把尺各跑一遍（不拉起客户端）。

    python b6-recheck-offline.py
产物：_b6_recheck.txt（同一段文本也打印）

留这个脚本的原因：真机驱动（b6-menu-anchor-alt.py）依赖另一个测试号的运行目录与在线会话，
复查时不一定拉得起；这四帧 + 本脚本就能把「锚定」结论重新算一遍，判据一字不差。
"""
import io
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
CK = os.path.join(HERE, 'b6-menu-anchor-check.py')

# (说明, 参数, 期望退出码) —— 0=PASS 1=MISSING(未开) 2=FAIL(开了但落位错)
# 蓝钮行法：帧 颗数 期望x 期望y 首颗y 底板高；剪影法：基线帧 气球帧 期望x 期望y 底板高
LATTICE = [
    ('目录 开', ['frame_02_menu_open.png', '4', '666', '545', '22', '140'], 0),
    ('基线 未开（阴性：气球没开时必须认不出）', ['frame_01_baseline.png', '4', '666', '545', '22', '140'], 1),
    ('ESC 后（阴性：关了同上）', ['frame_04_after_esc.png', '4', '666', '545', '22', '140'], 1),
    ('界面 开', ['frame_03_shortcut_open.png', '8', '722', '440', '22', '245'], 0),
    ('ESC 后按界面尺（阴性）', ['frame_04_after_esc.png', '8', '722', '440', '22', '245'], 1),
]
TAIL = [
    ('目录 开', ['frame_01_baseline.png', 'frame_02_menu_open.png', '666', '545', '140'], 0),
    ('界面 开', ['frame_01_baseline.png', 'frame_03_shortcut_open.png', '722', '440', '245'], 0),
    ('ESC 后（阴性对照）', ['frame_01_baseline.png', 'frame_04_after_esc.png', '666', '545', '140'], 1),
]

out = io.StringIO()
bad = []


def run(title, args, expect):
    # 只有帧名是相对本目录的路径，其余是数字参数 —— 别把 '4' 拼成 <目录>/4
    paths = [os.path.join(HERE, a) if a.endswith('.png') else a for a in args]
    r = subprocess.run([sys.executable, CK] + paths, capture_output=True)
    text = r.stdout.decode('gbk', 'replace').strip()
    if r.returncode not in (0, 1, 2):
        text += '\n[stderr] ' + r.stderr.decode('gbk', 'replace').strip()
    tag = 'OK  ' if r.returncode == expect else 'MISMATCH'
    if r.returncode != expect:
        bad.append(title)
    out.write('### [%s] %s（期望 exit=%d）\n%s\n[exit=%d]\n\n'
              % (tag, title, expect, text, r.returncode))


for t, a, e in LATTICE:
    run('蓝钮行尺 / ' + t, a, e)
for t, a, e in TAIL:
    run('剪影尺 / ' + t, ['--tail'] + a, e)

text = out.getvalue()
with open(os.path.join(HERE, '_b6_recheck.txt'), 'w', encoding='utf-8') as f:
    f.write(text)
# stdout 是 cp936，中文照打会抛 UnicodeEncodeError —— 只打 ASCII 摘要，全文看文件
print('wrote _b6_recheck.txt: PASS=%d cases=%d mismatch=%d %s'
      % (text.count('VERDICT: PASS'), len(LATTICE) + len(TAIL), len(bad),
         'ALL-AS-EXPECTED' if not bad else 'CHECK ' + str([b for b in bad])))
