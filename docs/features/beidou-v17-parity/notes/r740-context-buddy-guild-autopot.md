# r740 — ContextMenu Guild/Family/Fame + Buddy group + Guild leave self + AutoPot

## UI wire
- ContextMenu +Guild / Family / Fame 行（`GuildInvite` / `FamilyAdd` / `GiveFame`）
- BuddyList `BtAdd`：name→group SoftKey（默认 All）
- Guild `L` Leave：默认本机 cid/name，SoftKey 可改

## Sync
- AutoHp/Mp pot：KeyConfig 已存在则 makeactive；否则 ChatBar tip

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
