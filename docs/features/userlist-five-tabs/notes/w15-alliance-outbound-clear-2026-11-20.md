# W1.5 预留收口：联盟 Kick / 职级 / ChangeMaster（2026-11-20）

## 结论

此前「无 s9 出站字段」判定不完整：W1.5 只列了 c2s `0x01/02/03/04/0A`，漏了 handler 内已实现的 `0x06/07/09`。本波用 Angel IDA + s9 源码补齐字段并接线。

## 改动

| 文件 | 内容 |
|------|------|
| `Net/Packets/AlliancePackets.h` | `AllianceKickPacket` / `AllianceChangeMasterPacket` / `AllianceGradeChangePacket` |
| `IO/UITypes/UIUserList.cpp` | UNION Kick/Change/Up/Down 发包；`update_union_button_states` 随选中亮 |
| `evidence/alliance-kick-grade-changemaster-2026-11-20.md` | IDA + s9 字段表 |

## SoftKey

本步无输名；Kick/Grade/Master 均用已选中行，无 SoftKeyboard 弹窗。
