# 快捷键统一按 083 表显示（2026-11-19，用户「去掉」前8槽优先绑定）

## 用户指令
上一轮问「上行前8格仍走玩家实际绑定，要不要去掉让20格全按表显示？」→ 用户答「去掉」。

## 修改（逐行）
- IO/UITypes/UIStatusBar.cpp:2382-2386（refresh_modern_qs_icons）：`maple_key` 从 `(i<8)?get_quickslot_key(i):MODERN_QS_KEYS[i]` 改为恒 `MODERN_QS_KEYS[i]`。
- IO/UITypes/UIStatusBar.cpp:2461-2465（draw_modern_quickslot 键帽）：`mk` 同上改为恒 `MODERN_QS_KEYS[i]`。
- 删除 draw_modern_quickslot 内不再使用的 `const Keyboard& keyboard`（原:2438）。
- **未动** classic 分支的 `classic_qs_count==8 && i<8` 门闩（:2550/2640/... 等），083 classic 快捷栏绑定行为不变。

## 效果
20 格键位标签统一按 083 同构表（Shift/Ins/Hm/Pup/1-5/A S D F/Ctrl/Del/End/Pdn/Q W E R T Z...）显示，不再随玩家可改绑定变化。图标内容仍来自 `get_maple_mapping(key)` 的实际技能/道具绑定——这是快捷栏数据，属正常。

## 验证
BUILD_EXITCODE=0；live SHA `a60881b35d09a67604f179dbd815580437d8a4fb634faca1602b3aecce94052c`（实时核对）；截图 tools/_v10_qs.png：键帽 Shift/Ins/Hm/Pup/Ctrl/Del/End/Pdn 按表固定显示。
