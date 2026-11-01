# r735 — QuestAlarm / SP / Fame / Shop / Event ensure

## Ensure-open
- `quest_alarm`：QuestAlarm ensure 再 `set_tip`
- ShowStatusInfo SP：SkillBook/Stats 已开刷；否则 SkillUp ensure + tip
- Fame mode4 / FameResponse：CharInfo ensure + CharInfoRequest
- WeekEvent：Event ensure，不 tip
- ShopResult：Shop ensure（带 look/inv），不 tip

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
