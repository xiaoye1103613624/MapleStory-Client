# -*- coding: utf-8 -*-
"""round52 · K1 批量零风险项回归（drop/D7、drop/D4、statusbar-hud/D5、corewindow/D5）

四个决策点全部来自跨面 D 矩阵（docs/features/angel-calibration-d-matrix-2026-11/README.md）
的 K1 分桶（纯文档 / 纯日志 / 未初始化变量护栏，零协议零行为风险）。

本 harness 做两件事：
  1) 静态守卫：源码里必须出现「修后」形态，且不得残留「修前」形态与 095 线地址；
  2) 行为复刻：把一个最小的 Drop 构造/图标选择模型跑一遍，断言未知 mode 不再留下
     未初始化 state、meso 四档选图与 083 sub_506BFE 逐档一致。
"""
import io, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def read(p):
    return io.open(os.path.join(ROOT, p), encoding='utf-8', errors='replace').read()

FAIL = []

def check(name, cond, detail=''):
    if not cond:
        FAIL.append('%s %s' % (name, detail))
    return cond

# ───────────────────────── 1. 静态守卫 ─────────────────────────
drop_cpp = read('Gameplay/MapleMap/Drop.cpp')
mapdrops = read('Gameplay/MapleMap/MapDrops.cpp')
statusbar = read('IO/UITypes/UIStatusBar.cpp')
statsinfo = read('IO/UITypes/UIStatsInfo.cpp')
iteminv_cpp = read('IO/UITypes/UIItemInventory.cpp')
iteminv_h = read('IO/UITypes/UIItemInventory.h')
invhand = read('Net/Handlers/InventoryHandlers.cpp')

# drop/D7 —— switch(mode) 必须有 default 且落到 DROPPED
sw = drop_cpp[drop_cpp.find('switch (mode)'):]
sw = sw[:sw.find('}\n\t}') + 3] if '}\n\t}' in sw else sw[:2000]
check('d7_default_present', 'default:' in sw, 'switch(mode) 缺 default')
check('d7_default_assigns_state', re.search(r'default:[\s\S]{0,900}?state = Drop::State::DROPPED;', sw) is not None)
check('d7_still_three_cases', sw.count('case 0:') == 1 and sw.count('case 2:') == 1 and sw.count('case 3:') == 1)
# 合法 mode 的四个赋值点原样保留（逐字等价）
for frag in ['basey = static_cast<double>(dest.y() - 4);',
             'phobj.hspeed = static_cast<double>(dest.x() - start.x()) / 48;',
             'phobj.type = PhysicsObject::Type::FIXATED;']:
    check('d7_keep_%d' % len(frag), frag in drop_cpp, frag[:40])

# drop/D4 —— 缺 ItemData 时必须有 DEBUG 日志，且不再静默
check('d4_has_else', re.search(r'drops\.add\(spawn\.instantiate\(icon\)\);\s*\}\s*else\s*\{', mapdrops) is not None)
check('d4_log_line', re.search(r'else\s*\{[\s\S]{0,600}?LOG\(LOG_DEBUG, "MapDrops: no ItemData for itemid="', mapdrops) is not None)
check('d4_include_maplemstory', '#include "../../MapleStory.h"' in mapdrops)
check('d4_include_iostream', '#include <iostream>' in mapdrops)
check('d4_no_behavior_change', mapdrops.count('drops.add(spawn.instantiate(icon));') == 2)

# statusbar-hud/D5 —— 1006/1009，且不得再出现 2005/2006
check('sb_d5_has_1006', '1006/(768,515)' in statusbar)
check('sb_d5_has_1009', '1009/(573,515)' in statusbar)
check('sb_d5_no_2005', '2005/(768,515)' not in statusbar)
check('sb_d5_no_2006', '2006/(573,515)' not in statusbar)
check('sb_d5_cites_083', '0x8d01b2' in statusbar)

# corewindow/D5 —— 4 处 095 依据换成 083 EA
check('cw_083_cuistat', '0x8c4c7f' in statsinfo)
check('cw_083_cuiitem_create', '0x81c6c9' in iteminv_cpp)
check('cw_083_garonotify_0x81d01f', '0x81d01f' in iteminv_cpp)
check('cw_083_setarrange_0x81e3a6', '0x81e3a6' in iteminv_cpp)
check('cw_083_setarrange_header', '0x81e3a6' in iteminv_h)
check('cw_083_gather_result', '0xa1e943' in invhand)
check('cw_083_sort_result', '0xa1e96d' in invhand)

BAD_095 = ['0x867b90', '0x7CCC90', '0x7CE520', '0x7CC4F0', '0x9F1280', '0x9F12B0', 'Maplestory95']
for f, txt in [('UIStatsInfo.cpp', statsinfo), ('UIItemInventory.cpp', iteminv_cpp),
               ('UIItemInventory.h', iteminv_h), ('InventoryHandlers.cpp', invhand)]:
    for bad in BAD_095:
        check('no095_%s_%s' % (f, bad), bad not in txt, '%s 仍含 %s' % (f, bad))

# ───────────────────────── 2. 行为复刻 ─────────────────────────
MESO_TIERS = [  # 083 sub_506BFE @0x506bfe：<50→0 / 50..99→1 / 100..999→2 / >=1000→3
    (0, 0), (49, 0), (50, 1), (99, 1), (100, 2), (999, 2), (1000, 3), (10 ** 9, 3),
]
def ange_tier(meso):
    if meso < 50:
        return 0
    if meso >= 100:
        return (1 if meso >= 1000 else 0) + 2
    return 1

def client_tier(meso):
    return 3 if meso > 999 else 2 if meso > 99 else 1 if meso > 49 else 0

for meso, want in MESO_TIERS:
    check('tier_%d' % meso, ange_tier(meso) == want == client_tier(meso),
          'angel=%d client=%d want=%d' % (ange_tier(meso), client_tier(meso), want))

# Drop 构造模型：修前未知 mode → state 未初始化（模拟为 None）；修后 → DROPPED(0)
DROPPED, FLOATING, PICKEDUP = 0, 1, 2
def construct(mode, with_default):
    state = None
    if mode in (0, 1):
        state = DROPPED
    elif mode == 2:
        state = FLOATING
    elif mode == 3:
        state = PICKEDUP
    elif with_default:
        state = DROPPED
    return state

LEGAL = [0, 1, 2, 3]
for m in LEGAL:
    check('legal_mode_%d_equiv' % m, construct(m, False) == construct(m, True),
          'mode=%d 修前修后不等' % m)
for m in [4, 5, 7, 127, -1, -128]:
    check('illegal_mode_%d_uninit_before' % m, construct(m, False) is None)
    check('illegal_mode_%d_fixed' % m, construct(m, True) == DROPPED)

# ───────────────────────── 汇总 ─────────────────────────
total = 0
print('STATIC + BEHAVIOR GUARDS')
if FAIL:
    for f in FAIL:
        print('  FAIL', f)
    print('RESULT: %d failed' % len(FAIL))
    sys.exit(1)
print('PASS: K1 batch round52 (drop/D7 + drop/D4 + statusbar-hud/D5 + corewindow/D5)')
sys.exit(0)
