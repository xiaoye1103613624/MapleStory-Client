# r703 — SkillUp SoftKey + recv.available 收尾 + Carnival ensure

## SoftKey / shell
- SkillBook `P` → UISkillUp
- ItemInventory `F` → UIItemSearch；`E` → Equip SoftKey（find_equipslot）
- EquipInventory `I` → ItemInventory；`V` → ViciousHammer
- StatsInfo AP SoftKey 支持 `1-4`；`P` → SkillUp；`K` → SkillBook
- StatusBar `N` → TalkToNPC SoftKey；`T` → ItemSearch；`P` → SkillUp；`F` → FadeYesNo；`G` → GMBoard

## tip silence
- ShowStatusInfo mode5 SP：SkillUp 已开 → set_tip 静默
- mode1 quest pad：`recv.length` → `available`

## rem drain
- MapObject / Player / NpcInteraction / Quest / Inventory / Trade 残留 `recv.length()` → `available()`

## Carnival
- Summon / Died：ensure-open + set_message，场标仍保留

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
