# r720 — Quest tip silence + Carnival/Trade ensure-open

## Tip silence
- `QuestHandlers::quest_alarm`：QuestLog 已开则只 `refresh()`，不 chat/SM（对齐 Messaging mode1 `ql_open`）

## Ensure-open
- `MonsterCarnivalMessageHandler`：缺 UI 则 emplace，再 `set_message`（与 CP 路径一致）
- Trade `0x3A`/`0x3B`/`0x3D`/`0x3E`/`0x3F`：Minigame / MinigameTable ensure-open 后再刷 tip/棋盘

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
