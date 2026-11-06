# NPC_ACTION 移动分支 — 取证推进（显式预留接线）

> 更新：2026-11-06（VecCtrl 攒 path 条件钉死；仍缺实现与抓包）

## 已证实（s9）

`NPCAnimationHandler`（`BeiDou-Server_s9`）：

- `length == 6`：对话姿态（`int oid` + 2×`byte`）→ 广播同形
- `length > 6`：移动：`readBytes(length - 9)` 原样写入回包（**丢弃尾 9 字节**）

本端已有：

- 上行：`NpcActionTalkPacket`（oid + `0xFF` + `0x00`）挂在 `TalkToNPCPacket`
- 下行：`NpcActionHandler`：读 oid 后 `rem>0 → set_stance("move")` / 否则 `"stand"`；**不解析路径、不位移**

## IDA（Maplestory95.i64）

### 上轮：包形

见 [evidence/npc-action-generatemovepath-2026-11-06.md](../evidence/npc-action-generatemovepath-2026-11-06.md)

| 项 | 值 |
| --- | --- |
| 符号 | `?GenerateMovePath@CNpc@@IAEXJJ@Z` |
| 地址 | `0x671590` |
| v95 send op | **241** → s9 `NPC_ACTION` **197** |
| 移动 body | `bMove` 时 `Flush` → `Encode` |
| 尾 9 | `keyPadCount==0` → `1+8`，对齐 s9 `length-9` |

### 本轮：攒 path 条件（钉死）

见 [evidence/npc-vecctrl-path-accumulate-2026-11-06.md](../evidence/npc-vecctrl-path-accumulate-2026-11-06.md)

| 环节 | 结论 |
| --- | --- |
| 触发 | `Update`：`m_bMovePathSent==0` ∧ `bMove` ∧ `UpdatePassive==-1` |
| 攒 | ≤55× `UpdateActive`，直到 `IsTimeForFlush` |
| Flush 阈值 | 默认 **≥1000ms** gather，且 walk 需 **fh>0** |
| 入表 | `SetMovePathAttribute` → `MakeMovePath` → `m_lElem` AddTail/合并 |
| Interrupted | 动作帧 delay 切片物理 + `SetMovePathAttribute(2)` |

同族：`CMob::GenerateMovePath` @ `0x651100`。

## 与本端差异

`MovementPacket::writemovement` **≠** 原生 `CMovePath::Encode`。禁止直接拿玩家 Move 包当 NPC 移动 body。

本端无 `CVecCtrlNpc` / `MakeMovePath` 等价物 → **无法本地攒 ELEM 上行**。

## 缺什么（仍阻塞上行接线）

1. ~~攒 path 条件~~ → **本轮已钉死**
2. **本端路径生成器**（VecCtrlNpc 物理或可接受的简化落点/fh 合成）
3. **真机 c2s 197 `length>6` 十六进制样例**（本轮 packet_dump 仅见 talk `c5 00 … ff 00`）
4. **双开观感**

不解 2–3 之前：**禁止发明** move body / 禁止改 s9 handler。

下行可选下一刀（不依赖 VecCtrl）：按 Encode 表解析广播包，把 NPC `set_position` + stance——仍建议先有样例再写。

## 本轮结论

包形 + **攒 path 条件**均有 IDA 完整证据；**接线仍预留**（缺路径生成实现与抓包样例）。
