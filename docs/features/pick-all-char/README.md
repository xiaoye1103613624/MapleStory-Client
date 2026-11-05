# PICK_ALL_CHAR（ViewAllChar 选角）

> 日期：2026-11-05

## 问题

`UIViewAllChar` 先前误发 `SELECT_CHAR(19)`。s9 `ViewAllCharSelectedHandler` 期望 `PICK_ALL_CHAR(14)`，body 比 CHAR_SELECT **多一个 world int**，误发会导致 macs/hwid 错位。

## 取证

| 源 | 结论 |
| --- | --- |
| s9 `ViewAllCharSelectedHandler` | `int charId; int world(ignored); String macs; String hwid` |
| s9 `ViewAllCharSelectedWithPicHandler` | `String pic; int charId; int world; macs; hwid` → opcode `VIEW_ALL_WITH_PIC=0x20` |
| port-095 | `CLogin::SendSelectCharPacketByVAC` → `PICK_ALL_CHAR` |

## 实现

- `OutPacket.h`：`PICK_ALL_CHAR=14`、`VIEW_ALL_WITH_PIC=32`
- `SelectCharPackets.h`：`PickAllCharPacket` / `PickAllCharPicPacket`
- `UIViewAllChar`：选中时记录 `worldid`；BtEnter / SoftKey 改发上述包

## 待测

见 [pending-live-tests §8](../pending-live-tests/README.md)。
