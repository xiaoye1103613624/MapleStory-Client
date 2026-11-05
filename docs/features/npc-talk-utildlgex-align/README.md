# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：AskPet(9/10) 可证子集 + Mirror(14) 完整 UI

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11h ScriptMessage OnPacket | ✅ 已接线 |
| A11i AskAvatar UI | ✅ 7/8 |
| A11j AskBoxText UI | ✅ IDA `746C46`/`9A8910`：dlgType=8；cols/rows 定框；OK=`(13,1,str)` |
| A11j s9 回程 | ✅ `NPCMoreTalkHandler` 认 lastMsg 13；`getNPCTalkBoxText` / `sendAskBoxText` |
| 宋体 / 颜色 / α | ✅ 正文 A12M+DARKGREY；输入 A12M+BLACK；名牌 A11M+WHITE |
| Quiz（5） | ✅ s9 msgtype5+3×int；回程无 action 读 string；`UIInitialQuiz` + 出站 `5+string` |
| SpeedQuiz（6） | ✅ 协议已对齐；`UISpeedQuiz` 题面(Npc/Mob/Item)+出站 `6+string` |
| AskPet / AskPetAll（9/10） | ✅ 入站 long+byte；`UIAskPet` + 出站 `9\|10+action+int64`；完整立绘预留 |
| Mirror（14） | ✅ `UIDimensionalMirror`（SlideMenu/0）+ 出站 `14+action+int` |

## 证据

- [evidence/a11l-askpet-mirror-ida.md](evidence/a11l-askpet-mirror-ida.md)
- [evidence/a11j-askboxtext-ida.md](evidence/a11j-askboxtext-ida.md)
- [evidence/a11j-quiz-speedquiz-ida.md](evidence/a11j-quiz-speedquiz-ida.md)
- [evidence/a11k-speedquiz-ui-ida.md](evidence/a11k-speedquiz-ui-ida.md)
- [evidence/a11j-askpet-mirror-pending.md](evidence/a11j-askpet-mirror-pending.md)
- [evidence/a11i-askavatar-sendstyle-ida.md](evidence/a11i-askavatar-sendstyle-ida.md)
- [evidence/a11h-scriptmessage-onpacket-ida.md](evidence/a11h-scriptmessage-onpacket-ida.md)

## 改动文件

- 客户端：`IO/UITypes/UIMiscShells.{h,cpp}`、`IO/UIElement.h`、`Net/Handlers/NpcInteractionHandlers.cpp`、`Net/Packets/NpcInteractionPackets.h`
- 服务端：`NPCMoreTalkHandler.java`（AskPet long）
