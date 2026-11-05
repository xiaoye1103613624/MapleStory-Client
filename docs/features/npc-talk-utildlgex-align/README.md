# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：AskPet dlgType=6 完整 UI（立绘/等级/亲密条）

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
| AskPet / AskPetAll（9/10） | ✅ dlgType=6：UtilDlgEx_Pet + Prev/Next 轮播 + stand0 + nameTag + Lv/亲密/饱食；出站 int64 |
| Mirror（14） | ✅ `UIDimensionalMirror`（SlideMenu/0）+ 出站 `14+action+int` |

## 证据

- [evidence/a11m-askpet-dlgtype6-full-ui.md](evidence/a11m-askpet-dlgtype6-full-ui.md)
- [evidence/a11l-askpet-mirror-ida.md](evidence/a11l-askpet-mirror-ida.md)
- [evidence/a11j-askboxtext-ida.md](evidence/a11j-askboxtext-ida.md)
- [evidence/a11j-quiz-speedquiz-ida.md](evidence/a11j-quiz-speedquiz-ida.md)
- [evidence/a11k-speedquiz-ui-ida.md](evidence/a11k-speedquiz-ui-ida.md)
- [evidence/a11j-askpet-mirror-pending.md](evidence/a11j-askpet-mirror-pending.md)
- [evidence/a11i-askavatar-sendstyle-ida.md](evidence/a11i-askavatar-sendstyle-ida.md)
- [evidence/a11h-scriptmessage-onpacket-ida.md](evidence/a11h-scriptmessage-onpacket-ida.md)

## 改动文件

- 客户端：`IO/UITypes/UIMiscShells.{h,cpp}`、`Character/Inventory/{Pet,Inventory}.*`、`Net/Handlers/Helpers/ItemParser.cpp`、`Character/Look/PetLook.*`
- 服务端：本轮无改（AskPet long 已于 A11l 对齐）
