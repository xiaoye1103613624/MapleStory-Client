# r800 — DenyAlliance / Delivery / Family / ChatBar / Guild Leave

## Packet
- `DENY_ALLIANCE_REQUEST = 144` + `DenyAllianceRequestPacket`（byte0 + inviter + guild）
- UIAlliance SoftKey `D`：CharInfo 名 → 只问 guild

## SoftKey / tip→out
- Delivery SoftKey `I` 无背包：CharInfo 名 → meso[,iv,slot,qty]
- ChatBar BtItemLink：USE/ETC 选中直接 UseItem
- Family SoftKey `X`：FamilyTree 选中 / CharInfo cid → YesNo Separate
- Guild SoftKey `L`：有 self 则直接 Leave
- Alliance SoftKey `N`：tooltip 显示当前公告摘要

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
