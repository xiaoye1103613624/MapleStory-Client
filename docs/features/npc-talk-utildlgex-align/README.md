# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：A11d SENDSIMPLE 高亮 + A11e GetText/GetNumber 布局

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11b「下项」语义（IDA） | ✅ dlgType=4 → BtNext(8193) 提交当前高亮 selection（ee0e7fd） |
| A11c 框体 α | ✅ IDA blit 255 + UI.nx t/c/s 主体 α255；程序侧保持 opacity 1.0 |
| A11c TEXT_Y 居中 | ✅ IDA `(content_h-text_h)/2 + t.height + 7` → `text_draw_y` |
| A11c BtNext x450 vs BtOK 474 | ✅ IDA：Next=`W-78`、OK=`W-54`；纵向 Next=`BTN_DY-51` |
| A11d SENDSIMPLE 高亮 | ✅ IDA：黑 1px + `#919191` 虚线（宽=标签+8）；↑↓/悬停改 cur |
| A11e GetText/GetNumber 布局 | ✅ IDA `0x9A83F6`/`0x9ACC20`：inset=157、黑/白框、Edit、BtOK |
| 宋体路线 | ✅ 未改字体路径；保持 A11M/A12M + 思源宋体 |
| KeyDown 持续技松开取消 | ⏸ 证据不足：s9 仅有上行 `SKILL_EFFECT`、无 cancel recv |
| GetText Avatar 变体 / min-max 线参 | ⏸ 显式预留（见 a11e 局限） |

## 证据

- [evidence/a11b-sendsimple-next-ida.md](evidence/a11b-sendsimple-next-ida.md)
- [evidence/a11c-frame-alpha-texty-btnext.md](evidence/a11c-frame-alpha-texty-btnext.md)
- [evidence/a11d-sendsimple-highlight-ida.md](evidence/a11d-sendsimple-highlight-ida.md)
- [evidence/a11e-gettext-getnumber-layout-ida.md](evidence/a11e-gettext-getnumber-layout-ida.md)
- 几何：`docs/features/ui-v83-window-restore/evidence/r25-npctalk-geometry/`

## 改动文件

- `IO/UITypes/UINpcTalk.{h,cpp}`
