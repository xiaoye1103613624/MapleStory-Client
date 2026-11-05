# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：A11h ScriptMessage OnPacket 全量对齐 + tip 820

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11b「下项」语义（IDA） | ✅ dlgType=4 → BtNext(8193) 提交当前高亮 selection（ee0e7fd） |
| A11c 框体 α | ✅ IDA blit 255 + UI.nx t/c/s 主体 α255；程序侧保持 opacity 1.0 |
| A11c TEXT_Y 居中 | ✅ IDA `(content_h-text_h)/2 + t.height + 7` → `text_draw_y` |
| A11c BtNext x450 vs BtOK 474 | ✅ IDA：Next=`W-78`、OK=`W-54`；纵向 Next=`BTN_DY-51` |
| A11d SENDSIMPLE 高亮 | ✅ IDA：黑 1px + `#919191` 虚线（宽=标签+8）；↑↓/悬停改 cur |
| A11e GetText/GetNumber 布局 | ✅ IDA `0x9A83F6`/`0x9ACC20`：inset=157、黑/白框、Edit、BtOK |
| A11f GetText Avatar 变体 | ✅ IDA `0x9A8716`：Edit 226×15@(19,H-73)；OK@158 / Close@208 @(H-30)；无黑白框 |
| A11f 包内 min/max | ✅ GetNumber 解析 def/min/max；OK 全数字+范围校验；失败不关窗 |
| A11g tip 817–821 | ✅ IDA `sub_9929DD`/`99311B`：A12 白+描边 `#355A84` α1；校验失败弹 UIOk |
| A11g Pet inset=25 | ✅ IDA `sub_9ACC20`：`(speaker&6)!=0` → content_left=25 |
| A11h ScriptMessage OnPacket | ✅ IDA `sub_74660A` 逐 case；Say `speaker&4`；GetText short×2；5/6/7/8/9/10/13/14 吞包 `-1` |
| A11h tip 820 | ✅ 接线 `number_min_digits`；线路径 OnPacket 置 0（对齐 `9AC732` a5） |
| 宋体路线 | ✅ 未改字体路径；保持 A11M/A12M + 思源宋体 |
| KeyDown 持续技松开取消 | ⏸ 证据不足：s9 仅有上行 `SKILL_EFFECT`、无 cancel recv |

## 证据

- [evidence/a11b-sendsimple-next-ida.md](evidence/a11b-sendsimple-next-ida.md)
- [evidence/a11c-frame-alpha-texty-btnext.md](evidence/a11c-frame-alpha-texty-btnext.md)
- [evidence/a11d-sendsimple-highlight-ida.md](evidence/a11d-sendsimple-highlight-ida.md)
- [evidence/a11e-gettext-getnumber-layout-ida.md](evidence/a11e-gettext-getnumber-layout-ida.md)
- [evidence/a11f-gettext-avatar-minmax-ida.md](evidence/a11f-gettext-avatar-minmax-ida.md)
- [evidence/a11g-tip-817-821-pet-inset-ida.md](evidence/a11g-tip-817-821-pet-inset-ida.md)
- [evidence/a11h-scriptmessage-onpacket-ida.md](evidence/a11h-scriptmessage-onpacket-ida.md)
- 几何：`docs/features/ui-v83-window-restore/evidence/r25-npctalk-geometry/`

## 改动文件

- `IO/UITypes/UINpcTalk.{h,cpp}`
- `Net/Handlers/NpcInteractionHandlers.cpp`
