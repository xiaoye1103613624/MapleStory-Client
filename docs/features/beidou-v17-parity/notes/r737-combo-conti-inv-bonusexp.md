# r737 — Combo / Conti / Inv / BonusExp polish

## Sync
- ShowCombo：ComboCounter `makeactive`
- ContiState leave：`set_clock` tip（不再 `stop` 清文案）
- Gather/Sort：inv 存在则 `makeactive` 再刷
- InventoryGrow：优先 ItemInventory ensure（不再被 Equip 挡住）
- ExtraPendant：EquipInventory ensure
- BonusExp：StatsInfo 已开则 `update_all_stats`

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
