# r786 — Book 封面用图鉴/背包选中；Reset SoftKey S 用技能书

## UI
- `UIMonsterBook::get_selected_card_id()` 公开
- Book 打开 / SoftKey `C`：图鉴选中卡或背包选中道具 id → MonsterBookSetCover
- Reset 打开：技能书选中则 SpendSp；SoftKey `S` 同理（`A` 仍 SoftKey AP）

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
