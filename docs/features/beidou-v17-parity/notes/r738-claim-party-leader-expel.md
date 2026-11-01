# r738 — ClaimResult ensure + Party Leader/Expel buttons

## Ensure-open
- ClaimResult：优先 UIClaim ensure + `set_tip`（再回落 Admin）

## UI wire
- UIParty：可选 `BtLeader`/`BtExpel`（有 Party.img 节点才建）→ `prompt_leader`/`prompt_expel`（已有 OutPacket SoftKey）

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
