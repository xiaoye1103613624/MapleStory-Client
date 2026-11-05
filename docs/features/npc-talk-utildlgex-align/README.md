# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：Quiz msgtype 5/6 对齐 + InitialQuiz UI

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11h ScriptMessage OnPacket | ✅ 已接线 |
| A11i AskAvatar UI | ✅ 7/8 |
| A11j AskBoxText UI | ✅ IDA `746C46`/`9A8910`：dlgType=8；cols/rows 定框；OK=`(13,1,str)` |
| A11j s9 回程 | ✅ `NPCMoreTalkHandler` 认 lastMsg 13；`getNPCTalkBoxText` / `sendAskBoxText` |
| 宋体 / 颜色 / α | ✅ 正文 A12M+DARKGREY；输入 A12M+BLACK；名牌 A11M+WHITE |
| Quiz（5） | ✅ s9 msgtype5+3×int；回程无 action 读 string；`UIInitialQuiz` + 出站 `5+string` |
| SpeedQuiz（6） | ⏸ 协议已对齐 msgtype6；UI 仍吞包 `-1`（题面证据不足） |
| AskPet / Mirror | ⏸ 吞包 `-1`；见 evidence |

## 证据

- [evidence/a11j-askboxtext-ida.md](evidence/a11j-askboxtext-ida.md)
- [evidence/a11j-quiz-speedquiz-ida.md](evidence/a11j-quiz-speedquiz-ida.md)
- [evidence/a11j-askpet-mirror-pending.md](evidence/a11j-askpet-mirror-pending.md)
- [evidence/a11i-askavatar-sendstyle-ida.md](evidence/a11i-askavatar-sendstyle-ida.md)
- [evidence/a11h-scriptmessage-onpacket-ida.md](evidence/a11h-scriptmessage-onpacket-ida.md)

## 改动文件

- 客户端：`IO/UITypes/UINpcTalk.{h,cpp}`、`Net/Handlers/NpcInteractionHandlers.cpp`、`Net/Packets/NpcInteractionPackets.h`
- 服务端：`NPCMoreTalkHandler.java`、`PacketCreator.java`、`NPCConversationManager.java`
