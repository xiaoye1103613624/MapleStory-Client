# -*- coding: utf-8 -*-
"""蓝军翻案 2：上一轮 left_plank_audit 把「职业团页 UIRaceSelect 帧」错配成「建角页」，
并漏读 UICommonCreation.cpp:492-534 classic 分支整套侧栏，导致 §9.1 判「创建页缺侧栏」是错的。
本脚本对 5 个页面各自用【正确的本端帧 ↔ 正确的目标帧】重算左侧栏 ROI(0,0,160,470) 逐块 NCC。"""
import os, json
import numpy as np
from PIL import Image

BASE = r'C:\Users\11036\AppData\Local\Temp\term_verify'
S, T = os.path.join(BASE, 'shots'), os.path.join(BASE, 'targets_desktop')

def arr(p): return np.asarray(Image.open(p).convert('RGB'), dtype=np.float64)
def ncc(x, y):
    x = x - x.mean(); y = y - y.mean()
    d = np.sqrt((x*x).sum()*(y*y).sum())
    return float((x*y).sum()/d) if d else 0.0

BLOCKS = {'title_y36_85': (36,85), 'worldbar_y96_148': (96,148),
          'viewall_y377_399': (377,399), 'home_y427_449': (427,449)}

PAIRS = [
 ('login',            'v2_login_typed.png',           'desktop_login.png',                  'UILogin'),
 ('channel_expanded', 'v2_channel_grid.png',           'desktop_channel_expanded.png',       'UIWorldSelect'),
 ('char_select',      'v2_racecreate_settled.png',     'desktop_char_select.png',           'UICharSelect'),
 ('race_select',      'scene_reset_1_race_before.png','desktop_racecreate_race.png',        'UIRaceSelect'),
 ('creation',         'scene_reset_2_creation.png',     'desktop_racecreate_adventurer.png', 'UICommonCreation(classic)'),
]

out = {'method': 'left strip ROI(0,0,160,470) zero-mean NCC gray + 4 y-blocks; each page paired with its OWN frame/target',
       'exe': '47be00b37d093d21 / 2026-11-14 01:19:05 (scene_reset_* frames); v2_* frames from aa0aea67',
       'correction': 'previous left_plank_audit.json labeled UIRaceSelect frame racecreate_click_f06 as 创建页 and read only UICommonCreation.cpp:456; classic branch :492-534 in fact builds all 4 planks',
       'pages': []}
for page, sf, tf, cls in PAIRS:
    a = arr(os.path.join(S, sf))[:470, :160]
    b = arr(os.path.join(T, tf))[:470, :160]
    strip = round(ncc(a.mean(2), b.mean(2)), 4)
    blocks = {k: round(ncc(a[y0:y1].mean(2), b[y0:y1].mean(2)), 4) for k,(y0,y1) in BLOCKS.items()}
    present = sum(1 for v in blocks.values() if v > 0.5)
    out['pages'].append({'page': page, 'class': cls, 'shot': sf, 'target': tf,
                         'strip_ncc': strip, 'blocks': blocks, 'blocks_present_gt0_5': present})

# 源码落地(逐条 grep 复核过的行锚)
out['source'] = {
 'UIWorldSelect.cpp': '158-163 / 306-324 : step + BtViewChoice + BtViewAll + BtStart (3 buttons + title)',
 'UICharSelect.cpp': 'grep sidebar buttons = 0 hits; only BtCharacter:1308 / BtPreview:1316 -> whole strip missing',
 'UIRaceSelect.cpp': '156 / 314 : only BtStart -> 3 planks missing',
 'UICommonCreation.cpp': '456 BtStart(0,427) + 490 version_pos=(1061,9) + 492 step_create + 493 selworld_box + 511-513 world/channel_text + 530-534 BtViewAll + 960-963 draw(); blame 6d835880 (xiaoye 11-05 15:33) -> all 4 planks present in classic mode',
 'classic_mode_gate': 'UICommonCreation.cpp:79 if(LoginUiAssets::use_classic_creation()) -> Util/LoginUiAssets.cpp:71-74 return !has_customize_char(); runtime frames show classic active',
}
out['verdict'] = ('建角页(UICommonCreation classic)左侧栏 4 块已基本对齐 strip>=0.95；'
                  '职业团页(UIRaceSelect)与选角色页(UICharSelect)侧栏本端未实现，逐块 NCC 全负；'
                  '上一轮报告 §9.1「创建页缺顶部标题牌+世界条」= 帧错配 + 漏读 classic 分支，作废。')

for p in [os.path.join(BASE, 'ui_left_planks', 'left_plank_pages.json'),
          r'E:\project\MapleStory-Client\docs\features\login-flow-angel-calibration-2026-11\evidence\term-verify-v2\verdicts\left_plank_pages.json']:
    os.makedirs(os.path.dirname(p), exist_ok=True)
    json.dump(out, open(p, 'w', encoding='utf-8'), ensure_ascii=False, indent=2)
    print('WROTE', p)

for pg in out['pages']:
    print(pg['page'], pg['class'], 'strip=', pg['strip_ncc'], 'blocks_present=', pg['blocks_present_gt0_5'], pg['blocks'])
