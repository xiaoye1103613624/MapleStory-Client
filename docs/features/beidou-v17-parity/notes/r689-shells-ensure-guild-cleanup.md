# r689 — guild marker cleanup + SoftKey shells + ensure-open

- RemoveChar：清理 `cid^0x32000000` 公会场标（配 r688 SpawnChar）
- UIPlant `R`→UIRaise；UICharInfo `E`→MapleEvent / `K`→MonsterKilling；UIWedding `M`→MateMessage
- NewYearCardRes / IncubatorResult：ensure-open + set_tip（不 chat）
- FamilyChart：entitlement → 已开 UIFamily 时 `set_privileges`

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
