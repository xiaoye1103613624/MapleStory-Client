# r730 — Trade result / Messenger chat / MTS / tip sync

## Ensure-open
- Trade `0x0A` result：`ensure_trade` 再 `result`
- Messenger `0x06` chat：`ensure_ui` 再 `append_chat`
- MTS `0x23` not-yet-sold：缺窗则 emplace + makeactive 再刷列表
- FamilyChart privs：Family ensure-open 再 `set_privileges`
- Party `ensure_party` 补 `makeactive`

## Tip silence / UI sync
- CashShop inv slots：背包已开则 break，不 tip
- MonsterBook card effect：已开则 `reload()`

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
