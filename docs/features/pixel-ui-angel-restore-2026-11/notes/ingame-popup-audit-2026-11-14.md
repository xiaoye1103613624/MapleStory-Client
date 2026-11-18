# 游戏内弹窗 / 角色交互审计 · 2026-11-14

对照：`C:\Users\11036\Desktop\正确客户端截图` + 任务附图；主路径 orange-wz(128B) + IDA Angel `CUIContextMenu` / `CUIUserInfo` / `CUIQuestAlarm`。

## 根因表

| # | 现象 | 根因（证据） | 严重度 | 处置 |
| --- | --- | --- | --- | --- |
| 1 | 右击角色菜单只剩文字 / 样式不对 | `ContextMenu` IMG 仅有 `t/c/s`+`Bt*`，无 `backgrnd`；旧实现 `load_window_bg` 只吃到顶片 `t`(100×23) 后用纯文字画行，且空白右键永远开自己菜单 | P0 | 已修：Angel OnCreate 平铺 t/c/s + MapleButton；右键命中 MapChars |
| 2 | 双击角色无个人情报 | `UIStateGame::doubleclick` 只转发 UI 元素，不命中地图角色；`MapChars` 仅左键发包 | P0 | 已修：双击其他角色 `CharInfoRequest`；双击自己 `emplace UICharInfo` |
| 3 | NPC 对话多层叠在一起 | `UIStateGame::remove` 用 `unique_ptr::release()` **不析构**；`ScriptMessage` 每次 `emplace` 泄漏旧 `UINpcTalk`；叠加 FOCUSED 重开 | P0 | 已修：`reset()` 析构；Handler 复用已有 NpcTalk 槽 |
| 4 | QuestAlarm 只剩文字 | IMG 节点是 `backgrndmin/max/center/bottom`，无 `backgrnd` → `load_window_bg` 失败 | P1 | 已修：`load_window_bg` 回退 `backgrndmin/max` |
| 5 | 个人情报按钮缺 / 崩绘 | v83 `UserInfo` 扁平面：`BtTrade` 非 `BtTrad`；`farm` 常缺；`draw` 裸解引用 `target_character` | P1 | 已修：节点别名 + null/farm 守卫 |
| 6 | 界面菜单 / 目录主菜单 | StatusBar classic：`BtMenu→UIGameMenu`、`BtShort→UIShortCut` 已分流（前序修复） | — | 本轮只读确认，无改 |
| 7 | 聊天类型下拉 / Whisper 框 | `UIChatBar` + `UIWhisperTargetDlg`(Notice4 t/c/s) 已接 | — | 本轮只读确认；Whisper 缺资源仍会 diag |
| 8 | SoftKey | 走 `SoftKeyboard` 三层 backgrnd，路径正确 | — | 残差：若 live NX 缺节点才会文字化 |
| 9 | 背包 tip 半透明 | EquipTooltip 前序已按原生框校准；本轮无新证据差 | — | 残差：真机再对一次 α |
| 10 | 怪物手册 | 资源存在；深度像素未本轮重测 | — | 残差 |
| 11 | 任务窗 Q | Wave3 已校列表/页签高亮；本轮未再动 | — | 残差：寻找 NPC 真机点测 |

## Angel 校准摘要（ContextMenu）

- `CUIContextMenu` 构造：宽 100，高 `15*(mid_rows+2)`；基行 5，按 Party/Guild/Family 条件 +1。
- `OnCreate`：拼 `t` → 循环 `c`(y=23 起步 15) → `s`；钮 x=9，序 Info/Save/Trade/Whisper/(Party)/(Guild)/Claim/(Family)。

## 代码改动

- `Gameplay/MapleMap/MapChars.{h,cpp}` — `find_char_at`
- `IO/UIStateGame.cpp` — doubleclick/rightclick 命中；`remove`→`reset`
- `IO/UITypes/UIMiscShells.{h,cpp}` — ContextMenu 平铺+按钮；`load_window_bg` QuestAlarm
- `IO/UITypes/UICharInfo.cpp` — 按钮别名 / 空指针 / farm
- `Net/Handlers/NpcInteractionHandlers.cpp` — 复用 NpcTalk

## 自测清单

1. 右击其他角色：出现带底板的 ContextMenu（情报/存好友/交易/密语/组队/家族/举报/学院）
2. 空白地右键：自己菜单（无组队/家族等条件钮）
3. 双击其他角色 / 自己 → 个人情报窗（有底板与交易/道具钮）
4. NPC 连续「下项」：始终单层对话，无重影按钮
5. 任务告知（QuestAlarm）：有灰底条，非纯黄字
6. 目录 / 界面：主菜单 vs ShortCut 气球分别弹出
7. 聊天类型下拉 / 悄悄话框底板
8. 背包悬停 tip 半透明蓝框

## 构建 / 部署

- Staging：`build-popup-audit\MapleStory.exe`（5100544）
- Live：`E:\MXD\BeiDou-Client-xy\MapleStory.exe` — SHA256 `47BE00B37D093D214D9AF7E0406922848FEF3FC5602038C4D61B2D316F7EF447` **MATCH**
- MSBuild Release|x64，exit 0
