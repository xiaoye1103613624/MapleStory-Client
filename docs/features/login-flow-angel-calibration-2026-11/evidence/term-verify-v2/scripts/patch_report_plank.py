# -*- coding: utf-8 -*-
"""蓝军翻案 2 落报告：§9.1 把 UIRaceSelect(职业团)帧错配成创建页 + 漏读 classic 分支，
误判「创建页缺侧栏」。用 verify_left_plank_pages.py 五页逐块 NCC 纠正。"""
p = r'E:\project\MapleStory-Client\docs\features\login-flow-angel-calibration-2026-11\TERM_VERIFY_REPORT.md'
raw = open(p, 'r', encoding='utf-8', newline='').read()
NL = '\r\n' if '\r\n' in raw else '\n'
def L(s): return s.replace('\n', NL)
def rep(old, new):
    global raw
    o, n = L(old), L(new)
    c = raw.count(o)
    assert c == 1, 'REPLACE FAIL count=%d for: %s' % (c, old[:60])
    raw = raw.replace(o, n, 1)

# §9.1 审计段整段翻案
rep(
'**本端落地审计**（左侧栏 ROI(0,0,160,470) gray NCC）：登录 **0.9995** / 频道收起+展开 **0.8717**(edge 0.7436) / 选角色 **0.4192**(edge 0.3757) / 创建页 **0.5156**(edge 0.4132)。逐行暗像素密度木牌 y 区段：选角色页本端 `[]`（**整栏没渲染**）vs 目标 `[[36,85],[88,94],[99,148],[377,399],[427,449]]`；创建页本端仅 `[[151,168],[387,413],[445,469]]`（缺顶部标题牌+世界信息条）。源码坐实：`UIWorldSelect.cpp:158-163,306-324` 用 `WorldSelect/BtViewChoice`+`BtViewAll`+`Common/BtStart` 真资产；**`UICharSelect.cpp` 搜侧栏按钮 0 命中**（只有 `BtCharacter`(:1308) / `BtPreview`(:1316)，无 `BtStart`、无标题牌、无世界信息条）→ 选角色页整栏缺失是**本端没实现，不是位置漂**。`UIRaceSelect.cpp:156` / `UICommonCreation.cpp:456` 只构造了 `BtStart`（回到首页）这一块。',
'**本端落地审计（蓝军翻案 2，[`verdicts/left_plank_pages.json`](evidence/term-verify-v2/verdicts/left_plank_pages.json) / `scripts/verify_left_plank_pages.py`）**：上一版 `left_plank_audit.json` 把**职业团页帧 `racecreate_click_f06.png`（UIRaceSelect）错配成「创建页」**，且只读了 `UICommonCreation.cpp:456` 漏掉 classic 分支，误判「创建页缺顶部标题牌+世界条」。按每页各自正确的本端帧↔目标帧重算 ROI(0,0,160,470) 逐块零均值 NCC：\n\n| 页 | 本端类 | 本端帧 ↔ 目标帧 | strip NCC | 4 块逐块(标题/信息条/查看所有角色/回到首页) | 判定 |\n| --- | --- | --- | --- | --- | --- |\n| 登录 | UILogin | `v2_login_typed` ↔ `desktop_login` | **0.9995** | 1.0 / 1.0 / 1.0 / 1.0 | 4 块齐 |\n| 频道展开 | UIWorldSelect | `v2_channel_grid` ↔ `desktop_channel_expanded` | **0.8717** | 0.9997 / 0.8953 / 0.5931 / 0.8915 | 4 块齐 |\n| 选角色 | UICharSelect | `v2_racecreate_settled` ↔ `desktop_char_select` | **0.4192** | −0.266 / −0.201 / −0.208 / −0.450 | **整栏没实现** |\n| 职业团 | UIRaceSelect | `scene_reset_1_race_before` ↔ `desktop_racecreate_race` | **0.5156** | −0.180 / −0.207 / −0.154 / −0.048 | **3 块缺**(只 BtStart) |\n| 建角冒险家 | UICommonCreation(classic) | `scene_reset_2_creation` ↔ `desktop_racecreate_adventurer` | **0.9575** | 1.0 / 0.8062 / 1.0 / 0.6501 | **4 块齐** |\n\n源码坐实：`UIWorldSelect.cpp:158-163,306-324` 三钮真资产；**`UICharSelect.cpp` 搜侧栏按钮 0 命中**（只有 `BtCharacter`(:1308)/`BtPreview`(:1316)）→ 选角色整栏缺失；`UIRaceSelect.cpp:156/:314` 只 `BtStart` → 职业团页缺 3 块；**`UICommonCreation.cpp:456` BtStart(0,427) + `:490` version_pos=(1061,9) + `:492` step_create + `:493` selworld_box + `:511-513` world/channel_text + `:530-534` BtViewAll + `:960-963` draw() = classic 分支 4 块全套**（`blame 6d835880` xiaoye 11-05 15:33，早于本报告），门禁 `UICommonCreation.cpp:79 use_classic_creation()` → `Util/LoginUiAssets.cpp:71-74 return !has_customize_char()`，真机帧证 classic 生效。→ **「创建页缺侧栏」判词作废**；未实现的是选角色页与职业团页，不是建角页。')

# §9.6 判词补区分
rep(
'选角色页整栏本端没实现（只有主区木牌与 BtCharacter/BtPreview）；职业团「选择」进各职业建角页并换专属亭子背景，「回到首页」滚回选世界。',
'选角色页整栏本端没实现（只有主区木牌与 BtCharacter/BtPreview）、职业团页(UIRaceSelect)侧栏 3 块也缺，但**建角页(UICommonCreation classic)侧栏 4 块齐 strip 0.9575**（§9.1 蓝军翻案 2 纠正旧误判）；「选择」进各职业建角页并换专属亭子背景，「回到首页」滚回选世界。')

open(p, 'w', encoding='utf-8', newline='').write(raw)
print('PATCH OK bytes=', len(raw.encode('utf-8')))
