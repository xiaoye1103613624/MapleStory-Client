# opcode 对齐核对：MapleStory-Client (v83) ↔ 北斗 gms083

> **⚠️ 本文已被取代（2026-10-30）**：现行事实源是
> [`docs/features/s9-protocol-align/`](../s9-protocol-align/PLAN.md)
> —— 矩阵由 `tools/gen_opcode_matrix.py` 从两侧源码自动重算，并由 `tools/build_smoke.bat`
> 接成构建门禁；归因见 [`notes/opcode-diff.md`](../s9-protocol-align/notes/opcode-diff.md)。
>
> 本文当年那句「**opcode 本就完全对齐，无需修改任何代码**」**不成立**，留存仅为历史记录。
> 现已核实（数据源换成 s9，且客户端规模从 46 出 / 59 入 长到 103 / 303）：
>
> - c2s 有 **4 条 CLIENT_ONLY**，其中 `STORAGE=309`、`PLAYER_INTERACTION=314`、
>   `CASHSHOP_OPERATION=325` 是**把 s2c（`SendOpcode`）的值抄进了发侧枚举**，
>   服务端对应 handler 其实都在（62 / 123 / 229）—— 不是"服务端缺功能"，是客户端发错格子。
>   仓库、商城、雇佣商人/交易/迷你游戏三类出站包因此**一条都到不了服务端**。
> - 入站表有 **2 处撞值**（35、152），客户端 `handlers[O] = make_unique<T>()` 是覆盖写，
>   `ForcedStatResetHandler`、`AriantResultHandler` 被后面的注册盖掉，永不执行。
> - 本文的规模数字已过期：s9 实测 `RecvOpcode` 181 常量 / 180 值、`SendOpcode` 311 常量 /
>   250 个真正下发、173 个生效 handler（本文写的是 193 / 332，对齐对象也是 gms083 而非 s9）。

## 结论

**MapleStory-Client 与北斗 gms083 的 opcode 本就完全对齐，无需修改任何代码。**

> **范围说明（2026-10-21 补）**：本核对只覆盖客户端**已实现**的包，证明「同值即同包」，**不代表服务端全部能力都已在客户端落地**。服务端 `RecvOpcode` 有 193 项、`SendOpcode` 有 332 项，而客户端当前仅 46 个出站包、59 个入站 handler；覆盖缺口见 [`r00-baseline-parity.md`](r00-baseline-parity.md)。

- 协议版本已为 83：`Net/Cryptography.cpp:55` → `static const uint8_t MAPLEVERSION = 83;`
- s2c（服务端→客户端）：**57 / 57** 按值精确匹配北斗 `SendOpcode.java`
- c2s（客户端→服务端）：**44 / 45** 按值精确匹配北斗 `RecvOpcode.java`，唯一例外见下

因此本项（align-opcodes）的产出是**核对结论**，而非代码改动。

## 证据

全量对照表见 [`notes/opcode-align-full.md`](notes/opcode-align-full.md)。

核对方法：按**值**比对（同一数值在线路上即同一封包），而非按名称。
两份 opcode 的命名体系不同（客户端为 HeavenClient 命名，北斗为 OdinMS 命名），
故名称不同但值相同即视为同一封包，例如：

| 客户端名称 | 值 | 北斗名称 | 语义 |
|---|---|---|---|
| LOGIN_RESULT | 0x00 | LOGIN_STATUS | 登录结果 |
| CHAR_MOVED | 0xB9 | MOVE_PLAYER | 玩家移动 |
| ATTACKED_CLOSE | 0xBA | CLOSE_RANGE_ATTACK | 近战攻击 |
| MODIFY_INVENTORY | 0x1D | INVENTORY_OPERATION | 背包操作 |
| CHANGE_STATS | 0x1F | STAT_CHANGED | 属性变更 |
| SPAWN_CHAR | 0xA0 | SPAWN_PLAYER | 生成玩家 |

## 主链路对照（登录 → 进图 → 操作）

| 环节 | 客户端 | 值 | 北斗 | 值 |
|---|---|---|---|---|
| 登录 | LOGIN | 0x01 | LOGIN_PASSWORD | 0x01 |
| 服务器列表 | SERVERLIST_REQUEST | 0x0B | SERVERLIST_REQUEST | 0x0B |
| 角色列表 | CHARLIST_REQUEST | 0x05 | CHARLIST_REQUEST | 0x05 |
| 选择角色 | SELECT_CHAR | 0x13 | CHAR_SELECT | 0x13 |
| 玩家登录 | PLAYER_LOGIN | 0x14 | PLAYER_LOGGEDIN | 0x14 |
| 进图（服务端） | SET_FIELD | 0x7D | SET_FIELD | 0x7D |
| 移动 | MOVE_PLAYER | 0x29 | MOVE_PLAYER | 0x29 |
| 说话 | GENERAL_CHAT | 0x31 | GENERAL_CHAT | 0x31 |
| NPC 对话 | TALK_TO_NPC | 0x3A | NPC_TALK | 0x3A |
| 捡物 | PICKUP_ITEM | 0xCA | ITEM_PICKUP | 0xCA |

## 唯一差异与处置

- **`LOGIN_EMAIL = 0x03`**：北斗 `RecvOpcode` 中**无** 0x03 定义，即服务端未实现邮箱登录。
  - 处置：客户端走 `LOGIN`（0x01，密码登录）路径即可，邮箱登录不可用。
  - 本项不影响握手、登录、选角、进图、移动、攻击、NPC 对话主链路。

## 复现方式

用按值比对的脚本分别解析：

- 客户端 s2c：`Net/PacketSwitch.cpp` 的 `enum Opcode`（InPackets）
- 客户端 c2s：`Net/OutPacket.h` 的 `enum Opcode`（OutPackets）
- 北斗：`gms-server/src/main/java/org/gms/net/opcodes/SendOpcode.java`（s2c）、`RecvOpcode.java`（c2s）

再按数值做映射即可复现上表。

## 重要备注：cms085 不可用于对接北斗

`XyMapleStory-cms085` 是 **85** 版本客户端，其 `Protocol/Fyms085Opcodes.h`（cms085 服务端）与北斗 gms083 差异巨大：

- c2s：39 项中 **30 项不同**
- s2c：65 项中 **48 项不同**

即两者是**两套不同的 opcode 表**，不可混用。这反过来印证：
**MapleStory-Client（v83）才是对接北斗 gms083 的正确基线项目。**
