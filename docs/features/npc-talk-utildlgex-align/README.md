# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：A11i AskAvatar / sendStyle（msgtype 7/8）完整 UI

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11h ScriptMessage OnPacket | ✅ 已接线（2eccfba）；本轮 7/8 改展示 |
| A11i AskAvatar UI | ✅ IDA `74713D`/`9A92C9`：UtilDlgEx_Avatar 419×306；Prev/Next/OK2/Cancel2/Exit/Off/On |
| A11i 预览 | ✅ CharLook 基貌+选中样式；类别 face/hair/skin；Off=原貌 / On=新样式 |
| A11i 出站 | ✅ OK=`(7|8,1,sel)`；Cancel=`(7|8,0)`（Membershop 路径，防脚本挂死） |
| Quiz/Pet/Mirror/BoxText | ⏸ 仍吞包 `-1`；见 pending |
| 宋体 / 颜色 / α | ✅ 正文 A12M+DARKGREY；名牌 A11M+WHITE；未改字体路径 |

## 证据

- [evidence/a11i-askavatar-sendstyle-ida.md](evidence/a11i-askavatar-sendstyle-ida.md)
- [evidence/a11h-scriptmessage-onpacket-ida.md](evidence/a11h-scriptmessage-onpacket-ida.md)
- 几何：`docs/features/ui-v83-window-restore/evidence/r25-npctalk-geometry/`

## 改动文件

- `IO/UITypes/UINpcTalk.{h,cpp}`
- `Net/Handlers/NpcInteractionHandlers.cpp`
- `Net/Packets/NpcInteractionPackets.h`
