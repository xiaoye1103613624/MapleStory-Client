# NPC 对话框 UtilDlgEx 原端对齐

> 日期：2026-11-05 ｜ 本轮：A11b SENDSIMPLE「下项」接线 + 取证归档

## 本轮闭环

| 项 | 状态 |
| --- | --- |
| A11b「下项」语义（IDA） | ✅ dlgType=4 → BtNext(8193) 提交当前高亮 selection |
| A11b 接线 | ✅ `SENDSIMPLE` 激活 `BtNext`；悬停更新索引；钮/回车/N 发包 |
| UtilDlgEx 框体几何 | ✅ 沿用 R25（IMG 复核 t/s/Bt* 尺寸） |
| 宋体路线 | ✅ 未改字体路径；保持 A11M/A12M + 思源宋体 |
| KeyDown 持续技松开取消 | ⏸ 证据不足：s9 仅有上行 `SKILL_EFFECT`、无 cancel recv；IDA 仅有 KeyDownBar 素材串 |

## 证据

- [evidence/a11b-sendsimple-next-ida.md](evidence/a11b-sendsimple-next-ida.md)
- 几何：`docs/features/ui-v83-window-restore/evidence/r25-npctalk-geometry/`

## 改动文件

- `IO/UITypes/UINpcTalk.{h,cpp}`
