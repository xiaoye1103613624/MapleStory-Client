# r721 — SlideMenu / ContextMenu SoftKey + Party HP ensure

## SoftKey / shell
- SlideMenu `F` FindPlayer SoftKey；`C` ChangeChannel SoftKey；`X` 收起/关
- ContextMenu `W` Whisper SoftKey（无目标名则先 name 再 msg）；`I` CharInfo SoftKey cid；`X` 关

## Ensure-open
- `PartyMemberHPHandler`：缺 Party 则 emplace，再 `update_member_hp`

## 产物
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
