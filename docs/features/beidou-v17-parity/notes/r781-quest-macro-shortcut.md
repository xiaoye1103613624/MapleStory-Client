# r781 — QuestLog 选中 / Macro 挂技 / ShortCut 预填

## UI
- `UIQuestLog::get_selected_quest_id()` 公开
- QuestIcon 打开与 SoftKey `Q`/`S`/`C`/`F`：有选中任务则直接 start/complete/forfeit
- Macro SoftKey `A`：技能书选中则 `assign_skill`+保存，否则 SoftKey id
- ShortCut `{` SpendSp、`-` Forfeit、`[` Gather、`]` Sort：分别用技能书/任务日志/背包当前页预填

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
