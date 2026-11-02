# S8 切片：NPC / 脚本对话 / 传送门 / 反应器

> 协议事实源：`BeiDou-Server_s9`（s9）。opcode 名值以 `OutPacket.h`（客户端）与 `RecvOpcode.java`（s9）为准。
> 本切片 c2s：NPC 对话/任务/商店/反应器。传送门进入由 `CHANGEMAP`(S5) 承载（v83 下 portal 触发服务端切图，无独立 c2s opcode）。

## 1. 包清单（c2s）

| 客户端 opcode | 值 | s9 RecvOpcode | s9 handler | 状态 |
|---|---|---|---|---|
| TALK_TO_NPC | 58 (0x3A) | TALK_TO_NPC | NPCTalkHandler | 对齐 |
| NPC_TALK_MORE | 60 (0x3C) | NPC_TALK_MORE | NPCMoreTalkHandler | 对齐 |
| QUEST_ACTION | 107 (0x6B) | QUEST_ACTION | QuestActionHandler | ⚠ action0 结构性错配（见 §2.4） |
| DAMAGE_REACTOR | 205 (0xCD) | DAMAGE_REACTOR | ReactorHitHandler | 对齐 |
| NPC_SHOP_ACTION | 61 (0x3D) | NPC_SHOP_ACTION | NPCShopHandler | 对齐（见 S6 §2.7） |

## 2. 逐字段对账

### 2.1 TALK_TO_NPC (58) — 发起对话
- 客户端 `TalkToNPCPacket`（`Net/Packets/NpcInteractionPackets.h:26-29`）：`write_int(oid)`。
- s9 `NPCTalkHandler`（line 59）：`int oid = p.readInt();`。
- **对齐** ✓（单 int oid）。

### 2.2 NPC_TALK_MORE (60) — 对话推进/选择
- 客户端 `NpcTalkMorePacket`（NpcInteractionPackets.h:37-59）：base `write_byte(lastmsg) + write_byte(response)`；`lastmsg==2`(returnText) 重载追加 `write_string(response)`；`GetNumber` 重载 `write_byte(0)+write_byte(1)+write_int(amount)+write_int(0)`（selection=int）。
- s9 `NPCMoreTalkHandler`（line 36-64）：`readByte()`(lastMsg)+`readByte()`(action)；`lastMsg==2` → `readString()`(returnText)；否则 `selection`：`available>=4` 读 `readInt()`，否则 `readUnsignedByte()`。
- **对齐** ✓（lastmsg+action 字节，returnText 字符串、selection int/byte 均对应）。

### 2.3 DAMAGE_REACTOR (205) — 打反应器
- 客户端 `DamageReactorPacket`（`GameplayPackets.h:220-231`）：`write_int(oid) + write_point(position)(2×short=4B) + write_short(stance) + skip(4) + write_int(skillid)`。
- s9 `ReactorHitHandler`（line 38-42）：`readInt()`(oid)+`readInt()`(charPos=position 打包)+`readShort()`(stance)+`skip(4)`+`readInt()`(skillid)。
- **对齐** ✓（position 以 4 字节 point 写入、服务端作为 int 读回；skip(4) 两端一致）。

### 2.4 QUEST_ACTION (107) — 任务动作 ⚠ action==0 结构性错配
- 客户端 `QuestActionPacket`（`QuestPackets.h:12-24`）：`write_byte(action) + write_short(questid) + [action∈{1,2,4,5} 时 write_int(npcid)]`。即 action==3(forfeit) 与 **action==0(restoreLostItem) 均不写额外字段**（仅 byte+short）。
- s9 `QuestActionHandler`（line 81-159）：
  - action 1/2/4/5 → `readInt()`(npc) ✓（与客户端写 npcid 对应）
  - action 3 → `quest.forfeit()`（不读额外字段）✓
  - **action 0 → `p.readInt(); int itemid = p.readInt();`**（line 91-92）—— 但客户端 action==0 仅发 byte+short，**无 npcid/itemid**。
- **错配（action==0 仅）**：s9 为 restoreLostItem 读 2 个 int，客户端不为 action==0 发送 itemid。
  - 严重度：**低**。客户端 `QuestActionPacket` 无 UI 路径产生 action==0（v83 "找回丢失任务物品"界面在本端未接线），故该分支在实机不被触发，无现场影响；但属结构分歧（T14b 类：opcode 同、body 语义对 action0 不一致）。
  - 处置：按授权边界**不改客户端**；若后续要支持找回任务物品，须让 `QuestActionPacket` 在 action==0 时追加 `write_int(itemid)`（与 s9 一致）。

## 3. 传送门（portal）
- v83 下进入传送门不依赖独立 c2s opcode：客户端触发 portal 后由服务端经 `CHANGEMAP` 切图（见 S5 §2.1，含 `died/targetid/targetp/usewheel` 字段，已对齐）。本切片不另设 portal 包。

## 4. 结论
- S8 的 5 个 c2s 包中，**4 个逐项对齐**（TALK_TO_NPC / NPC_TALK_MORE / DAMAGE_REACTOR / NPC_SHOP_ACTION）。
- **1 个结构性疑点**：`QUEST_ACTION` 的 action==0（restoreLostItem）客户端不发送 itemid 而 s9 读 2 个 int；因客户端无该 UI 路径、无现场影响，列为低严重度待办（不改客户端）。
- 本切片**未动任何代码**，符合授权边界。
