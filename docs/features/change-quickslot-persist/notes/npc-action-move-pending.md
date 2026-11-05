# NPC_ACTION 移动分支 — 取证推进（显式预留接线）

> 更新：2026-11-06（Maplestory95.i64 反编译闭环）

## 已证实（s9）

`NPCAnimationHandler`（`BeiDou-Server_s9`）：

- `length == 6`：对话姿态（`int oid` + 2×`byte`）→ 广播同形
- `length > 6`：移动：`readBytes(length - 9)` 原样写入回包（**丢弃尾 9 字节**）

本端已有：

- 上行：`NpcActionTalkPacket`（oid + `0xFF` + `0x00`）挂在 `TalkToNPCPacket`
- 下行：`NpcActionHandler`：读 oid 后 `rem>0 → set_stance("move")` / 否则 `"stand"`；**不解析路径、不位移**

## IDA 本轮（Maplestory95.i64 ✅）

idalib session 打开 `E:\MXD\Maplestory95.i64`；完整伪代码见：

- [evidence/npc-action-generatemovepath-2026-11-06.md](../evidence/npc-action-generatemovepath-2026-11-06.md)
- [evidence/cnpc-generatemovepath-0x671590.decompiled.txt](../evidence/cnpc-generatemovepath-0x671590.decompiled.txt)
- [evidence/cmovepath-flush-0x668160.decompiled.txt](../evidence/cmovepath-flush-0x668160.decompiled.txt)
- [evidence/cmovepath-encode-0x666e20.decompiled.txt](../evidence/cmovepath-encode-0x666e20.decompiled.txt)

| 项 | 值 |
| --- | --- |
| 符号 | `?GenerateMovePath@CNpc@@IAEXJJ@Z` |
| 地址 | `0x671590`（size `0x217`） |
| v95 send op | **241** → s9 `NPC_ACTION` **197** |
| 包头 | `int oid` + `byte nAction` + `byte nChatIdx` |
| 移动 body | `bMove` 时 `CMovePath::Flush` → `CMovePath::Encode` |
| Encode 头 | `short x,y,vx,vy` + `byte elemCount` + attr 表 |
| 尾部 | `byte keyPadCount` + pad + `short left,top,right,bottom`（8） |
| 尾 9 字节 | `keyPadCount==0` 时 `1+8=9`，**与 s9 `length-9` 吻合** |

同族：`CMob::GenerateMovePath` @ `0x651100`（更大，同走 `CMovePath`）。

## 与本端差异

`MovementPacket::writemovement`（`command,x,y,lastx,lasty,fh,newstate,duration`）**≠** 原生 `CMovePath::Encode`。禁止直接拿玩家 Move 包当 NPC 移动 body。

## 缺什么（仍阻塞实现）

1. **本端路径生成器**：无 `CVecCtrlNpc` / 等价攒 ELEM（落点、fh、attr）
2. **真机 c2s 197 `length>6` 十六进制样例**（双开或 `packet_dump`）
3. **双端观感**：第二客户端看 NPC 是否播 move

不解 1–2 之前：**禁止发明** move body / 禁止改 s9 handler。

## 推荐下一刀

1. 同图抓走动 NPC 的 SEND `197` 样例，核对 Encode 字段
2. 评估移植 `CMovePath::Encode` vs 仅完善下行解析位移
3. （可选）对照 `CNpc::OnMove` 下行

## 本轮结论

包形与尾 9 语义已有 IDA 完整证据；**接线仍预留**（缺路径生成与抓包样例）。
