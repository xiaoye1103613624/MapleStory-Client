# r739 — Find / BuffList / tip surface polish

## Sync
- Whisper Find：BuddyList 无 tip API → 始终 ChatBar；Buddy 未开则 makeactive
- ApplyBuff：BuffList `makeactive` 后再 `add_buff`
- PartyValue：Party ensure 后 ChatBar 显 key=val（无 notice API）
- ShopLink fail：StoreBank notice 优先；Merchant 已开则 ChatBar
- BonusExp：StatusBar 已开时 ChatBar tip（条上无 bonus 表面）

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
