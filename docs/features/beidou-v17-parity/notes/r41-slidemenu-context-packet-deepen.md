# r41 — SlideMenu / ContextMenu 行为 + 活动包浅深解

## 行为
- **ContextMenu**：地图空白右键 → `open_at`；Info / Whisper / Close 可点
- **SlideMenu**：经典底栏 BtClaim → toggle 展开；BtEvent / BtPartySearch（有资源时）
- **FloatNotice**：`ServerMessage` type 4/5 同步 tip

## 包字段
- CarnivalStart：team + CP
- AriantScore：name:score 列表
- NewYearCard：mode + id + from

## 构建
`tools/build_smoke.bat` → `E:\MXD\BeiDou-Client-xy\MapleStory.exe`
