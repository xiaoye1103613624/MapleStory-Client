# NPC_ACTION 移动分支 — 接线进度

> 更新：2026-11-06（CMovePath Encode 最小上行已接；真机 197 length>6 抓包仍建议补）

## 已证实（s9）

`NPCAnimationHandler`（`BeiDou-Server_s9`）：

- `length == 6`：对话姿态（`int oid` + 2×`byte`）→ 广播同形
- `length > 6`：移动：`readBytes(length - 9)` 原样写入回包（**丢弃尾 9 字节**）

本端已有：

- 上行对话：`NpcActionTalkPacket`（oid + `0xFF` + `0x00`）挂在 `TalkToNPCPacket`
- 上行移动：**`NpcActionMovePacket`** + `MovePathEncode.h`（本轮）
- 下行：`NpcActionHandler`：对话姿态 / 移动头 `x,y` 写 `set_position` + `move` stance

## IDA（Maplestory95.i64）

| 项 | 值 |
| --- | --- |
| `CNpc::GenerateMovePath` | `0x671590` |
| `CMovePath::Encode` | `0x666e20` |
| `CMovePath::Flush` | `0x668160` |
| `IsTimeForFlush` | 默认 ≥1000ms + walk 需 fh>0 |
| v95 send op | **241** → s9 `NPC_ACTION` **197** |
| 尾 9 | `keyPadCount==0` → `1+8`，对齐 s9 `length-9` |

证据目录：`evidence/npc-action-generatemovepath-2026-11-06.md`、`npc-vecctrl-path-accumulate-2026-11-06.md`、`cmovepath-encode-0x666e20.decompiled.txt`。

## 本轮实现（最小可发送）

| 文件 | 作用 |
| --- | --- |
| `Net/Packets/MovePathEncode.h` | 移植 Encode attr switch（IDA 表）；空 keyPad + rect |
| `NpcActionMovePacket` | oid + action + chatIdx + Encode |
| `Npc` 授控 | `SPAWN_NPC_C` → `control=true`；`movable`（wz move）；≥1000ms + fh>0 发一节 attr=0 |
| `NpcActionHandler` | 广播 path 时读头 x/y 位移 |

**故意不做**：完整 `CVecCtrlNpc` 物理；`ClientOptMan GetOpt(2)` rand 附加（v83/s9 默认关）；发明非 IDA attr 字段。

`MovementPacket::writemovement` **仍禁止**用于 NPC_ACTION。

## 仍缺 / 预留

1. 真机 c2s 197 `length>6` 十六进制样例（对照 Encode 字节）
2. 双开观感（第二端看 move）
3. 完整 VecCtrl 攒多节 / Interrupted 动作帧
4. 下行完整 elem 表插值（本轮只吃头坐标）

## 本轮结论

包形 + 攒 path 条件 + **最小 Encode 上行/下行头解析**已接；完整原生 VecCtrl 仍预留。
