# 联盟职衔名编辑窗 CWndAllianceGrade（2026-11-20）

## 与「成员 ↑↓ 职级」的区别

| | 职衔名编辑（本窗） | 成员职级升降（UNION 底栏） |
|--|--|--|
| Angel | `CWndAllianceGrade` | `sub_91505B` |
| 入口 | UNION `BtInfo`（Angel btn id `0x809`）Toggle | `BtUp` / `BtDown` |
| 出站 | ALLIANCE_OPERATION **action `0x08`** + 5×string | action **`0x09`** + cid + raise |
| 语义 | 改联盟 5 段职衔**称号字符串** | 改选中成员的联盟职级**序号**（3..5） |

`BtChange`（Angel `0x80C`）仍为 **ChangeMaster**（action `0x07`），不是本窗入口。

## SoftKey

`OnChangeGradeName` 对位 Angel `CUtilDlgEx INPUT_STR` → `UIEnterString`（物理键盘 Textfield）。**禁止** SoftKeyboard 弹窗。保存走 `UIYesNo`。

## 证据

见 `evidence/alliance-grade-name-2026-11-20.md`。
