# r722 — QuestClear ensure-open + Messenger seat clear

## Ensure-open
- `QuestClearHandler` / `SetQuestClearHandler`：缺 QuestLog 则 emplace，再 `refresh()`；失败才 field marker
- Messenger `0x02` remove：改用 `ensure_ui()`，座位清零不因窗未开丢失

## SoftKey
- CharInfo 已有 `S` Whisper SoftKey（本轮不加）

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
