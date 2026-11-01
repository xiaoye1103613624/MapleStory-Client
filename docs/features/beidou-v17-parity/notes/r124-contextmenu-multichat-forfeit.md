# r124 — ContextMenu / MultiChat / Quest forfeit

- ContextMenu：Whisper 无 ChatBar 时 SoftKey → `WhisperPacket`；原 Close 行改为 Party → `PartyInvitePacket`；热键 1–4
- UIChat Friend：SoftKey cid → msg → `MultiChatPacket(BUDDY, recipients)`
- GameMenu `-` SoftKey questid → `QuestActionPacket(3, qid)` forfeit

产物：`E:\MXD\BeiDou-Client-xy\MapleStory.exe`
