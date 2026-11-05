# NPC_ACTION 移动 — IDA 取证摘要（2026-11-06）

IDB：`E:\MXD\Maplestory95.i64`（idalib session `2f07cae6`）  
源：`CNpc::GenerateMovePath` @ `0x671590`（`?GenerateMovePath@CNpc@@IAEXJJ@Z`，size `0x217`）

## 包头（与 s9 length==6 对话分支同前缀）

| 序 | 宽 | 字段 | 证据 |
| --- | --- | --- | --- |
| 0 | op | v95 `241` → s9 `NPC_ACTION` `197` | `COutPacket::COutPacket(..., nType: 241)` @ `0x67165b` |
| 1 | int32 | `m_dwNpcId` | write @ `0x6716af` |
| 2 | byte | `nAction`（入参低 8 位） | write @ `0x6716f5` |
| 3 | byte | `nChatIdx` | write @ `0x671743` |

对话姿态本端已发：`oid + 0xFF + 0x00`（无 path）。

## 移动 body（`m_pTemplate->bMove != 0`）

1. 可选：`GetActionFrameList` 累加 delay → `CVecCtrlNpc::UpdateActiveInterrupted`
2. 循环 ≤55：`CMovePath::IsTimeForFlush` / `CVecCtrl::UpdateActive` 攒路径
3. **`CMovePath::Flush` @ `0x668160`** → 内部调 **`CMovePath::Encode` @ `0x666e20`**
4. `SendPacket`；`m_bMovePathSent = 1`

### `CMovePath::Encode` 字段序（权威）

```
short x, y, vx, vy          // 当前安全坐标/速度
byte  elemCount
for each ELEM:
  byte nAttr
  switch(nAttr):            // 与 Mob/Player 同族 attr 表
    0/5/0xC/0xE/0x23/0x24: x,y,vx,vy,fh [,fhFallStart if 0xC], xOffset,yOffset
    1/2/0xD/0x10/...:      vx,vy + 公共尾
    3/4/6/7/8/0xA:         x,y,fh + 公共尾
    9:                     bStat（无公共尾）
    0xB:                   vx,vy,fhFallStart + 公共尾
    0x11:                  x,y,vx,vy
  // 公共尾（多数 case）: byte bMoveAction + short tElapse (+ 部分 attr 附加)
byte keyPadCount
keyPad packed bytes         // count==0 时无额外字节
short left, top, right, bottom   // m_rcMove = 8 字节
```

完整伪代码：

- [`cnpc-generatemovepath-0x671590.decompiled.txt`](cnpc-generatemovepath-0x671590.decompiled.txt)
- [`cmovepath-flush-0x668160.decompiled.txt`](cmovepath-flush-0x668160.decompiled.txt)
- [`cmovepath-encode-0x666e20.decompiled.txt`](cmovepath-encode-0x666e20.decompiled.txt)

## 尾 9 字节 ↔ s9 `length-9`

当 `keyPadCount == 0`：

`1 (count) + 0 (pad) + 8 (rect) = 9`

与 s9 `NPCAnimationHandler` 移动分支「丢弃尾 9 字节、原样中继 path」**吻合**。

## 与本端 `MovementPacket` 的差异（禁止直接复用）

本端 `Net/Packets/MovementPacket.h::writemovement`：

`command, xpos, ypos, lastx, lasty, fh, newstate, duration`

**不是** `CMovePath::Encode`（缺 vx/vy 头、attr 表、keyPad、rect）。  
NPC 移动接线若用现有 `MovementPacket` 会与原生/s9 错位。

## 仍缺（接线门禁）

| # | 项 | 状态 |
| --- | --- | --- |
| A | GenerateMovePath / Flush / Encode 伪代码 | ✅ 本轮 |
| B | 尾 9 语义（空 keyPad + rect） | ✅ 推理吻合 s9 |
| C | 本端 `CVecCtrlNpc` 等价攒 path（落点/fh） | ❌ 无；无则只能中继/不解 |
| D | 真机 c2s `197` `length>6` 十六进制样例 | ❌ 未抓 |
| E | 双开观感（第二端看 move 动画） | ❌ 未测 |

**结论**：证据已够写「包形文档」；**仍禁止**在无 C/D 的情况下发明 path 元素并改上行发包。下一刀：抓包样例 + 评估是否复用/移植 `CMovePath` Encode，或仅完善下行解析位移。
