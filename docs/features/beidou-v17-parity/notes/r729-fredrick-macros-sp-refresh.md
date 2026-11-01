# r729 — Fredrick ensure + SkillMacros / SP UI refresh

## Ensure-open
- Fredrick `0x24` / Message：`ensure_ui()` 再 notice；`0x1E` clear_and_close 不丢

## UI sync
- SkillMacrosHandler：SkillBook 已开则 `set_macro(true)` 重刷宏标签
- SP tip（ShowStatusInfo mode5）：SkillBook / Stats 已开则 `update_stat(SP)` / `update_all_stats`，不 tip

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
