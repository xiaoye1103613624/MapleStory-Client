# 083 键位布局复用（2026-11-19，用户「相同键位数快捷键窗用一样键位图」）

## 用户问题
192 键位序列哪里来？不同快捷键 UI 有不同键位；相同键位数的窗能否复用键位图？→ 查 083 键位布局，同构复用。

## 取证（逐条证据）
1. **192 端键位表**：IDA `CUIKeyConfig::GetShortCutPos@0x1396510` → `s_ptShortKeyPos_0[nIdx]`（**145 项**坐标表 @0x27c5978）。静态读前30项全 -1（运行时填充），无法直接还原内容。192 分卷 UI_004.wz 有 `UIWindow2.img/KeyConfig/quickslotConfig/key/{2..12…}` 位图贴纸、`KeyConfig/icon/*`，但**无静态绑定数据**（search_node ShortCut/HotKey/Default 空）。证据脚本 tools/_find_keycfg.py、tools/_keycfg_tree.py。
2. **083 端键位表（权威）**：源码 `IO/UITypes/UIStatusBar.cpp:127-134` `CLASSIC_QS_KEYS[26]`=13列×2行（源自 Angel）：
   - 上行 Shift/Ins/Hm/Pup/1/2/3/4/5/A/S/D/F
   - 下行 Ctrl/Del/End/Pdn/Q/W/E/R/T/Z/X/C/V
   配套贴纸 `CLASSIC_QS_KEYCAP[26]`(:157-161) + 文字 `CLASSIC_QS_KEYNAME[26]`(:163-166)：Shift/Ins/Hm/Pup/Ctrl/Del/End=key/0-6，数字 FontMemo 位图，字母描边文字。
3. **同构判定**：192 2行×10列=20 槽，与 083 前20 同构（都是 pitch35 网格的两行布局）。用户裁决「相同键位数复用键位图」→ 成立。

## 修复
`MODERN_QS_KEYS[20]`（:149-155）从手拍错版改为 083 `CLASSIC_QS_KEYS` 前20：
- [0-3] Shift/Ins/Hm/Pup, [4-8] NUM1-5, [9-12] A/S/D/F, [13-16] Ctrl/Del/End/Pdn, [17-19] Q/W/E。
键帽绘制沿用 `classic_qs_cap[cap_id]`（StatusBar3 KeyConfig/key 位图）+ 短名回退，与 083 同源。

## 验证
BUILD_EXITCODE=0；live SHA `9f51e4d1ab4270650c0f0a732bead0bb9ca126b5fe6eddfa867861012f8f8510`（实时核对）；截图 tools/_v9_qs.png：下行 S/D/F→Ctrl/Del/End/Pdn→Q/W 序列正确；上行前8槽走玩家实际绑定（Keyboard::quickslots，设计如此，非 MODERN_QS_KEYS 覆盖）。

## 残留（如实）
- 上行前8格显示玩家当前绑定（Shift/Ins/Hm/Pup/Ctrl/Del/End/Pdn），不是 083 表前8；这是「前8槽优先走 keyboard.get_quickslot_key」的既有设计，若要强制全表显示需去掉该优先逻辑（等用户裁决）。
- 图标与键帽顶条仍有轻微重叠（键帽本就在格顶小条）；图标主体已在格内。
