# r95 — Whisper 出站 SoftKey

- `OutPacket::WHISPER = 120`（s9 `0x78`）
- `WhisperPacket(name, text)` mode=6；`FindPlayerPacket(name)` mode=5
- ChatBar：`/w name text` / `/find name`；`focus_whisper(target)` SoftKey 直接发
- ContextMenu Whisper 行带目标名；CharInfo `BtVisit` → Whisper SoftKey

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
