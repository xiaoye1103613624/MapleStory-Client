# -*- coding: utf-8 -*-
rep=r'E:\project\MapleStory-Client\docs\features\login-flow-angel-calibration-2026-11\TERM_VERIFY_REPORT.md'
raw=open(rep,'rb').read()
assert raw[:3]!=b'\xef\xbb\xbf' and raw.count(b'\r\n')==0
assert b'### 15.9' in raw and b'### 15.10' not in raw
sec=('''
### 15.10 追加项：「结束游戏」木牌归属取证与移除（m08836，真机复验 R4b）

- 用户问：频道页左上角的「结束游戏」木牌原生到底在哪个页面。**78 张正确端截图全量扫描**（左上 (9,8,160,36) 暗块 frac）：登录界面 0.029 / 选择频道页 0.032 / 选择角色页 0.046 / 职业团·建角页族 0.044~0.050 —— 登录链路全部**无**此牌；进入游戏页 0.592 / 商城 1.000 / S-E-I-K 0.603 / 番茄田三帧 0.593 / 射手村地图 0.591 —— **in-game 页面全部有**。结论：`Common/BtExit` 木牌是**进图后页面专属组件**，频道页不该画它。
- 本端旧代码 `UIWorldSelect.cpp:305` `MapleButton(Common["BtExit"], WS_EXIT_POS(8,8))`，:163 注释自称出自正确端截图——实测该截图左上角 frac 仅 0.032，**注释系错引，证伪**。R4 后帧该区 frac 0.546 vs 目标 0.350，多出的暗块 bbox x9..159 y10..35 即木牌本体（`compare/cmp_topleft_exit_plank.png` 目视锁定）。
- 落刀：删构造行与 `WS_EXIT_POS` 常量（均留注释记因）；**ESC→退出确认键位行为保留**——`send_key` escape 分支 `button_pressed(Buttons::BtExit)`（:1139）与 `button_pressed` 的 `BtExit→emplace<UIQuitConfirm>`（:1470）是纯 id 分发，不解引用按钮实例，删除绘制不影响键位。
- 重编 R4b：worktree 三隔离干净构建 → **exe `a1d405eab0fe8776cf448d4197a1c388d528d8b2dc2d497c4203833688c36696` / 5,119,488 B / 2026-11-15 23:35:04**，`ERROR_COUNT=0`；live exe 未覆盖（别会话 23:30 产物保持原样）。
- 真机复验（同一护栏链）：`VERIFY_RC=0`、`AUTO_LOGIN_RESTORE_OK=True`、`R4_CHANNEL_FIX_VERIFIED` **16/16**，R4 四点数值不退化（`scroll_bottom 0.8006 / scroll_full 0.7662 / body_weighted 14.46`，三带 x_shift 全 0）。
- **专项收敛**：左上角 (0,0,150,60) frac **0.546 → 0.381**（目标 0.381，精确同值）；plank 带 (9,8,160,36) **0.546 前值的牌区 → 0.032 = 目标 0.032**，千分位精确同分；`compare/cmp_topleft_exit_plank_after.png` 目视两端只剩标题牌本体、无残影。
- 残差口径不变：扩展翻页箭头仍为本服功能件；§11.7 五项不签继续挂；m08338 属性板整轮拆出。
''').encode('utf-8')
open(rep,'ab').write(sec)
print('APPENDED',len(sec))
