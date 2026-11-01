# r802 — BBS / Owl 出站包 + SoftKey

## Opcode（s9 RecvOpcode）
- `OWL_ACTION = 66` / `OWL_WARP = 67`
- `BBS_OPERATION = 155`

## Packets
- `Net/Packets/BbsPackets.h`：List / Open / Delete / Reply / Post
- `Net/Packets/OwlPackets.h`：UseOwl / OwlWarp(owner,map)

## SoftKey
- UIGuild SoftKey `B`：page / o,id / d,id / r,id,text / p,title|text
- UIMTS SoftKey `O` UseOwl；`W` ownerid,mapid → OwlWarp

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
